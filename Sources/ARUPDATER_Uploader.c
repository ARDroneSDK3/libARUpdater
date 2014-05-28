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
        uploader->rootFolder = (char*) malloc(strlen(rootFolder) + 1);
        strcpy(uploader->rootFolder, rootFolder);
        
        uploader->product = product;
        uploader->ftpManager = ftpManager;
        
        uploader->isRunning = 0;
        uploader->isCanceled = 0;
        
        uploader->uploadConnection = NULL;
        
        if (progressArg != NULL)
        {
            uploader->progressArg = malloc(sizeof(progressArg));
            memcpy(uploader->progressArg, progressArg, sizeof(progressArg));
        }
        else
        {
            uploader->progressArg = NULL;
        }
        
        if (completionArg != NULL)
        {
            uploader->completionArg = malloc(sizeof(completionArg));
            memcpy(uploader->completionArg, completionArg, sizeof(completionArg));
        }
        else
        {
            uploader->completionArg = NULL;
        }
        
        uploader->progressCallback = progressCallback;
        uploader->completionCallback = completionCallback;
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
                
                if (manager->uploader->progressArg != NULL)
                {
                    free(manager->uploader->progressArg);
                }

                if (manager->uploader->completionArg)
                {
                    free(manager->uploader->completionArg);
                }
                
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
    
    eARUTILS_ERROR utilsError = ARUTILS_OK;
    eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;
    ARDATATRANSFER_Manager_t* dataTransferManager;

    ARUPDATER_Manager_t *manager = NULL;
    char *sourceFilePath;
    char *destFilePath;
    char *device;
    ARSAL_Sem_t uploadSem;
    
    if (managerArg != NULL)
    {
        manager = (ARUPDATER_Manager_t*)managerArg;
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
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
    
    // create the data transfer manager
    if (ARUPDATER_OK == error)
    {
        dataTransferManager = ARDATATRANSFER_Manager_New(&dataTransferError);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR;
        }
    }

    
    // create a new uploader
    if (ARUPDATER_OK == error)
    {
        dataTransferError = ARDATATRANSFER_Uploader_New(dataTransferManager, manager->uploader->ftpManager, destFilePath, sourceFilePath, manager->uploader->progressCallback, manager->uploader->progressArg, manager->uploader->completionCallback, manager->uploader->completionArg, ARDATATRANSFER_UPLOADER_RESUME_FALSE);
        
        ARDATATRANSFER_Uploader_ThreadRun(dataTransferManager);        
    }

    
    if (error != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_UPLOADER_TAG, "error: %s", ARUPDATER_Error_ToString (error));
    }
    
    free(device);
    free(sourceFilePath);
    
    return (void*)error;
}





eARUPDATER_ERROR ARUPDATER_Uploader_CancelThread(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    int resultSys = 0;
    
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
        
        // TODO:

        
        if (resultSys != 0)
        {
            error = ARUPDATER_ERROR_SYSTEM;
        }
    }
    
    return error;
}

