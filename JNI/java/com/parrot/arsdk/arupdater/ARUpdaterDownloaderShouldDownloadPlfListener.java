
package com.parrot.arsdk.arupdater;

public interface ARUpdaterDownloaderShouldDownloadPlfListener
{
	/**
     * Download the plf
     * @param arg Object Listener arg
     * @param shouldDownload indicate if the plf should be downloaded
     * @return void
     */
	void downloadPlf(Object arg, boolean shouldDownload);
}