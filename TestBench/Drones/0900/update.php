<?php

//Samples
//http://download.parrot.com/Drones/0902/update.php?product=0900&serialNo=1234&version=0.22.0
//http://172.20.5.146/Drones/0902/update.php?product=0900&serialNo=1234&version=0.20.0
//reply
//5|http://172.20.5.146/Drones/0902/delos_lucie_updater_payload.plf|9c66761e523a08682dcb676f60e6d58d
//0|Up to Date

define ("ERROR_OK" , 0);
define ("ERROR_BAD_REQUEST", 1);
define ("ERROR_PLATFORM_NOT_SUPPORTED", 2);
define ("ERROR_APP_VERSION_OUT_TO_DATE", 3);
define ("ERROR_FILE_NOT_FOUND", 4);
define ("ERROR_SHOULD_UPDATE" , 5);
define ("ERROR_TOO_MANY_PLF", 6);
define ("ERROR_OPENING_FILE", 7);
define ("ERROR_READING_FILE", 8);
define ("ERROR_MAGIC", 9);
define ("ERROR_PARSING_VERSIONS", 10);

define ("TOKEN", "|");

define ("DEBUG", FALSE);
//define ("DEBUG", TRUE);

$firstSupportedVersionArray = array('ANDROID' => '0.0.0', 'IOS' => '0.0.0'); // available app platforms

function sendErrorResponse($code, $errorMsg)
{
    echo $code . TOKEN . $errorMsg;
}

function checkAppVersion($platform, $appVersion)
{
	global $firstSupportedVersionArray;
	$errorToReturn = ERROR_OK;
	
	if (!array_key_exists(strtoupper($platform), $firstSupportedVersionArray))
	{
		$errorToReturn = ERROR_PLATFORM_NOT_SUPPORTED;
	}
	
	if ($errorToReturn == ERROR_OK)
	{
		$firstSupportedVersion = $firstSupportedVersionArray[strtoupper($platform)];
		
		logIfDebug("First supported version : ".$firstSupportedVersion."<br/>");
		
		$nbFirstVersionFound = sscanf($firstSupportedVersion,"%d.%d.%d",$first_ver,$first_edt,$first_ext);
		$nbAppVersionFound = sscanf($appVersion,"%d.%d.%d",$app_ver,$app_edt,$app_ext);
		
		if (($nbFirstVersionFound != 3) || ($nbAppVersionFound != 3))
		{
			logIfDebug("first = ".$nbFirstVersionFound." | app : ".$nbAppVersionFound."<br/>");
			$errorToReturn = ERROR_PARSING_VERSIONS;
		} 
	
		if ($errorToReturn == ERROR_OK) 
		{
			if($first_ver > $app_ver)
			{
				$errorToReturn = ERROR_APP_VERSION_OUT_TO_DATE;
			}
			else if($first_ver == $app_ver && $first_edt > $app_edt)
			{
				$errorToReturn = ERROR_APP_VERSION_OUT_TO_DATE;
			}
			else if($first_ver == $app_ver && $first_edt == $app_edt && $first_ext > $app_ext)
			{
				$errorToReturn = ERROR_APP_VERSION_OUT_TO_DATE;
			}
		}
	}
	
	return $errorToReturn;
}

function GetFileName($folder,$extension, &$error)
{
    $myDirectory = opendir($folder);
    $fileName = NULL;
    $plfCount = 0;

    // get each entry
    while($entryName = readdir($myDirectory))
    {
    	$needle = strrchr($entryName,$extension);
		if($needle == $extension)
        {
			$fileName = $entryName;
			$plfCount = $plfCount + 1;
		}
    }

	if ($plfCount == 0)
	{
		$error = ERROR_FILE_NOT_FOUND;
	}
    else if ($plfCount > 1)
    {
        $error = ERROR_TOO_MANY_PLF;
    }

    // close directory
    closedir($myDirectory);
    return $fileName;
}

function getFileSize($file, &$error)
{
    $fileSize = NULL;

    $fileSize = filesize($file);

    if($fileSize == FALSE)
    {
    	$error = ERROR_OPENING_FILE;
    }
    
    return $fileSize;
}

