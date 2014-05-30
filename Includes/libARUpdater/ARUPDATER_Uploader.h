/**
 * @file ARUPDATER_Uploader.h
 * @brief libARUpdater Uploader header file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_UPLOADER_H_
#define _ARUPDATER_UPLOADER_H_

#include <libARUpdater/ARUPDATER_Manager.h>
#include <libARDiscovery/ARDISCOVERY_Discovery.h>
#include <libARDataTransfer/ARDATATRANSFER_Uploader.h>


typedef struct ARUPDATER_Uploader_t ARUPDATER_Uploader_t;

/**
 * @brief Create an object to upload a plf file
 * @warning this function allocates memory
 * @post ARUPDATER_Uploader_Delete should be called
 * @param manager : pointer on the manager
 * @param[in] rootFolder : root folder
 * @param[in] ftpManager : ftp manager initialized with the correct network (wifi or ble)
 * @param[in] product : enumerator on the enum
 * @param[in] progressCallback : callback which tells the progress of the download
 * @param[in|out] progressArg : arg given to the progressCallback
 * @param[in] completionCallback : callback which tells when the download is completed
 * @param[in|out] completionArg : arg given to the completionCallback
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 * @see ARUPDATER_Uploader_Delete()
 */
eARUPDATER_ERROR ARUPDATER_Uploader_New(ARUPDATER_Manager_t* manager, const char *const rootFolder, ARUTILS_Manager_t *ftpManager, eARDISCOVERY_PRODUCT product, ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback, void *progressArg, ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback, void *completionArg);


/**
 * @brief Delete the Uploader of the Manager
 * @warning This function frees memory
 * @param manager a pointer on the ARUpdater Manager
 * @see ARUPDATER_Uploader_New ()
 */
eARUPDATER_ERROR ARUPDATER_Uploader_Delete(ARUPDATER_Manager_t *manager);

/**
 * @brief Upload a plf
 * @warning This function must be called in its own thread.
 * @post ARUPDATER_Uploader_CancelThread() must be called after.
 * @param managerArg : thread data of type ARUPDATER_Manager_t*
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 * @see ARUPDATER_Uploader_CancelThread()
 */
void* ARUPDATER_Uploader_ThreadRun(void *managerArg);

/**
 * @brief cancel the upload
 * @details Used to kill the thread calling ARUPDATER_Uploader_ThreadRun().
 * @param manager : pointer on the manager
 * @see ARUPDATER_Uploader_ThreadRun()
 */
eARUPDATER_ERROR ARUPDATER_Uploader_CancelThread(ARUPDATER_Manager_t *manager);

#endif
