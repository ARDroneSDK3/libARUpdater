/**
 * @file ARUPDATER_Downloader.c
 * @brief libARUpdater Downloader c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARUtils/ARUTILS_Http.h>
#include <libARDiscovery/ARDISCOVERY_Discovery.h>
#include "ARUPDATER_Manager.h"
#include "ARUPDATER_Downloader.h"
#include "ARUPDATER_Utils.h"
/* ***************************************
 *
 *             define :
 *
 *****************************************/
#define ARUPDATER_DOWNLOADER_TAG   "ARUPDATER_Downloader"

#define ARUPDATER_DOWNLOADER_SERVER_URL                    "172.20.5.48"
#define ARUPDATER_DOWNLOADER_PHP_URL                       "~d.bertrand/update.php"
#define ARUPDATER_DOWNLOADER_PARAM_MAX_LENGTH              255
#define ARUPDATER_DOWNLOADER_VERSION_BUFFER_MAX_LENGHT     10
#define ARUPDATER_DOWNLOADER_PRODUCT_PARAM                 "?product="
#define ARUPDATER_DOWNLOADER_SERIAL_PARAM                  "&serialNo="
#define ARUPDATER_DOWNLOADER_VERSION_PARAM                 "&version="
#define ARUPDATER_DOWNLOADER_VERSION_SEPARATOR             "."
#define ARUPDATER_DOWNLOADER_DOWNLOADED_FILE_PREFIX        "tmp_"
#define ARUPDATER_DOWNLOADER_SERIAL_DEFAULT_VALUE          "0000"

#define ARUPDATER_DOWNLOADER_PHP_ERROR_OK                  "0"
#define ARUPDATER_DOWNLOADER_PHP_ERROR_UNKNOWN             "1"
#define ARUPDATER_DOWNLOADER_PHP_ERROR_FILE                "2"
#define ARUPDATER_DOWNLOADER_PHP_ERROR_PLF                 "3"
#define ARUPDATER_DOWNLOADER_PHP_ERROR_MAGIC_PLF           "4"
#define ARUPDATER_DOWNLOADER_PHP_ERROR_UPDATE              "5"

#define ARUPDATER_DOWNLOADER_CHUNK_SIZE                    255

#define ARUPDATER_DOWNLOADER_DEVICE_STRING_MAX_SIZE        10
#define ARUPDATER_DOWNLOADER_FOLDER_SEPARATOR              "/"
#define ARUPDATER_DOWNLOADER_PLF_FILENAME                  "plf.plf"

#define ARUPDATER_DOWNLOADER_HTTP_HEADER                   "http://"


/* ***************************************
 *
 *             function implementation :
 *
 *****************************************/

