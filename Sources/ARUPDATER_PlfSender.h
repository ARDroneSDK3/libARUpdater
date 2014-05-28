/**
 * @file ARUPDATER_Updater.c
 * @brief libARUpdater PlfSender c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_PLFSENDER_PRIVATE_H_
#define _ARUPDATER_PLFSENDER_PRIVATE_H_

#include <libARUpdater/ARUPDATER_Error.h>
#include <libARUpdater/ARUPDATER_PlfSender.h>
#include <libARDataTransfer/ARDataTransfer.h>
#include <libARDataTransfer/ARDATATRANSFER_Uploader.h>

typedef struct ARUPDATER_PlfSender_t
{
    char *plfFolder;
    
    char *plfFileName;
    
    ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback;
    void *progressArg;
    ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback;
    void *completionArg;
    
}ARUPDATER_PlfSender_t;

/**
 * @brief Create a new plf Sender
 * @warning This function allocate memory
 * @post ARUPDATER_PlfSender_Delete() must be called to delete the plf sender and free the memory allocated.
 * @param[out] error : pointer on the error output.
 * @return Pointer on the new plf sender
 * @see ARUPDATER_PlfSender_Delete()
 */
ARUPDATER_PlfSender_t* ARUPDATER_PlfSender_New(eARUPDATER_ERROR *error);

/**
 * @brief Delete the plf sender
 * @warning This function free memory
 * @param plfSender : address of the pointer on the plfSender
 * @see ARUPDATER_PlfSender_New()
 */
void ARUPDATER_PlfSender_Delete(ARUPDATER_PlfSender_t **plfSender);

//TODO:
eARUPDATER_ERROR ARUPDATER_PlfSender_PrepareSendToDrone(ARUPDATER_PlfSender_t *plfSender, const char *const plfFolder, const char *const plfFileName, ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback, void *progressArg, ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback, void *completionArg);

/**
 * @brief Send the plf file on the smartphone/tablet and send it to the drone
 * @warning This function must be called in its own thread.
 * @post ARUPDATER_PlfSender_CancelThread() must be called after.
 * @param plfSenderArg : thread data of type ARUPDATER_PlfSender_t*
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 * @see ARUPDATER_PlfSender_CancelThread()
 */
eARUPDATER_ERROR ARUPDATER_PlfSender_SendToDroneThreadRun(void *plfSenderArg);

/**
 * @brief cancel the send
 * @details Used to kill the thread calling ARUPDATER_PlfSender_SendToDroneThreadRun().
 * @param plfSender : pointer on the plf sender
 * @see ARUPDATER_PlfSender_SendToDroneThreadRun()
 */
eARUPDATER_ERROR ARUPDATER_PlfSender_CancelThread(ARUPDATER_PlfSender_t *plfSender);
#endif
