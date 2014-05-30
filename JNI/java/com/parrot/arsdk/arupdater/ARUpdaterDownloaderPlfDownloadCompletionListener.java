
package com.parrot.arsdk.arupdater;

public interface ARUpdaterDownloaderPlfDownloadCompletionListener
{
	/**
     * Called when the plf download ends
     * @param arg Object Listener arg
     * @param result ARUPDATER_OK if success, else an {@link ARUPDATER_ERROR_ENUM} error code
     * @return void
     */
	void onPlfDownloadComplete(Object arg, ARUPDATER_ERROR_ENUM result);
}