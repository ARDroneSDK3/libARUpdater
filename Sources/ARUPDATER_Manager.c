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

#define ARUPDATER_MANAGER_VERSION_ELEMENT_BUFFER_SIZE   15
#define ARUPDATER_MANAGER_FULL_VERSION_BUFFER_SIZE      127
#define ARUPDATER_MANAGER_VERSION_SEPARATOR             "."

typedef struct ARUPDATER_Manager_PlfVersion
{
    eARDISCOVERY_PRODUCT product;
    int version;
    int edition;
    int extension;
} ARUPDATER_Manager_PlfVersion;

struct ARUPDATER_Manager_PlfVersion blackListedVersions[] = {
    // PUT HERE ALL BLACKLISTED VERSIONS
    // {product, version, edition, extension}
};

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

int ARUPDATER_Manager_PlfVersionIsUpToDate(ARUPDATER_Manager_t *manager, const char *const rootFolder, eARDISCOVERY_PRODUCT product, int version, int edition, int extension, const char *localVersionBuffer, int bufferSize, eARUPDATER_ERROR *error)
{
    eARUPDATER_ERROR err = ARUPDATER_OK;
    
    int sourceVersion, sourceEdition, sourceExtension;
    int retVal = 1;
    
    char *device = NULL;
    char *productFolder = NULL;
    char *plfFilename = NULL;
    char *sourceFilePath = NULL;
    
    if ((manager == NULL) ||
        (rootFolder == NULL))
    {
        err = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (err == ARUPDATER_OK)
    {
        uint16_t productId = ARDISCOVERY_getProductID(product);
        device = malloc(ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE);
        snprintf(device, ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE, "%04x", productId);
        
        int productFolderLength = strlen(rootFolder) + strlen(ARUPDATER_MANAGER_PLF_FOLDER) + strlen(device) + strlen(ARUPDATER_MANAGER_FOLDER_SEPARATOR) + 1;
        char *slash = strrchr(rootFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR[0]);
        if ((slash != NULL) && (strcmp(slash, ARUPDATER_MANAGER_FOLDER_SEPARATOR) != 0))
        {
            productFolderLength += 1;
        }
        productFolder = (char*) malloc(productFolderLength);
        strcpy(productFolder, rootFolder);
        
        if ((slash != NULL) && (strcmp(slash, ARUPDATER_MANAGER_FOLDER_SEPARATOR) != 0))
        {
            strcat(productFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        }
        strcat(productFolder, ARUPDATER_MANAGER_PLF_FOLDER);
        strcat(productFolder, device);
        strcat(productFolder, ARUPDATER_MANAGER_FOLDER_SEPARATOR);
        
        plfFilename = NULL;
        err = ARUPDATER_Utils_GetPlfInFolder(productFolder, &plfFilename);
    }
    
    if (err == ARUPDATER_OK)
    {
        sourceFilePath = malloc(strlen(productFolder) + strlen(plfFilename) + 1);
        strcpy(sourceFilePath, productFolder);
        strcat(sourceFilePath, plfFilename);

        err = ARUPDATER_Utils_GetPlfVersion(sourceFilePath, &sourceVersion, &sourceEdition, &sourceExtension);
    }
    
    if ((err == ARUPDATER_OK) && (localVersionBuffer != NULL))
    {
        char buffer[ARUPDATER_MANAGER_VERSION_ELEMENT_BUFFER_SIZE];
        char versionStrTmp[ARUPDATER_MANAGER_FULL_VERSION_BUFFER_SIZE];
        
        // version
        sprintf(buffer,"%i",sourceVersion);
        strcpy(versionStrTmp, buffer);
        strcat(versionStrTmp, ARUPDATER_MANAGER_VERSION_SEPARATOR);
        
        // edition
        sprintf(buffer,"%i",sourceEdition);
        strcat(versionStrTmp, buffer);
        strcat(versionStrTmp, ARUPDATER_MANAGER_VERSION_SEPARATOR);
        
        // extension
        sprintf(buffer,"%i",sourceExtension);
        strcat(versionStrTmp, buffer);
        
        if (strlen(versionStrTmp) <= bufferSize)
        {
            strcpy((char*)localVersionBuffer, versionStrTmp);
        }
        else
        {
            err = ARUPDATER_ERROR_MANAGER_BUFFER_TOO_SMALL;
        }
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
    
    if (productFolder)
    {
        free(productFolder);
    }
    if (sourceFilePath)
    {
        free(sourceFilePath);
    }
    if (device)
    {
        free(device);
    }
    if (plfFilename)
    {
        free(plfFilename);
    }
    
    if (error != NULL)
    {
        *error = err;
    }
    
    return retVal;
}

int ARUPDATER_Manager_PlfVersionIsBlacklisted(eARDISCOVERY_PRODUCT product, int version, int edition, int extension)
{
    int isBlackListed = 0;
    int nbBlackListedVersions = sizeof(blackListedVersions) / sizeof(ARUPDATER_Manager_PlfVersion);
    int i = 0;
    for (i = 0; i < nbBlackListedVersions; i++)
    {
        ARUPDATER_Manager_PlfVersion blackListedVersion = blackListedVersions[i];
        if ((product == blackListedVersion.product) &&
            (version == blackListedVersion.version) &&
            (edition == blackListedVersion.edition) &&
            (extension == blackListedVersion.extension))
        {
            isBlackListed = 1;
            break;
        }
    }
    
    return isBlackListed;
}
