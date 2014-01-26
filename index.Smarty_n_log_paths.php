<?php

try {
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
	$log->lwrite(' ');
	$log->lwrite('   Start');

	$Smarty = new Smarty;
	$Smarty->template_dir = "templates";
	$Smarty->compile_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/compile_dir";
	$Smarty->config_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/config_dir";
	$Smarty->cache_dir = "/home/rickchurch11/Downloads/Smarty-3.1.5/cache_dir";

	$db = new pders_pdo();
	$query = "select UserId, FirstName, LastName from USERS";
	$result = $db->Query($query);

	// we need to know if $result is a str containing the err msg or an oject 
	//    containing our result set
	$whatisit = gettype($result);
	$log->lwrite("\$result type is: ".$whatisit);
	if ($whatisit == "object") {
		// We have a normal result set returned
		$parseResult = True;
	} else {
		// Had an error executing the query
		$parseResult = False;
		$log->lwrite("DB ERROR: ".$result);
	}
	if (!$parseResult) {
		exit("Done");
	}

	//print_r($result);
	$info = array();
	foreach ($result AS $row) {
		$id = $row['UserId'];
		$fname = $row['FirstName'];
                $lname = $row['LastName'];
       	        $log->lwrite("DB Entry:" . $id . "   " . $fname ."   " . $lname);
                $info[] = array("id"=>$id, "fname"=>$fname, "lname"=>$lname);
	}
	$msg = "Total records returned: " . $result->rowCount();
	$log->lwrite($msg);

	//$userRes->free();   // free up the memory 
	//print_r($info2);  // to test what the array looks like
	$Smarty->assign("info", $info);
	$Smarty->display("main.tpl");

	
	//throw new exception("Hello - I am broke..");
	
	$log->lwrite('Script Done');
}
catch (Exception $e) {
        $msg = "ERROR: (File: " . $e->getFile() . "  Line: " . $e->getLine() . "): " . $e->getMessage();
	$log->lwrite($msg);
}




?>
