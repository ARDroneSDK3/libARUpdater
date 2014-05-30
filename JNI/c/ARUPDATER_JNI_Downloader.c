/**
 * @file ARUPDATER_Manager.c
 * @brief libARUpdater JNI_Downloader c file.
 **/

#ifdef NDEBUG
/* Android ndk-build NDK_DEBUG=0*/
#else
/* Android ndk-build NDK_DEBUG=1*/
#ifndef DEBUG
#define DEBUG
#endif
#endif

#include <jni.h>
#include <inttypes.h>
#include <stdlib.h>

#include <libARSAL/ARSAL_Sem.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARUtils/ARUTILS_Error.h>

#include "libARUpdater/ARUPDATER_Error.h"
#include "libARUpdater/ARUPDATER_Manager.h"
#include "libARUpdater/ARUPDATER_Downloader.h"
#include "ARUPDATER_JNI.h"

#define ARUPDATER_JNI_DOWNLOADER_TAG       "JNI"

jmethodID methodId_DownloaderListener_onPlfDownloadProgress = NULL;
jmethodID methodId_DownloaderListener_onPlfDownloadComplete = NULL;
jmethodID methodId_DownloaderListener_downloadPlf = NULL;

JNIEXPORT jboolean JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterDownloader_nativeStaticInit(JNIEnv *env, jclass jClass)
{
    jboolean jret = JNI_FALSE;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "");

    if (env == NULL)
    {
        error = JNI_FAILED;
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Downloader_NewListenersJNI(env);
    }

    if (error == JNI_OK)
    {
        jret = JNI_TRUE;
    }

    return jret;
}

JNIEXPORT jint JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterDownloader_nativeNew(JNIEnv *env, jobject jThis, jlong jManager, jstring jRootFolder, jobject jDownloadListener, jobject jDownloadArgs, jobject jProgressListener, jobject jProgressArgs, jobject jCompletionListener, jobject jCompletionArgs)
{
    ARUPDATER_Manager_t *nativeManager = (ARUPDATER_Manager_t*)(intptr_t)jManager;
    ARUPDATER_JNI_DownloaderCallbacks_t *callbacks = NULL;
    eARUPDATER_ERROR result = ARUPDATER_OK;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "%x", (int)nativeManager);

    callbacks = (ARUPDATER_JNI_DownloaderCallbacks_t*)calloc(1, sizeof(ARUPDATER_JNI_DownloaderCallbacks_t));

    if (callbacks == NULL)
    {
        error = JNI_FAILED;
    }
    else
    {
        if (jDownloadListener != NULL)
        {
            callbacks->jDownloadListener = (*env)->NewGlobalRef(env, jDownloadListener);
        }
        if (jDownloadArgs != NULL)
        {
            callbacks->jDownloadArgs = (*env)->NewGlobalRef(env, jDownloadArgs);
        }

        if (jProgressListener != NULL)
        {
            callbacks->jProgressListener = (*env)->NewGlobalRef(env, jProgressListener);
        }
        if (jProgressArgs != NULL)
        {
            callbacks->jProgressArgs = (*env)->NewGlobalRef(env, jProgressArgs);
        }

        if (jCompletionListener != NULL)
        {
            callbacks->jCompletionListener = (*env)->NewGlobalRef(env, jCompletionListener);
        }
        if (jCompletionArgs != NULL)
        {
            callbacks->jCompletionArgs = (*env)->NewGlobalRef(env, jCompletionArgs);
        }
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Downloader_NewListenersJNI(env);
    }
    
    const char *rootFolder = (*env)->GetStringUTFChars(env, jRootFolder, 0);

    if (error != JNI_OK)
    {
        result = ARUPDATER_ERROR_SYSTEM;
    }

    if (result == ARUPDATER_OK)
    {
        result = ARUPDATER_Downloader_New(nativeManager, rootFolder, ARUPDATER_JNI_Downloader_ShouldDownloadCallback, callbacks, ARUPDATER_JNI_Downloader_ProgressCallback, callbacks, ARUPDATER_JNI_Downloader_CompletionCallback, callbacks);
    }

    if ((result != ARUPDATER_OK) && (callbacks != NULL))
    {
        ARUPDATER_JNI_Downloader_FreeDownloaderCallbacks(env, &callbacks);
    }

    if (rootFolder != NULL)
    {
        (*env)->ReleaseStringUTFChars(env, jRootFolder, rootFolder);
    }

    return result;
}



