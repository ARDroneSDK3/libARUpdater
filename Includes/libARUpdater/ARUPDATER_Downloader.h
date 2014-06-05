/**
 * @file ARUPDATER_Downloader.h
 * @brief libARUpdater Downloader header file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_DOWNLOADER_H_
#define _ARUPDATER_DOWNLOADER_H_

#include <libARUpdater/ARUPDATER_Manager.h>
#include <libARSAL/ARSAL_MD5_Manager.h>

typedef struct ARUPDATER_Downloader_t ARUPDATER_Downloader_t;

/**
 * @brief Whether the plf file should be updated or not
 * @param arg The pointer of the user custom argument
 * @param shouldDownload 1 if the plf file should be downloaded, 0 otherwise
 */
typedef void (*ARUPDATER_Downloader_ShouldDownloadPlfCallback_t) (void* arg, int shouldDownload);

/**
 * @brief Progress callback of the plf download
 * @param arg The pointer of the user custom argument
 * @param percent The percent size of the plf file already downloaded
 * @see ARUPDATER_Manager_CheckLocaleVersionThreadRun ()
 */
typedef void (*ARUPDATER_Downloader_PlfDownloadProgressCallback_t) (void* arg, uint8_t percent);

/**
 * @brief Completion callback of the Media download
 * @param arg The pointer of the user custom argument
 * @param media The media to be downloaded
 * @param error The error status to indicate the plf downloaded status
 * @see ARUPDATER_Manager_CheckLocaleVersionThreadRun ()
 */
typedef void (*ARUPDATER_Downloader_PlfDownloadCompletionCallback_t) (void* arg, eARUPDATER_ERROR error);

/**
 * @brief Create an object to download all plf files
 * @warning this function allocates memory
 * @pre rootFolder should already exist
 * @post ARUPDATER_Downloader_Delete should be called
 * @param manager : pointer on the manager
 * @param[in] rootFolder : root folder
 * @param[in] md5Manager : md5 manager
 * @param[in] shouldDownloadCallback : callback which tells if the plf will be downloaded or not
 * @param[in|out] downloadArg : arg given to the shouldDownloadCallback
 * @param[in] progressCallback : callback which tells the progress of the download
 * @param[in|out] progressArg : arg given to the progressCallback
 * @param[in] completionCallback : callback which tells when the download is completed
 * @param[in|out] completionArg : arg given to the completionCallback
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 * @see ARUPDATER_Downloader_Delete()
 */
eARUPDATER_ERROR ARUPDATER_Downloader_New(ARUPDATER_Manager_t* manager, const char *const rootFolder, ARSAL_MD5_Manager_t *md5Manager, ARUPDATER_Downloader_ShouldDownloadPlfCallback_t shouldDownloadCallback, void *downloadArg, ARUPDATER_Downloader_PlfDownloadProgressCallback_t progressCallback, void *progressArg, ARUPDATER_Downloader_PlfDownloadCompletionCallback_t completionCallback, void *completionArg);

/**
 * @brief Delete the Downloader of the Manager
 * @warning This function frees memory
 * @param manager a pointer on the ARUpdater Manager
 * @see ARUPDATER_Downloader_New ()
 */
eARUPDATER_ERROR ARUPDATER_Downloader_Delete(ARUPDATER_Manager_t *manager);

/**
 * @brief Download all plf if needed
 * @warning This function must be called in its own thread.
 * @post ARUPDATER_Downloader_CancelThread() must be called after.
 * @param managerArg : thread data of type ARUPDATER_Manager_t*
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 * @see ARUPDATER_Downloader_CancelThread()
 */
void* ARUPDATER_Downloader_ThreadRun(void *managerArg);

/**
 * @brief cancel the update
 * @details Used to kill the thread calling ARUPDATER_Downloader_ThreadRun().
 * @param manager : pointer on the manager
 * @see ARUPDATER_Downloader_ThreadRun()
 */
eARUPDATER_ERROR ARUPDATER_Downloader_CancelThread(ARUPDATER_Manager_t *manager);
#endif
