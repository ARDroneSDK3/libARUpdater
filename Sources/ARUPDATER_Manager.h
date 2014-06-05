/**
 * @file ARUPDATER_Manager.h
 * @brief libARUpdater Manager header file.
 * @date 07/01/2014
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUPDATER_MANAGER_PRIVATE_H_
#define _ARUPDATER_MANAGER_PRIVATE_H_

#include "ARUPDATER_Downloader.h"
#include "ARUPDATER_Uploader.h"

#define ARUPDATER_MANAGER_DEVICE_STRING_MAX_SIZE        10
#define ARUPDATER_MANAGER_FOLDER_SEPARATOR              "/"
#define ARUPDATER_MANAGER_PLF_FOLDER                    "plfFolder/"
#define ARUPDATER_MANAGER_PLF_EXTENSION                 ".plf"

/**
 * @brief Manager structure
 * @param 
 * @param 
 * @see
 */
struct ARUPDATER_Manager_t
{
    ARUPDATER_Downloader_t *downloader;
    ARUPDATER_Uploader_t *uploader;
    
};

#endif /* _ARUPDATER_MANAGER_PRIVATE_H_ */

