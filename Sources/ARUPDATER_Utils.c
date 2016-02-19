/*
    Copyright (C) 2014 Parrot SA

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the 
      distribution.
    * Neither the name of Parrot nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/
/**
 * @file ARUPDATER_Utils.h
 * @brief libARUpdater Utils c file.
 * @date 23/05/2014
 * @author djavan.bertrand@parrot.com
 **/

#ifndef WIN32
#include <sys/types.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "ARUPDATER_Utils.h"
#include "ARUPDATER_Plf.h"
#include "ARUPDATER_Manager.h"

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

eARUPDATER_ERROR ARUPDATER_Utils_GetPlfInFolder(const char *const plfFolder, char **plfFileName)
{
    eARUPDATER_ERROR error = ARUPDATER_OK;
    
    if ((plfFolder == NULL) || (plfFileName == NULL))
    {
        return ARUPDATER_ERROR_BAD_PARAMETER;
    }

    *plfFileName = NULL;
    if (ARUPDATER_OK == error)
    {
        DIR *dir = opendir(plfFolder);
        
        struct dirent *entry = NULL;
        int found = 0;
        if (dir)
        {
            while((found == 0) && (entry = readdir(dir)))
            {
                char *filename = entry->d_name;
                char *extension = strrchr(filename, ARUPDATER_MANAGER_PLF_EXTENSION[0]);
                if ((extension != NULL) && (strcmp(extension, ARUPDATER_MANAGER_PLF_EXTENSION) == 0))
                {
                    int plfFileNameLength = strlen(filename) + 1;
                    *plfFileName = malloc(plfFileNameLength);
                    if (*plfFileName == NULL)
                    {
                        error = ARUPDATER_ERROR_ALLOC;
                    }
                    else
                    {
                        strcpy(*plfFileName, filename);
                    }
                    found = 1;
                }
            }
            closedir(dir);
        }
        
        if (found == 0)
        {
            error = ARUPDATER_ERROR_PLF_FILE_NOT_FOUND;
        }
    }
    
    return error;
}
