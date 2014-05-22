
/**
 * @file ARUpdater/ARUPDATER_Error.h
 * @brief libARUpdater error header, This file contains all error of ARUPDATER
 * @date 26/12/2013
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _LIBARUPDATER_ERROR_H_
#define _LIBARUPDATER_ERROR_H_

/**
 * @brief libARUpdater errors known.
 */
typedef enum
{
    ARUPDATER_OK = 0, /**< No error */
    ARUPDATER_ERROR = -1000, /**< Unknown generic error */
    ARUPDATER_ERROR_ALLOC, /**< Memory allocation error */
    ARUPDATER_ERROR_BAD_PARAMETER, /**< Bad parameters error */
    ARUPDATER_ERROR_SYSTEM, /**< System error */
        
} eARUPDATER_ERROR;

/**
 * @brief Gets the error string associated with an eARUPDATER_ERROR
 * @param error The error to describe
 * @return A static string describing the error
 *
 * @note User should NEVER try to modify a returned string
 */
char* ARUPDATER_Error_ToString (eARUPDATER_ERROR error);

#endif /* _LIBARUPDATER_ERROR_H_ */
