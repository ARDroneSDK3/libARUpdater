/*
    Copyright (C) 2014 Parrot SA

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the 
      distribution.
    * Neither the name of Parrot nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/
package com.parrot.testarupdater;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import com.parrot.arsdk.ardatatransfer.ARDATATRANSFER_ERROR_ENUM;
import com.parrot.arsdk.ardiscovery.ARDISCOVERY_PRODUCT_ENUM;
import com.parrot.arsdk.arupdater.ARUPDATER_ERROR_ENUM;
import com.parrot.arsdk.arupdater.ARUpdaterDownloader;
import com.parrot.arsdk.arupdater.ARUpdaterException;
import com.parrot.arsdk.arupdater.ARUpdaterManager;
import com.parrot.arsdk.arupdater.ARUpdaterPlfDownloadCompletionListener;
import com.parrot.arsdk.arupdater.ARUpdaterPlfDownloadProgressListener;
import com.parrot.arsdk.arupdater.ARUpdaterPlfUploadCompletionListener;
import com.parrot.arsdk.arupdater.ARUpdaterPlfUploadProgressListener;
import com.parrot.arsdk.arupdater.ARUpdaterShouldDownloadPlfListener;
import com.parrot.arsdk.arupdater.ARUpdaterUploader;
import com.parrot.arsdk.arutils.ARUTILS_ERROR_ENUM;
import com.parrot.arsdk.arutils.ARUtilsException;
import com.parrot.arsdk.arutils.ARUtilsManager;

public class MainActivity extends Activity {
	
	private static final String TAG = "TestARUpdater";

	private Button mStartTestUploadByWifiButton;
	private Button mStartTestDownloadByWifiButton;
	
	private boolean isUploadByWifiRunning;
	
	private ARUpdaterShouldDownloadPlfListener mDownloadListener = new ARUpdaterShouldDownloadPlfListener() {
		
		@Override
		public void downloadPlf(Object arg, boolean shouldDownload) {
			if (shouldDownload) {
				Log.d(TAG, "PLF should be downloaded");
			}
			else {
				Log.d(TAG, "No need to download the plf :)");
			}
		}
	};
	
	private ARUpdaterPlfDownloadProgressListener mDownloaderProgressListener = new ARUpdaterPlfDownloadProgressListener() {
		
		@Override
		public void onPlfDownloadProgress(Object arg, int progress) {
			Log.d(TAG, "downloading plf : [" + progress + "]%");
		}
	};
	
	private ARUpdaterPlfDownloadCompletionListener mDownloadCompletionListener = new ARUpdaterPlfDownloadCompletionListener() {
		
		@Override
		public void onPlfDownloadComplete(Object arg, ARUPDATER_ERROR_ENUM result) {
			Log.d(TAG, "download complete. Result : [" + result.name() + "]");
			runOnUiThread(new Runnable() {
				
				@Override
				public void run() {
					mStartTestDownloadByWifiButton.setEnabled(true);
				}
			});
			
		}
	};
	
	
	private ARUpdaterPlfUploadProgressListener mUploadProgressListener = new ARUpdaterPlfUploadProgressListener() {
		
		@Override
		public void onPlfUploadProgress(Object arg, int progress) {
			Log.d(TAG, "uploading plf : [" + progress + "]%");
		}
	};
	
	private ARUpdaterPlfUploadCompletionListener mUploadCompletionListener = new ARUpdaterPlfUploadCompletionListener() {
		
		@Override
		public void onPlfUploadComplete(Object arg, ARDATATRANSFER_ERROR_ENUM result) {
			Log.d(TAG, "upload complete. Result : [" + result.name() + "]");
			runOnUiThread(new Runnable() {
				
				@Override
				public void run() {
					mStartTestUploadByWifiButton.setEnabled(true);
				}
			});
			
		}
	};
	
	
	static {
		System.loadLibrary("curl");
		System.loadLibrary("arsal");
		System.loadLibrary("arsal_android");
		System.loadLibrary("arutils");
		System.loadLibrary("arutils_android");
		System.loadLibrary("ardiscovery");
		System.loadLibrary("ardiscovery_android");
		System.loadLibrary("ardatatransfer");
		System.loadLibrary("ardatatransfer_android");
		System.loadLibrary("arupdater");
		System.loadLibrary("arupdater_android");
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mStartTestUploadByWifiButton = (Button)findViewById(R.id.start_upload_by_wifi_test_button);
		mStartTestUploadByWifiButton.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				mStartTestUploadByWifiButton.setEnabled(false);
				startWifiUploadTest();
			}
		});
		
		mStartTestDownloadByWifiButton = (Button)findViewById(R.id.start_download_by_wifi_test_button);
		mStartTestDownloadByWifiButton.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				mStartTestDownloadByWifiButton.setEnabled(false);
				startWifiDownloadTest();
			}
		});
		
	}
	
	private void startWifiDownloadTest() {
		ARUpdaterManager updaterManager = null;
		try {
			updaterManager = new ARUpdaterManager();
		} catch (ARUpdaterException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (updaterManager != null) {
			ARUpdaterDownloader downloader = updaterManager.getARUpdaterDownloader();
			
			try {
				downloader.createUpdaterDownloader(this.getFilesDir().getAbsolutePath(), mDownloadListener, null, mDownloaderProgressListener, null, mDownloadCompletionListener, null);
				Runnable downloaderRunnable = downloader.getDownloaderRunnable();
				if (downloaderRunnable != null) {
					Thread t = new Thread(downloaderRunnable);
					Log.d(TAG, "Starting downloading thread");
					t.start();
				}
			} catch (ARUpdaterException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			
			
		}
	}
	
	
	private void startWifiUploadTest() {
		Log.d(TAG, "Starting wifi uploading test");
		ARUpdaterManager updaterManager = null;
		try {
			updaterManager = new ARUpdaterManager();
		} catch (ARUpdaterException e) {
			Log.w(TAG, "Failed to init ARUpdaterManager");
			e.printStackTrace();
		}
		if (updaterManager != null) {
			ARUpdaterUploader uploader = updaterManager.getARUpdaterUploader();
			ARUtilsManager utilsManager = null;
			ARUTILS_ERROR_ENUM utilsError = ARUTILS_ERROR_ENUM.ARUTILS_OK;
			try {
				utilsManager = new ARUtilsManager();
				utilsError = utilsManager.initWifiFtp("172.20.5.36", 21, "anonymous", "");
			} catch (ARUtilsException e1) {
				Log.w(TAG, "Failed to init ARUtilsManager");
				e1.printStackTrace();
			}
			
			if (utilsManager != null && utilsError == ARUTILS_ERROR_ENUM.ARUTILS_OK) {
				Log.d(TAG, "Creating uploader ");
				try {
					uploader.createUpdaterUploader(this.getFilesDir().getAbsolutePath(), utilsManager, ARDISCOVERY_PRODUCT_ENUM.ARDISCOVERY_PRODUCT_JS, mUploadProgressListener, null, mUploadCompletionListener, null);
					Runnable uploaderRunnable = uploader.getUploaderRunnable();
					if (uploaderRunnable != null) {
						Thread t = new Thread(uploaderRunnable);
						Log.d(TAG, "Starting uploading thread");
						t.start();
					}
					
				} catch (ARUpdaterException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
			
			
			
		}
	}
}
