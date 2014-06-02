
package com.parrot.arsdk.arupdater;

import com.parrot.arsdk.ardatatransfer.ARDATATRANSFER_ERROR_ENUM;

public interface ARUpdaterPlfUploadCompletionListener
{
	/**
     * Called when the plf upload ends
     * @param arg Object Listener arg
     * @param result ARDATATRANSFER_OK if success, else an {@link ARDATATRANSFER_ERROR_ENUM} error code
     * @return void
     */
	void onPlfUploadComplete(Object arg, ARDATATRANSFER_ERROR_ENUM result);
}