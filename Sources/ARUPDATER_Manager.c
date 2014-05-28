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
            ARUPDATER_PlfSender_Delete(&(manager->plfSender));
            
            free(manager->plfFolder);
            
            free(manager);
            *managerPtrAddr = NULL;
        }
    }
}

eARUPDATER_ERROR ARUPDATER_Manager_GetPlfVersion(ARUPDATER_Manager_t *manager, const char *const plfFileName, int *version, int *edition, int *extension)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if ((manager != NULL) &&
        (plfFileName != NULL) &&
        (version != NULL) &&
        (edition != NULL) &&
        (extension != NULL))
    {
        char *filePath = malloc(strlen(manager->plfFolder) + strlen(plfFileName) + 1);
        strcpy(filePath, manager->plfFolder);
        strcat(filePath, plfFileName);
        
        error = ARUPDATER_Utils_GetPlfVersion(filePath, version, edition, extension);
    }
    
    return error;
}

