<?php

define ("DEBUG", FALSE);
//define ("DEBUG", TRUE);

define ("ERROR_OK" , 0);
define ("ERROR_BAD_REQUEST", 1);

define ("TOKEN", "|");

define ("BLACKLIST_MIN_APP_VERSION", "3.3.1");  // first implementation in the ios and android app of the firmware blacklist (return code ERROR_MUST_UPDATE)

$blacklistedFirmwareVersionArray = array('0900' => array(),
                                         '0901' => array(),
                                         '0902' => array(),
                                         '0903' => array());  // list here all blacklisted versions of firmware => it will forced the user to update firmware before using it


function splitVersion($version, &$ver, &$edt, &$ext)
{
    $nbParamsFound = sscanf($version,"%d.%d.%d",$ver,$edt,$ext);
		
	return $nbParamsFound;
}

function isGreaterThan($ver1, $edt1, $ext1, $ver2, $edt2, $ext2)
{
    $isGreater = FALSE;
    if($ver1 > $ver2)
    {
        $isGreater = TRUE;
    }
    else if($ver1 == $ver2 && $edt1 > $edt2)
    {
        $isGreater = TRUE;
    }
    else if($ver1 == $ver2 && $edt1 == $edt2 && $ext1 > $ext2)
    {
        $isGreater = TRUE;
    }
    
    return $isGreater;
}

function isEqualTo($ver1, $edt1, $ext1, $ver2, $edt2, $ext2)
{
    $isEqual = FALSE;
    if($ver1 == $ver2 && $edt1 == $edt2 && $ext1 == $ext2)
    {
        $isEqual = TRUE;
    }
    
    return $isEqual;
}

function errorDescription($error)
{
	$description = NULL;
	switch($error)
	{
		case ERROR_OK : 
			$description = "OK";
			break;
		case ERROR_BAD_REQUEST : 
			$description = "Error : Bad request";
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

function sendErrorResponse($code, $errorMsg)
{
    echo $code . TOKEN . $errorMsg;
}

function main()
{
    global $blacklistedFirmwareVersionArray;
    
	$error = ERROR_OK;
	$platform = NULL;
	$appVersion = NULL;
	
    logIfDebug('Debug mode with verbose info: <br />');
	
	if (!isset($_GET["platform"]) || !isset($_GET["appVersion"]))
	{
		$error = ERROR_BAD_REQUEST;
	}
	
	if ($error == ERROR_OK)
	{
	    // platform and appVersion is not used for now, but it can be usefull in the future
		$platform = $_GET["platform"];
		$appVersion = $_GET["appVersion"];

		logIfDebug('App: ['.$platform.']['.$appVersion.']<br/>');
	}

	// if is up to date and everything went well, get url
	if ($error == ERROR_OK)
	{
		sendErrorResponse($error, json_encode($blacklistedFirmwareVersionArray));
	}   
	else
	{
		sendErrorResponse($error, errorDescription($error));
	}
}

main();

?>
