<?php

//sample
//http://172.20.5.146/Drones/0902/update.php?product=0900&serialNo=1234&version=0.20.0
//reply
//5|http://172.20.5.146/Drones/0902/delos_lucie_updater_payload.plf|9c66761e523a08682dcb676f60e6d58d
//0|Up to Date

$ERROR_OK           ='0';
$ERROR_UNKNOWN      ='1';
$ERROR_FILE         ='2';
$ERROR_PLF          ='3';
$ERROR_MAGIC_PLF    ='4';
$ERROR_UPDATE       ='5';

$TOKEN="|";

function sendErrorResponse($code, $errorMsg)
{
    global $TOKEN;
    echo $code . $TOKEN . $errorMsg;
}

function GetFileName($folder,$extension)
{
    $myDirectory = opendir($folder);
    $foundFile = FALSE;

    // get each entry
    while($entryName = readdir($myDirectory))
    {
        //echo $entryName.'<br />';
		if((true == stristr($entryName,$extension)))
        {
			$foundFile = $entryName;
			//echo $entryName.'<br />';
			break;
		}
    }

    // close directory
    closedir($myDirectory);
    return $foundFile;
}

function GetPlfVersion($fileName)
{
    $ret=FALSE;
    $file_handle = fopen($fileName, "rb");

    if($file_handle != FALSE)
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

        if($p_magic == FALSE || $p_ver == FALSE || $p_edt == FALSE || p_ext == FALSE)
        {
            $ret=FALSE;
            sendErrorResponse($ERROR_PLF, "read error\n");
        }

        $ver=((ord($p_ver[0]))<<0)+((ord($p_ver[1]))<<8)+((ord($p_ver[2]))<<16)+((ord($p_ver[3]))<<24);
        $edt=((ord($p_edt[0]))<<0)+((ord($p_edt[1]))<<8)+((ord($p_edt[2]))<<16)+((ord($p_edt[3]))<<24);
        $ext=((ord($p_ext[0]))<<0)+((ord($p_ext[1]))<<8)+((ord($p_ext[2]))<<16)+((ord($p_ext[3]))<<24);

        $magic=((ord($p_magic[0]))<<0)+((ord($p_magic[1]))<<8)+((ord($p_magic[2]))<<16)+((ord($p_magic[3]))<<24);
        $size=((ord($p_size[0]))<<0)+((ord($p_size[1]))<<8)+((ord($p_size[2]))<<16)+((ord($p_size[3]))<<24);

        //Verify Magic
        if($ret == FALSE)
        {
            //Error occured, and reported, go to close
        }
        if($magic != 0x21464c50)// ==  "PLF!"
        {
            $ret=FALSE;
            sendErrorResponse(ERROR_MAGIC_PLF,"magic wrong ".$magic."<br>");
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
        else
        {
            $ret=$ver.".".$edt.".".$ext;
        }

        //echo $ret;
        fclose($file_handle);
    }
    else
    {
        $ret=FALSE;
        sendErrorResponse($ERROR_FILE, "file open");
    }

    return $ret;
}

function isUpToDate($last_version, $actual_version)
{
	if ($DEBUG==TRUE)
	{
        echo 'file:  ' . $last_version . '<br/>';
        echo 'url: ' . $actual_version . '<br/>';
	}

	$last = sscanf($last_version,"%d.%d.%d",$last_ver,$last_edt,$last_ext);
	$actual = sscanf($actual_version,"%d.%d.%d",$actual_ver,$actual_edt,$actual_ext);

	if($last_ver > $actual_ver)
		return FALSE;
	if($last_ver == $actual_ver && $last_edt > $actual_edt)
		return FALSE;
	if($last_ver == $actual_ver && $last_edt == $actual_edt && $last_ext > $actual_ext)
		return FALSE;

	//Version is up to date!
	return TRUE;
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

$DEBUG=FALSE;
//$DEBUG=TRUE;
if ($DEBUG == TRUE)
{
    echo 'Debug mode with verbose info: <br />';
}

if ( isset($_GET["product"]) && isset($_GET["serialNo"]) && isset($_GET["version"]))
{
    $product = $_GET["product"];
    $serialNo = $_GET["serialNo"];
    $version = $_GET["version"];
    $file = FALSE;

    if ($DEBUG == TRUE)
    {
        echo 'Product: ['.$product.']['.$serialNo.']['.$version.']';
        echo '<br />';
    }

    $ret = GetFileName('./', '.plf');
    $file = $ret;
    if ($DEBUG == TRUE)
    {
        echo 'PLF: '.$ret;
        echo '<br />';
    }

    if ($ret != FALSE)
    {
        $ret = GetPlfVersion($ret);
        if ($DEBUG == TRUE)
        {
            echo 'file version: ' . $ret;
            echo '<br />';
        }
    }

    if ($ret != FALSE)
    {
        $ret = isUpToDate($ret, $version);
        if ($DEBUG == TRUE)
        {
            echo 'isUpToDate: ' . (($ret == TRUE) ? 'true' : 'false');
            echo '<br />';
        }
    }

    if ($ret == FALSE)
    {
        $url = getUrlPath($file);
        $md5 = md5_file($file);
        if ($DEBUG == TRUE)
        {
            echo $url . ',' . $md5;
            echo '<br />';
        }

        sendErrorResponse($ERROR_UPDATE, $url . $TOKEN . $md5);
    }
    else
    {
        sendErrorResponse($ERROR_OK, 'Up to Date');
    }
}
else
{
    sendErrorResponse($ERROR_UNKNOWN, 'Bad Request');
}
?>
