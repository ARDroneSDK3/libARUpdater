
package com.parrot.arsdk.arupdater;

public interface ARUpdaterShouldDownloadPlfListener
{
	/**
     * Download the plf
     * @param arg Object Listener arg
     * @param nbPlfToBeUploaded : number of plf which are out to date
     * @return void
     */
	void downloadPlf(Object arg, int nbPlfToBeUploaded);
}