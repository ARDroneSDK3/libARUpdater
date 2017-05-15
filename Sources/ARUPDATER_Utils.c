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
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

#include <libARSAL/ARSAL_Print.h>
#include <libARSAL/ARSAL_Endianness.h>

#include "ARUPDATER_Utils.h"
#include "ARUPDATER_Plf.h"
#include "ARUPDATER_Manager.h"

#define ARUPDATER_UTILS_TAG                   "ARUPDATER_Utils"

eARUPDATER_ERROR ARUPDATER_Utils_PlfVersionFromString(const char *str, ARUPDATER_PlfVersion *v)
{
	char buf[256];
	size_t i;
	int ret;

	if (!str || !v) {
		return ARUPDATER_ERROR_BAD_PARAMETER;
	}

	memset(v, 0, sizeof(*v));

	/* lower string */
	memset(buf, 0, sizeof(buf));
	for (i = 0; (str[i] != '\0') && (i < sizeof(buf) / sizeof(buf[0]) - 1); i++)
		buf[i] = tolower(str[i]);

	/* try alpha format */
	ret = sscanf(buf, "%u.%u.%u-alpha%u", &v->ver, &v->edit, &v->ext, &v->patch);
	if (ret == 4) {
		v->type = ARUPDATER_PLF_TYPE_ALPHA;
		return ARUPDATER_OK;
	}

	/* try beta format */
	ret = sscanf(buf, "%u.%u.%u-beta%u", &v->ver, &v->edit, &v->ext, &v->patch);
	if (ret == 4) {
		v->type = ARUPDATER_PLF_TYPE_BETA;
		return ARUPDATER_OK;
	}

	/* try rc format */
	ret = sscanf(buf, "%u.%u.%u-rc%u", &v->ver, &v->edit, &v->ext, &v->patch);
	if (ret == 4) {
		v->type = ARUPDATER_PLF_TYPE_RC;
		return ARUPDATER_OK;
	}

	/* try production format */
	ret = sscanf(buf, "%u.%u.%u", &v->ver, &v->edit, &v->ext);
	if (ret == 3) {
		/* check no other suffix */
		snprintf(buf, sizeof(buf), "%u.%u.%u", v->ver, v->edit, v->ext);
		if (strncmp(buf, str, sizeof(buf)) == 0) {
			v->type = ARUPDATER_PLF_TYPE_PROD;
			v->patch = 0;
			return ARUPDATER_OK;
		}
	}

	/* strange version : fallback to alpha version */
	ARSAL_PRINT(ARSAL_PRINT_ERROR, ARUPDATER_UTILS_TAG,
			"unable to parse version '%s'", str);

	v->type = ARUPDATER_PLF_TYPE_ALPHA;
	v->patch = 1;
	return ARUPDATER_OK;
}

eARUPDATER_ERROR ARUPDATER_Utils_PlfVersionToString(const ARUPDATER_PlfVersion *v,
		char *buf, size_t size)
{
	static const char *const strtypes[] = {
		[ARUPDATER_PLF_TYPE_ALPHA] = "alpha",
		[ARUPDATER_PLF_TYPE_BETA] = "beta",
		[ARUPDATER_PLF_TYPE_RC] = "rc",
		[ARUPDATER_PLF_TYPE_PROD] = "prod",
	};

	if (!v || !buf || size == 0 || v->type > ARUPDATER_PLF_TYPE_PROD)
		return ARUPDATER_ERROR_BAD_PARAMETER;

	if (v->type == ARUPDATER_PLF_TYPE_PROD)
		snprintf(buf, size, "%u.%u.%u",
			 v->ver, v->edit, v->ext);
	else
		snprintf(buf, size, "%u.%u.%u-%s%u",
			 v->ver, v->edit, v->ext,
			 strtypes[v->type], v->patch);

	return ARUPDATER_OK;
}


eARUPDATER_ERROR ARUPDATER_Utils_ReadPlfVersion(const char *plfFilePath, ARUPDATER_PlfVersion *v)
{
	eARUPDATER_ERROR ret;
	plf_phdr_t hdr;
	uint32_t lg;
	char lang[4];

	if (!plfFilePath || !v)
		return ARUPDATER_ERROR_BAD_PARAMETER;

	ret = ARUPDATER_Plf_GetHeader(plfFilePath, &hdr);
	if (ret != ARUPDATER_OK)
		return ret;

	v->ver = hdr.p_ver;
	v->edit = hdr.p_edit;
	v->ext = hdr.p_ext;

	/* lang is set to 0 on production version */
	if (hdr.p_lang == 0) {
		v->type = ARUPDATER_PLF_TYPE_PROD;
		v->patch = 0;
	} else {
		/* lang field is little endian */
		lg = dtohl(hdr.p_lang);
		memcpy(lang, &lg, 4);

		switch (lang[0]) {
		case 'R':
			v->type = ARUPDATER_PLF_TYPE_RC;
		break;
		case 'B':
			v->type = ARUPDATER_PLF_TYPE_BETA;
		break;
		case 'A':
		default:
			v->type = ARUPDATER_PLF_TYPE_ALPHA;
		break;
		}

		/* check & convert next patch */
		if (lang[1] >= '0' && lang[1] <= '9' &&
		    lang[2] >= '0' && lang[2] <= '9') {
			v->patch = (lang[1] - '0') * 10 + lang[2] - '0';
		} else {
			/* unknown version */
			v->type = ARUPDATER_PLF_TYPE_ALPHA;
			v->patch = 1;
		}

	}

	return ARUPDATER_OK;
}

