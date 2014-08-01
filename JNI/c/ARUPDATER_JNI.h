
#ifndef _ARUPDATER_JNI_H_
#define _ARUPDATER_JNI_H_

#ifndef JNI_OK
#define JNI_OK      0
#endif
#ifndef JNI_FAILED
#define JNI_FAILED  -1
#endif

extern JavaVM* ARUPDATER_JNI_Manager_VM;

/**
 * @brief Downloader Callbacks structure
 * @param jProgressListener The progress Listener
 * @param jProgressArg The progress Arg object
 * @param jCompletionListener The completion Listener
 * @param jCompletionArg The completion Arg object
 * @param jDownloadListener The should download Listener
 * @param jDownloadArg The should download Arg object
 * @see ARUPDATER_JNI_Downloader_FreeDownloaderCallbacks
 */
typedef struct _ARUPDATER_JNI_DownloaderCallbacks_t_
{
    jobject jWillDownloadPlfListener;
    jobject jWillDownloadPlfArgs;
    jobject jProgressListener;
    jobject jProgressArgs;
    jobject jCompletionListener;
    jobject jCompletionArgs;
    jobject jDownloadListener;
    jobject jDownloadArgs;

} ARUPDATER_JNI_DownloaderCallbacks_t;

/**
 * @brief Uploader Callbacks structure
 * @param jProgressListener The progress Listener
 * @param jProgressArg The progress Arg object
 * @param jCompletionListener The completion Listener
 * @param jCompletionArg The completion Arg object
 * @see ARUPDATER_JNI_Downloader_FreeUploaderCallbacks
 */
typedef struct _ARUPDATER_JNI_UploaderCallbacks_t_
{
    jobject jProgressListener;
    jobject jProgressArgs;
    jobject jCompletionListener;
    jobject jCompletionArgs;

} ARUPDATER_JNI_UploaderCallbacks_t;

/**
 * @brief Throw a new ARUpdaterException
 * @param env The java env
 * @param nativeError The error
 * @retval void
 * @see ARUPDATER_JNI_Manager_NewARUpdaterException
 */
void ARUPDATER_JNI_Manager_ThrowARUpdaterException(JNIEnv *env, eARUPDATER_ERROR nativeError);

/**
 * @brief Create a new ARUpdaterException
 * @param env The java env
 * @param nativeError The error
 * @retval the new ARUpdaterException
 * @see ARUPDATER_JNI_Manager_ThrowARUpdaterException
 */
jobject ARUPDATER_JNI_Manager_NewARUpdaterException(JNIEnv *env, eARUPDATER_ERROR nativeError);

/**
 * @brief Get the ARUpdaterException JNI class
 * @param env The java env
 * @retval JNI_TRUE if Success, else JNI_FALSE
 * @see ARUPDATER_JNI_Manager_FreeARUpdaterExceptionJNI
 */
int ARUPDATER_JNI_Manager_NewARUpdaterExceptionJNI(JNIEnv *env);

/**
 * @brief Free the ARUpdaterException JNI class
 * @param env The java env
 * @retval void
 * @see ARUPDATER_JNI_Manager_NewARUpdaterExceptionJNI
 */
void ARUPDATER_JNI_Manager_FreeARUpdaterExceptionJNI(JNIEnv *env);

/**
 * @brief Create a new ARUPDATER_ERROR_ENUM
 * @param env The java env
 * @param nativeError The error
 * @retval the new ARUPDATER_ERROR_ENUM
 * @see ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI
 */
jobject ARUPDATER_JNI_Manager_NewERROR_ENUM(JNIEnv *env, eARUPDATER_ERROR nativeError);

/**
 * @brief Get the ARUPDATER_ERROR_ENUM JNI class
 * @param env The java env
 * @retval JNI_TRUE if Success, else JNI_FALSE
 * @see ARUPDATER_JNI_Manager_FreeERROR_ENUM_JNI
 */
int ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI(JNIEnv *env);

/**
 * @brief Free the ARUPDATER_ERROR_ENUM JNI class
 * @param env The java env
 * @retval void
 * @see ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI
 */
void ARUPDATER_JNI_Manager_FreeERROR_ENUM_JNI(JNIEnv *env);





/*************************************************
	Downloader callback methods
**************************************************/


/**
 * @brief Get the ARUpdaterShouldDownloadPlfListener, ARUpdaterPlfDownloadProgressListener and ARUpdaterPlfDownloadCompletionListener JNI classes
 * @param env The java env
 * @retval JNI_TRUE if Success, else JNI_FALSE
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
int ARUPDATER_JNI_Downloader_NewListenersJNI(JNIEnv *env);

/**
 * @brief Free the ARUpdaterShouldDownloadPlfListener, ARUpdaterPlfDownloadProgressListener and ARUpdaterPlfDownloadCompletionListener JNI classes
 * @param env The java env
 * @retval void
 * @see ARUPDATER_JNI_Downloader_NewListenersJNI
 */