JNIEXPORT jint JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterDownloader_nativeDelete(JNIEnv *env, jobject jThis, jlong jManager)
{
    ARUPDATER_Manager_t *nativeManager = (ARUPDATER_Manager_t*)(intptr_t)jManager;
    eARUPDATER_ERROR result = ARUPDATER_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "");

    result = ARUPDATER_Downloader_Delete(nativeManager);

    return result;
}



JNIEXPORT void JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterDownloader_nativeThreadRun(JNIEnv *env, jobject jThis, jlong jManager)
{
    ARUPDATER_Manager_t *nativeManager = (ARUPDATER_Manager_t*)(intptr_t)jManager;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "");

    ARUPDATER_Downloader_ThreadRun(nativeManager);

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "exit");
}

JNIEXPORT jint JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterDownloader_nativeCancelThread(JNIEnv *env, jobject jThis, jlong jManager)
{
    ARUPDATER_Manager_t *nativeManager = (ARUPDATER_Manager_t*)(intptr_t)jManager;
    eARUPDATER_ERROR result = ARUPDATER_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "");

    result = ARUPDATER_Downloader_CancelThread(nativeManager);

    return result;
}


/**
 * @brief Get the ARUpdaterShouldDownloadPlfListener, ARUpdaterPlfDownloadProgressListener and ARUpdaterPlfDownloadCompletionListener JNI classes
 * @param env The java env
 * @retval JNI_TRUE if Success, else JNI_FALSE
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
int ARUPDATER_JNI_Downloader_NewListenersJNI(JNIEnv *env)
{
    jclass classDownloaderProgressListener = NULL;
    jclass classDownloaderCompletionListener = NULL;
    jclass classDownloaderShouldDownloadListener = NULL;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "");

    if (env == NULL)
    {
        error = JNI_FAILED;
    }

    if (methodId_DownloaderListener_onPlfDownloadProgress == NULL)
    {
        if (error == JNI_OK)
        {
            classDownloaderProgressListener = (*env)->FindClass(env, "com/parrot/arsdk/arupdater/ARUpdaterPlfDownloadProgressListener");

            if (classDownloaderProgressListener == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "ARUpdaterPlfDownloadProgressListener class not found");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            methodId_DownloaderListener_onPlfDownloadProgress = (*env)->GetMethodID(env, classDownloaderProgressListener, "onPlfDownloadProgress", "(Ljava/lang/Object;I)V");

            if (methodId_DownloaderListener_onPlfDownloadProgress == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "Listener didProgress method not found");
                error = JNI_FAILED;
            }
        }
    }

    if (methodId_DownloaderListener_onPlfDownloadComplete == NULL)
    {
        if (error == JNI_OK)
        {
            classDownloaderCompletionListener = (*env)->FindClass(env, "com/parrot/arsdk/arupdater/ARUpdaterPlfDownloadCompletionListener");

            if (classDownloaderCompletionListener == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "ARUpdaterPlfDownloadCompletionListener class not found");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            methodId_DownloaderListener_onPlfDownloadComplete = (*env)->GetMethodID(env, classDownloaderCompletionListener, "onPlfDownloadComplete", "(Ljava/lang/Object;Lcom/parrot/arsdk/arupdater/ARUPDATER_ERROR_ENUM;)V");

            if (methodId_DownloaderListener_onPlfDownloadComplete == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "Listener didComplete method not found");
                error = JNI_FAILED;
            }
        }
    }

    if (methodId_DownloaderListener_downloadPlf == NULL)
    {
        if (error == JNI_OK)
        {
            classDownloaderShouldDownloadListener = (*env)->FindClass(env, "com/parrot/arsdk/arupdater/ARUpdaterShouldDownloadPlfListener");

            if (classDownloaderShouldDownloadListener == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "ARUpdaterShouldDownloadPlfListener class not found");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            methodId_DownloaderListener_downloadPlf = (*env)->GetMethodID(env, classDownloaderShouldDownloadListener, "downloadPlf", "(Ljava/lang/Object;Z)V");

            if (methodId_DownloaderListener_downloadPlf == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "Listener downloadPlf method not found");
                error = JNI_FAILED;
            }
        }
    }

    return error;
}

/**
 * @brief Callback that give the download progress percent
 * @param arg The arg
 * @param percent The progress percent
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_ProgressCallback(void* arg, uint8_t percent)
{
    ARUPDATER_JNI_DownloaderCallbacks_t *callbacks = (ARUPDATER_JNI_DownloaderCallbacks_t*)arg;

    if (callbacks != NULL)
    {
        if ((ARUPDATER_JNI_Manager_VM != NULL) && (callbacks->jProgressListener != NULL) && (methodId_DownloaderListener_onPlfDownloadProgress != NULL))
        {
            JNIEnv *env = NULL;
            jint jPercent = 0;
            jint jResultEnv = 0;
            int error = JNI_OK;

            jResultEnv = (*ARUPDATER_JNI_Manager_VM)->GetEnv(ARUPDATER_JNI_Manager_VM, (void **) &env, JNI_VERSION_1_6);

            if (jResultEnv == JNI_EDETACHED)
            {
                 (*ARUPDATER_JNI_Manager_VM)->AttachCurrentThread(ARUPDATER_JNI_Manager_VM, &env, NULL);
            }

            if (env == NULL)
            {
                error = JNI_FAILED;
            }

            if ((error == JNI_OK) && (methodId_DownloaderListener_onPlfDownloadProgress != NULL))
            {
                jPercent = percent;

                (*env)->CallVoidMethod(env, callbacks->jProgressListener, methodId_DownloaderListener_onPlfDownloadProgress, callbacks->jProgressArgs, jPercent);
            }

            if ((jResultEnv == JNI_EDETACHED) && (env != NULL))
            {
                 (*ARUPDATER_JNI_Manager_VM)->DetachCurrentThread(ARUPDATER_JNI_Manager_VM);
            }
        }
    }
}


/**
 * @brief Callback that give the download completion status
 * @param arg The arg
 * @param nativeError The error status of the plf download
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_CompletionCallback(void* arg, eARUPDATER_ERROR nativeError)
{
    ARUPDATER_JNI_DownloaderCallbacks_t *callbacks = (ARUPDATER_JNI_DownloaderCallbacks_t*)arg;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "%x, %d", (int)arg, nativeError);

    if (callbacks != NULL)
    {
        if (ARUPDATER_JNI_Manager_VM != NULL)
        {
            JNIEnv *env = NULL;
            jobject jError = NULL;
            jint jResultEnv = 0;

            jResultEnv = (*ARUPDATER_JNI_Manager_VM)->GetEnv(ARUPDATER_JNI_Manager_VM, (void **) &env, JNI_VERSION_1_6);

            if (jResultEnv == JNI_EDETACHED)
            {
                 (*ARUPDATER_JNI_Manager_VM)->AttachCurrentThread(ARUPDATER_JNI_Manager_VM, &env, NULL);
            }

            if (env == NULL)
            {
                //error = JNI_FAILED;
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "error no env");
            }

            if ((env != NULL) && (callbacks->jCompletionListener != NULL) && (methodId_DownloaderListener_onPlfDownloadComplete != NULL))
            {
                int error = JNI_OK;


                if (error == JNI_OK)
                {
                    jError = ARUPDATER_JNI_Manager_NewERROR_ENUM(env, nativeError);

                    if (jError == NULL)
                    {
                        error = JNI_FAILED;
                        ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "error %d, %x", error, jError);
                    }
                }

                if ((error == JNI_OK) && (methodId_DownloaderListener_onPlfDownloadComplete != NULL))
                {
                     (*env)->CallVoidMethod(env, callbacks->jCompletionListener, methodId_DownloaderListener_onPlfDownloadComplete, callbacks->jCompletionArgs, jError);
                }
            }

            if (env != NULL)
            {
                if (jError != NULL)
                {
                    (*env)->DeleteLocalRef(env, jError);
                }

                ARUPDATER_JNI_Downloader_FreeDownloaderCallbacks(env, &callbacks);
            }

            if ((jResultEnv == JNI_EDETACHED) && (env != NULL))
            {
                (*ARUPDATER_JNI_Manager_VM)->DetachCurrentThread(ARUPDATER_JNI_Manager_VM);
            }
        }

        if (callbacks != NULL)
        {
            free(callbacks);
        }
    }
}

/**
 * @brief Callback that indicates if a plf should be download
 * @param arg The arg
 * @param shouldDownload 1 if a plf should be downloaded, else 0
 * @retval void
 * @see ARUPDATER_JNI_Downloader_FreeListenersJNI
 */
