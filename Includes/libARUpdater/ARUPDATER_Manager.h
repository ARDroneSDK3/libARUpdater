/**
 * @file ARUPDATER_Manager.h
 * @brief libARUpdater Manager header file.
 * @date 07/01/2014
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUPDATER_MANAGER_H_
#define _ARUPDATER_MANAGER_H_

#include <libARUpdater/ARUPDATER_Updater.h>
#include <libARUpdater/ARUPDATER_Error.h>

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
ARUPDATER_Manager_t* ARUPDATER_Manager_New(const char *const plfFolder, eARUPDATER_ERROR *error);

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

/**
 * @brief Set the variable before checking the locale version
 * @param manager : pointer on the manager
 * @param[in] device : the device codename
 * @param[in] plfFileName : the file name of the plf
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 */
eARUPDATER_ERROR ARUPDATER_Manager_PrepareCheckLocaleVersion(ARUPDATER_Manager_t *manager, const char *const device, const char *const plfFileName, ARUPDATER_Updater_ShouldDownloadPlfCallback_t shouldDownloadCallback, ARUPDATER_Updater_PlfDownloadProgressCallback_t progressCallback, ARUPDATER_Updater_PlfDownloadCompletionCallback_t completionCallback);

/**
 * @brief Manage the update of the plf file
 * @pre need to call ARUPDATER_Manager_PrepareCheckLocaleVersion before
 * @warning This function must be called in its own thread.
 * @post ARUPDATER_Manager_CancelThread() must be called.
 * @param managerArg : thread data of type ARUPDATER_Manager_t*
 * @return ARUPDATER_OK if operation went well, a descirption of the error otherwise
 * @see ARUPDATER_Manager_CancelThread()
 */
eARUPDATER_ERROR ARUPDATER_Manager_CheckLocaleVersionThreadRun (void *managerArg);

/**
 * @brief cancel the update
 * @details Used to kill the thread calling ARUPDATER_Manager_ThreadRun().
 * @param manager pointer on the manager
 * @see ARUPDATER_Manager_ThreadRun()
 */
eARUPDATER_ERROR ARUPDATER_Manager_CancelThread (ARUPDATER_Manager_t *manager);

#endif /* _ARUPDATER_MANAGER_H_ */