function GetPlfVersion($fileName, &$error)
{
	$version = NULL;
    $file_handle = fopen($fileName, "rb");
	$ver = NULL;
	$edt = NULL;
	$ext = NULL;
	$magic = NULL;
	$size = NULL;
	$p_ver = NULL;
	$p_edt = NULL;
	$p_ext = NULL;
	$p_magic = NULL;
	$p_size = NULL;
	$error = ERROR_OK;

	if($file_handle == FALSE)
    {
    	$error = ERROR_OPENING_FILE;
    }

	if($error == ERROR_OK)
	{
		//ACcording to /usr/local/shar/plf/include/plf.h
        $p_magic      = @fread ($file_handle,4);
        $p_plfversion = @fread ($file_handle,4);
        $p_phdrsize   = @fread ($file_handle,4);
        $p_shdrsize   = @fread ($file_handle,4);
        $p_type       = @fread ($file_handle,4);
        $p_entry      = @fread ($file_handle,4);
        $p_target     = @fread ($file_handle,4);
        $p_app        = @fread ($file_handle,4);
        $p_crc        = @fread ($file_handle,4);//or header hdw before v13
        $p_ver        = @fread ($file_handle,4);
        $p_edt        = @fread ($file_handle,4);
        $p_ext        = @fread ($file_handle,4);
        $p_lang       = @fread ($file_handle,4);
        $p_size       = @fread ($file_handle,4);

        if($p_magic == FALSE || $p_ver == FALSE || $p_edt == FALSE || $p_ext == FALSE)
        {
            $error = ERROR_READING_FILE;
        }
	}
	
	if($error == ERROR_OK)
	{
		$ver=((ord($p_ver[0]))<<0)+((ord($p_ver[1]))<<8)+((ord($p_ver[2]))<<16)+((ord($p_ver[3]))<<24);
        $edt=((ord($p_edt[0]))<<0)+((ord($p_edt[1]))<<8)+((ord($p_edt[2]))<<16)+((ord($p_edt[3]))<<24);
        $ext=((ord($p_ext[0]))<<0)+((ord($p_ext[1]))<<8)+((ord($p_ext[2]))<<16)+((ord($p_ext[3]))<<24);

        $magic=((ord($p_magic[0]))<<0)+((ord($p_magic[1]))<<8)+((ord($p_magic[2]))<<16)+((ord($p_magic[3]))<<24);
        $size=((ord($p_size[0]))<<0)+((ord($p_size[1]))<<8)+((ord($p_size[2]))<<16)+((ord($p_size[3]))<<24);
        
        if($magic != 0x21464c50)// ==  "PLF!"
        {
            $error = ERROR_MAGIC;
        }
        /*else if($size != @filesize($path))
        {
            $ret=FALSE;
            sendErrorResponse(SIZE_PLF_ERROR,"size wrong ".$size."<br>");
        }
        else if(($zone != null) && (trim($p_lang) != trim(strtoupper($zone))))
        {
            $ret=FALSE;
            sendErrorResponse(LANGUAGE_PLF_ERROR,"language wrong ".SEPARATOR.$p_lang."!=".$zone."<br>");
        }
        else if(strlen($ver) > 1)//new versionning
        {
            $major=floor($ver/10);
            $minor=$ver%10;
            $ret=$major.".".$minor.".".$edt.".".$ext;
            $ret=$ver.".".$edt.".".$ext;
        }*/
	}
	
	if($error == ERROR_OK)
	{
		$version = $ver.".".$edt.".".$ext;
	}
	
	if($error != ERROR_OPENING_FILE)
	{
		fclose($file_handle);
	}
	
    return $version;
}

function isUpToDate($remote_version, $local_version, &$error)
{
	$nbRemoteParamsFound = sscanf($remote_version,"%d.%d.%d",$remote_ver,$remote_edt,$remote_ext);
	$nbLocalParamsFound = sscanf($local_version,"%d.%d.%d",$local_ver,$local_edt,$local_ext);
	
	$isUpToDate = TRUE;
	
	if (($nbRemoteParamsFound != 3) || ($nbLocalParamsFound != 3))
	{
		$error = ERROR_PARSING_VERSIONS;
	} 
	
	if ($error == ERROR_OK) 
	{
		if($local_ver > $remote_ver)
		{
			$isUpToDate = FALSE;
		}
		else if($local_ver == $remote_ver && $local_edt > $remote_edt)
		{
			$isUpToDate = FALSE;
		}
		else if($local_ver == $remote_ver && $local_edt == $remote_edt && $local_ext > $remote_ext)
		{
			$isUpToDate = FALSE;
		}
	}

	return $isUpToDate;
}

function getUrlPath($file)
{
    $url = 'http' . (isset($_SERVER['HTTPS']) ? 's' : '');
    $url = $url . '://' . $_SERVER['SERVER_NAME'];

    $uri = strtok($_SERVER['REQUEST_URI'],'?');
    $path = str_replace("update.php", "", $uri);

    $url = $url . $path . $file;
    return $url;
}