int ARUPDATER_Utils_PlfVersionCompare(const ARUPDATER_PlfVersion *v1, const ARUPDATER_PlfVersion *v2)
{
	if (!v1 || !v2)
		return 0;

	/* compare version */
	if (v1->ver != v2->ver)
		return v1->ver > v2->ver ? 1 : -1;

	/* compare edition */
	if (v1->edit != v2->edit)
		return v1->edit > v2->edit ? 1 : -1;

	/* compare extension */
	if (v1->ext != v2->ext)
		return v1->ext > v2->ext ? 1 : -1;

	/* compare plf type (prod > rc > beta > alpha) */
	if (v1->type != v2->type)
		return (int)v1->type > (int)v2->type ? 1 : -1;

	/* compare patch level for not production firmware */
	if (v1->type != ARUPDATER_PLF_TYPE_PROD && v1->patch != v2->patch)
		return v1->patch > v2->patch ? 1 : -1;

	/* versions are equals */
	return 0;
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

#if defined(BUILD_LIBPLFNG)

static void ARUPDATER_Utils_ExtractUnixFileFromPlf_Logger(void *priv, int prio, const char *fmt, va_list args)
{
    static const eARSAL_PRINT_LEVEL priotab[] = {
	[LOG_EMERG]   = ARSAL_PRINT_FATAL,
	[LOG_ALERT]   = ARSAL_PRINT_FATAL,
	[LOG_CRIT]    = ARSAL_PRINT_FATAL,
	[LOG_ERR]     = ARSAL_PRINT_ERROR,
	[LOG_WARNING] = ARSAL_PRINT_WARNING,
	[LOG_NOTICE]  = ARSAL_PRINT_INFO,
	[LOG_INFO]    = ARSAL_PRINT_INFO,
	[LOG_DEBUG]   = ARSAL_PRINT_DEBUG,
    };
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    ARSAL_PRINT(priotab[prio], ARUPDATER_UTILS_TAG, "libplfng: %s", buf);
}

eARUPDATER_ERROR ARUPDATER_Utils_ExtractUnixFileFromPlf(const char *plfFileName, const char *outFolder, const char *unixFileName)
{
    FILE *fp;
    int ret, i, count, pos = -1;
    plf_unixhdr hdr;
    char *p, *buf = NULL;
    const char *filename;
    struct plfng *plf = NULL;
    const int bufsize = 4096; /* avoid PATH_MAX headaches */

    fp = fopen(plfFileName, "rb");
    if (fp == NULL) {
	ret = errno;
	ARSAL_PRINT(ARSAL_PRINT_ERROR, ARUPDATER_UTILS_TAG, "fopen(%s): %s", plfFileName, strerror(ret));
	ret = ARUPDATER_ERROR;
	goto finish;
    }

    plf = plfng_new_from_file(fp);
    if (plf == NULL) {
	ret = ARUPDATER_ERROR_ALLOC;
	goto finish;
    }

    buf = malloc(bufsize);
    if (buf == NULL) {
	ret = ARUPDATER_ERROR_ALLOC;
	goto finish;
    }

    plfng_set_log_fn(plf, ARUPDATER_Utils_ExtractUnixFileFromPlf_Logger, NULL);

    count = plfng_get_section_count(plf);
    if (count < 0) {
	ret = ARUPDATER_ERROR_PLF;
	goto finish;
    }

    for (i = 0; i < count; i++) {
	/* probe section */
	ret = plfng_get_unixfile_path(plf, i, &hdr, buf, bufsize);
	if ((ret < 0) || !S_ISREG((mode_t)hdr.s_mode))
	    /* probably not a U_UNIXFILE regular file */
	    continue;

	/* get trailing component of path */
	p = strrchr(buf, '/');
	filename = p ? p+1 : buf;

	if (strcmp(filename, unixFileName) == 0) {
	    /* we have a match */
	    pos = i;
	    break;
	}
    }

    if (pos < 0) {
	/* file was not found */
	ARSAL_PRINT(ARSAL_PRINT_ERROR, ARUPDATER_UTILS_TAG, "file '%s' not found in PLF file %s", unixFileName, plfFileName);
	ret = ARUPDATER_ERROR_PLF;
	goto finish;
    }

    /* now do the extraction */
    snprintf(buf, bufsize, "%s/%s", outFolder, unixFileName);
    ret = plfng_extract_unixfile(plf, pos, buf);
    if (ret < 0) {
	ret = ARUPDATER_ERROR_PLF;
	goto finish;
    }

    /* everything went smoothly */
    ret = ARUPDATER_OK;

 finish:
    if (fp)
	fclose(fp);
    plfng_destroy(plf);
    free(buf);

    return ret;
}

#else

eARUPDATER_ERROR ARUPDATER_Utils_ExtractUnixFileFromPlf(const char *plfFileName, const char *outFolder, const char *unixFileName)
{
    return ARUPDATER_ERROR;
}

#endif /* BUILD_LIBPLFNG */
