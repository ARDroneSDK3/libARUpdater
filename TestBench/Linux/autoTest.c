/**
 * @file autoTest.c
 * @brief libARUpdater TestBench automatic
 * @date 05/23/2014
 * @author djava.bertrand@parrot.com
 */

/*****************************************
 *
 *             include file :
 *
 *****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <libARUpdater/ARUpdater.h>
#include <libARDiscovery/ARDISCOVERY_Discovery.h>
#include <libARSAL/ARSAL.h>

/* ****************************************
 *
 *             define :
 *
 **************************************** */

#define AUTOTEST_TAG "Autotest"


/* ****************************************
 *
 *           variable declarations :
 *
 **************************************** */


/* ****************************************
 *
 *           function declarations :
 *
 **************************************** */




/*****************************************
 *
 *          implementation :
 *
 *****************************************/

void test_http_progress_callback(void* arg, float percent)
{
    char *message = (char *)arg;
    if ((percent == 5) ||
        (percent == 20) ||
        (percent == 50) ||
        (percent == 80) ||
        (percent == 99))
    {
        printf("%s %f%%\n", message ? message : "null", percent);
    }
}

void test_http_should_download_callback(void* arg, int nbDownload, eARUPDATER_ERROR error)
{
    if (error == ARUPDATER_OK)
    {
        if (nbDownload > 0)
        {
            fprintf(stderr, "I should download %i plf file\n", nbDownload);
        }
        else
        {
            fprintf(stderr, "Plf File is up to date\n");
        }
    }
    else if(error == ARUPDATER_ERROR_DOWNLOADER_PHP_APP_OUT_TO_DATE_ERROR)
    {
        fprintf(stderr, "This version is out of date\n");
    }
}

void test_willDownloadPlfCallback(void* arg, eARDISCOVERY_PRODUCT product, const char *const remotePlfVersion)
{
    fprintf(stderr, "Download will start for product : %i (version : %s)\n",product, remotePlfVersion);
}

void test_http_download_completion_callback(void* arg, eARUPDATER_ERROR error)
{
    if (error == ARUPDATER_OK)
    {
        fprintf(stderr, "Download went well\n");
    }
    else
    {
        fprintf(stderr, "Download error !!\n");
    }
}

void test_ftp_upload_callback(void* arg, eARUPDATER_ERROR error)
{
    if (error == ARDATATRANSFER_OK)
    {
        fprintf(stderr, "Upload went well\n");
    }
    else
    {
        fprintf(stderr, "Upload error !!\n");
    }
}

int main(int argc, char *argv[])
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    eARSAL_ERROR arsalError = ARSAL_OK;
    ARUPDATER_Manager_t *manager = NULL;
    
    // this md5Manager is useless with Unix target
    ARSAL_MD5_Manager_t* md5Manager = ARSAL_MD5_Manager_New(&arsalError);
    fprintf(stderr, "****** Watch out, This autotest can't succeed on Unix because md5 checksum is not implemented ****\n");
    if (arsalError != ARSAL_OK)
    {
        error = ARUPDATER_ERROR_SYSTEM;
    }
    
    if (error == ARUPDATER_OK)
    {
        manager = ARUPDATER_Manager_New(&error);
    }
    
    if(error == ARUPDATER_OK)
    {
        error =  ARUPDATER_Downloader_New(manager, "./test", md5Manager, ARUPDATER_DOWNLOADER_ANDROID_PLATFORM, "3.0.1",test_http_should_download_callback, NULL, test_willDownloadPlfCallback, NULL, test_http_progress_callback, "test : ", test_http_download_completion_callback, NULL);
        
        if (error == ARUPDATER_OK)
        {
            error = (eARUPDATER_ERROR)ARUPDATER_Downloader_ThreadRun(manager);
        }
        
        fprintf(stderr, "Download finish, uploading now \n");
        if (error == ARUPDATER_OK)
        {
            eARUTILS_ERROR ftpError = ARUTILS_OK;
            ARUTILS_Manager_t *ftpManager = ARUTILS_Manager_New(&ftpError);
            
            if(ftpError == ARUTILS_OK)
            {
                ftpError = ARUTILS_Manager_InitWifiFtp(ftpManager, "172.20.5.48", 21, "", "");
            }
            
            if(ftpError == ARUTILS_OK)
            {
                error = ARUPDATER_Uploader_New(manager, "./test", ftpManager, md5Manager, ARDISCOVERY_PRODUCT_MINIDRONE, test_http_progress_callback, "prog : ", test_ftp_upload_callback, NULL);
            }
            else
            {
                error = ARUPDATER_ERROR_UPLOADER_ARUTILS_ERROR;
            }
        }
        
        if (error == ARUPDATER_OK)
        {
            error = (eARUPDATER_ERROR)ARUPDATER_Uploader_ThreadRun(manager);
        }
        
        ARUPDATER_Downloader_Delete(manager);
        ARUPDATER_Uploader_Delete(manager);
        ARUPDATER_Manager_Delete(&manager);
    }
    else if(manager != NULL)
    {
        // if there was an error in creating a new manager
        ARUPDATER_Manager_Delete(&manager);
    }
    
    ARSAL_MD5_Manager_Delete(&md5Manager);
    
    fprintf(stderr, "Sum up : %s\n", ARUPDATER_Error_ToString(error));
    
    return 0;
}