function errorDescription($error)
{
	$description = NULL;
	switch($error)
	{
		case ERROR_OK : 
			$description = "Up to Date";
			break;
		case ERROR_SHOULD_UPDATE : 
			$description = "Not up to Date";
			break;
		case ERROR_BAD_REQUEST : 
			$description = "Error : Bad request";
			break;
		case ERROR_PLATFORM_NOT_SUPPORTED : 
			$description = "Error : Platform is not supported";
			break;
		case ERROR_APP_VERSION_OUT_TO_DATE : 
			$description = "Error : App version is out to date";
			break;
		case ERROR_FILE_NOT_FOUND : 
			$description = "Error : Local plf file not found";
			break;
		case ERROR_TOO_MANY_PLF : 
			$description = "Error : Too many plf files";
			break;
		case ERROR_OPENING_FILE : 
			$description = "Error : Error while opening plf file";
			break;
		case ERROR_READING_FILE : 
			$description = "Error : Error while reading plf file";
			break;
		case ERROR_MAGIC : 
			$description = "Error : Bad Magic";
			break;
		case ERROR_PARSING_VERSIONS : 
			$description = "Error : Error while parsing versions";
			break;
		default :
			$description = "Error : Unknown error ";
	}
	
	return $description;
}

function logIfDebug($message)
{
	if (DEBUG == TRUE)
	{
    	echo $message;
	}
}

function main()
{
	$error = ERROR_OK;
	$product = NULL;
	$serialNo = NULL;
	$remoteVersion = NULL;
	$file = NULL;
	$size = NULL;
	$localVersion = NULL;
	$isUpToDate = NULL;
	$platform = NULL;
	$appVersion = NULL;
	
    logIfDebug('Debug mode with verbose info: <br />');
	
	if ( !isset($_GET["product"]) || !isset($_GET["serialNo"]) || !isset($_GET["version"]) /*|| !isset($_GET["platform"]) || !isset($_GET["appVersion"])*/)
	{
		$error = ERROR_BAD_REQUEST;
	}
	
	if ($error == ERROR_OK)
	{
		$product = $_GET["product"];
		$serialNo = $_GET["serialNo"];
		$remoteVersion = $_GET["version"];
		$platform = $_GET["platform"];
		$appVersion = $_GET["appVersion"];

		logIfDebug('Product: ['.$product.']['.$serialNo.']['.$remoteVersion.']<br/>');
		logIfDebug('App: ['.$platform.']['.$appVersion.']<br/>');
	}
	
	// check app version
	if (($error == ERROR_OK) && isset($_GET["platform"]) && isset($_GET["appVersion"]))
	{
		$error = checkAppVersion($platform, $appVersion);
	}
	
	// get filename
	if ($error == ERROR_OK)
	{
		$ret = GetFileName('./', '.plf', $error);

		if ($error == ERROR_OK)
		{
			$file = $ret;
			logIfDebug('PLF: '.$file.'<br />');
		}
		else
		{
			logIfDebug('Error while getting the plf file name: '.errorDescription($error).' <br/>');
		}
	}

	// get file size
	if ($error == ERROR_OK)
	{
		$ret = GetFileSize($file, $error);
	
		if ($error == ERROR_OK)
		{
			$size = $ret;
			logIfDebug('PLF size: '.$size.'<br />');
		}
		else
		{
			logIfDebug('Error while getting the plf file size: '.errorDescription($error).' <br />');
		}
	}

	// get plf version
	if ($error == ERROR_OK)
	{
		$ret = GetPlfVersion($file, $error);
	
		if ($error == ERROR_OK)
		{
			$localVersion = $ret;
			logIfDebug('PLF version: '.$localVersion.'<br />');
		}
		else
		{
			logIfDebug('Error while getting the version of the local file: '.errorDescription($error).' <br />');
		}
	}

	// check if is up to date
	if ($error == ERROR_OK)
	{
		$ret = isUpToDate($remoteVersion, $localVersion, $error);
		if ($error == ERROR_OK)
		{
			$isUpToDate = $ret;
			
			if(DEBUG == TRUE)
			{
				$isUpToDateStr = ($isUpToDate == TRUE) ? 'true' : 'false';
				logIfDebug('Is up to date: '.$isUpToDateStr.'<br />');
			}
		}
		else
		{
			logIfDebug('Error while checking if file is up to date: '.errorDescription($error).' <br />');
		}
	}

	// if is up to date and everything went well, get url
	if (($error == ERROR_OK) && ($isUpToDate == FALSE))
	{
		$url = getUrlPath($file);
		$md5 = md5_file($file);
		sendErrorResponse(ERROR_SHOULD_UPDATE, $url . TOKEN . $md5 . TOKEN . $size);
	}   
	else
	{
		sendErrorResponse($error, errorDescription($error));
	}
}

main();

?>
