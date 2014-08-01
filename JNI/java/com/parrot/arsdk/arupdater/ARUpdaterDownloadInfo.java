
package com.parrot.arsdk.arupdater;

import com.parrot.arsdk.ardiscovery.ARDISCOVERY_PRODUCT_ENUM;
import com.parrot.arsdk.ardiscovery.ARDiscoveryService;

public class ARUpdaterDownloadInfo
{
        public final String downloadUrl;
        public final String plfVersion;
        public final ARDISCOVERY_PRODUCT_ENUM product;

	/**
     * @brief Info about the plf version on server
     */
	public ARUpdaterDownloadInfo(String downloadUrl, String plfVersion, int product)
    {
        this.downloadUrl = downloadUrl;
        this.plfVersion = plfVersion;
        this.product = ARDiscoveryService.getProductFromProductID(product);
    }

}
