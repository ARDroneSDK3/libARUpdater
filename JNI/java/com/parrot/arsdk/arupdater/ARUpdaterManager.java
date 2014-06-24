
package com.parrot.arsdk.arupdater;

import java.lang.Runnable;
import com.parrot.arsdk.arsal.ARSALPrint;
import com.parrot.arsdk.ardiscovery.ARDISCOVERY_PRODUCT_ENUM;
import com.parrot.arsdk.arutils.ARUtilsManager;


public class ARUpdaterManager
{

	private static final String TAG = "ARUpdaterManager";

	/* Native Functions */
	private static native void nativeStaticInit();
    private native int nativeNew() throws ARUpdaterException;
    private native int nativeDelete(long manager);
    private native boolean nativePlfVersionIsUpToDate(long manager, String rootFolder, int discoveryProduct, int version, int edition, int extension) throws ARUpdaterException;
    private native boolean nativePlfVersionIsBlacklisted(int discoveryProduct, int version, int edition, int extension);

    private long nativeManager = 0;
    private String localVersion = null;
    private boolean isInit = false;

    // Uploader from Android device to product
    ARUpdaterUploader uploader;

    // Downloader from Internet to Android device
    ARUpdaterDownloader downloader;

    /*  Static Block */
    static
    {
        nativeStaticInit();
    }  

    public ARUpdaterManager() throws ARUpdaterException
    {
        if (!isInit)
        {
            nativeManager = nativeNew();
        }
        if (nativeManager != 0)
        {
            isInit = true;
        }
    }


	/**
     * Deletes the ARUpdater Manager
     */
    public void dispose()
    {
        if (nativeManager != 0)
        {
            if (downloader != null)
            {
                downloader.dispose();
            }

            if (uploader != null)
            {
                uploader.dispose();
            }

            nativeDelete(nativeManager);
            nativeManager = 0;
            isInit = false;
        }
        
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
                dispose();
            }
        }
        finally
        {
            super.finalize ();
        }
    }

    /**
     * Gets the Manager status Initialized or not
     * @return true if the Manager is already created else false
     */     
    public boolean isInitialized()
    {
        return isInit;
    }


    /**
     * Gets the ARUpdater Downloader object {@link ARUpdaterDownloader}
     * @return an ARUpdater Downloader object, null if Manager not initialized
     */     
    public ARUpdaterDownloader getARUpdaterDownloader()
    {
        if (isInit == false)
        {
            return null;
        }
        
        if (downloader == null)
        {
            downloader = new ARUpdaterDownloader(nativeManager);
        }
        
        return downloader;
    }
    
    /**
     * Gets the ARUpdater Uploader object {@link ARUpdaterUploader}
     * @return an ARUpdater Uploader object, null if Manager not initialized
     */
    public ARUpdaterUploader getARUpdaterUploader()
    {
        if (isInit == false)
        {
            return null;
        }
        
        if (uploader == null)
        {
            uploader = new ARUpdaterUploader(nativeManager);
        }
        
        return uploader;
    }

    /**
     * Test if the plf in the root folder is up to date compare to the given product information
     * @return true is the plf is up to date, false otherwise
     * @throws ARUpdaterException throws ARUpdaterException if there is a bad parameter or if there is no plf file in the root folder
     */
    public boolean isPlfVersionUpToDate(String rootFolder, ARDISCOVERY_PRODUCT_ENUM product, int version, int edition, int extension, StringBuilder localVersionBuilder) throws ARUpdaterException
    {
        boolean toReturn = nativePlfVersionIsUpToDate(nativeManager, rootFolder, product.getValue(), version, edition, extension);
        if (localVersionBuilder != null)
        {
            int currentSize = localVersionBuilder.length();
            if (currentSize > 0)
            {
                localVersionBuilder.delete(0, currentSize);
            }
            localVersionBuilder.append(this.localVersion);
        }
        return toReturn;
    }

    public boolean isPlfVersionBlacklisted(ARDISCOVERY_PRODUCT_ENUM product, int version, int edition, int extension)
    {
        return nativePlfVersionIsBlacklisted(product.getValue(), version, edition, extension);
    }

}
