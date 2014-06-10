/**
 * @file ARUPDATER_Updater.c
 * @brief libARUpdater PlfSender c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_UPLOADER_PRIVATE_H_
#define _ARUPDATER_UPLOADER_PRIVATE_H_

#include <libARUpdater/ARUPDATER_Uploader.h>
#include <libARDataTransfer/ARDATATRANSFER_Uploader.h>
#include <libARDataTransfer/ARDATATRANSFER_Downloader.h>
#include <libARSAL/ARSAL_Mutex.h>

struct ARUPDATER_Uploader_t
{
    char *rootFolder;
    eARDISCOVERY_PRODUCT product;
    ARUTILS_Manager_t *ftpManager;
    
    int isRunning;
    int isCanceled;
    int isUploadThreadRunning;
    int isDownloadMd5ThreadRunning;
    
    ARSAL_MD5_Manager_t *md5Manager;
    
    ARSAL_Mutex_t uploadLock;
    
    ARDATATRANSFER_Manager_t* dataTransferManager;
    
    ARUPDATER_Uploader_PlfUploadProgressCallback_t progressCallback;
    ARUPDATER_Uploader_PlfUploadCompletionCallback_t completionCallback;
    void *progressArg;
    void *completionArg;
    
    eARDATATRANSFER_ERROR uploadError;
    
};

void ARUPDATER_Uploader_ProgressCallback(void* arg, uint8_t percent);
void ARUPDATER_Uploader_CompletionCallback(void* arg, eARDATATRANSFER_ERROR error);

#endif
