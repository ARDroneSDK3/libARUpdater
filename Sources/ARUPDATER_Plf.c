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