void ARUPDATER_JNI_Downloader_ShouldDownloadCallback(void* arg, int shouldDownload)
{
    ARUPDATER_JNI_DownloaderCallbacks_t *callbacks = (ARUPDATER_JNI_DownloaderCallbacks_t*)arg;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "%d %x", shouldDownload, (int)arg);

    if (callbacks != NULL)
    {
        if (ARUPDATER_JNI_Manager_VM != NULL)
        {
            JNIEnv *env = NULL;
            jint jResultEnv = 0;
            jobject jMedia = NULL;
            int error = JNI_OK;

            jResultEnv = (*ARUPDATER_JNI_Manager_VM)->GetEnv(ARUPDATER_JNI_Manager_VM, (void **) &env, JNI_VERSION_1_6);

            if (jResultEnv == JNI_EDETACHED)
            {
                 (*ARUPDATER_JNI_Manager_VM)->AttachCurrentThread(ARUPDATER_JNI_Manager_VM, &env, NULL);
            }

            if (env == NULL)
            {
                error = JNI_FAILED;
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "error no env");
            }

            if ((error == JNI_OK) && (env != NULL) && (callbacks->jDownloadListener != NULL) && (methodId_DownloaderListener_downloadPlf != NULL))
            {
                (*env)->CallVoidMethod(env, callbacks->jDownloadListener, methodId_DownloaderListener_downloadPlf, callbacks->jDownloadArgs, (jboolean)shouldDownload);
            }

            if ((env != NULL) && (jMedia != NULL))
            {
                (*env)->DeleteLocalRef(env, jMedia);
            }

            if ((jResultEnv == JNI_EDETACHED) && (env != NULL))
            {
                (*ARUPDATER_JNI_Manager_VM)->DetachCurrentThread(ARUPDATER_JNI_Manager_VM);
            }
        }
    }
}


