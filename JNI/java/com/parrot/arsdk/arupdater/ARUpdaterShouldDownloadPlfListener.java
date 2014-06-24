
package com.parrot.arsdk.arupdater;

public interface ARUpdaterShouldDownloadPlfListener
{
	/**
     * Download the plf
     * @param arg Object Listener arg
     * @param nbPlfToBeUploaded : number of plf which are out to date
     * @param error The error status to indicate the plf check status
     * @return void
     */
	void downloadPlf(Object arg, int nbPlfToBeUploaded, ARUPDATER_ERROR_ENUM error);
}
