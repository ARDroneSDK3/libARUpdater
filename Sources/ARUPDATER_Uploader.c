/**
 * @file ARUPDATER_Updater.c
 * @brief libARUpdater PlfSender header file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#include <stdio.h>
#include <stdlib.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARUtils/ARUtils.h>
#include <libARSAL/ARSAL_Error.h>
#include "ARUPDATER_Manager.h"

#include "ARUPDATER_Uploader.h"
#include "ARUPDATER_Utils.h"

/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARUPDATER_UPLOADER_TAG                   "ARUPDATER_Uploader"
#define ARUPDATER_UPLOADER_REMOTE_FOLDER         "/"
#define ARUPDATER_UPLOADER_MD5_FILENAME          "md5_check.txt"
#define ARUPDATER_UPLOADER_CHUNK_SIZE            32
/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

eARUPDATER_ERROR ARUPDATER_Uploader_New(ARUPDATER_Manager_t* manager, const char *const rootFolder, ARUTILS_Manager_t *ftpManager, ARSAL_MD5_Manager_t *md5Manager, eARDISCOVERY_PRODUCT product, ARUPDATER_Uploader_PlfUploadProgressCallback_t progressCallback, void *progressArg, ARUPDATER_Uploader_PlfUploadCompletionCallback_t completionCallback, void *completionArg)
{
    ARUPDATER_Uploader_t *uploader = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    // Check parameters
    if ((manager == NULL) || (rootFolder == NULL) || (ftpManager == NULL) || (md5Manager == NULL))
    {
        err = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if(err == ARUPDATER_OK)
    {
        /* Create the uploader */
        uploader = malloc (sizeof (ARUPDATER_Uploader_t));
        if (uploader == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if (err == ARUPDATER_OK)
    {
        if (manager->uploader != NULL)
        {
            err = ARUPDATER_ERROR_ALREADY_INITIALIZED;
        }
        else
        {
            manager->uploader = uploader;
        }
    }
    
    /* Initialize to default values */
    if(err == ARUPDATER_OK)
    {
        int rootFolderLength = strlen(rootFolder) + 1;
        char *slash = strrchr(rootFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR[0]);
        if ((slash != NULL) && (strcmp(slash, ARUPDATER_MANAGER_FOLDER_SEPARATOR) != 0))
        {
            rootFolderLength += 1;
        }
        uploader->rootFolder = (char*) malloc(rootFolderLength);
        strcpy(uploader->rootFolder, rootFolder);
        
        if ((slash != NULL) && (strcmp(slash, ARUPDATER_MANAGER_FOLDER_SEPARATOR) != 0))
        {
            strcat(uploader->rootFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        }
        
        uploader->product = product;
        uploader->ftpManager = ftpManager;
        uploader->md5Manager = md5Manager;
        
        uploader->isRunning = 0;
        uploader->isCanceled = 0;
        uploader->isUploadThreadRunning = 0;
        
        uploader->uploadError = ARDATATRANSFER_OK;
                
        uploader->progressArg = progressArg;
        uploader->completionArg = completionArg;
        
        uploader->progressCallback = progressCallback;
        uploader->completionCallback = completionCallback;
    }
    
    // create the data transfer manager
    if (ARUPDATER_OK == err)
    {
        eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;
        uploader->dataTransferManager = ARDATATRANSFER_Manager_New(&dataTransferError);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            err = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    
    if (err == ARUPDATER_OK)
    {
        int resultSys = ARSAL_Mutex_Init(&manager->uploader->uploadLock);
        
        if (resultSys != 0)
        {
            err = ARUPDATER_ERROR_SYSTEM;
        }
    }
    
    /* delete the uploader if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_UPLOADER_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_Uploader_Delete (manager);
    }
    
    return err;
}


eARUPDATER_ERROR ARUPDATER_Uploader_Delete(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    if (manager == NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    else
    {
        if (manager->uploader == NULL)
        {
            error = ARUPDATER_ERROR_NOT_INITIALIZED;
        }
        else
        {
            if (manager->uploader->isRunning != 0)
            {
                error = ARUPDATER_ERROR_THREAD_PROCESSING;
            }
            else
            {
                ARSAL_Mutex_Destroy(&manager->uploader->uploadLock);
                free(manager->uploader->rootFolder);
                
                ARDATATRANSFER_Manager_Delete(&manager->uploader->dataTransferManager);
                
                free(manager->uploader);
                manager->uploader = NULL;
            }
        }
    }
    
    return error;
}

void* ARUPDATER_Uploader_ThreadRun(void *managerArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
 
    ARUPDATER_Manager_t *manager = NULL;
    if (managerArg != NULL)
    {
        manager = (ARUPDATER_Manager_t*)managerArg;
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if ((manager != NULL) && (manager->uploader != NULL))
    {
        manager->uploader->isRunning = 1;
    }
    
    eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;

    char *sourceFileFolder = NULL;
    char *sourceFilePath = NULL;
    char *sourceMD5FilePath = NULL;
    char *destFilePath = NULL;
    char *device = NULL;
    char *fileName = NULL;
    char *md5Txt = NULL;
    
    uint16_t productId = ARDISCOVERY_getProductID(manager->uploader->product);
    device = malloc(ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE);
    snprintf(device, ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
    
    sourceFileFolder = malloc(strlen(manager->uploader->rootFolder) + strlen(ARUPDATER_MANAGER_PLF_FOLDER) + strlen(device) + strlen(ARUPDATER_MANAGER_FOLDER_SEPARATOR) + 1);
    strcpy(sourceFileFolder, manager->uploader->rootFolder);
    strcat(sourceFileFolder, ARUPDATER_MANAGER_PLF_FOLDER);
    strcat(sourceFileFolder, device);
    strcat(sourceFileFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
    
    fileName = malloc(1);
    error = ARUPDATER_Utils_GetPlfInFolder(sourceFileFolder, &fileName);
    
    if (error == ARUPDATER_OK)
    {
        destFilePath = malloc(strlen(ARUPDATER_UPLOADER_REMOTE_FOLDER) + strlen(fileName) + 1);
        strcpy(destFilePath, ARUPDATER_UPLOADER_REMOTE_FOLDER);
        strcat(destFilePath, fileName);
        
        sourceFilePath = malloc(strlen(sourceFileFolder) + strlen(fileName) + 1);
        strcpy(sourceFilePath, sourceFileFolder);
        strcat(sourceFilePath, fileName);
        
        sourceMD5FilePath = malloc(strlen(sourceFileFolder) + strlen(ARUPDATER_UPLOADER_MD5_FILENAME) + 1);
        strcpy(sourceMD5FilePath, sourceFileFolder);
        strcat(sourceMD5FilePath, ARUPDATER_UPLOADER_MD5_FILENAME);
    }
    
    if (error == ARUPDATER_OK)
    {
        uint8_t *md5 = malloc(ARSAL_MD5_LENGTH);
        eARSAL_ERROR arsalError = ARSAL_MD5_Manager_Compute(manager->uploader->md5Manager, sourceFilePath, md5, ARSAL_MD5_LENGTH);
        if (ARSAL_OK == arsalError)
        {
            // get md5 in text
            md5Txt = malloc(ARSAL_MD5_LENGTH * 2);
            int i = 0;
            for (i = 0; i < ARSAL_MD5_LENGTH; i++)
            {
                sprintf(&md5Txt[i * 2], "%02x", md5[i]);
            }
        }
        else
        {
            error = ARUPDATER_ERROR_UPLOADER_ARSAL_ERROR;
        }
        free(md5);
    }
    
    // by default, do not resume an upload
    eARDATATRANSFER_UPLOADER_RESUME resumeMode = ARDATATRANSFER_UPLOADER_RESUME_FALSE;
    
    // check if an upload was in progress
    if (ARUPDATER_OK == error)
    {
        // an upload should be resumed if and only if the md5 file is present and the md5 in file match with the plf file md5
        FILE *md5File = fopen(sourceMD5FilePath, "rb");
        if (md5File != NULL)
        {
            int size = 0;
            char line[ARUPDATER_UPLOADER_CHUNK_SIZE];
            int allocatedSize = 1;
            char *uploadedMD5 = malloc(allocatedSize);
            strcpy(uploadedMD5, "");
            while ((size = fread(line, 1, ARUPDATER_UPLOADER_CHUNK_SIZE, md5File)) != 0)
            {
                allocatedSize += size;
                uploadedMD5 = realloc(uploadedMD5, allocatedSize);
                strcat(uploadedMD5, line);
            }
            fclose(md5File);
            md5File = NULL;
            
            // md5s match, so we can resume the upload
            if (strcmp(md5Txt, uploadedMD5) == 0)
            {
                resumeMode = ARDATATRANSFER_UPLOADER_RESUME_TRUE;
            } // ELSE md5s don't match, so keep the default value of resumeMode (=> begin a new upload)
            
            free(uploadedMD5);
            uploadedMD5 = NULL;
        }
    }
    
    ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
    // create a new uploader
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Uploader_New(manager->uploader->dataTransferManager, manager->uploader->ftpManager, destFilePath, sourceFilePath, ARUPDATER_Uploader_UploadProgressCallback, manager, ARUPDATER_Uploader_UploadCompletionCallback, manager, resumeMode);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
    
    if ((ARUPDATER_OK == error) && (resumeMode == ARDATATRANSFER_UPLOADER_RESUME_FALSE))
    {
        // if the upload is a new one, store the md5 in a file
        FILE *md5File = fopen(sourceMD5FilePath, "wb");
        if (md5File != NULL && md5Txt != NULL)
        {
            fprintf(md5File, "%s", md5Txt);
            fclose(md5File);
        }
        else
        {
            error = ARUPDATER_ERROR_UPLOADER;
        }
    }
    
    if (md5Txt != NULL)
    {
        free(md5Txt);
        md5Txt = NULL;
    }
    
    if ((ARUPDATER_OK == error) && (manager->uploader->isCanceled == 0))
    {
        manager->uploader->isUploadThreadRunning = 1;
        ARDATATRANSFER_Uploader_ThreadRun(manager->uploader->dataTransferManager);
        manager->uploader->isUploadThreadRunning = 0;
        if (manager->uploader->uploadError != ARDATATRANSFER_OK)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    
    ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Uploader_Delete(manager->uploader->dataTransferManager);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
    
    if (error != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_UPLOADER_TAG, "error: %s", ARUPDATER_Error_ToString (error));
    }
    
    if (sourceFileFolder != NULL)
    {
        free(sourceFileFolder);
    }
    if (sourceMD5FilePath != NULL)
    {
        free(sourceMD5FilePath);
    }
    if (sourceFilePath != NULL)
    {
        free(sourceFilePath);
    }
    if (device != NULL)
    {
        free(device);
    }
    if (fileName != NULL)
    {
        free(fileName);
    }
    if (destFilePath != NULL)
    {
        free(destFilePath);
    }
    
    if ((manager != NULL) && (manager->uploader != NULL))
    {
        manager->uploader->isRunning = 0;
    }
    
    if (manager->uploader->completionCallback != NULL)
    {
        manager->uploader->completionCallback(manager->uploader->completionArg, error);
    }
    
    return (void*)error;
}

void ARUPDATER_Uploader_UploadProgressCallback(void* arg, uint8_t percent)
{
    ARUPDATER_Manager_t *manager = (ARUPDATER_Manager_t *)arg;
    if (manager->uploader->progressCallback != NULL)
    {
        manager->uploader->progressCallback(manager->uploader->progressArg, percent);
    }
}

void ARUPDATER_Uploader_UploadCompletionCallback(void* arg, eARDATATRANSFER_ERROR error)
{
    ARUPDATER_Manager_t *manager = (ARUPDATER_Manager_t *)arg;
    if (manager->uploader != NULL)
    {
        manager->uploader->uploadError = error;
    }
}

eARUPDATER_ERROR ARUPDATER_Uploader_CancelThread(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if (manager == NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if ((error == ARUPDATER_OK) && (manager->uploader == NULL))
    {
        error = ARUPDATER_ERROR_NOT_INITIALIZED;
    }
    
    if (error == ARUPDATER_OK)
    {
        manager->uploader->isCanceled = 1;
        
        ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
        if (manager->uploader->isUploadThreadRunning == 1)
        {
            ARDATATRANSFER_Uploader_CancelThread(manager->uploader->dataTransferManager);
        }
        ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);

    }
    
    if (error == ARUPDATER_OK)
    {
        manager->uploader->isRunning = 0;
    }
    
    return error;
}

int ARUPDATER_Uploader_ThreadIsRunning(ARUPDATER_Manager_t* manager, eARUPDATER_ERROR *error)
{
    eARUPDATER_ERROR err = ARUPDATER_OK;
    int isRunning = 0;
    
    if (manager == NULL)
    {
        err = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if ((err == ARUPDATER_OK) && (manager->uploader == NULL))
    {
        err = ARUPDATER_ERROR_NOT_INITIALIZED;
    }
    
    if (err == ARUPDATER_OK)
    {
        isRunning = manager->uploader->isRunning;
    }
    
    if (error != NULL)
    {
        *error = err;
    }
    
    return isRunning;
}

