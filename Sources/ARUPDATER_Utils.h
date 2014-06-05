/**
 * @file ARUPDATER_Utils.h
 * @brief libARUpdater Utils header file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef _ARUPDATER_UTILS_PRIVATE_H_
#define _ARUPDATER_UTILS_PRIVATE_H_

#include <libARUpdater/ARUPDATER_Error.h>

/**
 * @brief get the version of a given plf file
 * @param[in] plfFilePath : file path of the plf file
 * @param[out] version : pointer on the version to be returned
 * @param[out] edition : pointer on the edition to be returned
 * @param[out] extension : pointer on the extension to be returned
 * @return ARUPDATER_OK if operation went well, the description of the error otherwise
 */
eARUPDATER_ERROR ARUPDATER_Utils_GetPlfVersion(const char *const plfFilePath, int *version, int *edition, int *extension);

/**
 * @brief get the plf path of the first plf file found in a given folder
 * @param[in] plfFolder : the folder to look for the plf file
 * @param[out] plfFileName : name of the plf file. Must be allocated
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 */
eARUPDATER_ERROR ARUPDATER_Utils_GetPlfInFolder(const char *const plfFolder, char **plfFileName);

#endif
