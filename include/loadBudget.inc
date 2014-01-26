<?php


require_once "include/pders_db.inc";
require_once "include/logger.inc";
require_once "include/reconcileNumbers.inc";
require_once "include/functions.inc";

function loadBudgetData($budgetYearLoad) {
	//  This is for Preston  Management company.  If you need another management company, change
	//    this interface to have that passed in.

	date_default_timezone_set('America/Denver');
        //echo date('D,F j, Y, h:i:s A'). "<br>";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');
        // write message to the log file 
        $log->lwrite(__FILE__,'   Start');
	
	$filePath = sprintf("/home/rickchurch11/PrestonFiles/Legacy_Budget_%s.txt", $budgetYearLoad);
	$log->lwrite(__FILE__, sprintf("File Name to load: %s", $filePath));

	$file=fopen($filePath, "r");
	$log->lwrite(__FILE__, "======================================================================================================");
	while(!feof($file)) {
		$cntr += 1; 
		if ($cntr > 300){
			$log->lwrite(__FILE__, "");
                	$log->lwrite(__FILE__, "ERROR !!     hit the max counter of 300 - Problem ??");
			$log->lwrite(__FILE__, "");
               	        break;
		}
        	$line = fgets($file). "<br />";
                $line = trim($line, "<br />");
		$line = trim($line);

		$acctNum = substr($line, 0, 4);
		$newStr = sprintf("%d", $acctNum);
		if ( $acctNum > 5990 && $acctNum < 9500 ) {
			//if ( $acctNum == 6000 ) {
			//$log->lwrite(__FILE__, $line);

			$lineLen = strlen($line);
			$safeCntr = 0;
			//$log->lwrite(__FILE__, sprintf("line length: %d", $lineLen));
			$quoteFlag = False;
			$buildValue = "";
			$lineItemCnt = 0;
			for ( $cntr= 4; $cntr < $lineLen; $cntr++ ) {
				$chr = substr($line, $cntr, 1);
				//  if chr is comma, then start building value but check for first val = "
				if ( $chr == "," ) {
					if ( ! $quoteFlag ) {
						// must not be in middle of quotes
						// save and then reset buildValue variable
						$buildValue = trim($buildValue);
						if ( $buildValue != "" ) {
							$newStr = sprintf("%s__%s", $newStr, $buildValue);
							$lineItemCnt++;
						}
						$buildValue = "";
					} else {
						// the comma must be numeric place holder
					}
				} elseif ( $chr == '"' ) {
					if ( $quoteFlag ) {
						// must have just completed a value, so save it and reset buildValue and quoteFlag
						$buildValue = trim($buildValue);
						if ( $buildValue != "" ) {
							$newStr = sprintf("%s__%s", $newStr, $buildValue);
							$lineItemCnt++;
						}
						$buildValue = "";
						$quoteFlag = False;
					} else {
						// must be just starting a new value
						$quoteFlag = True;
					}
				} elseif ( $chr == '(' ) {
					// must be a negative value - change it to a minus
					$buildValue = sprintf("%s-", $buildValue);
				} elseif ( $chr == ')' ) {
					// ignore the trailing or closing parens 
				} else {
					// add chr to buildValue
					$buildValue = sprintf("%s%s", $buildValue, $chr);
				}


				//$log->lwrite(__FILE__, sprintf("cntr: %d", $cntr));
				if ( $safeCntr > 200 ) {
					$log->lwrite(__FILE__, "Error - Hit max cntr = 200 for line chr's");
					break;
				}
				$safeCntr++;
			}
			if ($lineItemCnt > 4) {
				if ($lineItemCnt != 14 ) {
					$log->lwrite(__FILE__, sprintf("****  Unexpected number of line items: %d    ****", $lineItemCnt));
				}

				//  Don't save this line if they are all zeros
				if ( ! isTotalZero($newStr) ) {
					$log->lwrite(__FILE__, $line);
					$log->lwrite(__FILE__, $newStr);	
					// Had to execute the next line (insertBudgetLineItems) an additional time to pick up items that were in first budget but not the second budget
					//insertBudgetLineItemsForNewMgmntCo($newStr, "Preston");
					$test = insertBudgetLineItem($budgetYearLoad, $newStr, "Preston");
					//$log->lwrite(__FILE__, sprintf("maxId: [%s]", $test));
					$log->lwrite(__FILE__, "======================================================================================================");
				}
			}

			//if ($acctNum == 6014 ) break;
		}

	}
	$log->lwrite(__FILE__, "Done parsing Budget file.");
	fclose($file);
		
}


