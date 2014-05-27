/**
 * @file ARUPDATER_Manager.h
 * @brief libARUpdater Manager header file.
 * @date 07/01/2014
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUPDATER_MANAGER_PRIVATE_H_
#define _ARUPDATER_MANAGER_PRIVATE_H_

#include "ARUPDATER_Updater.h"

/**
 * @brief Manager structure
 * @param 
 * @param 
 * @see
 */
struct ARUPDATER_Manager_t
{
    ARUPDATER_Updater_t *updater;
    
    char *plfFolder;
};

#endif /* _ARUPDATER_MANAGER_PRIVATE_H_ */

