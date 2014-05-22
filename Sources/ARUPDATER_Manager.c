/**
 * @file ARUPDATER_Manager.c
 * @brief libARUpdater Manager c file.
 * @date07/01/2014
 * @author david.flattin.ext@parrot.com
 **/
#include <stdlib.h>
#include <libARSAL/ARSAL_Print.h>

#include "libARUpdater/ARUPDATER_Error.h"
#include "libARUpdater/ARUPDATER_Manager.h"
#include "ARUPDATER_Manager.h"

#define ARUPDATER_MANAGER_TAG   "Updater"

//ARUTILS_BLEDevice_t device, const char* deviceIP, int port
ARUPDATER_Manager_t* ARUPDATER_Manager_New (eARUPDATER_ERROR *error)
{
    ARUPDATER_Manager_t *newManager = NULL;
    eARUPDATER_ERROR result = 0;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, "");

    newManager = calloc(1, sizeof(ARUPDATER_Manager_t));

    if (newManager == NULL)
    {
        result = ARUPDATER_ERROR_ALLOC;
    }

    if (result != ARUPDATER_OK)
    {
        ARUPDATER_Manager_Delete(&newManager);
    }

    *error = result;
    return newManager;
}


void ARUPDATER_Manager_Delete (ARUPDATER_Manager_t **managerPtrAddr)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, "");

    if (managerPtrAddr != NULL)
    {
        ARUPDATER_Manager_t *manager = *managerPtrAddr;
        
        if (manager != NULL)
        {
            free(manager);
            *managerPtrAddr = NULL;
        }
    }
}

eARUPDATER_ERROR ARUPDATER_Manager_ThreadRun (void *managerArg)
{
    ARUPDATER_Manager_t *manager = (ARUPDATER_Manager_t*)managerArg;
    eARUPDATER_ERROR result = ARUPDATER_OK;
    
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, "");
    
    return result;
}

eARUPDATER_ERROR ARUPDATER_Manager_CancelThread (ARUPDATER_Manager_t *manager)
{
    eARUPDATER_ERROR result = ARUPDATER_OK;
    
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_MANAGER_TAG, "");
    
    return result;
}