function insertBudgetLineItem($budgetYear, $budgetRow, $mngmntCompany) {
        //  expect something like this for $budgetRow;
        //     7398__TOTAL UTILITIES__13935__13925__13915__13840__13930__13930__13925__13910__13910__13800__13870__13855__166745
        //   The above example is  acctNum, acctName, JanBudgetAmt, FebBudgetAmt, . . . . DecBudgetAmt, 12MonthTotal
        //      all separated by double underscore
	//  $date should be something like    201109 which indicates Sept 2011

        date_default_timezone_set('America/Denver');
        //echo date('D,F j, Y, h:i:s A'). "<br>";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $dbh = new pders_pdo();
        $table = "BUDGET_ENTRIES";
        $query = sprintf("select max(BE_EntryId) FROM %s", $table);
        $result = $dbh->Query($query);
        //$log->lwrite(__FILE__, sprintf("1 insertBudgetLineItems -- maxid [%s]", $result[0][0]));
        $maxId = $result[0][0];
        if ( $maxId == "" ) $maxId = 1000;
        $maxId++;

        $parts_du = explode("__", $budgetRow);
        //$data = array($parts_du[0], $parts_du[1], $parts_du[2], $parts_du[3], $parts_du[4], $parts_du[5], $parts_du[6], $parts_du[7],
        //    $parts_du[8], $parts_du[9], $parts_du[10], $parts_du[11], $parts_du[12], $parts_du[13], $parts_du[14]);

        $query3 = "select BU_Id from BUDGET_LINE_ITEMS where BU_AcctNumber = ? and BU_MgmntCompany = ?";
        $params3 = array($parts_du[0], $mngmntCompany);
        $result3 = $dbh->Query($query3, $params3);
	if ( count($result3) == 0 ) {
		$log->lwrite(__FILE__, "");
		$msg = sprintf("ERROR - Could NOT find budget line item for acct: %s", $parts_du[0]);
		$log->lwrite(__FILE__, $msg);
		$log->lwrite(__FILE__, "");
		return;
	} else {
		$budgetLineItem = $result3[0][0];
	}

	$yearMonth = sprintf("%s01", $budgetYear);

        //$msg = sprintf("table [%s]      yearMonth  [%s]     budgetLineItem   [%s]", $table, $yearMonth, $budgetLineItem);
        //$log->lwrite(__FILE__, $msg);

	$query2 = "select BE_EntryId FROM BUDGET_ENTRIES where BE_Date = ? and BE_LineItem = ?";
        $params2 = array($yearMonth, $budgetLineItem);
        $result2 = $dbh->Query($query2, $params2);
        $R2_cnt = count($result2);
        $log->lwrite(__FILE__, sprintf("insertBudgetLineItems -- q2 result count [%d]", $R2_cnt));
        if ( $R2_cnt != 0 ) {
                //  We already have an entry for Jan so this line is probably already entered - abort
		$msg = sprintf("We already have an entry for Jan for this Acct Num:  %s  so this line is probably already entered - abort", $parts_du[0]);
		$log->lwrite(__FILE__, $msg);
		return;
	}

	$monthConverter= array("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "999");
	$entryId = $maxId;
	for ( $cntr=2; $cntr < count($parts_du); $cntr++ ) {
		$date = sprintf("%s%s", $budgetYear, $monthConverter[$cntr-1]);
		$data = array('BE_EntryId'=>$entryId, 'BE_Date'=>$date, 'BE_LineItem'=>$budgetLineItem, 'BE_Amount'=>$parts_du[$cntr] );
		$msg = sprintf("EntryId  [%s]      Date  [%s]    LineItem  [%s]  Amt  [%s]", $entryId, $date, $budgetLineItem, $parts_du[$cntr]);
		$log->lwrite(__FILE__, $msg);
                $dbh->InsertRow($table, $data);
		$entryId++;
        }
}



function insertBudgetLineItemsForNewMgmntCo($budgetRow, $mgmntCoName) {
	//  expect something like for $budgetRow;
	//     7398__TOTAL UTILITIES__13935__13925__13915__13840__13930__13930__13925__13910__13910__13800__13870__13855__166745
	//   The above example is  acctNum, acctName, JanBudgetAmt, FebBudgetAmt, . . . . DecBudgetAmt, 12MonthTotal
	//      all separated by double underscore

        date_default_timezone_set('America/Denver');
        //echo date('D,F j, Y, h:i:s A'). "<br>";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

	$dbh = new pders_pdo();
	$table = "BUDGET_LINE_ITEMS";
	$query = "select max(BU_Id) FROM BUDGET_LINE_ITEMS";
        $result = $dbh->Query($query);
	//$log->lwrite(__FILE__, sprintf("1 insertBudgetLineItems -- maxid [%s]", $result[0][0]));
	$maxId = $result[0][0];
	if ( $maxId == "" ) $maxId = 1000;
	$maxId++;
	$parts_du = explode("__", $budgetRow);
	//$data = array($parts_du[0], $parts_du[1], $parts_du[2], $parts_du[3], $parts_du[4], $parts_du[5], $parts_du[6], $parts_du[7],
	//    $parts_du[8], $parts_du[9], $parts_du[10], $parts_du[11], $parts_du[12], $parts_du[13], $parts_du[14]);
	$query2 = "select BU_Id FROM BUDGET_LINE_ITEMS where BU_AcctNumber = ?";
	$params2 = array($parts_du[0]);
        $result2 = $dbh->Query($query2, $params2);
	$R2_cnt = count($result2);
	//$log->lwrite(__FILE__, sprintf("insertBudgetLineItems -- q2 result count [%d]", $R2_cnt));
	if ( $R2_cnt == 0 ) {
		//  This acct# has not yet been inserted to db
		$data = array('BU_Id'=>$maxId, 'BU_AcctNumber'=>$parts_du[0], 'BU_Name'=>$parts_du[1], 'BU_MgmntCompany'=>$mgmntCoName);
		$dbh->InsertRow($table, $data);
	}
}


function isTotalZero($budgetRow) {
        //  expect something like;
        //     7398__TOTAL UTILITIES__13935__13925__13915__13840__13930__13930__13925__13910__13910__13800__13870__13855__166745

        $parts_du = explode("__", $budgetRow);
	if ( $parts_du[14] == 0 ) return True;
	return False;
}




?>
