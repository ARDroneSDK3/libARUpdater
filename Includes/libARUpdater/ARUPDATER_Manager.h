/**
 * @file ARUPDATER_Manager.h
 * @brief libARUpdater Manager header file.
 * @date 07/01/2014
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUPDATER_MANAGER_H_
#define _ARUPDATER_MANAGER_H_

#include <libARUpdater/ARUPDATER_Error.h>
#include <libARDataTransfer/ARDataTransfer.h>
#include <libARDataTransfer/ARDATATRANSFER_Uploader.h>
#include <libARDiscovery/ARDISCOVERY_Discovery.h>

/**
 * @brief Manager structure
 * @see ARUPDATER_Manager_New ()
 */
typedef struct ARUPDATER_Manager_t ARUPDATER_Manager_t;

/**
 * @brief Create a new ARUpdater Manager
 * @warning This function allocates memory
 * @param[out] error A pointer on the error output. Can be null
 * @return Pointer on the new ARUpdater Manager
 * @see ARUPDATER_Manager_Delete ()
 */
ARUPDATER_Manager_t* ARUPDATER_Manager_New(eARUPDATER_ERROR *error);

/**
 * @brief Delete an ARUpdater Manager
 * @warning This function frees memory
 * @param manager The address of the pointer on the ARUpdater Manager
 * @see ARUPDATER_Manager_New ()
 */
void ARUPDATER_Manager_Delete (ARUPDATER_Manager_t **managerPtrAddr);

/**
 * @brief get the version of a given plf file
 * @param manager : pointer on the manager
 * @param[in] rootFolder : root folder of the plf
 * @param[in] product : the plf of the product to be tested
 * @param[in] version : the version of the remote plf
 * @param[in] edition : the edition of the remote plf
 * @param[in] extension : the extension of the remote plf
 * @param[out] error : ARUPDATER_OK if operation went well, the description of the error otherwise. Can be null
 * @return 1 if the product plf is at the same version (or more recent) as the local plf
 */
int ARUPDATER_Manager_PlfVersionIsUpToDate(ARUPDATER_Manager_t *manager, const char *const rootFolder, eARDISCOVERY_PRODUCT product, int version, int edition, int extension, eARUPDATER_ERROR *error);

#endif /* _ARUPDATER_MANAGER_H_ */


