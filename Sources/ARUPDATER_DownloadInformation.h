/**
 * @file ARUPDATER_DownloadInformation.c
 * @brief libARUpdater download information c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_DOWNLOAD_INFORMATION_PRIVATE_H_
#define _ARUPDATER_DOWNLOAD_INFORMATION_PRIVATE_H_

#include <libARDiscovery/ARDISCOVERY_Discovery.h>
#include <libARUpdater/ARUPDATER_Error.h>

typedef struct ARUPDATER_DownloadInformation_t
{
    char *downloadUrl;
    char *md5Expected;
    eARDISCOVERY_PRODUCT product;
    
}ARUPDATER_DownloadInformation_t;

ARUPDATER_DownloadInformation_t* ARUPDATER_DownloadInformation_New(const char *const downloadUrl, const char *const md5Expected, const eARDISCOVERY_PRODUCT product, eARUPDATER_ERROR *error);

void ARUPDATER_DownloadInformation_Delete(ARUPDATER_DownloadInformation_t **downloadInfo);

#endif
