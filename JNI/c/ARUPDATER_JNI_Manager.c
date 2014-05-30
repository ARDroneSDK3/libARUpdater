/**
 * @file ARUPDATER_Manager.c
 * @brief libARUpdater JNI_Manager c file.
 * @date 19/12/2013
 * @author david.flattin.ext@parrot.com
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

#include "libARUpdater/ARUPDATER_Error.h"
#include "libARUpdater/ARUPDATER_Manager.h"
#include "libARUpdater/ARUPDATER_Downloader.h"
 #include "libARUpdater/ARUPDATER_Uploader.h"

#include "ARUPDATER_JNI.h"

#define ARUPDATER_JNI_MANAGER_TAG       "JNI"

JavaVM* ARUPDATER_JNI_Manager_VM = NULL;
int ARUPDATER_Manager_Count = 0;

jclass classUPDATER_Exception = NULL;
jmethodID methodId_UPDATER_Exception_Init = NULL;

jclass classUPDATER_ERROR_ENUM = NULL;
jmethodID methodId_UPDATER_ERROR_ENUM_getFromValue = NULL;

/*****************************************
 *
 *             Private implementation:
 *
 *****************************************/

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *VM, void *reserved)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "Library has been loaded: (arupdater_android.so) %x", (int)VM);

    ARUPDATER_JNI_Manager_VM = VM;

    return JNI_VERSION_1_6;
}

JNIEXPORT jboolean JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterManager_nativeStaticInit(JNIEnv *env, jclass jClass)
{
    jboolean jret = JNI_FALSE;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "%x", (int)env);

    if (env == NULL)
    {
        error = JNI_FAILED;
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Manager_NewARUpdaterExceptionJNI(env);
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI(env);
    }

    if (error == JNI_OK)
    {
        jret = JNI_TRUE;
    }

    return jret;
}

JNIEXPORT jlong JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterManager_nativeNew(JNIEnv *env, jobject jThis)
{
    ARUPDATER_Manager_t *nativeManager = NULL;
    eARUPDATER_ERROR result = ARUPDATER_OK;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "");

    nativeManager = ARUPDATER_Manager_New(&result);

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Manager_NewARUpdaterExceptionJNI(env);
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI(env);
    }

    if (error != JNI_OK)
    {
        result = ARUPDATER_ERROR_SYSTEM;
    }

    if (result == ARUPDATER_OK)
    {
        ARUPDATER_Manager_Count++;
    }
    else
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARUPDATER_JNI_MANAGER_TAG, "error: %d occurred", result);

        ARUPDATER_JNI_Manager_ThrowARUpdaterException(env, result);
    }

    return (long)nativeManager;
}

JNIEXPORT void JNICALL Java_com_parrot_arsdk_arupdater_ARUpdaterManager_nativeDelete(JNIEnv *env, jobject jThis, jlong jManager)
{
    ARUPDATER_Manager_t *nativeManager = (ARUPDATER_Manager_t*) (intptr_t) jManager;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "");

    ARUPDATER_Manager_Delete (&nativeManager);

    if (ARUPDATER_Manager_Count > 0)
    {
        ARUPDATER_Manager_Count--;

        if (ARUPDATER_Manager_Count == 0)
        {
            ARUPDATER_JNI_Manager_FreeARUpdaterExceptionJNI(env);
            ARUPDATER_JNI_Manager_FreeERROR_ENUM_JNI(env);

            ARUPDATER_JNI_Downloader_FreeListenersJNI(env);
        }
    }
}

/*****************************************
 *
 *             Private implementation:
 *
 *****************************************/

int ARUPDATER_JNI_Manager_NewARUpdaterExceptionJNI(JNIEnv *env)
{
    jclass locClassUPDATER_Exception = NULL;
    int error = JNI_OK;

    if (classUPDATER_Exception == NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "");

        if (env == NULL)
        {
           error = JNI_FAILED;
        }

        if (error == JNI_OK)
        {
            locClassUPDATER_Exception  = (*env)->FindClass(env, "com/parrot/arsdk/arupdater/ARUpdaterException");

            if (locClassUPDATER_Exception == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "ARUpdaterException class not found");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            classUPDATER_Exception = (*env)->NewGlobalRef(env,locClassUPDATER_Exception);

            if (classUPDATER_Exception == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "ARUpdaterException global ref failed");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            methodId_UPDATER_Exception_Init = (*env)->GetMethodID(env, classUPDATER_Exception, "<init>", "(I)V");

            if (methodId_UPDATER_Exception_Init == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "init method not found");
                error = JNI_FAILED;
            }
        }
    }

    return error;
}