/**
 * @brief Free the ARUpdaterShouldDownloadPlfListener, ARUpdaterPlfDownloadProgressListener and ARUpdaterPlfDownloadCompletionListener JNI classes
 * @param env The java env
 * @retval void
 * @see ARUPDATER_JNI_Downloader_NewListenersJNI
 */
void ARUPDATER_JNI_Downloader_FreeListenersJNI(JNIEnv *env)
{
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "");

    if (env == NULL)
    {
        error = JNI_FAILED;
    }

    if (error == JNI_OK)
    {
        methodId_DownloaderListener_onPlfDownloadProgress = NULL;
        methodId_DownloaderListener_onPlfDownloadComplete = NULL;
        methodId_DownloaderListener_downloadPlf = NULL;
    }
}








/**
 * @brief Free Callbacks structure
 * @warning This function frees memory
 * @param env The java env
 * @param callbacks The callbacks structure
 * @retval void
 * @see ARUPDATER_JNI_DownloaderCallbacks_t
 */
void ARUPDATER_JNI_Downloader_FreeDownloaderCallbacks(JNIEnv *env, ARUPDATER_JNI_DownloaderCallbacks_t **callbacksParam)
{
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_DOWNLOADER_TAG, "%x", callbacksParam ? *callbacksParam : 0);

    if (callbacksParam != NULL)
    {
        ARUPDATER_JNI_DownloaderCallbacks_t *callbacks = *callbacksParam;

        if (callbacks != NULL)
        {
            if (env != NULL)
            {
                if (callbacks->jProgressListener != NULL)
                {
                    (*env)->DeleteGlobalRef(env, callbacks->jProgressListener);
                }

                if (callbacks->jProgressArgs != NULL)
                {
                    (*env)->DeleteGlobalRef(env, callbacks->jProgressArgs);
                }

                if (callbacks->jCompletionListener != NULL)
                {
                    (*env)->DeleteGlobalRef(env, callbacks->jCompletionListener);
                }

                if (callbacks->jCompletionArgs != NULL)
                {
                    (*env)->DeleteGlobalRef(env, callbacks->jCompletionArgs);
                }

                if (callbacks->jDownloadListener != NULL)
                {
                    (*env)->DeleteGlobalRef(env, callbacks->jDownloadListener);
                }

                if (callbacks->jDownloadArgs != NULL)
                {
                    (*env)->DeleteGlobalRef(env, callbacks->jDownloadArgs);
                }


            }

            free(callbacks);
        }

        *callbacksParam = NULL;
    }
}
