//
//  ARUPDATER_Updater.h
//  ARUpdaterCLibProject
//
//  Created by Djavan Bertrand on 23/05/2014.
//
//

#ifndef _ARUPDATER_UPDATER_PRIVATE_H_
#define _ARUPDATER_UPDATER_PRIVATE_H_

#include <libARUpdater/ARUPDATER_Error.h>
#include <libARUpdater/ARUPDATER_Updater.h>
#include <libARUpdater/ARUPDATER_Manager.h>
#include "ARUPDATER_Plf.h"

typedef struct ARUPDATER_Updater_t
{
    char *plfFileName;
    char *plfFolder;
    char *device;
    ARUPDATER_Updater_ShouldDownloadPlfCallback_t shouldDownloadCallback;
    ARUPDATER_Updater_PlfDownloadProgressCallback_t plfDownloadProgressCallback;
    ARUPDATER_Updater_PlfDownloadCompletionCallback_t plfDownloadCompletionCallback;
}ARUPDATER_Updater_t;

/**
 * @brief Create a new Updater
 * @warning This function allocate memory
 * @post ARUPDATER_Updater_Delete() must be called to delete the updater and free the memory allocated.
 * @param[in] the file path of the plf file
 * @param[out] error : pointer on the error output.
 * @return Pointer on the new udpater
 * @see ARUPDATER_Updater_Delete()
 */
ARUPDATER_Updater_t* ARUPDATER_Updater_New(eARUPDATER_ERROR *error);

/**
 * @brief Delete the udpater
 * @warning This function free memory
 * @param updater : address of the pointer on the updater
 * @see ARUPDATER_Updater_New()
 */
void ARUPDATER_Updater_Delete(ARUPDATER_Updater_t **updater);

/**
 * @brief Set the variable before checking the locale version
 * @param updater : pointer on the updater
 * @param[in] device : the device codename
 * @param[in] plfFolder : the folder where is stored the plfFile
 * @param[in] plfFileName : the file name of the plf
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 */
eARUPDATER_ERROR ARUPDATER_Updater_PrepareCheckLocaleVersion(ARUPDATER_Updater_t *updater, const char *const device, const char *const plfFolder, const char *const plfFileName, ARUPDATER_Updater_ShouldDownloadPlfCallback_t shouldDownloadCallback, ARUPDATER_Updater_PlfDownloadProgressCallback_t progressCallback, ARUPDATER_Updater_PlfDownloadCompletionCallback_t completionCallback);

/**
 * @brief Update of the plf file if needed
 * @warning This function must be called in its own thread.
 * @post Before join the thread calling this function, ARUPDATER_Updater_CancelThread() must be called.
 * @param updaterArg : thread data of type ARUPDATER_Updater_t*
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 * @see ARUPDATER_Updater_CancelThread()
 */
eARUPDATER_ERROR ARUPDATER_Updater_CheckLocaleVersionThreadRun(void *updaterArg);

/**
 * @brief cancel the update
 * @details Used to kill the thread calling ARUPDATER_Updater_ThreadRun().
 * @param udpater : pointer on the updater
 * @see ARUPDATER_Updater_ThreadRun()
 */
eARUPDATER_ERROR ARUPDATER_Updater_CancelThread(ARUPDATER_Updater_t *updater);

#endif
