<?php

session_start();

try {
	//echo "start here";

	require_once "include/logger.inc";	
        require_once "include/pders_db.inc";
        require_once "include/functions.inc";
        require_once "/home/rickchurch11/Downloads/Smarty-3.1.5/libs/Smarty.class.php";
        
        date_default_timezone_set('America/Denver');
        //echo date('D,F j, Y, h:i:s A'). "<br>";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/www/logs/pders.log');
        // write message to the log file 
        $log->lwrite(__FILE__, ' ');
        $log->lwrite(__FILE__, '   Start');
	$log->lwrite(__FILE__, sprintf("UserName: %s", $_SESSION['userName']));
	$log->lwrite(__FILE__, 'Resetting the login.');
	$_SESSION['login'] = "";
	
	$dates = array("01"=>"Jan", "02"=>"Feb", "03"=>"Mar", "04"=>"Apr", "05"=>"May", "06"=>"Jun", "07"=>"Jul", "08"=>"Aug", "09"=>"Sept", "10"=>"Oct", "11"=>"Nov", "12"=>"Dec");
	
        $Smarty = new Smarty;
        $Smarty->template_dir = "templates";
        $Smarty->compile_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/compile_dir";
        $Smarty->config_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/config_dir";
        $Smarty->cache_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/cache_dir";


	/*
	$db = new pders_pdo();
	$query = "select LI_AcctNumber, LI_DisplayPosition from LINE_ITEMS where LI_DisplayPosition is NULL order by LI_AcctNumber";
	$result = $db->Query($query);
	$cntr = 85;
	foreach ($result as $row) {
		$acctNum = $row['LI_AcctNumber'];
		$displayPos = $row['LI_DisplayPosition'];
		$log->lwrite(__FILE__, sprintf("acct NUm: %s      display pos: %s", $acctNum, $displayPos));
		if ($acctNum > 4000 ) {
			$query2 = sprintf("update LINE_ITEMS set LI_DisplayPosition = %d where LI_AcctNumber = %s", $cntr, $acctNum);
			$result = $db->Query($query2);
			$cntr += 5;
		}

	}
	*/


	$disDates = getList_reportDates();
	
        $Smarty->assign("dates", $disDates);
        $Smarty->display("main.tpl");

	exit();
        //throw new exception("Hello - I am broke..");

        $log->lwrite(__FILE__, 'Script Done');
}
catch (Exception $e) {
        $msg = "ERROR: (File: " . $e->getFile() . "  Line: " . $e->getLine() . "): " . $e->getMessage();
        $log->lwrite(__FILE__, $msg);
}


//balanceMonthlyNumbers("201102", False, False, True);
exit("This error coming from index.php");





?>

