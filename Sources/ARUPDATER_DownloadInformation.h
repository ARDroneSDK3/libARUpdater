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
#include <libARUpdater/ARUPDATER_Downloader.h>


ARUPDATER_DownloadInformation_t* ARUPDATER_DownloadInformation_New(const char *const downloadUrl, const char *const md5Expected, const char *const plfVersion, int remoteSize, const eARDISCOVERY_PRODUCT product, eARUPDATER_ERROR *error);

void ARUPDATER_DownloadInformation_Delete(ARUPDATER_DownloadInformation_t **downloadInfo);

#endif
