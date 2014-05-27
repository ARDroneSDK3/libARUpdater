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

#include <libARUPDATER/ARUPDATER_Error.h>

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
