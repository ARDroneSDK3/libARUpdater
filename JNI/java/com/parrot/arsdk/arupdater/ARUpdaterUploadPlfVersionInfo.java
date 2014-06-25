
package com.parrot.arsdk.arupdater;

public class ARUpdaterUploadPlfVersionInfo
{
        private final boolean mIsUpToDate;
        private final String mLocalVersion;

	/**
         * @brief Info about the uploading plf version
         * @param isUpToDate : if the remote version is up to date with the local version
         * @param localVersion : the local plf version
         */
	public ARUpdaterUploadPlfVersionInfo(boolean isUpToDate, String localVersion)
        {
            this.mIsUpToDate = isUpToDate;
            this.mLocalVersion = localVersion;
        }

        public boolean isUpToDate()
        {
            return this.mIsUpToDate;
        }

        public String getLocalVersion()
        {
            return this.mLocalVersion;
        }
}
