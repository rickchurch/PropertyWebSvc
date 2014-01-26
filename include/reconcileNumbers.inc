<?php

require_once "include/pders_db.inc";
require_once "include/logger.inc";


//balanceMonthlyNumbers("201102", False, False, True);
//exit("Done");





function balanceMonthlyNumbers($date, $showEaEntry=False, $showLineItemReport=False, $showReconcilliation=False) {

	// This function will query the db for all records in ENTRIES table where $date matches
	//     ENTRIES.EN_Date.  It will then sort each entry into the appropriate array
	//     Income, Expense, Other and return a master array of the 3 combined.  This 
	//     function has 3 parameters to print/echo additional info if set to True.  It
	//     will print (echo) for each line as it sorts thru the entries (showEaEntry)
	//     or print/echo all 3 arrays (showLineItemReport) or print/echo a reconcilliation
	//     (showReconcilliation) showing that the total entries (income or expense) match 
	//     the line on the original report showing total Income and expense.

        require_once "include/logger.inc";
	
        date_default_timezone_set('America/Denver');
        //echo date('D,F j, Y, h:i:s A'). "<br>";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');
        // write message to the log file 
        $log->lwrite(__FILE__, "Entering balanceMonthlyNumbers()");
	
        $dbh = new pders_pdo();
        $query = sprintf("select EN_LineItem, EN_Amount_mo, EN_Amount_ytd FROM ENTRIES where EN_Date = %s", $date);
        $result = $dbh->Query($query);
        $whatisit = gettype($result);
        if ($whatisit == "object" || $whatisit == "array") {
                // We have a normal result set returned
                $parseResult = True;
        } else {
                // Had an error executing the query
                $parseResult = False;
        }
        if (!$parseResult) {
                echo "DB Error: ".$result."<br>";
                exit("done");
        }
	$expenseTot = 0;
	$incomeTot = 0;
	$expArray = array();
	$incArray = array();
	$othrArray = array();
	
        foreach ($result AS $row) {
                $li = $row['EN_LineItem'];
		$amt_mo = $row['EN_Amount_mo'];
		$amt_ytd = $row['EN_Amount_ytd'];
	        $query2 = sprintf("select LI_Type, LI_FullName FROM LINE_ITEMS where LI_Id = %s", $li);
	        $result2 = $dbh->Query($query2);
	        $whatisit2 = gettype($result2);
	        if ($whatisit2 == "object" || $whatisit2 == "array") {
	                // We have a normal result set returned
	                $parseResult2 = True;
	        } else {
	                // Had an error executing the query
	                $parseResult2 = False;
	        }
	        if (!$parseResult2) {
	                echo "2 DB Error: ".$result2."<br>";
	                exit("done");
	        }
	        foreach ($result2 AS $q2row) {
			$type = $q2row['LI_Type'];
			$name = $q2row['LI_FullName'];
		}

		// Sort each entry into the appropriate array (Income, Expense, Other)

		if ($type == "Expense"){
			if ($name != "Prepaid Expenses" && $name != "Mortgages Payable" ) {
				// if mortgage interest is 201108 or later, don't inc in expenses
				if ($date >= "201108" && $name == "Mortgage Interest") {
					// skip
				} else {
					$expenseTot += $amt_mo;
				}
			}
			
			$dollrCents = addDecimalComma($amt_mo);
			//$log->lwrite(__FILE__, sprintf("%s        %s", $name, $dollrCents));
			$expArray[] = array("Name"=>$name, "Amt_mo"=>$amt_mo, "Amt_display"=>$dollrCents);
		}
		if ($type == "Income") {
			$incomeTot += $amt_mo;
			$dollrCents = addDecimalComma($amt_mo);
			$incArray[] = array("Name"=>$name, "Amt_mo"=>$amt_mo, "Amt_display"=>$dollrCents);
		}
		if ($type == "Other" && $name == "Total Operating Income") {
			$totalIncLiItem = $amt_mo;
			continue;
		}
		if ($type == "Other" && $name == "Total Operating Expenditure") {
			$totalExpLiItem = $amt_mo;
			continue;
		}
		if ($type == "Other"){
			$dollrCents = addDecimalComma($amt_mo);
			$othrArray[] = array("Name"=>$name, "Amt_mo"=>$amt_mo, "Amt_display"=>$dollrCents);
			continue;
		}
		if ($showEaEntry == True) {
			echo "--------------------------------------<br>";
			echo sprintf("LI#: %s<br>", $li);
			echo sprintf("Type: %s<br>", $type);
			echo sprintf("Name: %s<br>", $name);
			echo sprintf("Amt_mo: %s<br>", $amt_mo);
			echo sprintf("Amt_ytd: %s<br>", $amt_ytd);
			echo "--------------------------------------<br>";
		}
	}
	if ($showReconcilliation == True) {
		echo sprintf("Month for Reconcilliation: %s<br><br>", $date);
		echo sprintf("Total Exp: %d<br>", $expenseTot);
		echo sprintf("Total Exp reported: %d<br>", $totalExpLiItem);
		if (($expenseTot - $totalExpLiItem) < 0.0001 && ($expenseTot - $totalExpLiItem) > -0.0001) {
			echo sprintf("MATCH !!!!!!!!!    Diff: %.2f<br>", $expenseTot - $totalExpLiItem);
		} else {
			echo sprintf("Problem...    Diff: %.2f<br>", $expenseTot - $totalExpLiItem);
		}
		echo sprintf("Total Inc: %d<br>", $incomeTot);
		echo sprintf("Total Inc reported: %d<br>", $totalIncLiItem);
		echo sprintf("Diff: %d<br>", $incomeTot - $totalIncLiItem);
	}
	if ($showLineItemReport == True) {
		echo "<br><br><br>";
		$space = "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";

		// print each Income line item
		foreach ($incArray AS $irow) {
			$name = $irow['Name'];
			$amt = $irow['Amt_mo'];
			$disAmt = $irow['Amt_display'];
			echo sprintf("%s%s %.2f %s %s<br>",$name, $space, $amt, $space, $disAmt );
		}
		echo "<br><br><br>";

		// print each Expense line item
		foreach ($expArray AS $erow) {
                	$name = $erow['Name'];
	                $amt = $erow['Amt_mo'];
			$disAmt = $erow['Amt_display'];
        	        echo sprintf("%s%s %.2f %s %s<br>",$name, $space, $amt, $space, $disAmt );
	        }
		echo "<br><br><br>";

		// print each Other line item
	        foreach ($othrArray AS $orow) {
       	        	$name = $orow['Name'];
	                $amt = $orow['Amt_mo'];
			$disAmt = $orow['Amt_display'];
        	        echo sprintf("%s%s %.2f %s %s<br>",$name, $space, $amt, $space, $disAmt );
	        }
	}
	$allLineItemsArray = array();
	$allLineItemsArray[] = $incArray;
	$allLineItemsArray[] = $expArray;
	$allLineItemsArray[] = $othrArray;
	return $allLineItemsArray;
}

