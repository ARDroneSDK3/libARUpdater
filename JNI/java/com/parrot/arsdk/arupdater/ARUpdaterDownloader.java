
package com.parrot.arsdk.arupdater;

import java.lang.Runnable;
import com.parrot.arsdk.arsal.ARSALPrint;
import com.parrot.arsdk.arsal.ARSALMd5Manager;


public class ARUpdaterDownloader
{

	private static final String TAG = "ARUpdaterDownloader";

	/* Native Functions */
	private static native void nativeStaticInit();
    private native int nativeNew(long manager, String rootFolder, long md5Manager, int platform, String appVersion, ARUpdaterShouldDownloadPlfListener shouldDownloadPlfListener, Object downloadArgs, 
    	ARUpdaterPlfDownloadProgressListener plfDownloadProgressListener, Object progressArgs, 
    	ARUpdaterPlfDownloadCompletionListener plfDownloadCompletionListener, Object completionArgs);
    private native int nativeDelete(long manager);
    private native void nativeThreadRun (long manager);
    private native int nativeCancelThread (long manager);
    private native int nativeCheckUpdatesAsync(long manager);
    private native int nativeCheckUpdatesSync(long manager) throws ARUpdaterException;

    private long nativeManager = 0;
    private Runnable downloaderRunnable = null;
    private boolean isInit = false;

    /*  Static Block */
    static
    {
        nativeStaticInit();
    }  

    protected ARUpdaterDownloader(long _nativeManager)
    {
    	this.nativeManager = _nativeManager;
    	this.downloaderRunnable = new Runnable() {
    		public void run() {
    			nativeThreadRun(nativeManager);
    		}
    	};
    }


	/**
     * Creates the ARUpdater Downloader
     * @param rootFolder The root folder
     * @param shouldDownloadPlfListener The available download listener
     * @param downloadArgs The available download listener arg
     * @param plfDownloadProgressListener The available progress listener
     * @param progressArgs The available progress listener arg
     * @param completionArgs The available completion listener
     * @param progressArgs The available completion listener arg
     * @return void
     * @throws ARUpdaterException if error
     */
    public void createUpdaterDownloader(String rootFolder, ARSALMd5Manager md5Manager, String appVersion, ARUpdaterShouldDownloadPlfListener shouldDownloadPlfListener, Object downloadArgs, 
    	ARUpdaterPlfDownloadProgressListener plfDownloadProgressListener, Object progressArgs, 
    	ARUpdaterPlfDownloadCompletionListener plfDownloadCompletionListener, Object completionArgs) throws ARUpdaterException
    {
    	int result = nativeNew(nativeManager, rootFolder, md5Manager.getNativeManager(),  ARUPDATER_Downloader_Platforms_ENUM.ARUPDATER_DOWNLOADER_ANDROID_PLATFORM.getValue(), appVersion, shouldDownloadPlfListener, downloadArgs, plfDownloadProgressListener, progressArgs, plfDownloadCompletionListener, completionArgs);

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
     * Deletes the ARUpdater Downloader
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

    public Runnable getDownloaderRunnable()
    {
        Runnable runnable = null;

        if (isInit == true)
        {
            runnable = this.downloaderRunnable;
        }

        return runnable;
    }

    /**
     * Use this to check asynchronously update from internet (must be called from a background thread)
     * The ARUpdaterPlfShouldDownloadPlfListener callback set in the 'createUpdaterDownloader' method will be called
     */
    public ARUPDATER_ERROR_ENUM checkUpdatesAsync()
    {
        int result = nativeCheckUpdatesAsync(nativeManager);

        ARUPDATER_ERROR_ENUM error = ARUPDATER_ERROR_ENUM.getFromValue(result);

        return error;
    }

    /**
     * Use this to check synchronously update from internet
     */
    public int checkUpdatesSync() throws ARUpdaterException
    {
        int nbPlfToBeUpdated = nativeCheckUpdatesSync(nativeManager);

        return nbPlfToBeUpdated;
    }
}
