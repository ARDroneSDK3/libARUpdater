
package com.parrot.arsdk.arupdater;


public interface ARUpdaterPlfUploadCompletionListener
{
	/**
     * Called when the plf upload ends
     * @param arg Object Listener arg
     * @param result ARUPDATER_OK if success, else an {@link ARUPDATER_ERROR_ENUM} error code
     * @return void
     */
	void onPlfUploadComplete(Object arg, ARUPDATER_ERROR_ENUM result);
}
