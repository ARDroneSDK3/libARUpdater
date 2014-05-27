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

void test_http_progress_callback(void* arg, uint8_t percent)
{
    char *message = (char *)arg;
    
    printf("%s %d%%\n", message ? message : "null", percent);
}

void test_http_should_download_callback(void* arg, int shouldDownload)
{
    if (shouldDownload == 1)
    {
        fprintf(stderr, "I should download the plf file\n");
    }
    else
    {
        fprintf(stderr, "Plf File is up to date\n");
    }
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

void test_ftp_upload_callback(void* arg, eARDATATRANSFER_ERROR error)
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
    
    ARUPDATER_Manager_t *managerPtr = ARUPDATER_Manager_New("./test/", &error);
    if(error == ARUPDATER_OK)
    {
        error = ARUPDATER_Manager_PrepareCheckLocaleVersion(managerPtr, "0900", "delos_lucie_updater_payload.plf", test_http_should_download_callback, NULL, test_http_progress_callback, "test : ", test_http_download_completion_callback, NULL);
        
        if (error == ARUPDATER_OK)
        {
            // launch the thread
            error = ARUPDATER_Manager_CheckLocaleVersionThreadRun(managerPtr);
        }
        
        if (error == ARUPDATER_OK)
        {
            error = ARUPDATER_Manager_PrepareSendToDrone(managerPtr, "delos_lucie_updater_payload.plf", test_http_progress_callback, "send progress : ", test_ftp_upload_callback, NULL);
        }
        
        if (error == ARUPDATER_OK)
        {
            // launch the thread
            error = ARUPDATER_Manager_SendToDroneThreadRun(managerPtr);
        }
        
        ARUPDATER_Manager_Delete(&managerPtr);
    }
    else
    {
        // if there was an error in creating a new manager
        ARUPDATER_Manager_Delete(&managerPtr);
    }
    
    fprintf(stderr, "Sum up : %s\n", ARUPDATER_Error_ToString(error));
    
    return 0;
}