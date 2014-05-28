//
//  ARUPDATER_Updater.h
//  ARUpdaterCLibProject
//
//  Created by Djavan Bertrand on 23/05/2014.
//
//

#ifndef _ARUPDATER_DOWNLOADER_PRIVATE_H_
#define _ARUPDATER_DOWNLOADER_PRIVATE_H_

#include <libARUpdater/ARUPDATER_Error.h>
#include <libARUpdater/ARUPDATER_Downloader.h>

struct ARUPDATER_Downloader_t
{
    char *plfFileName;
    char *plfFolder;
    char *device;
    
    void *downloadArg;
    void *progressArg;
    void *completionArg;
    
    int isRunning;
    int isCanceled;
    
    ARSAL_Sem_t requestSem;
    ARSAL_Sem_t dlSem;
    
    ARUPDATER_Downloader_ShouldDownloadPlfCallback_t shouldDownloadCallback;
    ARUPDATER_Downloader_PlfDownloadProgressCallback_t plfDownloadProgressCallback;
    ARUPDATER_Downloader_PlfDownloadCompletionCallback_t plfDownloadCompletionCallback;
};


#endif
