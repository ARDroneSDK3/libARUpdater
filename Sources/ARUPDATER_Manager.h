/**
 * @file ARUPDATER_Manager.h
 * @brief libARUpdater Manager header file.
 * @date 07/01/2014
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUPDATER_MANAGER_PRIVATE_H_
#define _ARUPDATER_MANAGER_PRIVATE_H_

#include "ARUPDATER_Downloader.h"
#include "ARUPDATER_PlfSender.h"

/**
 * @brief Manager structure
 * @param 
 * @param 
 * @see
 */
struct ARUPDATER_Manager_t
{
    ARUPDATER_Downloader_t *downloader;
    ARUPDATER_PlfSender_t *plfSender;
    
    char *plfFolder;
};

#endif /* _ARUPDATER_MANAGER_PRIVATE_H_ */

