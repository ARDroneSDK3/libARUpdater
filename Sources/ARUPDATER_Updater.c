/**
 * @file ARUPDATER_Updater.c
 * @brief libARUpdater Updater c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARUtils/ARUTILS_Http.h>
#include "ARUPDATER_Updater.h"
#include "ARUPDATER_Utils.h"
/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARUPDATER_UDPATER_TAG   "ARUPDATER_Updater"

#define ARUPDATER_UDPATER_SERVER_URL                    "172.20.5.48"
#define ARUPDATER_UDPATER_PHP_URL                       "~d.bertrand/update.php"
#define ARUPDATER_UDPATER_PARAM_MAX_LENGTH              255
#define ARUPDATER_UDPATER_VERSION_BUFFER_MAX_LENGHT     10
#define ARUPDATER_UDPATER_PRODUCT_PARAM                 "?product="
#define ARUPDATER_UDPATER_SERIAL_PARAM                  "&serialNo="
#define ARUPDATER_UDPATER_VERSION_PARAM                 "&version="
#define ARUPDATER_UDPATER_VERSION_SEPARATOR             "."
#define ARUPDATER_UDPATER_DOWNLOADED_FILE_PREFIX        "tmp_"
#define ARUPDATER_UDPATER_SERIAL_DEFAULT_VALUE          "0000"

#define ARUPDATER_UDPATER_PHP_ERROR_OK                  "0"
#define ARUPDATER_UDPATER_PHP_ERROR_UNKNOWN             "1"
#define ARUPDATER_UDPATER_PHP_ERROR_FILE                "2"
#define ARUPDATER_UDPATER_PHP_ERROR_PLF                 "3"
#define ARUPDATER_UDPATER_PHP_ERROR_MAGIC_PLF           "4"
#define ARUPDATER_UDPATER_PHP_ERROR_UPDATE              "5"

#define ARUPDATER_UDPATER_CHUNK_SIZE                    255

#define ARUPDATER_UDPATER_HTTP_HEADER                   "http://"


/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

ARUPDATER_Updater_t* ARUPDATER_Updater_New(eARUPDATER_ERROR *error)
{
    ARUPDATER_Updater_t *updater = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    /* Check parameters */
    if(err == ARUPDATER_OK)
    {
        /* Create the Manager */
        updater = malloc (sizeof (ARUPDATER_Updater_t));
        if (updater == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if(err == ARUPDATER_OK)
    {
        /* Initialize to default values */

    }
    
    /* delete the Manager if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_UDPATER_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_Updater_Delete (&updater);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    }
    
    return updater;
}

/**
 * @brief Delete the udpater
 * @warning This function free memory
 * @param updater : address of the pointer on the updater
 * @see ARUPDATER_Updater_New()
 */
void ARUPDATER_Updater_Delete(ARUPDATER_Updater_t **updater)
{
    ARUPDATER_Updater_t *updaterPtr = NULL;
    
    if (updater)
    {
        updaterPtr = *updater;
        
        // Uninitialize here
        
        if (updaterPtr)
        {
            free(updaterPtr->plfFolder);
            free(updaterPtr->plfFileName);
            free(updaterPtr->device);
            
            free (updaterPtr);
            updaterPtr = NULL;
        }
        
        *updater = NULL;
    }

}

eARUPDATER_ERROR ARUPDATER_Updater_PrepareCheckLocaleVersion(ARUPDATER_Updater_t *updater, const char *const device, const char *const plfFolder, const char *const plfFileName, ARUPDATER_Updater_ShouldDownloadPlfCallback_t shouldDownloadCallback, ARUPDATER_Updater_PlfDownloadProgressCallback_t progressCallback, ARUPDATER_Updater_PlfDownloadCompletionCallback_t completionCallback)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    updater->device = (char*) malloc(strlen(device) + 1);
    if (updater->device == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (ARUPDATER_OK == error)
    {
        strcpy(updater->device, device);
    }
    
    updater->plfFolder = (char*) malloc(strlen(plfFolder) + 1);
    if (updater->plfFolder == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (ARUPDATER_OK == error)
    {
        strcpy(updater->plfFolder, plfFolder);
    }
    
    updater->plfFileName = (char*) malloc(strlen(plfFileName) + 1);
    if (updater->plfFileName == NULL)
    {
        error = ARUPDATER_ERROR_ALLOC;
    }
    
    if (ARUPDATER_OK == error)
    {
        strcpy(updater->plfFileName, plfFileName);
    }
    
    updater->shouldDownloadCallback = shouldDownloadCallback;
    updater->plfDownloadProgressCallback = progressCallback;
    updater->plfDownloadCompletionCallback = completionCallback;
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_Updater_CheckLocaleVersionThreadRun(void *updaterArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    ARUTILS_Http_Connection_t *connection = NULL;
    int version;
    int edit;
    int ext;
    eARUTILS_ERROR utilsError = ARUTILS_OK;
    ARUPDATER_Updater_t *updater = NULL;
    char *filePath;
    ARSAL_Sem_t requestSem;
    
    char **dataPtr = malloc(sizeof(char*));
    int dataSize;
    
    if (updaterArg != NULL)
    {
        updater = (ARUPDATER_Updater_t*)updaterArg;
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    // check that the member variables are set
    if ((updater->plfFileName == NULL) ||
        (updater->plfFolder == NULL) ||
        (updater->device == NULL))
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    // read the header of the plf file
    if (error == ARUPDATER_OK)
    {
        filePath = malloc(strlen(updater->plfFolder) + strlen(updater->plfFileName) + 1);
        strcpy(filePath, updater->plfFolder);
        strcat(filePath, updater->plfFileName);
        
        error = ARUPDATER_Utils_GetPlfVersion(filePath, &version, &edit, &ext);
        
        edit = 20; // TODO: delete
    }
    
    // init the connection
    if (error == ARUPDATER_OK)
    {
        
        ARSAL_Sem_Init(&requestSem, 0, 0);
        
        connection = ARUTILS_Http_Connection_New(&requestSem, ARUPDATER_UDPATER_SERVER_URL, 80, HTTPS_PROTOCOL_FALSE, NULL, NULL, &utilsError);
    }
    
    // request the php
    if (error == ARUPDATER_OK)
    {
        char buffer[ARUPDATER_UDPATER_VERSION_BUFFER_MAX_LENGHT];
        // create the url params
        char *params = malloc(ARUPDATER_UDPATER_PARAM_MAX_LENGTH);
        strncpy(params, ARUPDATER_UDPATER_PRODUCT_PARAM, strlen(ARUPDATER_UDPATER_PRODUCT_PARAM));
        strncat(params, updater->device, strlen(updater->device));
        
        strncat(params, ARUPDATER_UDPATER_SERIAL_PARAM, strlen(ARUPDATER_UDPATER_SERIAL_PARAM));
        strncat(params, ARUPDATER_UDPATER_SERIAL_DEFAULT_VALUE, strlen(ARUPDATER_UDPATER_SERIAL_DEFAULT_VALUE));
        
        strncat(params, ARUPDATER_UDPATER_VERSION_PARAM, strlen(ARUPDATER_UDPATER_VERSION_PARAM));
        sprintf(buffer,"%i",version);
        strncat(params, buffer, strlen(buffer));
        strncat(params, ARUPDATER_UDPATER_VERSION_SEPARATOR, strlen(ARUPDATER_UDPATER_VERSION_SEPARATOR));
        sprintf(buffer,"%i",edit);
        strncat(params, buffer, strlen(buffer));
        strncat(params, ARUPDATER_UDPATER_VERSION_SEPARATOR, strlen(ARUPDATER_UDPATER_VERSION_SEPARATOR));
        sprintf(buffer,"%i",ext);
        strncat(params, buffer, strlen(buffer));

        utilsError = ARUTILS_Http_Request(connection, ARUPDATER_UDPATER_PHP_URL, params, dataPtr, &dataSize);
        if (utilsError != ARUTILS_OK)
        {
            error = ARUPDATER_ERROR_UPDATER_ARUTILS_ERROR;
        }
        
        if (error == ARUPDATER_OK)
        {
            utilsError = ARUTILS_Http_Connection_Cancel(connection);
            if (utilsError != ARUTILS_OK)
            {
                error = ARUPDATER_ERROR_UPDATER_ARUTILS_ERROR;
            }
        }
        
        if (error == ARUPDATER_OK)
        {
            ARUTILS_Http_Connection_Delete(&connection);
            ARSAL_Sem_Destroy(&requestSem);
        }

        
        free(params);
    }
    
    // check if data fetch from request is valid
    if (error == ARUPDATER_OK)
    {
        if ((*dataPtr != NULL) &&
            (strlen(*dataPtr) != dataSize))
        {
            error = ARUPDATER_ERROR_UPDATER_DOWNLOAD;
        }
    }
    
    // check if plf file need to be updated
    if (error == ARUPDATER_OK)
    {
        char *data = *dataPtr;
        char *result;
        result = strtok(data, "|");
        
        if(strcmp(result, ARUPDATER_UDPATER_PHP_ERROR_OK) == 0)
        {
            // no need to update the plf file
            if (updater->shouldDownloadCallback != NULL)
            {
                updater->shouldDownloadCallback(updaterArg, 0);
            }
        }
        else if (strcmp(result, ARUPDATER_UDPATER_PHP_ERROR_UPDATE) == 0)
        {
            // need to update the plf file
            if (updater->shouldDownloadCallback != NULL)
            {
                updater->shouldDownloadCallback(updaterArg, 1);
            }
            ARUTILS_Http_Connection_t *dlConnection = NULL;
            ARSAL_Sem_t cancelSem;
            char *downloadUrl = strtok(NULL, "|");
            char *md5 = strtok(NULL, "|");
            char *downloadEndUrl;
            char *downloadServer;
            char *downloadedFilePath = malloc(strlen(ARUPDATER_UDPATER_DOWNLOADED_FILE_PREFIX) + strlen(filePath) + 1);
            strcpy(downloadedFilePath, updater->plfFolder);
            strcat(downloadedFilePath, ARUPDATER_UDPATER_DOWNLOADED_FILE_PREFIX);
            strcat(downloadedFilePath, updater->plfFileName);
            fprintf(stderr, "need to download : %s | md5 : %s\n", downloadUrl, md5);
            
            // explode the download url into server and endUrl
            if (strncmp(downloadUrl, ARUPDATER_UDPATER_HTTP_HEADER, strlen(ARUPDATER_UDPATER_HTTP_HEADER)) != 0)
            {
                error = ARUPDATER_ERROR_UPDATER_PHP_ERROR;
            }
            
            // download the file
            if (error == ARUPDATER_OK)
            {
                char *urlWithoutHttpHeader = downloadUrl + strlen(ARUPDATER_UDPATER_HTTP_HEADER);
                const char delimiter = '/';
                
                downloadEndUrl = strchr(urlWithoutHttpHeader, delimiter);
                int serverLength = strlen(urlWithoutHttpHeader) - strlen(downloadEndUrl);
                downloadServer = malloc(serverLength + 1);
                strncpy(downloadServer, urlWithoutHttpHeader, serverLength);
                
                ARSAL_Sem_Init(&cancelSem, 0, 0);
                
                dlConnection = ARUTILS_Http_Connection_New(&cancelSem, downloadServer, 80, HTTPS_PROTOCOL_FALSE, NULL, NULL, &utilsError);
                if (utilsError != ARUTILS_OK)
                {
                    ARUTILS_Http_Connection_Delete(&dlConnection);
                    error = ARUPDATER_ERROR_UPDATER_ARUTILS_ERROR;
                }
            }
            
            if (error == ARUPDATER_OK)
            {
                utilsError = ARUTILS_Http_Get(dlConnection, downloadEndUrl, downloadedFilePath, updater->plfDownloadProgressCallback, "progress : ");
                if (utilsError != ARUTILS_OK)
                {
                    error = ARUPDATER_ERROR_UPDATER_ARUTILS_ERROR;
                    fprintf(stderr, "error : %i\n", utilsError);
                }
            }
            
            if (error == ARUPDATER_OK)
            {
                utilsError = ARUTILS_Http_Connection_Cancel(dlConnection);
                if (utilsError != ARUTILS_OK)
                {
                    error = ARUPDATER_ERROR_UPDATER_ARUTILS_ERROR;
                }
            }
            
            if (error == ARUPDATER_OK)
            {
                ARUTILS_Http_Connection_Delete(&dlConnection);
                ARSAL_Sem_Destroy(&cancelSem);
            }
            
            // if no error, check the md5
            if (error == ARUPDATER_OK)
            {
                /*// TODO: check md5
                crypto_hash_t *md5=crypto_hash_md5_new();
                
                char line[ARUPDATER_UDPATER_CHUNK_SIZE];
                uint8_t *test = malloc(16);
                FILE* file = fopen(downloadedFilePath, "r");
                while (fread(line, ARUPDATER_UDPATER_CHUNK_SIZE, 1, file) != NULL)
                {
                    fprintf(stderr, "%i\n", strlen(line));
                    md5->push_data(md5, (uint8_t*)line, ARUPDATER_UDPATER_CHUNK_SIZE);
                }
                md5->get_hash(md5, test, 128/8);
                md5->destroy(md5);
                fprintf(stderr, "md5 = %x\n", test);*/
            }
            
            if (error == ARUPDATER_OK)
            {
                if (rename(downloadedFilePath, filePath) != 0)
                {
                    error = ARUPDATER_ERROR_UPDATER_RENAME_FILE;
                }
            }
            
            free(downloadServer);
            free(downloadedFilePath);
            
            if (updater->plfDownloadCompletionCallback != NULL)
            {
                updater->plfDownloadCompletionCallback(updaterArg, error);
            }
        }
        else
        {
            error = ARUPDATER_ERROR_UPDATER_PHP_ERROR;
        }
    }
    
    free(dataPtr);
    
    return error;
}


eARUPDATER_ERROR ARUPDATER_Updater_CancelThread(ARUPDATER_Updater_t *updater)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    return error;
}