function addDecimalComma($amt){
	//echo sprintf("orig number: %s<br>", $amt);
	$pointParts = explode(".", $amt);
	if (count($pointParts) == 2 ) {
		$cents = $pointParts[1];
		$cents = substr($cents, 0,2);
	} else {
		$cents = "00";
	}
	if (strlen($cents) == 1) $cents = sprintf("%s0", $cents);
	
	$lenD = strlen($pointParts[0]);
	$dollar = "";
	for ($cntr = 0;;$cntr++) {
		if ($cntr >= $lenD) break;
		if ($cntr >= 20) break;
		$rvsCntr = $lenD-(1+$cntr);
		if ($cntr == 3 || $cntr == 6) {
			if (substr($pointParts[0], $rvsCntr, 1) == "-") {
				$dollar = sprintf("%s%s", substr($pointParts[0], $rvsCntr, 1), $dollar);
			} else {
				$dollar = sprintf("%s,%s", substr($pointParts[0], $rvsCntr, 1), $dollar);
			}
		} else {
			$dollar = sprintf("%s%s", substr($pointParts[0], $rvsCntr, 1), $dollar);
		}
	}
	//echo sprintf("dollars: %s<br>", $dollar);
	//echo sprintf("cents: %s<br>", $cents);

	$result = sprintf("%s.%s",$dollar, $cents);
	// if we have 0.00 lets show that as just 0
	if ( $dollar == "0" && $cents == "00" ) {
		$result = "0";
	}
	return $result;
}


?>