void ARUPDATER_JNI_Downloader_FreeListenersJNI(JNIEnv *env);

/**
 * @brief Callback fired just before the uploading of a plf file
 * @param arg The arg
 * @param product : Description of the product targeted by the plf downloaded
 * @param remotePlfVersion : The version of the plf file that will be downloaded
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_WillDownloadPlfCallback(void* arg, eARDISCOVERY_PRODUCT product, const char *const remotePlfVersion);

/**
 * @brief Callback that give the download progress percent
 * @param arg The arg
 * @param percent The progress percent
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_ProgressCallback(void* arg, float percent);

/**
 * @brief Callback that give the download completion status
 * @param arg The arg
 * @param nativeError The error status of the plf download
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_CompletionCallback(void* arg, eARUPDATER_ERROR nativeError);

/**
 * @brief Callback that indicates if a plf should be download
 * @param arg The arg
 * @param shouldDownload 1 if a plf should be downloaded, else 0
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_ShouldDownloadCallback(void* arg, int shouldDownload, eARUPDATER_ERROR nativeError);

/**
 * @brief Free Callbacks structure
 * @warning This function frees memory
 * @param env The java env
 * @param callbacks The callbacks structure
 * @retval void
 * @see ARUPDATER_JNI_DownloaderCallbacks_t
 */
void ARUPDATER_JNI_Downloader_FreeDownloaderCallbacks(JNIEnv *env, ARUPDATER_JNI_DownloaderCallbacks_t **callbacks);

int ARUPDATER_JNI_Downloader_NewDownloadInfoJNI(JNIEnv *env);

void ARUPDATER_JNI_Downloader_FreeDownloadInfoJNI(JNIEnv *env);

/*************************************************
	Uploader callback methods
**************************************************/

/**
 * @brief Get the ARUpdaterPlfUploaderProgressListener and ARUpdaterPlfUploadCompletionListener JNI classes
 * @param env The java env
 * @retval JNI_TRUE if Success, else JNI_FALSE
 * @see ARUPDATER_JNI_Uploader_FreeListenersJNI
 */
int ARUPDATER_JNI_Uploader_NewListenersJNI(JNIEnv *env);

/**
 * @brief Free the ARUpdaterPlfUploaderProgressListener and ARUpdaterPlfUploadCompletionListener JNI classes
 * @param env The java env
 * @retval void
 * @see ARUPDATER_JNI_Uploader_NewListenersJNI
 */
void ARUPDATER_JNI_Uploader_FreeListenersJNI(JNIEnv *env);

/**
 * @brief Callback that give the Upload progress percent
 * @param arg The arg
 * @param percent The progress percent
 * @retval void
 * @see ARUPDATER_JNI_Uploader_FreeListenersJNI
 */
void ARUPDATER_JNI_Uploader_ProgressCallback(void* arg, float percent);

/**
 * @brief Callback that give the Up completion status
 * @param arg The arg
 * @param nativeError The error status of the plf Up
 * @retval void
 * @see ARUPDATER_JNI_Uploader_FreeListenersJNI
 */
void ARUPDATER_JNI_Uploader_CompletionCallback(void* arg, eARUPDATER_ERROR nativeError);


/**
 * @brief Free Callbacks structure
 * @warning This function frees memory
 * @param env The java env
 * @param callbacks The callbacks structure
 * @retval void
 * @see ARUPDATER_JNI_UploaderCallbacks_t
 */
void ARUPDATER_JNI_Uploader_FreeUploaderCallbacks(JNIEnv *env, ARUPDATER_JNI_UploaderCallbacks_t **callbacks);


/*************************************************
    ARDATATRANSFER enum generator
**************************************************/

/**
 * @brief Create a new ARUPDATER_ERROR_ENUM
 * @param env The java env
 * @param nativeError The error
 * @retval the new ARUPDATER_ERROR_ENUM
 * @see ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI
 */
jobject ARUPDATER_JNI_Manager_NewDATATRANSFER_ERROR_ENUM(JNIEnv *env, eARDATATRANSFER_ERROR nativeError);

/**
 * @brief Get the ARUPDATER_ERROR_ENUM JNI class
 * @param env The java env
 * @retval JNI_TRUE if Success, else JNI_FALSE
 * @see ARUPDATER_JNI_Manager_FreeERROR_ENUM_JNI
 */
int ARUPDATER_JNI_Manager_NewDATATRANSFER_ERROR_ENUM_JNI(JNIEnv *env);

/**
 * @brief Free the ARUPDATER_ERROR_ENUM JNI class
 * @param env The java env
 * @retval void
 * @see ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI
 */
void ARUPDATER_JNI_Manager_FreeDATATRANSFER_ERROR_ENUM_JNI(JNIEnv *env);

#endif /* _ARUPDATER_JNI_H_ */

