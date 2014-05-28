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


/**
 * @brief Manager structure
 * @see ARUPDATER_Manager_New ()
 */
typedef struct ARUPDATER_Manager_t ARUPDATER_Manager_t;

/**
 * @brief Create a new ARUpdater Manager
 * @warning This function allocates memory
 * @param[in] plfFolder : the folder where is stored the plfFile
 * @param[out] error A pointer on the error output
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
 * @param[in] plfFileName : file name of the plf file
 * @param[out] version : pointer on the version to be returned
 * @param[out] edition : pointer on the edition to be returned
 * @param[out] extension : pointer on the extension to be returned
 * @return ARUPDATER_OK if operation went well, the description of the error otherwise
 */
 eARUPDATER_ERROR ARUPDATER_Manager_GetPlfVersion(ARUPDATER_Manager_t *manager, const char *const plfFileName, int *version, int *edition, int *extension);

#endif /* _ARUPDATER_MANAGER_H_ */


