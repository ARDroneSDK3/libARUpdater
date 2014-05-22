/**
 * @file ARUPDATER_Manager.h
 * @brief libARUpdater Manager header file.
 * @date 07/01/2014
 * @author david.flattin.ext@parrot.com
 **/

#ifndef _ARUPDATER_MANAGER_H_
#define _ARUPDATER_MANAGER_H_

/**
 * @brief Manager structure
 * @see ARUPDATER_Manager_New ()
 */
typedef struct ARUPDATER_Manager_t ARUPDATER_Manager_t;

/**
 * @brief Create a new ARUpdater Manager
 * @warning This function allocates memory
 * @param[out] error A pointer on the error output
 * @return Pointer on the new ARUpdater Manager
 * @see ARUPDATER_Manager_Delete ()
 */
ARUPDATER_Manager_t* ARUPDATER_Manager_New (eARUPDATER_ERROR *error);

/**
 * @brief Delete an ARUpdater Manager
 * @warning This function frees memory
 * @param manager The address of the pointer on the ARUpdater Manager
 * @see ARUPDATER_Manager_New ()
 */
void ARUPDATER_Manager_Delete (ARUPDATER_Manager_t **managerPtrAddr);


#endif /* _ARUPDATER_MANAGER_H_ */


