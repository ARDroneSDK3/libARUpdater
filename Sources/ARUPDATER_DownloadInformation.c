/**
 * @file ARUPDATER_DownloadInformation.c
 * @brief libARUpdater Download information c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libARSAL/ARSAL_Print.h>
#include "ARUPDATER_DownloadInformation.h"

/* ***************************************
 *
 *             define :
 *
 *****************************************/

#define ARUPDATER_DOWNLOAD_INFORMATION_TAG "DownloadInformation"

/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

ARUPDATER_DownloadInformation_t* ARUPDATER_DownloadInformation_New(const char *const downloadUrl, const char *const md5Expected, const eARDISCOVERY_PRODUCT product, eARUPDATER_ERROR *error)
{
    ARUPDATER_DownloadInformation_t *downloadInfo = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    if(err == ARUPDATER_OK)
    {
        /* Create the dlInfo */
        downloadInfo = malloc (sizeof (ARUPDATER_DownloadInformation_t));
        if (downloadInfo == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    /* Initialize to default values */
    if(err == ARUPDATER_OK)
    {
        if (downloadUrl != NULL)
        {
            downloadInfo->downloadUrl = malloc(strlen(downloadUrl) + 1);
            strcpy(downloadInfo->downloadUrl, downloadUrl);
        }
        else
        {
            downloadInfo->downloadUrl = NULL;
        }
        
        if (md5Expected != NULL)
        {
            downloadInfo->md5Expected = malloc(strlen(md5Expected) + 1);
            strcpy(downloadInfo->md5Expected, md5Expected);
        }
        else
        {
            downloadInfo->md5Expected = NULL;
        }
        
        downloadInfo->product = product;
    }
    
    /* delete the downloader if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_DOWNLOAD_INFORMATION_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_DownloadInformation_Delete (&downloadInfo);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    } // else let error at its previous value
    
    return downloadInfo;

}

void ARUPDATER_DownloadInformation_Delete(ARUPDATER_DownloadInformation_t **downloadInfo)
{
    ARUPDATER_DownloadInformation_t *downloadInfoPtr = NULL;
    
    if (downloadInfo)
    {
        downloadInfoPtr = *downloadInfo;
        
        if (downloadInfoPtr)
        {
            // Uninitialize here
            if (downloadInfoPtr->downloadUrl != NULL)
            {
                free(downloadInfoPtr->downloadUrl);
                downloadInfoPtr->downloadUrl = NULL;
            }
            
            if (downloadInfoPtr->md5Expected != NULL)
            {
                free(downloadInfoPtr->md5Expected);
                downloadInfoPtr->md5Expected = NULL;
            }
            
            
            free (downloadInfoPtr);
            downloadInfoPtr = NULL;
        }
        
        *downloadInfo = NULL;
    }
}