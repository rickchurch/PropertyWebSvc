<?php

session_start();
//header('Content-Type: text/html; charset-UTF-8');



try {
	require_once "include/logger.inc";
        require_once "include/pders_db.inc";
        require_once "include/functions.inc";
        require_once "/home/rickchurch11/Downloads/Smarty-3.1.5/libs/Smarty.class.php";

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

	$username = $_POST['username'];
	$pwd = $_POST['pwd'];

	if ($username == "") {
		// must mean that we are already logged in
		$username = $_SESSION['userName'];
		$login = $_SESSION['login'];

		$graph = False;
		$graphAcctArray = array();
		if (is_array($_POST['chkbxAcct']) ) {
			while ( list($key, $val) = each($_POST['chkbxAcct'])) {
		                $log->lwrite(__FILE__, sprintf("Acct Number to graph: %s", $val ));
				$graphAcctArray[]= $val;
				$graph = True;
			}
		}

	}

	$disDates = getList_reportDates();
	$maxDate = "200010";
	foreach ($disDates AS $tmpDate) {
		if ($tmpDate['DateValue'] > $maxDate) $maxDate = $tmpDate['DateValue'];
	}
	$log->lwrite(__FILE__, sprintf("maxDate: %s",$maxDate));

	$msg = sprintf("userName: %s", $username);
	$log->lwrite(__FILE__, $msg);
	$msg = sprintf("Session userName: %s", $_SESSION['userName']);
        $log->lwrite(__FILE__, $msg);

	$msg = sprintf("Server IP Address: %s", $_SERVER['REMOTE_ADDR']);
        $log->lwrite(__FILE__, $msg);

	$_SESSION['userName'] = $username;

	$proceed = False;
	//  Initial login for me
	if ($username == "churchr" && $pwd == "pw" ) {
		$_SESSION['login'] = 'adminpass';

		$msg = sprintf("userName %s is now logged in.", $username);
		$log->lwrite(__FILE__, $msg);
		$proceed = True;
	//  Initial login for Bob or Legacy owners
	} elseif ( ($username == "churchrm" && $pwd == "cat") || ( $username == "property" && $pwd == "c@shflow" ) ) {
		$_SESSION['login'] = 'pass';

		$msg = sprintf("userName %s is now logged in.", $username);
                $log->lwrite(__FILE__, $msg);
		$proceed = True;
	} elseif ( (($username == "churchrm" || $username == "property") && $login == "pass") || ($username == "churchr" && $login == "adminpass") ) {
                //$_SESSION['login'] = 'pass';   shouldn't need to save login - already have it to get here...

		$msg = sprintf("userName %s still logged in.", $username);
                $log->lwrite(__FILE__, $msg);
		$proceed = True;
	} else {
		$msg = "Sorry - login unsuccessful - you will be assimilated";
		$logMsg = sprintf("login unsuccessful - pwd: [%s]", $pwd);
		$log->lwrite(__FILE__, $logMsg);

		$Smarty->assign("errmsg", $msg);
		$disDates = getList_reportDates();
		$Smarty->assign("dates", $disDates);
		$Smarty->display("login.tpl");
		exit();
	}

	if ( $graph ) {	
		$log->lwrite(__FILE__, "");
                $log->lwrite(__FILE__, "*****  User requested GRAPH  *****");
                $log->lwrite(__FILE__, "");

		$graphPic = generateGraph($graphAcctArray);

		$Smarty->assign("graphPic", $graphPic);
		$Smarty->display("showGraph.tpl");
		exit();
	}

	// User must have successfully logged in, so send email alert that we have
	//    authorized user in system
	$recipient = "rschurch85@gmail.com";
	$subject = sprintf("Web Page accessed by %s", $username);
	$msg = sprintf("User is: %s   and IP is: %s", $username, $_SERVER['REMOTE_ADDR']);
	sendMail($recipient, $subject, $msg);

	if ( $proceed == True ) {
		$Smarty->assign("login", $_SESSION['login']);
		$Smarty->assign("maxDate", $maxDate);
		$Smarty->assign("dates", $disDates);
                $Smarty->display("mainAdmin.tpl");
                exit();
	}

        //throw new exception("Hello - I am broke..");

        $log->lwrite(__FILE__, 'Script Done');
}
catch (Exception $e) {
        $msg = "ERROR: (File: " . $e->getFile() . "  Line: " . $e->getLine() . "): " . $e->getMessage();
        $log->lwrite(__FILE__, $msg);
}


// commented out on 21Jan    delete if not needed...
//balanceMonthlyNumbers("201102", False, False, True);
//exit("Done");


?>

