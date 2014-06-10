
package com.parrot.arsdk.arupdater;

public interface ARUpdaterPlfUploadProgressListener
{
	/**
     * Give the PLF upload progress
     * @param arg Object Listener arg
     * @param progress the progress in percent
     * @return void
     */
	void onPlfUploadProgress(Object arg, float progress);
}