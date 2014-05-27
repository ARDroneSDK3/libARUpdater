//
//  ARUPDATER_Updater.h
//  ARUpdaterCLibProject
//
//  Created by Djavan Bertrand on 27/05/2014.
//
//

#ifndef _ARUPDATER_UPDATER_H_
#define _ARUPDATER_UPDATER_H_

/**
 * @brief Whether the plf file should be updated or not
 * @param arg The pointer of the user custom argument
 * @param shouldDownload 1 if the plf file should be downloaded, 0 otherwise
 */
typedef void (*ARUPDATER_Updater_ShouldDownloadPlfCallback_t) (void* arg, int shouldDownload);

/**
 * @brief Progress callback of the plf download
 * @param arg The pointer of the user custom argument
 * @param percent The percent size of the plf file already downloaded
 * @see ARUPDATER_Manager_CheckLocaleVersionThreadRun ()
 */
typedef void (*ARUPDATER_Updater_PlfDownloadProgressCallback_t) (void* arg, uint8_t percent);

/**
 * @brief Completion callback of the Media download
 * @param arg The pointer of the user custom argument
 * @param media The media to be downloaded
 * @param error The error status to indicate the plf downloaded status
 * @see ARUPDATER_Manager_CheckLocaleVersionThreadRun ()
 */
typedef void (*ARUPDATER_Updater_PlfDownloadCompletionCallback_t) (void* arg, eARUPDATER_ERROR error);

#endif
