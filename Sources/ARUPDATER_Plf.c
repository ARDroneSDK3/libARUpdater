/*
 *  plf.c
 *  ARUpdater
 *
 *  Created by f.dhaeyer on 06/07/10.
 *  Copyright 2010 Parrot SA. All rights reserved.
 *
 */
#include <stdio.h>
#include <string.h>
#include "ARUPDATER_Plf.h"

eARUPDATER_ERROR ARUPDATER_Plf_GetHeader(const char *plf_filename, plf_phdr_t *header)
{
    eARUPDATER_ERROR error =  ARUPDATER_OK;
    if (header == NULL)
    {
        error = ARUPDATER_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARUPDATER_OK)
    {
        plf_phdr_t h;
        FILE *f = fopen(plf_filename, "rb");
        if(f != NULL)
        {
            if(fread(&h, 1, sizeof(plf_phdr_t), f) == sizeof(plf_phdr_t))
            {
                memcpy(header, &h, sizeof(plf_phdr_t));
            }
            fclose(f);
        }
        else
        {
            error = ARUPDATER_ERROR_PLF_FILE_NOT_FOUND;
        }
    }
	
	return error;
}
