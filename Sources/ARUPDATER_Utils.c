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
        error = ARUPDATER_ERROR_BAD_PARAMETER;
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