eARUPDATER_ERROR ARUPDATER_Downloader_New(ARUPDATER_Manager_t* manager, const char *const plfFolder, ARUPDATER_Downloader_ShouldDownloadPlfCallback_t shouldDownloadCallback, void *downloadArg, ARUPDATER_Downloader_PlfDownloadProgressCallback_t progressCallback, void *progressArg, ARUPDATER_Downloader_PlfDownloadCompletionCallback_t completionCallback, void *completionArg)
{
    ARUPDATER_Downloader_t *downloader = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    /* Check parameters */
    if(err == ARUPDATER_OK)
    {
        /* Create the downloader */
        downloader = malloc (sizeof (ARUPDATER_Downloader_t));
        if (downloader == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }
    
    if (err == ARUPDATER_OK)
    {
        if (manager->downloader != NULL)
        {
            err = ARUPDATER_ERROR_ALREADY_INITIALIZED;
        }
        else
        {
            manager->downloader = downloader;
        }
    }
    
    // TODO: check variables
    
    /* Initialize to default values */
    if(err == ARUPDATER_OK)
    {
        downloader->plfFolder = (char*) malloc(strlen(plfFolder) + 1);
        strcpy(downloader->plfFolder, plfFolder);
        
        if (downloadArg != NULL)
        {
            downloader->downloadArg = malloc(sizeof(downloadArg));
            memcpy(downloader->downloadArg, downloadArg, sizeof(downloadArg));
        }
        
        if (progressArg != NULL)
        {
            downloader->progressArg = malloc(sizeof(progressArg));
            memcpy(downloader->progressArg, progressArg, sizeof(progressArg));
        }
        
        if (completionArg != NULL)
        {
            downloader->completionArg = malloc(sizeof(completionArg));
            memcpy(downloader->completionArg, completionArg, sizeof(completionArg));
        }
        
        
        downloader->shouldDownloadCallback = shouldDownloadCallback;
        downloader->plfDownloadProgressCallback = progressCallback;
        downloader->plfDownloadCompletionCallback = completionCallback;
        
        downloader->isRunning = 0;
        downloader->isCanceled = 0;
    }

    /* delete the downloader if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_DOWNLOADER_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_Downloader_Delete (manager);
    }
    
    return err;
}


eARUPDATER_ERROR ARUPDATER_Downloader_Delete(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    if (manager == NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    else
    {
        if (manager->downloader == NULL)
        {
            error = ARUPDATER_ERROR_NOT_INITIALIZED;
        }
        else
        {
            if (manager->downloader->isRunning != 0)
            {
                error = ARUPDATER_ERROR_THREAD_PROCESSING;
            }
            else
            {
                
                ARSAL_Sem_Destroy(&manager->downloader->requestSem);
                ARSAL_Sem_Destroy(&manager->downloader->dlSem);
                
                free(manager->downloader->plfFolder);
                free(manager->downloader->downloadArg);
                free(manager->downloader->progressArg);
                free(manager->downloader->completionArg);
                
                free(manager->downloader);
                manager->downloader = NULL;
            }
        }
    }
    
    return error;
}

eARUPDATER_ERROR ARUPDATER_Downloader_ThreadRun(void *managerArg)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    ARUTILS_Http_Connection_t *connection = NULL;
    int version;
    int edit;
    int ext;
    eARUTILS_ERROR utilsError = ARUTILS_OK;
    ARUPDATER_Manager_t *manager = NULL;
    char *device;
    char *deviceFolder;
    char *filePath;
    int plfFileNotExisting = 0;
    int dataSize;
    char **dataPtr;
    
    
    if (managerArg != NULL)
    {
        manager = (ARUPDATER_Manager_t*)managerArg;
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    int product = 0;
    while ((error == ARUTILS_OK) && (product < ARDISCOVERY_PRODUCT_MAX))
    {
        // for each product, check if update is needed
        uint16_t productId = ARDISCOVERY_getProductID(product);
        
        device = malloc(ARUPDATER_DOWNLOADER_DEVICE_STRING_MAX_SIZE);
        snprintf(device, ARUPDATER_DOWNLOADER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
        
        dataPtr = malloc(sizeof(char*));

        
        // read the header of the plf file
        deviceFolder = malloc(strlen(manager->downloader->plfFolder) + strlen(device) + strlen(ARUPDATER_DOWNLOADER_FOLDER_SEPARATOR) + strlen(ARUPDATER_DOWNLOADER_PLF_FILENAME) + 1);
        strcpy(deviceFolder, manager->downloader->plfFolder);
        strcat(deviceFolder, device);
        strcat(deviceFolder, ARUPDATER_DOWNLOADER_FOLDER_SEPARATOR);
        
        // file path = deviceFolder + plfFilename + \0
        filePath = malloc(strlen(deviceFolder) + strlen(ARUPDATER_DOWNLOADER_PLF_FILENAME) + 1);
        strcpy(filePath, manager->downloader->plfFolder);
        strcat(filePath, device);
        strcat(filePath, ARUPDATER_DOWNLOADER_FOLDER_SEPARATOR);
        strcat(filePath, ARUPDATER_DOWNLOADER_PLF_FILENAME);

        error = ARUPDATER_Utils_GetPlfVersion(filePath, &version, &edit, &ext);

        edit = 20; // TODO: delete
        
        // if the file does not exist, force to download
        if (error == ARUPDATER_ERROR_PLF_FILE_NOT_FOUND)
        {
            version = 0;
            edit = 0;
            ext = 0;
            error = ARUPDATER_OK;
            
            // also check that the directory exists
            FILE *dir = fopen(manager->downloader->plfFolder, "r");
            if (dir == NULL)
            {
                mkdir(manager->downloader->plfFolder, S_IRWXU);
            }
            else
            {
                fclose(dir);
            }
            
            dir = fopen(deviceFolder, "r");
            if (dir == NULL)
            {
                mkdir(deviceFolder, S_IRWXU);
            }
            else
            {
                fclose(dir);
            }
        }
        
        // init the request semaphore
        if (error == ARUPDATER_OK)
        {
            int resultSys = ARSAL_Sem_Init(&manager->downloader->requestSem, 0, 0);
            if (resultSys != 0)
            {
                error = ARUPDATER_ERROR_SYSTEM;
            }
        }
        
        // init the connection
        if (error == ARUPDATER_OK)
        {
            connection = ARUTILS_Http_Connection_New(&manager->downloader->requestSem, ARUPDATER_DOWNLOADER_SERVER_URL, 80, HTTPS_PROTOCOL_FALSE, NULL, NULL, &utilsError);
            if (utilsError != ARUTILS_OK)
            {
                error = ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR;
            }
        }
        
        // request the php
        if (error == ARUPDATER_OK)
        {
            char buffer[ARUPDATER_DOWNLOADER_VERSION_BUFFER_MAX_LENGHT];
            // create the url params
            char *params = malloc(ARUPDATER_DOWNLOADER_PARAM_MAX_LENGTH);
            strncpy(params, ARUPDATER_DOWNLOADER_PRODUCT_PARAM, strlen(ARUPDATER_DOWNLOADER_PRODUCT_PARAM));
            strncat(params, device, strlen(device));
            
            strncat(params, ARUPDATER_DOWNLOADER_SERIAL_PARAM, strlen(ARUPDATER_DOWNLOADER_SERIAL_PARAM));
            strncat(params, ARUPDATER_DOWNLOADER_SERIAL_DEFAULT_VALUE, strlen(ARUPDATER_DOWNLOADER_SERIAL_DEFAULT_VALUE));
            
            strncat(params, ARUPDATER_DOWNLOADER_VERSION_PARAM, strlen(ARUPDATER_DOWNLOADER_VERSION_PARAM));
            sprintf(buffer,"%i",version);
            strncat(params, buffer, strlen(buffer));
            strncat(params, ARUPDATER_DOWNLOADER_VERSION_SEPARATOR, strlen(ARUPDATER_DOWNLOADER_VERSION_SEPARATOR));
            sprintf(buffer,"%i",edit);
            strncat(params, buffer, strlen(buffer));
            strncat(params, ARUPDATER_DOWNLOADER_VERSION_SEPARATOR, strlen(ARUPDATER_DOWNLOADER_VERSION_SEPARATOR));
            sprintf(buffer,"%i",ext);
            strncat(params, buffer, strlen(buffer));
            
            utilsError = ARUTILS_Http_Request(connection, ARUPDATER_DOWNLOADER_PHP_URL, params, dataPtr, &dataSize);
            if (utilsError != ARUTILS_OK)
            {
                error = ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR;
            }
            
            if (error == ARUPDATER_OK)
            {
                utilsError = ARUTILS_Http_Connection_Cancel(connection);
                if (utilsError != ARUTILS_OK)
                {
                    error = ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR;
                }
            }
            
            if (error == ARUPDATER_OK)
            {
                ARUTILS_Http_Connection_Delete(&connection);
                ARSAL_Sem_Destroy(&manager->downloader->requestSem);
            }
            
            
            free(params);
        }
        
        // check if data fetch from request is valid
        if (error == ARUPDATER_OK)
        {
            if ((*dataPtr != NULL) &&
                (strlen(*dataPtr) != dataSize))
            {
                error = ARUPDATER_ERROR_DOWNLOADER_DOWNLOAD;
            }
        }
        
        // check if plf file need to be updated
        if (error == ARUPDATER_OK)
        {
            char *data = *dataPtr;
            char *result;
            result = strtok(data, "|");
            
            if(strcmp(result, ARUPDATER_DOWNLOADER_PHP_ERROR_OK) == 0)
            {
                // no need to update the plf file
                if (manager->downloader->shouldDownloadCallback != NULL)
                {
                    manager->downloader->shouldDownloadCallback(manager->downloader->downloadArg, 0);
                }
            }
            else if (strcmp(result, ARUPDATER_DOWNLOADER_PHP_ERROR_UPDATE) == 0)
            {
                // need to update the plf file
                if (manager->downloader->shouldDownloadCallback != NULL)
                {
                    manager->downloader->shouldDownloadCallback(manager->downloader->downloadArg, 1);
                }
                ARUTILS_Http_Connection_t *dlConnection = NULL;
                char *downloadUrl = strtok(NULL, "|");
                char *md5 = strtok(NULL, "|");
                char *downloadEndUrl;
                char *downloadServer;
                char *downloadedFilePath = malloc(strlen(ARUPDATER_DOWNLOADER_DOWNLOADED_FILE_PREFIX) + strlen(filePath) + 1);
                fprintf(stderr, "test\n");
                strcpy(downloadedFilePath, deviceFolder);
                strcat(downloadedFilePath, ARUPDATER_DOWNLOADER_DOWNLOADED_FILE_PREFIX);
                strcat(downloadedFilePath, ARUPDATER_DOWNLOADER_PLF_FILENAME);
                fprintf(stderr, "dl path : %s\n", downloadedFilePath);
                fprintf(stderr, "need to download : %s | md5 : %s\n", downloadUrl, md5);
                
                // explode the download url into server and endUrl
                if (strncmp(downloadUrl, ARUPDATER_DOWNLOADER_HTTP_HEADER, strlen(ARUPDATER_DOWNLOADER_HTTP_HEADER)) != 0)
                {
                    error = ARUPDATER_ERROR_DOWNLOADER_PHP_ERROR;
                }
                
                // init the request semaphore
                if (error == ARUPDATER_OK)
                {
                    int resultSys = ARSAL_Sem_Init(&manager->downloader->dlSem, 0, 0);
                    if (resultSys != 0)
                    {
                        error = ARUPDATER_ERROR_SYSTEM;
                    }
                }
                
                // download the file
                if (error == ARUPDATER_OK)
                {
                    char *urlWithoutHttpHeader = downloadUrl + strlen(ARUPDATER_DOWNLOADER_HTTP_HEADER);
                    const char delimiter = '/';
                    
                    downloadEndUrl = strchr(urlWithoutHttpHeader, delimiter);
                    int serverLength = strlen(urlWithoutHttpHeader) - strlen(downloadEndUrl);
                    downloadServer = malloc(serverLength + 1);
                    strncpy(downloadServer, urlWithoutHttpHeader, serverLength);
                    
                    dlConnection = ARUTILS_Http_Connection_New(&manager->downloader->dlSem, downloadServer, 80, HTTPS_PROTOCOL_FALSE, NULL, NULL, &utilsError);
                    if (utilsError != ARUTILS_OK)
                    {
                        ARUTILS_Http_Connection_Delete(&dlConnection);
                        ARSAL_Sem_Destroy(&manager->downloader->dlSem);
                        error = ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR;
                    }
                }
                
                if (error == ARUPDATER_OK)
                {
                    utilsError = ARUTILS_Http_Get(dlConnection, downloadEndUrl, downloadedFilePath, manager->downloader->plfDownloadProgressCallback, manager->downloader->progressArg);
                    if (utilsError != ARUTILS_OK)
                    {
                        error = ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR;
                        fprintf(stderr, "error : %i\n", utilsError);
                    }
                }
                
                if (error == ARUPDATER_OK)
                {
                    utilsError = ARUTILS_Http_Connection_Cancel(dlConnection);
                    if (utilsError != ARUTILS_OK)
                    {
                        error = ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR;
                    }
                }
                
                if (error == ARUPDATER_OK)
                {
                    ARUTILS_Http_Connection_Delete(&dlConnection);
                    ARSAL_Sem_Destroy(&manager->downloader->dlSem);
                }
                
                // if no error, check the md5
                if (error == ARUPDATER_OK)
                {
                    /*// TODO: check md5
                    crypto_hash_t *md5=crypto_hash_md5_new();
                    
                    char line[ARUPDATER_DOWNLOADER_CHUNK_SIZE];
                    uint8_t *test = malloc(16);
                    FILE* file = fopen(downloadedFilePath, "r");
                    while (fread(line, ARUPDATER_DOWNLOADER_CHUNK_SIZE, 1, file) != NULL)
                    {
                        fprintf(stderr, "%i\n", strlen(line));
                        md5->push_data(md5, (uint8_t*)line, ARUPDATER_DOWNLOADER_CHUNK_SIZE);
                    }
                    md5->get_hash(md5, test, 128/8);
                    md5->destroy(md5);
                    fprintf(stderr, "md5 = %x\n", test);*/
                }
                
                if (error == ARUPDATER_OK)
                {
                    if (rename(downloadedFilePath, filePath) != 0)
                    {
                        error = ARUPDATER_ERROR_DOWNLOADER_RENAME_FILE;
                    }
                }
                
                free(downloadServer);
                free(downloadedFilePath);
                
                if (manager->downloader->plfDownloadCompletionCallback != NULL)
                {
                    manager->downloader->plfDownloadCompletionCallback(manager->downloader->completionArg, error);
                }
            }
            else
            {
                error = ARUPDATER_ERROR_DOWNLOADER_PHP_ERROR;
            }
        }
    
        free(deviceFolder);
        free(filePath);
        free(device);
        free(dataPtr);
        product++;
    }

    return error;
}

eARUPDATER_ERROR ARUPDATER_Downloader_CancelThread(ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    int resultSys = 0;
    
    if (manager == NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if ((error == ARUPDATER_OK) && (manager->downloader == NULL))
    {
        error = ARUPDATER_ERROR_NOT_INITIALIZED;
    }
    
    if (error == ARUPDATER_OK)
    {
        manager->downloader->isCanceled = 1;
        
        resultSys = ARSAL_Sem_Post(&manager->downloader->requestSem);
        resultSys = ARSAL_Sem_Post(&manager->downloader->dlSem);
        
        if (resultSys != 0)
        {
            error = ARUPDATER_ERROR_SYSTEM;
        }
    }
    
    return error;
}

