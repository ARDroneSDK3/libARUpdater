/**
 * @file ARUPDATER_Utils.h
 * @brief libARUpdater Utils c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#include <stdlib.h>
#include "ARUPDATER_Utils.h"
#include "ARUPDATER_Plf.h"

eARUPDATER_ERROR ARUPDATER_Utils_GetPlfVersion(const char *const plfFilePath, int *version, int *edition, int *extension)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    plf_phdr_t header;
    
    if ((plfFilePath != NULL)   &&
        (version != NULL)       &&
        (edition != NULL)       &&
        (extension != NULL))
    {
        error = ARUPDATER_Plf_GetHeader(plfFilePath, &header);
    }
    else
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (ARUPDATER_OK == error)
    {
        *version = header.p_ver;
        *edition = header.p_edit;
        *extension = header.p_ext;
    }
    
    return error;
}