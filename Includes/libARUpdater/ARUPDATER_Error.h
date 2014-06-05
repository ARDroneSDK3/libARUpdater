
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
    ARUPDATER_OK = 0,                                   /**< No error */
    ARUPDATER_ERROR = -1000,                            /**< Unknown generic error */
    ARUPDATER_ERROR_ALLOC,                              /**< Memory allocation error */
    ARUPDATER_ERROR_BAD_PARAMETER,                      /**< Bad parameters error */
    ARUPDATER_ERROR_SYSTEM,                             /**< System error */
    ARUPDATER_ERROR_ALREADY_INITIALIZED,                /**< Already initialized error */
    ARUPDATER_ERROR_NOT_INITIALIZED,                    /**< Not initialized error */
    ARUPDATER_ERROR_THREAD_PROCESSING,                  /**< Thread processing error */    
    
    ARUPDATER_ERROR_PLF = -2000,                        /**< Generic PLF error */
    ARUPDATER_ERROR_PLF_FILE_NOT_FOUND,                 /**< Plf File not found */
    
    ARUPDATER_ERROR_DOWNLOADER = -3000,                    /**< Generic Updater error */
    ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR,              /**< error on a ARUtils operation */
    ARUPDATER_ERROR_DOWNLOADER_DOWNLOAD,                   /**< error downloading a file */
    ARUPDATER_ERROR_DOWNLOADER_PHP_ERROR,                  /**< error given by the PHP script on server */
    ARUPDATER_ERROR_DOWNLOADER_RENAME_FILE,                /**< error when renaming files */
    ARUPDATER_ERROR_DOWNLOADER_FILE_NOT_FOUND,             /**< Plf file not found in the downloader */
    ARUPDATER_ERROR_DOWNLOADER_MD5_DONT_MATCH,             /**< MD5 checksum does not match with the remote file */
    
    ARUPDATER_ERROR_UPLOADER = -4000,                   /**< Generic Uploader error */
    ARUPDATER_ERROR_UPLOADER_ARUTILS_ERROR,             /**< error on a ARUtils operation in uploader*/
    ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR,      /**< error on a ARDataTransfer operation in uploader*/
    ARUPDATER_ERROR_UPLOADER_ARSAL_ERROR,               /**< error on a ARSAL operation in uploader*/
    
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
