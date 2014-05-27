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
#include <libARDataTransfer/ARDATATRANSFER.h>
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
 * @param[in] shouldDownloadCallback : callback which tells if the plf will be downloaded or not
 * @param[in|out] downloadArg : arg given to the shouldDownloadCallback
 * @param[in] progressCallback : callback which tells the progress of the download
 * @param[in|out] progressArg : arg given to the progressCallback
 * @param[in] completionCallback : callback which tells when the download is completed
 * @param[in|out] completionArg : arg given to the completionCallback
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 */
eARUPDATER_ERROR ARUPDATER_Manager_PrepareCheckLocaleVersion(ARUPDATER_Manager_t *manager, const char *const device, const char *const plfFileName, ARUPDATER_Updater_ShouldDownloadPlfCallback_t shouldDownloadCallback, void *downloadArg, ARUPDATER_Updater_PlfDownloadProgressCallback_t progressCallback, void *progressArg, ARUPDATER_Updater_PlfDownloadCompletionCallback_t completionCallback, void *completionArg);

/**
 * @brief Manage the update of the plf file
 * @pre need to call ARUPDATER_Manager_PrepareCheckLocaleVersion before
 * @warning This function must be called in its own thread.
 * @post ARUPDATER_Manager_CancelThread() must be called after.
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

// TODO: comment
eARUPDATER_ERROR ARUPDATER_Manager_PrepareSendToDrone(ARUPDATER_Manager_t *manager, const char *const plfFileName, ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback, void *progressArg, ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback, void *completionArg);

/**
 * @brief Manage the transfer of the plf file to the drone
 * @pre need to call ARUPDATER_PlfSender_PrepareSendToDrone before
 * @warning This function must be called in its own thread.
 * @post ARUPDATER_Manager_CancelThread() must be called after.
 * @param managerArg : thread data of type ARUPDATER_Manager_t*
 * @return ARUPDATER_OK if operation went well, a descirption of the error otherwise
 * @see ARUPDATER_Manager_CancelThread()
 */
eARUPDATER_ERROR ARUPDATER_Manager_SendToDroneThreadRun (void *managerArg);

/**
 * @brief cancel the update
 * @details Used to kill the thread calling ARUPDATER_Manager_ThreadRun().
 * @param manager pointer on the manager
 * @see ARUPDATER_Manager_ThreadRun()
 */
//eARUPDATER_ERROR ARUPDATER_Manager_CancelThread (ARUPDATER_Manager_t *manager);

#endif /* _ARUPDATER_MANAGER_H_ */


