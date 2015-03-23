/*
    Copyright (C) 2014 Parrot SA

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the 
      distribution.
    * Neither the name of Parrot nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/
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
#define ARUPDATER_UPLOADER_MD5_FILENAME          "md5_check.md5"
#define ARUPDATER_UPLOADER_UPLOADED_FILE_SUFFIX  ".tmp"
#define ARUPDATER_UPLOADER_CHUNK_SIZE            32
/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

eARUPDATER_ERROR ARUPDATER_Uploader_New(ARUPDATER_Manager_t* manager, const char *const rootFolder, ARUTILS_Manager_t *ftpManager, ARSAL_MD5_Manager_t *md5Manager, int isAndroidApp, eARDISCOVERY_PRODUCT product, ARUPDATER_Uploader_PlfUploadProgressCallback_t progressCallback, void *progressArg, ARUPDATER_Uploader_PlfUploadCompletionCallback_t completionCallback, void *completionArg)
{
    ARUPDATER_Uploader_t *uploader = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    char *slash = NULL;
    
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
            err = ARUPDATER_ERROR_MANAGER_ALREADY_INITIALIZED;
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
        slash = strrchr(rootFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR[0]);
        if ((slash != NULL) && (strcmp(slash, ARUPDATER_MANAGER_FOLDER_SEPARATOR) != 0))
        {
            rootFolderLength += 1;
        }
        uploader->rootFolder = (char*) malloc(rootFolderLength);
        if (uploader->rootFolder == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if (err == ARUPDATER_OK)
    {
        strcpy(uploader->rootFolder, rootFolder);
        
        if ((slash != NULL) && (strcmp(slash, ARUPDATER_MANAGER_FOLDER_SEPARATOR) != 0))
        {
            strcat(uploader->rootFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        }
        
        uploader->product = product;
        uploader->isAndroidApp = isAndroidApp;
        uploader->ftpManager = ftpManager;
        uploader->md5Manager = md5Manager;
        
        uploader->isRunning = 0;
        uploader->isCanceled = 0;
        uploader->isUploadThreadRunning = 0;
        uploader->isDownloadMd5ThreadRunning = 0;
        
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
            error = ARUPDATER_ERROR_MANAGER_NOT_INITIALIZED;
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
                manager->uploader->rootFolder = NULL;
                
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
        if ((manager->uploader->product == ARDISCOVERY_PRODUCT_MINIDRONE) && (manager->uploader->isAndroidApp == 1))
        {
            error = ARUPDATER_Uploader_ThreadRunAndroidDelos(manager);
        }
        else
        {
            // upload plf the normal way
            error = ARUPDATER_Uploader_ThreadRunNormal(manager);
        }
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    return (void*)error;
}

eARUPDATER_ERROR ARUPDATER_Uploader_ThreadRunAndroidDelos(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if ((manager != NULL) && (manager->uploader != NULL))
    {
        manager->uploader->isRunning = 1;
    }
    
    eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;
    
    char *sourceFileFolder = NULL;
    char *sourceFilePath = NULL;
    char *device = NULL;
    char *fileName = NULL;
    
    uint16_t productId = ARDISCOVERY_getProductID(manager->uploader->product);
    device = malloc(ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE);
    if (device == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (error == ARUPDATER_OK)
    {
        snprintf(device, ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
        
        sourceFileFolder = malloc(strlen(manager->uploader->rootFolder) + strlen(ARUPDATER_MANAGER_PLF_FOLDER) + strlen(device) + strlen(ARUPDATER_MANAGER_FOLDER_SEPARATOR) + 1);
        strcpy(sourceFileFolder, manager->uploader->rootFolder);
        strcat(sourceFileFolder, ARUPDATER_MANAGER_PLF_FOLDER);
        strcat(sourceFileFolder, device);
        strcat(sourceFileFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        
        error = ARUPDATER_Utils_GetPlfInFolder(sourceFileFolder, &fileName);
    }
    
    if (error == ARUPDATER_OK)
    {
        sourceFilePath = malloc(strlen(sourceFileFolder) + strlen(fileName) + 1);
        if (sourceFilePath == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if (ARUPDATER_OK == error)
    {
        strcpy(sourceFilePath, sourceFileFolder);
        strcat(sourceFilePath, fileName);
        
        // by default, do not resume an upload
        eARDATATRANSFER_UPLOADER_RESUME resumeMode = ARDATATRANSFER_UPLOADER_RESUME_FALSE;
        
        ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
        // create a new uploader
        dataTransferError = ARDATATRANSFER_Uploader_New(manager->uploader->dataTransferManager, manager->uploader->ftpManager, "", sourceFilePath, ARUPDATER_Uploader_ProgressCallback, manager, ARUPDATER_Uploader_CompletionCallback, manager, resumeMode);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
        ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
    }
    
    if ((ARUPDATER_OK == error) && (manager->uploader->isCanceled == 0))
    {
        manager->uploader->isUploadThreadRunning = 1;
        ARDATATRANSFER_Uploader_ThreadRun(manager->uploader->dataTransferManager);
        manager->uploader->isUploadThreadRunning = 0;
        if (manager->uploader->uploadError != ARDATATRANSFER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARUPDATER_UPLOADER_TAG, "ARDataTransferError = %d", manager->uploader->uploadError);
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

    if ((manager != NULL) && (manager->uploader != NULL))
    {
        manager->uploader->isRunning = 0;
    }
    
    if (manager->uploader->completionCallback != NULL)
    {
        manager->uploader->completionCallback(manager->uploader->completionArg, error);
    }
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_Uploader_ThreadRunNormal(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if ((manager != NULL) && (manager->uploader != NULL))
    {
        manager->uploader->isRunning = 1;
    }
    
    eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;

    char *sourceFileFolder = NULL;
    char *sourceFilePath = NULL;
    char *tmpDestFilePath = NULL;
    char *finalDestFilePath = NULL;
    char *device = NULL;
    char *fileName = NULL;
    char *md5Txt = NULL;
    char *md5RemotePath = NULL;
    char *md5LocalPath = NULL;
    uint8_t *md5 = NULL;
    
    uint16_t productId = ARDISCOVERY_getProductID(manager->uploader->product);
    device = malloc(ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE);
    if (device == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (ARUPDATER_OK == error)
    {
        snprintf(device, ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
        
        sourceFileFolder = malloc(strlen(manager->uploader->rootFolder) + strlen(ARUPDATER_MANAGER_PLF_FOLDER) + strlen(device) + strlen(ARUPDATER_MANAGER_FOLDER_SEPARATOR) + 1);
        if (sourceFileFolder == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        else
        {
            strcpy(sourceFileFolder, manager->uploader->rootFolder);
            strcat(sourceFileFolder, ARUPDATER_MANAGER_PLF_FOLDER);
            strcat(sourceFileFolder, device);
            strcat(sourceFileFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        }
    }
    
    if (ARUPDATER_OK == error)
    {
        error = ARUPDATER_Utils_GetPlfInFolder(sourceFileFolder, &fileName);
    }
    
    if (ARUPDATER_OK == error)
    {
        tmpDestFilePath = malloc(strlen(ARUPDATER_UPLOADER_REMOTE_FOLDER) + strlen(fileName) + strlen(ARUPDATER_UPLOADER_UPLOADED_FILE_SUFFIX) + 1);
        if (tmpDestFilePath == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        else
        {
            strcpy(tmpDestFilePath, ARUPDATER_UPLOADER_REMOTE_FOLDER);
            strcat(tmpDestFilePath, fileName);
            strcat(tmpDestFilePath, ARUPDATER_UPLOADER_UPLOADED_FILE_SUFFIX);
        }
    }
    
    if (ARUPDATER_OK == error)
    {
        finalDestFilePath = malloc(strlen(ARUPDATER_UPLOADER_REMOTE_FOLDER) + strlen(fileName) + 1);
        if (finalDestFilePath == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        else
        {
            strcpy(finalDestFilePath, ARUPDATER_UPLOADER_REMOTE_FOLDER);
            strcat(finalDestFilePath, fileName);
        }
    }
    
    if (ARUPDATER_OK == error)
    {
        sourceFilePath = malloc(strlen(sourceFileFolder) + strlen(fileName) + 1);
        if (sourceFilePath == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        else
        {
            strcpy(sourceFilePath, sourceFileFolder);
            strcat(sourceFilePath, fileName);
        }
    }
    
    if (ARUPDATER_OK == error)
    {
        md5LocalPath = malloc(strlen(sourceFileFolder) + strlen(ARUPDATER_UPLOADER_MD5_FILENAME) + 1);
        if (md5LocalPath == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        else
        {
            strcpy(md5LocalPath, sourceFileFolder);
            strcat(md5LocalPath, ARUPDATER_UPLOADER_MD5_FILENAME);
        }
    }
    
    if (ARUPDATER_OK == error)
    {
        md5RemotePath = malloc(strlen(ARUPDATER_UPLOADER_REMOTE_FOLDER) + strlen(ARUPDATER_UPLOADER_MD5_FILENAME) + 1);
        if (md5RemotePath == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        else
        {
            strcpy(md5RemotePath, ARUPDATER_UPLOADER_REMOTE_FOLDER);
            strcat(md5RemotePath, ARUPDATER_UPLOADER_MD5_FILENAME);
        }
    }
    
    // get md5 of the plf file to upload
    if (error == ARUPDATER_OK)
    {
        md5 = malloc(ARSAL_MD5_LENGTH);
        if (md5 == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
    }
    if (error == ARUPDATER_OK)
    {
        eARSAL_ERROR arsalError = ARSAL_MD5_Manager_Compute(manager->uploader->md5Manager, sourceFilePath, md5, ARSAL_MD5_LENGTH);
        if (ARSAL_OK == arsalError)
        {
            // get md5 in text
            md5Txt = malloc(ARSAL_MD5_LENGTH * 2 + 1);
            int i = 0;
            for (i = 0; i < ARSAL_MD5_LENGTH; i++)
            {
                snprintf(&md5Txt[i * 2], 3, "%02x", md5[i]);
            }
        }
        else
        {
            error = ARUPDATER_ERROR_UPLOADER_ARSAL_ERROR;
        }
    }
    
    if (md5 != NULL)
    {
        free(md5);
        md5 = NULL;
    }
    
    // by default, do not resume an upload
    eARDATATRANSFER_UPLOADER_RESUME resumeMode = ARDATATRANSFER_UPLOADER_RESUME_FALSE;
    
    // read distant plf md5
    ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Downloader_New(manager->uploader->dataTransferManager, manager->uploader->ftpManager, md5RemotePath, md5LocalPath, NULL, NULL, ARUPDATER_Uploader_CompletionCallback, manager, ARDATATRANSFER_DOWNLOADER_RESUME_FALSE);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
    
    if ((ARUPDATER_OK == error) && (manager->uploader->isCanceled == 0))
    {
        manager->uploader->isDownloadMd5ThreadRunning = 1;
        ARDATATRANSFER_Downloader_ThreadRun(manager->uploader->dataTransferManager);
        manager->uploader->isDownloadMd5ThreadRunning = 0;
    }
    
    ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Downloader_Delete(manager->uploader->dataTransferManager);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
    
    // check if an upload was in progress
    if (ARUPDATER_OK == error)
    {
        // an upload should be resumed if and only if the md5 file is present and the md5 in file match with the plf file md5
        FILE *md5File = fopen(md5LocalPath, "rb");
        if (md5File != NULL)
        {
            int size = 0;
            char line[ARUPDATER_UPLOADER_CHUNK_SIZE];
            int allocatedSize = 1;
            char *uploadedMD5 = malloc(allocatedSize);
            if (uploadedMD5 != NULL)
            {
                strcpy(uploadedMD5, "");
            }
            else
            {
                error = ARUPDATER_ERROR_ALLOC;
            }
            
            while ((ARUPDATER_OK == error) && (size = fread(line, 1, ARUPDATER_UPLOADER_CHUNK_SIZE, md5File)) != 0)
            {
                char *uploadedMD5Reallocated = realloc(uploadedMD5, allocatedSize + size + 1);
                if (uploadedMD5Reallocated != NULL)
                {
                    allocatedSize += size;
                    uploadedMD5 = uploadedMD5Reallocated;
                    strncat(uploadedMD5, line, size);
                    uploadedMD5[allocatedSize] = '\0';
                }
                else
                {
                    error = ARUPDATER_ERROR_ALLOC;
                }
            }
            fclose(md5File);
            md5File = NULL;
            
            if (ARUPDATER_OK == error)
            {
                // md5s match, so we can resume the upload
                if (strcmp(md5Txt, uploadedMD5) == 0)
                {
                    resumeMode = ARDATATRANSFER_UPLOADER_RESUME_TRUE;
                } // ELSE md5s don't match, so keep the default value of resumeMode (=> begin a new upload)
            }
            
            free(uploadedMD5);
            uploadedMD5 = NULL;
            
            // delete the md5LocalPath file
            unlink(md5LocalPath);
        }
    }
    
    // store in md5LocalPath the md5 of the file that will be uploaded if the upload is a new one
    if ((ARUPDATER_OK == error) && (resumeMode == ARDATATRANSFER_UPLOADER_RESUME_FALSE))
    {
        FILE *md5File = fopen(md5LocalPath, "wb");
        if ((md5File != NULL) && (md5Txt != NULL))
        {
            fprintf(md5File, "%s", md5Txt);
            fclose(md5File);
        }
        else
        {
            error = ARUPDATER_ERROR_UPLOADER;
        }
        
        // create an uploader to upload the md5 file
        ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
        if (ARUPDATER_OK == error)
        {
            dataTransferError = ARDATATRANSFER_Uploader_New(manager->uploader->dataTransferManager, manager->uploader->ftpManager, md5RemotePath, md5LocalPath, NULL, NULL, ARUPDATER_Uploader_CompletionCallback, manager, ARDATATRANSFER_UPLOADER_RESUME_FALSE);
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
        
        if (ARUPDATER_OK == error)
        {
            unlink(md5LocalPath);
        }
    }
    
    if (md5Txt != NULL)
    {
        free(md5Txt);
        md5Txt = NULL;
    }
    
    ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
    // create a new uploader
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Uploader_New(manager->uploader->dataTransferManager, manager->uploader->ftpManager, tmpDestFilePath, sourceFilePath, ARUPDATER_Uploader_ProgressCallback, manager, ARUPDATER_Uploader_CompletionCallback, manager, resumeMode);
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
        if (manager->uploader->uploadError != ARDATATRANSFER_OK)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }
    
    // rename the plf file if the operation went well
    if ((ARUPDATER_OK == error) && (manager->uploader->isCanceled == 0))
    {
        ARDATATRANSFER_Uploader_Rename(manager->uploader->dataTransferManager, tmpDestFilePath, finalDestFilePath);
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
    if (md5LocalPath != NULL)
    {
        free(md5LocalPath);
    }
    if (md5RemotePath != NULL)
    {
        free(md5RemotePath);
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
    if (tmpDestFilePath != NULL)
    {
        free(tmpDestFilePath);
    }
    if (finalDestFilePath != NULL)
    {
        free(finalDestFilePath);
    }
    
    if ((manager != NULL) && (manager->uploader != NULL))
    {
        manager->uploader->isRunning = 0;
    }
    
    if (manager->uploader->completionCallback != NULL)
    {
        manager->uploader->completionCallback(manager->uploader->completionArg, error);
    }
    
    return error;
}

void ARUPDATER_Uploader_ProgressCallback(void* arg, float percent)
{
    ARUPDATER_Manager_t *manager = (ARUPDATER_Manager_t *)arg;
    if (manager->uploader->progressCallback != NULL)
    {
        manager->uploader->progressCallback(manager->uploader->progressArg, percent);
    }
}

void ARUPDATER_Uploader_CompletionCallback(void* arg, eARDATATRANSFER_ERROR error)
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
        error = ARUPDATER_ERROR_MANAGER_NOT_INITIALIZED;
    }
    
    if (error == ARUPDATER_OK)
    {
        manager->uploader->isCanceled = 1;
        
        ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
        if (manager->uploader->isDownloadMd5ThreadRunning == 1)
        {
            ARDATATRANSFER_Downloader_CancelThread(manager->uploader->dataTransferManager);
        }
        ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);
        
        ARSAL_Mutex_Lock(&manager->uploader->uploadLock);
        if (manager->uploader->isUploadThreadRunning == 1)
        {
            ARDATATRANSFER_Uploader_CancelThread(manager->uploader->dataTransferManager);
        }
        ARSAL_Mutex_Unlock(&manager->uploader->uploadLock);

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
        err = ARUPDATER_ERROR_MANAGER_NOT_INITIALIZED;
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

