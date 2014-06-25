
package com.parrot.arsdk.arupdater;

import com.parrot.arsdk.ardiscovery.ARDISCOVERY_PRODUCT_ENUM;

public interface ARUpdaterWillDownloadPlfListener
{
	/**
         * @brief Fired just before the uploading of a plf file
         * @param arg The pointer of the user custom argument
         * @param product : Description of the product targeted by the plf downloaded
         * @param remotePlfVersion : The version of the plf file that will be downloaded
         */
	void onWillDownloadPlf(Object arg, ARDISCOVERY_PRODUCT_ENUM product, String remotePlfVersion);
}
