/**
 * @file ARUPDATER_Updater.c
 * @brief libARUpdater PlfSender header file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#include <stdio.h>
#include <stdlib.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARUtils/ARUTILS.h>

#include "ARUPDATER_PlfSender.h"

/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARUPDATER_UDPATER_TAG                   "ARUPDATER_PlfSender"
#define ARUPDATER_PLFSENDER_REMOTE_FOLDER       "/"

/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

ARUPDATER_PlfSender_t* ARUPDATER_PlfSender_New(eARUPDATER_ERROR *error)
{
    ARUPDATER_PlfSender_t *plfSender = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    /* Check parameters */
    if(err == ARUPDATER_OK)
    {
        /* Create the Manager */
        plfSender = malloc (sizeof (ARUPDATER_PlfSender_t));
        if (plfSender == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if(err == ARUPDATER_OK)
    {
        /* Initialize to default values */
        plfSender->plfFolder = NULL;
        plfSender->plfFileName = NULL;
        plfSender->progressArg = NULL;
        plfSender->completionArg = NULL;

    }
    
    /* delete the plf Sender if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_UDPATER_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_PlfSender_Delete (&plfSender);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    }
    
    return plfSender;
}


void ARUPDATER_PlfSender_Delete(ARUPDATER_PlfSender_t **plfSender)
{
    ARUPDATER_PlfSender_t *plfSenderPtr = NULL;
    
    if (plfSender)
    {
        plfSenderPtr = *plfSender;
        
        // Uninitialize here
        
        if (plfSenderPtr)
        {
            free(plfSenderPtr->plfFolder);
            free(plfSenderPtr->plfFileName);
            
            free (plfSenderPtr);
            plfSenderPtr = NULL;
        }
        
        *plfSender = NULL;
    }
}

eARUPDATER_ERROR ARUPDATER_PlfSender_PrepareSendToDrone(ARUPDATER_PlfSender_t *plfSender, const char *const plfFolder, const char *const plfFileName, ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback, void *progressArg, ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback, void *completionArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if (plfSender->plfFolder != NULL)
    {
        free(plfSender->plfFolder);
    }
    plfSender->plfFolder = (char*) malloc(strlen(plfFolder) + 1);
    if (plfSender->plfFolder == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (ARUPDATER_OK == error)
    {
        strcpy(plfSender->plfFolder, plfFolder);
    }
    
    if (plfSender->plfFileName != NULL)
    {
        free(plfSender->plfFileName);
    }
    plfSender->plfFileName = (char*) malloc(strlen(plfFileName) + 1);
    if (plfSender->plfFileName == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (ARUPDATER_OK == error)
    {
        strcpy(plfSender->plfFileName, plfFileName);
    }
    
    if (plfSender->progressArg != NULL)
    {
        free(plfSender->progressArg);
        plfSender->progressArg = NULL;
    }
    if (progressArg != NULL)
    {
        plfSender->progressArg = malloc(sizeof(progressArg));
        if (plfSender->progressArg == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        if (ARUPDATER_OK == error)
        {
            memcpy(plfSender->progressArg, progressArg, sizeof(progressArg));
        }
    }
    
    if (plfSender->completionArg != NULL)
    {
        free(plfSender->completionArg);
        plfSender->completionArg = NULL;
    }
    if (completionArg != NULL)
    {
        plfSender->completionArg = malloc(sizeof(completionArg));
        if (plfSender->completionArg == NULL)
        {
            error = ARUPDATER_ERROR_ALLOC;
        }
        if (ARUPDATER_OK == error)
        {
            memcpy(plfSender->completionArg, completionArg, sizeof(completionArg));
        }
    }
    
    plfSender->progressCallback = progressCallback;
    plfSender->completionCallback = completionCallback;
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_PlfSender_SendToDroneThreadRun(void *plfSenderArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    ARUPDATER_PlfSender_t *plfSender;
    eARDATATRANSFER_ERROR dataTransferError = ARDATATRANSFER_OK;
    eARUTILS_ERROR utilsError = ARUTILS_OK;
    ARDATATRANSFER_Manager_t* dataTransferManager;
    ARUTILS_Manager_t* utilsManager;
    
    if (plfSenderArg != NULL)
    {
        plfSender = (ARUPDATER_PlfSender_t*)plfSenderArg;
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    // check that the member variables are set
    if ((plfSender->plfFileName == NULL) ||
        (plfSender->plfFolder == NULL))
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    // create the data transfer manager
    if (ARUPDATER_OK == error)
    {
        dataTransferManager = ARDATATRANSFER_Manager_New(&dataTransferError);
        if (ARDATATRANSFER_OK != dataTransferError)
        {
            error = ARUPDATER_ERROR_PLFSENDER_ARDATATRANSFER_ERROR;
        }
    }
    
    // create the ARUtils manager
    if (ARUPDATER_OK == error)
    {
        utilsManager = ARUTILS_Manager_New(&utilsError);
        if (ARUTILS_OK != utilsError)
        {
            error = ARUPDATER_ERROR_PLFSENDER_ARDATATRANSFER_ERROR;
        }
    }
    
    // create a new uploader
    if (ARUPDATER_OK == error)
    {
        char *localPath = malloc(strlen(plfSender->plfFolder) + strlen(plfSender->plfFileName) + 1);
        strcpy(localPath, plfSender->plfFolder);
        strcat(localPath, plfSender->plfFileName);
        
        //dataTransferError = ARDATATRANSFER_Uploader_New(dataTransferManager, utilsManager, ARUPDATER_PLFSENDER_REMOTE_FOLDER, localPath, plfSender->progressCallback, plfSender->progressArg, plfSender->completionCallback, plfSender->completionArg, ARDATATRANSFER_UPLOADER_RESUME_TRUE);
        
        ARDATATRANSFER_Uploader_ThreadRun(dataTransferManager);
        
        free(localPath);
    }
    
    return error;
}


eARUPDATER_ERROR ARUPDATER_PlfSender_CancelThread(ARUPDATER_PlfSender_t *plfSender)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    // TODO
    return error;
}












