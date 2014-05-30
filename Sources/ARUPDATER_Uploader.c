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
#include "ARUPDATER_Manager.h"

#include "ARUPDATER_Uploader.h"

/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARUPDATER_UPLOADER_TAG                   "ARUPDATER_Uploader"
#define ARUPDATER_UPLOADER_REMOTE_FOLDER         "/"


/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

eARUPDATER_ERROR ARUPDATER_Uploader_New(ARUPDATER_Manager_t* manager, const char *const rootFolder, ARUTILS_Manager_t *ftpManager, eARDISCOVERY_PRODUCT product, ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback, void *progressArg, ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback, void *completionArg)
{
    ARUPDATER_Uploader_t *uploader = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    // Check parameters
    if (rootFolder == NULL)
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
        
        uploader->isRunning = 0;
        uploader->isCanceled = 0;
        uploader->isUploadThreadRunning = 0;
                
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
    
    if (manager != NULL)
    {
        manager->uploader->isRunning = 1;
    }
    
    eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;

    char *sourceFilePath;
    char *destFilePath;
    char *device;
    
    uint16_t productId = ARDISCOVERY_getProductID(manager->uploader->product);
    device = malloc(ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE);
    snprintf(device, ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
    
    destFilePath = malloc(strlen(ARUPDATER_UPLOADER_REMOTE_FOLDER) + strlen(ARUPDATER_MANAGER_PLF_FILENAME) + 1);
    strcpy(destFilePath, ARUPDATER_UPLOADER_REMOTE_FOLDER);
    strcat(destFilePath, ARUPDATER_MANAGER_PLF_FILENAME);

    sourceFilePath = malloc(strlen(manager->uploader->rootFolder) + strlen(ARUPDATER_MANAGER_PLF_FOLDER) + strlen(device) + strlen(ARUPDATER_MANAGER_FOLDER_SEPARATOR) + strlen(ARUPDATER_MANAGER_PLF_FILENAME) + 1);
    strcpy(sourceFilePath, manager->uploader->rootFolder);
    strcat(sourceFilePath, ARUPDATER_MANAGER_PLF_FOLDER);
    strcat(sourceFilePath, device);
    strcat(sourceFilePath, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
    strcat(sourceFilePath, ARUPDATER_MANAGER_PLF_FILENAME);
    
    ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
    // create a new uploader
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Uploader_New(manager->uploader->dataTransferManager, manager->uploader->ftpManager, destFilePath, sourceFilePath, manager->uploader->progressCallback, manager->uploader->progressArg, manager->uploader->completionCallback, manager->uploader->completionArg, ARDATATRANSFER_UPLOADER_RESUME_FALSE);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
    
    if ((ARUPDATER_OK == error) && (manager->uploader->isCanceled == 0))
    {
        manager->uploader->isUploadThreadRunning = 1;
        ARDATATRANSFER_Uploader_ThreadRun(manager->uploader->dataTransferManager);
        manager->uploader->isUploadThreadRunning = 0;
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
    
    free(device);
    free(sourceFilePath);
    
    if (manager != NULL)
    {
        manager->uploader->isRunning = 0;
    }
    
    return (void*)error;
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
        ARSAL_Mutex_Lock(&manager->uploader->uploadLock);

    }
    
    if (error == ARUPDATER_OK)
    {
        manager->uploader->isRunning = 0;
    }
    
    return error;
}

