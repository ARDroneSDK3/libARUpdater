/**
 * @file ARUPDATER_Updater.c
 * @brief libARUpdater PlfSender c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_UPLOADER_PRIVATE_H_
#define _ARUPDATER_UPLOADER_PRIVATE_H_

#include <libARUpdater/ARUPDATER_Uploader.h>
#include <libARSAL/ARSAL_Mutex.h>

struct ARUPDATER_Uploader_t
{
    char *rootFolder;
    eARDISCOVERY_PRODUCT product;
    ARUTILS_Manager_t *ftpManager;
    
    int isRunning;
    int isCanceled;
    int isUploadThreadRunning;
    
    ARSAL_Mutex_t uploadLock;
    
    ARDATATRANSFER_Manager_t* dataTransferManager;
    
    ARDATATRANSFER_Uploader_ProgressCallback_t progressCallback;
    ARDATATRANSFER_Uploader_CompletionCallback_t completionCallback;
    void *progressArg;
    void *completionArg;
    
};

#endif
