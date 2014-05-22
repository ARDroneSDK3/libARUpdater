/*
 * GENERATED FILE
 *  Do not modify this file, it will be erased during the next configure run
 */

/**
 * @file ARUPDATER_Error.c
 * @brief ToString function for eARUPDATER_ERROR enum
 */

#include <libARUpdater/ARUPDATER_Error.h>

char* ARUPDATER_Error_ToString (eARUPDATER_ERROR error)
{
    switch (error)
    {
    case ARUPDATER_OK:
        return "No error";
        break;
    case ARUPDATER_ERROR:
        return "Unknown generic error";
        break;
    case ARUPDATER_ERROR_ALLOC:
        return "Memory allocation error";
        break;
    case ARUPDATER_ERROR_BAD_PARAMETER:
        return "Bad parameters error";
        break;
    case ARUPDATER_ERROR_SYSTEM:
        return "System error";
        break;
    default:
        return "Unknown value";
        break;
    }
    return "Unknown value";
}
