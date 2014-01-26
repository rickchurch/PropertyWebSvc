<?php

session_start();

try {
	require_once "include/logger.inc";
        require_once "include/pders_db.inc";
        require_once "include/functions.inc";
	require_once "include/reconcileNumbers.inc";
        require_once "/home/rickchurch11/Downloads/Smarty-3.1.5/libs/Smarty.class.php";

	//echo "here <br>";
	//exit();
	
        date_default_timezone_set('America/Denver');
        //echo date('D,F j, Y, h:i:s A'). "<br>";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');
        // write message to the log file 
        $log->lwrite(__FILE__,'   Start');
	
        $Smarty = new Smarty;
        $Smarty->template_dir = "templates";
        $Smarty->compile_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/compile_dir";
        $Smarty->config_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/config_dir";
        $Smarty->cache_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/cache_dir";

	// ricks old IP was 24.119.91.138 until April 11 2012
	if ( $_SERVER['REMOTE_ADDR'] == "24.116.255.43" ) {
		$log->lwrite(__FILE__,"Rick has arrived and doesn't want to login  ;-)" );
		$_SESSION['userName'] = "churchr";
		$_SESSION['login'] = 'adminpass';
		$disDates = getList_reportDates();

	        $maxDate = "200010";
        	foreach ($disDates AS $tmpDate) {
                	if ($tmpDate['DateValue'] > $maxDate) $maxDate = $tmpDate['DateValue'];
	        }
        	$log->lwrite(__FILE__, sprintf("maxDate: %s",$maxDate));

		$Smarty->assign("login", $_SESSION['login']);
		$Smarty->assign("maxDate", $maxDate);
		$Smarty->assign("dates", $disDates);

		$Smarty->display("mainAdmin.tpl");
		exit();
	}

	$Smarty->display("login.tpl");
	//$Smarty->display("maintenance.tpl");
	exit();
	
}
catch (Exception $e) {
        $msg = "ERROR: (File: " . $e->getFile() . "  Line: " . $e->getLine() . "): " . $e->getMessage();
        $log->lwrite(__FILE__, $msg);
}


?>

