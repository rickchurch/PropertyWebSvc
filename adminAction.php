<?php

session_start();

try {
	require_once "include/reconcileNumbers.inc";
	require_once "include/logger.inc";
        require_once "include/pders_db.inc";
        require_once "include/functions.inc";
	require_once "include/loadBudget.inc";
        require_once "/home/rickchurch11/Downloads/Smarty-3.1.5/libs/Smarty.class.php";
	require_once "include/monthData.inc";

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

	$username = $_SESSION['userName'];
	//$pwd = $_SESSION['pwd'];
	$_SESSION['beginMonth'] = "";
	$_SESSION['endMonth'] = "";

	$selDate = $_POST['selDate'];
	$beginMonth = $_POST['selStartMonth'];
	$endMonth = $_POST['selEndMonth'];
	$buttonSelected = $_POST['SubmitBtn'];	
	$moAve3 = $_POST['3moave'];
	$moAve6 = $_POST['6moave'];
	$moAve12 = $_POST['12moave'];
	$reportSize = $_POST['reportWidth'];
	$budgetYearLoad = $_POST['budgetYearToLoad'];

	$log->lwrite(__FILE__,sprintf("UserName: %s", $username));
	$log->lwrite(__FILE__,sprintf("Date selected: %s", $selDate));
	$log->lwrite(__FILE__,sprintf("Start date selected: %s", $beginMonth));
	$log->lwrite(__FILE__,sprintf("End date selected: %s", $endMonth));
	$log->lwrite(__FILE__,sprintf("Button selected: %s", $buttonSelected));
	$log->lwrite(__FILE__,sprintf("3 Month Average Display: %s", $moAve3));
	$log->lwrite(__FILE__,sprintf("6 Month Average Display: %s", $moAve6));
	$log->lwrite(__FILE__,sprintf("12 Month Average Display: %s", $moAve12));
	$log->lwrite(__FILE__,sprintf("Report Size: %s", $reportSize));

	// balanceMonthlyNumbers($date, $showEaEntry, $showLineItemReport, $showReconcilliation)
	if ($buttonSelected == "Logout") {
		$log->lwrite(__FILE__,sprintf("User [%s] is logging out.", $username));
		$_SESSION['userName'] = "";
		$_SESSION['login'] = "";

		$Smarty->display("login.tpl");
	        exit();
	} elseif ($buttonSelected == "ReadMoFinData_eaEntry") {
		balanceMonthlyNumbers($selDate, True, False, False);
	} elseif ($buttonSelected == "Show Line Item Rpt (like a reg monthly rpt)") {
		balanceMonthlyNumbers($selDate, False, True, False);
	} elseif ($buttonSelected == "Show Reconcilliation") {
		balanceMonthlyNumbers($selDate, False, False, True);
	} elseif ($buttonSelected == "Load financial data - use flat file") {
		$monthToLoad = $_POST['monthOfDataToLoad'];
		if (monthToLoadIsValid($monthToLoad)) {
			LoadMonthly($monthToLoad);
		}
        } elseif ($buttonSelected == "Load BUDGET data - use flat file" ) {
		$log->lwrite(__FILE__,sprintf("Begin loading Budget data for Year %s", $budgetYearLoad));
		loadBudgetData($budgetYearLoad);
		$log->lwrite(__FILE__,sprintf("Finished loading Budget data for Year %s", $budgetYearLoad));

        } elseif ($buttonSelected == "Generate Function List File" ) {
		generateFunctionList();

        } elseif ($buttonSelected == "Test new class" ) {

                $_SESSION['beginMonth'] = $beginMonth;
                $_SESSION['endMonth'] = $endMonth;

		$displayAcctNumArray = getAcctNumberArray("Preston");

                //
                // convert begin and end months to an array of months that span that time period
                //
		$monthArray = convertDateRangeToArray($beginMonth, $endMonth);		

                //
                // Cycle thru each month requested and generate a class object for each month
                //    that contains the primary data (ea acct# which then contains month & YTD 
                //    numbers (real & display format)
                //
                foreach ($monthArray AS $item) {
                        $month = $item['dateValue'];
                        $monthData[$month] = new monthData($month);
                }

                //  create an array from the class objects for each month so that we can pass this 
                //     to the template.  We don't want template to have access to class objects.
                $masterMonthData = createMultiMonthFinancialsArray($monthData);

                //$aveReportSizeSelected = array('one'=>3, 'two'=>6, 'three'=>12);
                $aveReportSizeSelected =array();
                if ($moAve3 == 'yes' ) $aveReportSizeSelected[] = 3;
                if ($moAve6 == 'yes' ) $aveReportSizeSelected[] = 6;
                if ($moAve12 == 'yes' ) $aveReportSizeSelected[] = 12;

                $masterDataAndAveData = insertMonthlyAveragesIntoMonthArray($masterMonthData, $monthArray, $aveReportSizeSelected);

		$averageReportSizeQualified = $masterDataAndAveData[1];
                $masterMonthData = setMissingAcctNumbersToZero($masterDataAndAveData[0], $monthArray);
		$reportSize = validateReportwidth($reportSize);

                $msg = sprintf("masterMonthData[%s][%s] = %s", "201111", "4000", $masterMonthData["201111"]["4000"]['amount_mo_real']);
                //$log->lwrite(__FILE__, $msg);

                $Smarty->assign("reportSize", $reportSize);
                $Smarty->assign("AverageReportSize", $averageReportSizeQualified);
                $Smarty->assign("monthArray", $monthArray);
                $Smarty->assign("displayAcctNumArray", $displayAcctNumArray);
                $Smarty->assign("MasterMonthData", $masterMonthData);
                $Smarty->display("multiMonthData2.tpl");
                exit();



		//   OLD LOGIC BELOW HERE
                $tester = test_class($monthArray);

		$acctNumStr = '4000';
		//$log->lwrite(__FILE__,sprintf("test new class - month attribute2: %s", $tester->$acctNumStr->FullName));
		$log->lwrite(__FILE__, "Calling displayMonth.tpl");



		//
		//   NEED TO TEST NEW ARRAY AGAINST MULTI MONTH WEB PAGE
		//


	
	        $Smarty->assign("tester", $tester);
        	$Smarty->display("displayMonth.tpl");
	        exit();

	} elseif ($buttonSelected == "Show Selected Months" ) {
                $log->lwrite(__FILE__, "");
                $log->lwrite(__FILE__, "*****  User requested DATA DISPLAY  *****");
                $log->lwrite(__FILE__, "");
		$log->lwrite(__FILE__,sprintf("*****    Start Month selected: %s", $beginMonth));
		$log->lwrite(__FILE__,sprintf("*****    End Month selected: %s", $endMonth));	

		$_SESSION['beginMonth'] = $beginMonth;
		$_SESSION['endMonth'] = $endMonth;

		//$aveReportSize=array('one'=>3, 'two'=>6, 'three'=>12);
		$aveReportSize=array();
		if ($moAve3 == 'yes' ) $aveReportSize[] = 3;
		if ($moAve6 == 'yes' ) $aveReportSize[] = 6;
		if ($moAve12 == 'yes' ) $aveReportSize[] = 12;

		$masterDataAndAveData = getFinancialsMultMonths($beginMonth, $endMonth, $aveReportSize);
		$MasterMonthData = $masterDataAndAveData[0];
		$aveReportSize = $masterDataAndAveData[1];

		if ( ! is_numeric($reportSize) ) {
			$log->lwrite(__FILE__, "User requested Report size set improperly (not numeric) - reset to zero");
			$reportSize = 0;
		}
		if ( $reportSize >= 0 && $reportSize <= 401 ) {
			// good to go
		} else {
			$reportSize = 0;
			$log->lwrite(__FILE__, "User requested Report size set improperly - reset to zero");
		}
		//echo print_r($MasterMonthDat);
		$displayAcctNumArray = getAcctNumberArray("Preston");
		// ensure that the order of the accts are what I really want or add another 
		//    field DisplayPosition and sort by that one to get acct nums.  Then access each acct
		foreach ($displayAcctNumArray as $row) {
			$msg = sprintf("LI_AcctNumber: %s      LI_Category: %s", $row['LI_AcctNumber'], $row['LI_Category']);
			//$log->lwrite(__FILE__, $msg);
		}
		$monthArray = convertDateRangeToArray($beginMonth, $endMonth);
		
		//  check all monthly amount values and if they don't exist, set to zero.  Do this 
		//    by checking for the acctNum key in the parent array.
		foreach ($monthArray as $month) {
			$log->lwrite(__FILE__,sprintf("Checking for keys for: %s", $month['dateValue']));
			$logFlag =True;
			foreach ($displayAcctNumArray as $acctNum ) {
				if ( is_array( $MasterMonthData[$month['dateValue']]) ) {
					if (! array_key_exists($acctNum['LI_AcctNumber'], $MasterMonthData[$month['dateValue']]) ) {
						$msg = sprintf("No key for %s   %s - setting to zero.",$month['dateValue'], $acctNum['LI_AcctNumber']);
						if ( $logFlag ) {
							// only want to log once so I know it is working
							$log->lwrite(__FILE__, $msg);
							$logFlag = False;
						}
						$MasterMonthData[$month['dateValue']][$acctNum['LI_AcctNumber']]['Amount_mo_disp'] = 0;
					}
				} else {
					$log->lwrite(__FILE__,"");
					$log->lwrite(__FILE__,sprintf("ERROR - This is not a MasterDataArray.  Month:: %s", $month['dateValue']));
					$log->lwrite(__FILE__,"");
				}
			}
		}

		$log->lwrite(__FILE__, "Calling multiMonthData.tpl");

		$Smarty->assign("reportSize", $reportSize);
		$Smarty->assign("AverageReportSize", $aveReportSize);
		$Smarty->assign("monthArray", $monthArray);
		$Smarty->assign("displayAcctNumArray", $displayAcctNumArray);
		$Smarty->assign("MasterMonthData", $MasterMonthData);
		$Smarty->display("multiMonthData.tpl");

		exit();
	} elseif ($buttonSelected == "Show Pictures" ) {
		$dirName = "/home/rickchurch11/www/images/LegacyPics/";

		$msg = sprintf("Searching directory: %s", $dirName);
		$log->lwrite(__FILE__, $msg);

		$dh = opendir($dirName);
		$PicArray = array();
		while ($file = readdir($dh)) {
			if ($file == "." || $file == ".." ) continue;
			$msg = sprintf("Adding to array - file Name: %s--%s", $dirName, $file);
			$log->lwrite(__FILE__, $msg);
			//  file name is assumed to look like <width>_<height>__<file name>   ie:
			//                                                  430_573__Vacant_227_LR.jpg
			$dblParts = explode("__", $file);
			$undrscrParts = explode("_", $dblParts[0]);
			$width = $undrscrParts[0];
			$height = $undrscrParts[1];
			$fileName = $dblParts[1];
			$dotParts = explode(".", $fileName);
			$msg = sprintf("width [%s] || height [%s]  || fileName [%s]", $width, $height, $fileName);
			//$log->lwrite(__FILE__, $msg);
			$PicArray[] = array('file'=>sprintf("/images/LegacyPics/%s", $file),'caption'=>$dotParts[0],'width'=>$width, 'height'=>$height);
		}
		closedir($dh);

		$log->lwrite(__FILE__, "Calling displayPics.tpl");

                $Smarty->assign("PicArray", $PicArray);
                $Smarty->display("displayPics.tpl");

		exit();
	}

	//      $masterMonthData['201103']['5015']['Amount_mo_disp']  would give
	//               the month amount for acct 5015(asset Mgr) for Mar 2011

	//exit();
	$disDates = getList_reportDates();
	//$Smarty->assign("errmsg", $msg);
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







exit("AdminAction.php - Probably caught an exeption and ended up here...");





?>

