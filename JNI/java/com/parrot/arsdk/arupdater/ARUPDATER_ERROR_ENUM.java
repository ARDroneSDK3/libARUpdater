/*
 * GENERATED FILE
 *  Do not modify this file, it will be erased during the next configure run
 */

package com.parrot.arsdk.arupdater;

import java.util.HashMap;

/**
 * Java copy of the eARUPDATER_ERROR enum
 */
public enum ARUPDATER_ERROR_ENUM {
   /** No error */
    ARUPDATER_OK (0, "No error"),
   /** Unknown generic error */
    ARUPDATER_ERROR (-1000, "Unknown generic error"),
   /** Memory allocation error */
    ARUPDATER_ERROR_ALLOC (-999, "Memory allocation error"),
   /** Bad parameters error */
    ARUPDATER_ERROR_BAD_PARAMETER (-998, "Bad parameters error"),
   /** System error */
    ARUPDATER_ERROR_SYSTEM (-997, "System error"),
   /** Already initialized error */
    ARUPDATER_ERROR_ALREADY_INITIALIZED (-996, "Already initialized error"),
   /** Not initialized error */
    ARUPDATER_ERROR_NOT_INITIALIZED (-995, "Not initialized error"),
   /** Thread processing error */
    ARUPDATER_ERROR_THREAD_PROCESSING (-994, "Thread processing error"),
   /** Generic PLF error */
    ARUPDATER_ERROR_PLF (-2000, "Generic PLF error"),
   /** Generic PLF error */
    ARUPDATER_ERROR_PLF_FILE_NOT_FOUND (-1999, "Generic PLF error"),
   /** Generic Updater error */
    ARUPDATER_ERROR_DOWNLOADER (-3000, "Generic Updater error"),
   /** error on a ARUtils operation */
    ARUPDATER_ERROR_DOWNLOADER_ARUTILS_ERROR (-2999, "error on a ARUtils operation"),
   /** error downloading a file */
    ARUPDATER_ERROR_DOWNLOADER_DOWNLOAD (-2998, "error downloading a file"),
   /** error given by the PHP script on server */
    ARUPDATER_ERROR_DOWNLOADER_PHP_ERROR (-2997, "error given by the PHP script on server"),
   /** error when renaming files */
    ARUPDATER_ERROR_DOWNLOADER_RENAME_FILE (-2996, "error when renaming files"),
   /** Generic Uploader error */
    ARUPDATER_ERROR_UPLOADER (-4000, "Generic Uploader error"),
   /** error on a ARUtils operation in uploader */
    ARUPDATER_ERROR_UPLOADER_ARUTILS_ERROR (-3999, "error on a ARUtils operation in uploader"),
   /** error on a ARDataTransfer operation in uploader */
    ARUPDATER_ERROR_UPLOADER_ARDATATRANSFER_ERROR (-3998, "error on a ARDataTransfer operation in uploader");

    private final int value;
    private final String comment;
    static HashMap<Integer, ARUPDATER_ERROR_ENUM> valuesList;

    ARUPDATER_ERROR_ENUM (int value) {
        this.value = value;
        this.comment = null;
    }

    ARUPDATER_ERROR_ENUM (int value, String comment) {
        this.value = value;
        this.comment = comment;
    }

    /**
     * Gets the int value of the enum
     * @return int value of the enum
     */
    public int getValue () {
        return value;
    }

    /**
     * Gets the ARUPDATER_ERROR_ENUM instance from a C enum value
     * @param value C value of the enum
     * @return The ARUPDATER_ERROR_ENUM instance, or null if the C enum value was not valid
     */
    public static ARUPDATER_ERROR_ENUM getFromValue (int value) {
        if (null == valuesList) {
            ARUPDATER_ERROR_ENUM [] valuesArray = ARUPDATER_ERROR_ENUM.values ();
            valuesList = new HashMap<Integer, ARUPDATER_ERROR_ENUM> (valuesArray.length);
            for (ARUPDATER_ERROR_ENUM entry : valuesArray) {
                valuesList.put (entry.getValue (), entry);
            }
        }
        return valuesList.get (value);
    }

    /**
     * Returns the enum comment as a description string
     * @return The enum description
     */
    public String toString () {
        if (this.comment != null) {
            return this.comment;
        }
        return super.toString ();
    }
}
