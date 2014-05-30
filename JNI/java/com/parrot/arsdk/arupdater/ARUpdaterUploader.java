
package com.parrot.arsdk.arupdater;

import java.lang.Runnable;
import com.parrot.arsdk.arsal.ARSALPrint;
import com.parrot.arsdk.ardiscovery.ARDISCOVERY_PRODUCT_ENUM;
import com.parrot.arsdk.arutils.ARUtilsManager;


public class ARUpdaterUploader
{

	private static final String TAG = "ARUpdaterUploader";

	/* Native Functions */
	private static native void nativeStaticInit();
    private native int nativeNew(long manager, String rootFolder, long utilsManager, ARDISCOVERY_PRODUCT_ENUM product, 
    	ARUpdaterPlfUploadProgressListener plfUploadProgressListener, Object progressArgs, 
    	ARUpdaterPlfUploadCompletionListener plfUploadCompletionListener, Object completionArgs);
    private native int nativeDelete(long manager);
    private native void nativeThreadRun (long manager);
    private native int nativeCancelThread (long manager);

    private long nativeManager = 0;
    private Runnable uploaderRunnable = null;
    private boolean isInit = false;

    /*  Static Block */
    static
    {
        nativeStaticInit();
    }  

    protected ARUpdaterUploader(long _nativeManager)
    {
    	this.nativeManager = _nativeManager;
    	this.uploaderRunnable = new Runnable() {
    		public void run() {
    			nativeThreadRun(nativeManager);
    		}
    	};
    }


	/**
     * Creates the ARUpdater Uploader
     * @param rootFolder The root folder
     * @param utilsManager The utils manager initialized with the correct network (wifi or ble)
     * @param product see {@link ARDISCOVERY_PRODUCT_ENUM}
     * @param plfUploadProgressListener The available progress listener
     * @param progressArgs The available progress listener arg
     * @param completionArgs The available completion listener
     * @param progressArgs The available completion listener arg
     * @return void
     * @throws ARUpdaterException if error
     */
    public void createUpdaterUploader(String rootFolder, ARUtilsManager utilsManager, ARDISCOVERY_PRODUCT_ENUM product, 
        ARUpdaterPlfUploadProgressListener plfUploadProgressListener, Object progressArgs, 
        ARUpdaterPlfUploadCompletionListener plfUploadCompletionListener, Object completionArgs) throws ARUpdaterException
    {
    	int result = nativeNew(nativeManager, rootFolder, utilsManager.getManager(), product, plfUploadProgressListener, progressArgs, plfUploadCompletionListener, completionArgs);

    	ARUPDATER_ERROR_ENUM error = ARUPDATER_ERROR_ENUM.getFromValue(result);

    	if (error != ARUPDATER_ERROR_ENUM.ARUPDATER_OK)
    	{
    		throw new ARUpdaterException();
    	}
    	else
    	{
    		this.isInit = true;
    	}
    }

	/**
     * Deletes the ARUpdater Uploader
     * @return ARUPDATER_OK if success, else an {@link ARUPDATER_ERROR_ENUM} error code
     */
    public ARUPDATER_ERROR_ENUM dispose()
    {
        ARUPDATER_ERROR_ENUM error = ARUPDATER_ERROR_ENUM.ARUPDATER_OK;
        
        if (isInit)
        {
            int result = nativeDelete(nativeManager);
            
            error = ARUPDATER_ERROR_ENUM.getFromValue(result);
            if (error == ARUPDATER_ERROR_ENUM.ARUPDATER_OK)
            {
                isInit = false;
            }
        }
        
        return error;
    }

    /**
     * Destructor<br>
     * This destructor tries to avoid leaks if the object was not disposed
     */
    protected void finalize () throws Throwable
    {
        try
        {
            if (isInit)
            {
                ARSALPrint.e (TAG, "Object " + this + " was not disposed !");
                ARUPDATER_ERROR_ENUM error = dispose ();
                if(error != ARUPDATER_ERROR_ENUM.ARUPDATER_OK)
                {
                    ARSALPrint.e (TAG, "Unable to dispose object " + this + " ... leaking memory !");
                }
            }
        }
        finally
        {
            super.finalize ();
        }
    }

    public ARUPDATER_ERROR_ENUM cancel()
    {
    	int result = nativeCancelThread(nativeManager);

    	ARUPDATER_ERROR_ENUM error = ARUPDATER_ERROR_ENUM.getFromValue(result);

    	return error;
    }


}