void ARUPDATER_JNI_Manager_FreeARUpdaterExceptionJNI(JNIEnv *env)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "");

    if (env != NULL)
    {
        if (classUPDATER_Exception != NULL)
        {
            (*env)->DeleteGlobalRef(env, classUPDATER_Exception);
            classUPDATER_Exception = NULL;
        }

        methodId_UPDATER_Exception_Init = NULL;
    }
}

jobject ARUPDATER_JNI_Manager_NewARUpdaterException(JNIEnv *env, eARUPDATER_ERROR nativeError)
{
    jobject jException = NULL;
    jint jError = JNI_OK;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "%d", nativeError);

    if (env == NULL)
    {
       error = JNI_FAILED;
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Manager_NewARUpdaterExceptionJNI(env);
    }

    if (error == JNI_OK)
    {
         jError = nativeError;

        jException = (*env)->NewObject(env, classUPDATER_Exception, methodId_UPDATER_Exception_Init, jError);
    }

    return jException;
}

void ARUPDATER_JNI_Manager_ThrowARUpdaterException(JNIEnv *env, eARUPDATER_ERROR nativeError)
{
    jthrowable jThrowable = NULL;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "%d", error);

    if (env == NULL)
    {
       error = JNI_FAILED;
    }

    if (error == JNI_OK)
    {
        jThrowable = ARUPDATER_JNI_Manager_NewARUpdaterException(env, nativeError);

        if (jThrowable == NULL)
        {
           error = JNI_FAILED;
        }
    }

    if (error == JNI_OK)
    {
        (*env)->Throw(env, jThrowable);
    }
}

int ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI(JNIEnv *env)
{
    jclass locClassUPDATER_ERROR_ENUM = NULL;
    int error = JNI_OK;

    if (classUPDATER_ERROR_ENUM == NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "");

        if (env == NULL)
        {
            error = JNI_FAILED;
        }

        if (error == JNI_OK)
        {
            locClassUPDATER_ERROR_ENUM = (*env)->FindClass(env, "com/parrot/arsdk/arupdater/ARUPDATER_ERROR_ENUM");

            if (locClassUPDATER_ERROR_ENUM == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "ARUPDATER_ERROR_ENUM class not found");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            classUPDATER_ERROR_ENUM = (*env)->NewGlobalRef(env, locClassUPDATER_ERROR_ENUM);

            if (classUPDATER_ERROR_ENUM == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "ARUPDATER_ERROR_ENUM global ref failed");
                error = JNI_FAILED;
            }
        }

        if (error == JNI_OK)
        {
            methodId_UPDATER_ERROR_ENUM_getFromValue = (*env)->GetStaticMethodID(env, classUPDATER_ERROR_ENUM, "getFromValue", "(I)Lcom/parrot/arsdk/arupdater/ARUPDATER_ERROR_ENUM;");

            if (methodId_UPDATER_ERROR_ENUM_getFromValue == NULL)
            {
                ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "getFromValue method not found");
                error = JNI_FAILED;
            }
        }
    }

    return error;
}

void ARUPDATER_JNI_Manager_FreeERROR_ENUM_JNI(JNIEnv *env)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "");

    if (env != NULL)
    {
        if (classUPDATER_ERROR_ENUM != NULL)
        {
            (*env)->DeleteGlobalRef(env, classUPDATER_ERROR_ENUM);
            classUPDATER_ERROR_ENUM = NULL;
        }

        methodId_UPDATER_ERROR_ENUM_getFromValue = NULL;
    }
}

jobject ARUPDATER_JNI_Manager_NewERROR_ENUM(JNIEnv *env, eARUPDATER_ERROR nativeError)
{
    jobject jERROR_ENUM = NULL;
    jint jError;
    int error = JNI_OK;

    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARUPDATER_JNI_MANAGER_TAG, "%d", nativeError);

    if (env == NULL)
    {
       error = JNI_FAILED;
    }

    if (error == JNI_OK)
    {
        error = ARUPDATER_JNI_Manager_NewERROR_ENUM_JNI(env);
    }

    if (error == JNI_OK)
    {
        jError = nativeError;

        jERROR_ENUM = (*env)->CallStaticObjectMethod(env, classUPDATER_ERROR_ENUM, methodId_UPDATER_ERROR_ENUM_getFromValue, jError);
    }

    return jERROR_ENUM;
}

