/**
 * @file ARUPDATER_Manager.c
 * @brief libARUpdater Manager c file.
 * @date07/01/2014
 * @author david.flattin.ext@parrot.com
 **/
#include <stdlib.h>
#include <stdio.h>
#include <libARSAL/ARSAL_Print.h>

#include <libARUpdater/ARUPDATER_Error.h>
#include <libARUpdater/ARUPDATER_Manager.h>
#include "ARUPDATER_Manager.h"
#include "ARUPDATER_Utils.h"

#define ARUPDATER_MANAGER_TAG   "ARUPDATER_Manager"


ARUPDATER_Manager_t* ARUPDATER_Manager_New(eARUPDATER_ERROR *error)
{
    ARUPDATER_Manager_t *manager = NULL;
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    /* Check parameters */
    if(err == ARUPDATER_OK)
    {
        /* Create the Manager */
        manager = malloc (sizeof (ARUPDATER_Manager_t));
        if (manager == NULL)
        {
            err = ARUPDATER_ERROR_ALLOC;
        }
    }

    /* Initialize to default values */
    if (ARUPDATER_OK == err)
    {
        manager->downloader = NULL;
        manager->uploader = NULL;
    }
    
    /* delete the Manager if an error occurred */
    if (err != ARUPDATER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_MANAGER_TAG, "error: %s", ARUPDATER_Error_ToString (err));
        ARUPDATER_Manager_Delete (&manager);
    }
    
    /* return the error */
    if (error != NULL)
    {
        *error = err;
    }
    
    return manager;
}

void ARUPDATER_Manager_Delete (ARUPDATER_Manager_t **managerPtrAddr)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, "     ");

    if (managerPtrAddr != NULL)
    {
        ARUPDATER_Manager_t *manager = *managerPtrAddr;
        
        if (manager != NULL)
        {
            if (manager->downloader != NULL)
            {
                ARUPDATER_Downloader_Delete(manager);
            }
            
            if (manager->uploader != NULL)
            {
                ARUPDATER_Uploader_Delete(manager);
            }
                        
            free(manager);
            *managerPtrAddr = NULL;
        }
    }
}

int ARUPDATER_Manager_PlfVersionIsUpToDate(ARUPDATER_Manager_t *manager, const char *const rootFolder, eARDISCOVERY_PRODUCT product, int version, int edition, int extension, eARUPDATER_ERROR *error)
{
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    int sourceVersion, sourceEdition, sourceExtension;
    int retVal = 1;
    
    if ((manager == NULL) ||
        (rootFolder == NULL))
    {
        err = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (err == ARUPDATER_OK)
    {
        uint16_t productId = ARDISCOVERY_getProductID(product);
        char *device = malloc(ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE);
        snprintf(device, ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
        
        char *sourceFilePath = malloc(strlen(rootFolder) + strlen(ARUPDATER_MANAGER_PLF_FOLDER) + strlen(device) + strlen(ARUPDATER_MANAGER_FOLDER_SEPARATOR) + strlen(ARUPDATER_MANAGER_PLF_FILENAME) + 1);
        strcpy(sourceFilePath, rootFolder);
        strcat(sourceFilePath, ARUPDATER_MANAGER_PLF_FOLDER);
        strcat(sourceFilePath, device);
        strcat(sourceFilePath, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        strcat(sourceFilePath, ARUPDATER_MANAGER_PLF_FILENAME);

        err = ARUPDATER_Utils_GetPlfVersion(sourceFilePath, &sourceVersion, &sourceEdition, &sourceExtension);
        
        free(sourceFilePath);
        free(device);
    }
    
    if (err == ARUPDATER_OK)
    {
        if (sourceVersion > version)
        {
            retVal = 0;
        }
        else if((sourceVersion == version) && (sourceEdition > edition))
        {
            retVal = 0;
        }
        else if((sourceVersion == version) && (sourceEdition == edition) && (sourceExtension > extension))
        {
            retVal = 0;
        }
    }
    
    if (error != NULL)
    {
        *error = err;
    }
    
    return retVal;
}

