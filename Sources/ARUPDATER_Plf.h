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
 *  plf.h
 *  ARUpdater
 *
 *  Created by f.dhaeyer on 06/07/10.
 *  Copyright 2010 Parrot SA. All rights reserved.
 *
 */

#ifndef _ARUPDATER_PLF_PRIVATE_H_
#define _ARUPDATER_PLF_PRIVATE_H_

#define PLF_CURRENT_VERSION  10
#define PLF_HEADER_MAGIC     0x21464c50 //!< PLF magic number

#include <libARUpdater/ARUPDATER_Error.h>

typedef unsigned int   Plf_Word;        //!< Unsigned 32 bits integer
typedef unsigned short Plf_Half;        //!< Unsigned 16 bits integer
typedef void*          Plf_Add;         //!< 32 bits address

//! PLF file header
typedef struct {
	Plf_Word    p_magic;                  //!< PLF magic number
	Plf_Word    p_plfversion;             //!< PLF format version
	Plf_Word    p_phdrsize;               //!< File header size
	Plf_Word    p_shdrsize;               //!< Section header size
	Plf_Word    p_type;                   //!< File type
	Plf_Word    p_entry;                  //!< Executable entry point
	Plf_Word    p_targ;                   //!< Target platform
	Plf_Word    p_app;                    //!< Target application
	Plf_Word    p_hdw;                    //!< Hardware compatibility
	Plf_Word    p_ver;                    //!< Version
	Plf_Word    p_edit;                   //!< Edition
	Plf_Word    p_ext;                    //!< Extension
	Plf_Word    p_lang;                   //!< Language zone
	Plf_Word    p_size;                   //!< File size in bytes
} plf_phdr_t;

/**
 * @brief read the header of a plf file
 * @param[in] plf_filepath : path of the plf file to read
 * @param[out] header : a struct representing the header of the file
 * @return ARUPDATER_OK if operation went well, a description of the error otherwise
 */
eARUPDATER_ERROR ARUPDATER_Plf_GetHeader(const char *plf_filename, plf_phdr_t *header);
#endif // _PLF_H_
