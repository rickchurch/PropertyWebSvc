<?php
require_once "include/pders_db.inc";
require_once "include/logger.inc";
require_once "include/reconcileNumbers.inc";


function validateReportwidth($width) {
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

	if ( ! is_numeric($width) ) {
        	$log->lwrite(__FILE__, "User requested Report size (width) set improperly (not numeric) - reset to zero");
                return 0;
        } elseif ( $width >= 0 && $width <= 401 ) {
                // good to go
		return $width;
        } else {
                $log->lwrite(__FILE__, "User requested Report size (width) set improperly - reset to zero");
		return 0;
        }
}





function setMissingAcctNumbersToZero($masterMonthData, $monthArray) {
	// To ensure that the display of monthly data is consistant, we need to set any missing
	//    acct nums to zero else, the looks of the data will not be consistant.

        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

	$displayAcctNumArray = getAcctNumberArray("Preston");

        foreach ($monthArray as $month) {
	        $log->lwrite(__FILE__,sprintf("Checking for keys for: %s", $month['dateValue']));
                $logFlag =True;
                foreach ($displayAcctNumArray as $acctNum ) {
        	        if ( is_array( $masterMonthData[$month['dateValue']]) ) {
	                        if (! array_key_exists($acctNum['LI_AcctNumber'], $masterMonthData[$month['dateValue']]) ) {
        	                        $msg = sprintf("No key for %s   %s - setting to zero.",$month['dateValue'], $acctNum['LI_AcctNumber']);
                                        if ( $logFlag ) {
                	                        // only want to log once so I know it is working
                                                $log->lwrite(__FILE__, $msg);
                                                $logFlag = False;
	                                }
                                        $masterMonthData[$month['dateValue']][$acctNum['LI_AcctNumber']]['amount_mo_disp'] = 0;
                                }
                        } else {
                        	$log->lwrite(__FILE__,"");
                                $log->lwrite(__FILE__,sprintf("ERROR - This is not a MasterDataArray.  Month:: %s", $month['dateValue']));
                                $log->lwrite(__FILE__,"");
                        }
                }
	}
	return $masterMonthData;
	// end setMissingAcctNumbersToZero($masterMonthData, $monthArray)
}


function insertMonthlyAveragesIntoMonthArray($masterMonthData, $monthArray, $aveReportSizeSelected) {
        //  Add monthly averages to $masterMonthData so that it is accessable as follows;
        //        $masterMonthData['6moAve'][$acctNum]  returns the (display formatted) average number  
	//                                                      (6moAve could also be 3moAve, ect)

        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, "Entering insertMonthlyAveragesIntoMonthArray");
        $log->lwrite(__FILE__, "");

        $monthArraySize = count($monthArray);
        $log->lwrite(__FILE__, sprintf("Number of months to retrieve: %d", $monthArraySize));

        $dbh = new pders_pdo();
        // Generate a db query result that contains all line items (ie: all acct numbers)
        $query = "SELECT * FROM LINE_ITEMS";
        $result = $dbh->Query($query);

        // $aveReportSizeSelected (passed into our function) will define what averages we calc and (try to) display
        // $aveReportSizeSelected=array('one'=>3, 'two'=>6, 'three'=>12);    This is passed into the function - if user selected all 3
        $AverageReportSize = array();

        //
        // Add a TOTAL amt and average of ALL months selected into $masterMonthData array
        //
        foreach ($result as $lineItem) {
                $li_id = $lineItem['LI_Id'];
                $acctNum = $lineItem['LI_AcctNumber'];
                $cntr2 = 0;
                $aveTot = 0;
                //  Loop thru ea month to get current acct number amount - we'll use this to get an average number
                //    for all months for each acct AND a total amt and save to masterMonthData main array object.
                for ( $moCntr = 0; $moCntr < $monthArraySize; $moCntr++ ) {
                        $month = $monthArray[$moCntr]['dateValue'];
                        $moAmt = $masterMonthData[$month][$acctNum]['amount_mo_real'];
                        $msg = sprintf("Month: %s     acctNum: %s       Tot accumulated: %s", $month, $acctNum, $aveTot);
                        //$log->lwrite(__FILE__, $msg);
                        //$log->lwrite(__FILE__, sprintf("Monthly amt to add into ave tot: %s", $moAmt));
                        $aveTot = $aveTot + $moAmt;
                        if ($cntr2 > 50 ) {
                               $log->lwrite(__FILE__, "");
                               $log->lwrite(__FILE__, sprintf("ERROR - Loop exeeded 50 iterations for acct num: %s", $acctNum));
                               $log->lwrite(__FILE__, "");
                               break;
                        }
                }
                $msg = sprintf("Total amt for %s: %d    Aver amt: %d    # months: [%d]", $acctNum, $aveTot, $aveTot/$monthArraySize, $monthArraySize );
                //$log->lwrite(__FILE__, $msg);
                $key = sprintf("%dmoAve", $monthArraySize);
                //  save averages with the following format;
                //       $masterMonthData['6moAve'][$acctNum]  returns the display number  (6moAve could also be 3moAve, ect)
                $masterMonthData["AVERAGE"][$acctNum] = addDecimalComma($aveTot / $monthArraySize);
                $masterMonthData["TOTAL"][$acctNum] = addDecimalComma($aveTot);
        }
        $AverageReportSize[] = "TOTAL";
	$AverageReportSize[] = "AVERAGE";

	//
	// Add the selected average months into the $masterMonthData array
	//
        foreach ($aveReportSizeSelected as $monAve){
                $log->lwrite(__FILE__, sprintf("Number of months to average out: %s", $monAve));
		$msg = sprintf("first month to get for %d mo average: %s", $monAve, $monthArray[$monthArraySize-$monAve]['dateValue']);
                $log->lwrite(__FILE__, $msg);
                //  skip the calcs and saving data if total months to be displayed are less than average report (ie:3,6,12)
                if ( $monAve < $monthArraySize ) {
                        // $result is the db query result containing all the acct numbers
                        foreach ($result as $lineItem) {
                                $li_id = $lineItem['LI_Id'];
                                $acctNum = $lineItem['LI_AcctNumber'];
                                $cntr2 = 0;
                                $aveTot = 0;
                                //  Loop thru ea month to get current acct number amount - we'll use this to get an average number
                                //    for each acct and save to masterMonthData main array object. Thus we will end up with 
                                //    $masterMonthData['6moAve'][$acctNum]  average amount - will also have 3mo and 12mo amounts if applicable.
                                for ( $moCntr = $monthArraySize - $monAve; $moCntr < $monthArraySize; $moCntr++ ) {
                                        $month = $monthArray[$moCntr]['dateValue'];
                                        $moAmt = $masterMonthData[$month][$acctNum]['amount_mo_real'];
					$msg = sprintf("Month: %s     acctNum: %s       Tot accumulated: %s", $month, $acctNum, $aveTot);
                                        //$log->lwrite(__FILE__, $msg);
                                        //$log->lwrite(__FILE__, sprintf("Monthly amt to add into ave tot: %s", $moAmt));
                                        $aveTot = $aveTot + $moAmt;
                                        if ($cntr2 > 50 ) {
						$log->lwrite(__FILE__, "");
                                                $log->lwrite(__FILE__, sprintf("ERROR - Loop exeeded 50 iterations for acct num: %s", $acctNum));
						$log->lwrite(__FILE__, "");
                                                break;
                                        }
                                }
                                //$log->lwrite(__FILE__, sprintf("Total %d mo amt for %s: %d", $monAve, $acctNum, $aveTot ));
                                $key = sprintf("%dmoAve", $monAve);
                                //  save averages with the following format;
                                //       $masterMonthData['6moAve'][$acctNum]  returns the display number  (6moAve could also be 3moAve, ect)
                                $masterMonthData[$key][$acctNum] = addDecimalComma($aveTot / $monAve);
				$msg = sprintf("masterMonthData[%s][%s] = %s  _______", $key, $acctNum, addDecimalComma($aveTot / $monAve));
				//$log->lwrite(__FILE__, $msg);
                        }
                        $AverageReportSize[] = $monAve;
                } else {
			$msg =sprintf("Total months displayed %d is less than %d months needed to calc %d ave", $monthArraySize, $monAve, $monAve);
                        $log->lwrite(__FILE__, $msg);
                        //  Need to also remove the array item from $aveReportSizeSelected array
                }
        }

        $masterDataAndAveData[0] = $masterMonthData;
        $masterDataAndAveData[1] = $AverageReportSize;

        return $masterDataAndAveData;
	// end insertMonthlyAveragesIntoMonthArray($masterMonthData, $monthArray, $aveReportSizeSelected)
}



function createMultiMonthFinancialsArray($financialsObject) {
	// $financialsObject is an array of month objects each containing financial details
	//    for that month - see class monthData.
	//    $financialsObject[month]->monthData(class)
	//         monthData class has the properties: month, dataType PLUS acctNum class objects
	//               acctNum class has the following properties;
	//                      number, dataType, amount_mo_real, amount_mo_disp, amount_ytd_real, amount_ytd_disp
	//                      fullName, category, type
	//
	// Take the passed in $financialsObject and iterate thru it creating
	//   an array that has this structure;
	//           $masterMonthData[month][acct Number]['amount_mo_real']
        //           $masterMonthData[month][acct Number]['amount_mo_disp']
        //           $masterMonthData[month][acct Number]['amount_ytd_real']
        //           $masterMonthData[month][acct Number]['amount_ytd_disp']
	//           $masterMonthData[month][acct Number]['fullName']

        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, "Entering createMultiMonthFinancialsArray");
        $log->lwrite(__FILE__, "");

	$masterMonthData = array();
	foreach($financialsObject as $key1 => $value1) {
		//
		//  interating thru all the various     monthData(classes)
		//
		//$log->lwrite(__FILE__, sprintf("Key (month): %s", $key1));   // s/b month value ie: 201106

                if ( $value1->dataType != "monthData" ) {
	                $log->lwrite(__FILE__, "");
                        $msg = sprintf("Unexpected ERROR: Expected dataType of monthData, but have %s.  Abort function.!!", $value1->dataType);
                        $log->lwrite(__FILE__, $msg);
                        $log->lwrite(__FILE__, "");
                        return;
                } else {
                        $msg = sprintf("dataType: %s       key: %s", $value1->dataType, $key1);
                        $log->lwrite(__FILE__, $msg);
                }
		foreach($value1 as $key2 => $value2) {
			//
			// interating thru all the monthData properties  (month, dataType PLUS acctNum class objects)
			//
	                if ( $key2 == "month" || $key2 == "dataType" ) {
		                $msg = sprintf("key: %s       value: %s", $key2, $value2);
				$log->lwrite(__FILE__, $msg);
                               	continue;
	                } elseif ( $value2->dataType == "acctNum" ) {
        	        	//  save values    number, dataType, amount_mo_real, amount_mo_disp, amount_ytd_real, amount_ytd_disp
                	        //       fullName
				$masterMonthData[$key1][$value2->number]['amount_mo_real']= $value2->amount_mo_real;
				$masterMonthData[$key1][$value2->number]['amount_mo_disp']= $value2->amount_mo_disp;
				$masterMonthData[$key1][$value2->number]['amount_ytd_real']= $value2->amount_ytd_real;
				$masterMonthData[$key1][$value2->number]['amount_ytd_disp']= $value2->amount_ytd_disp;
				$masterMonthData[$key1][$value2->number]['fullName']= $value2->fullName;
				$msg = sprintf("masterMonthData[%s][%s]['amount_mo_disp']= %s", $key1, $value2->number, $value2->amount_mo_disp);
				//$log->lwrite(__FILE__, $msg);
				$msg = sprintf("masterMonthData[%s][%s]['fullName']= %s __________", $key1, $value2->number, $value2->fullName);
				//$log->lwrite(__FILE__, $msg);
                        } else {
				$log->lwrite(__FILE__, "");
				$msg = sprintf("Unexpected ERROR: Unexpected property of monthData.  key: %s", $key2);
				$log->lwrite(__FILE__, $msg);
				$log->lwrite(__FILE__, "");
			}
		}		
	}
	return $masterMonthData;
	// end createMultiMonthFinancialsArray($financialsObject) 
}


function test_class($monthArray) {

	require_once "include/monthData.inc";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, "Entering test_class");
        $log->lwrite(__FILE__, "");

	try {
	        //
        	// Cycle thru each month requested and generate a class object for each month
	        //    that contains the primary data (ea acct# which then contains month & YTD 
        	//    numbers (real & display format)
        	//
	        foreach ($monthArray AS $item) {
        	        $month = $item['dateValue'];
                	$monthTest[$month] = new monthData($month);
	        }

		//  create an array from the class objects for each month so that we can pass this 
		//     to the template.  We don't want template to have access to class objects.
		$masterMonthData = createMultiMonthFinancialsArray($monthTest);
		//
		$msg = sprintf("masterMonthData[%s][%s] = %s", "201111", "4000", $masterMonthData["201111"]["4000"]['amount_mo_real']);
		//$log->lwrite(__FILE__, $msg);
		//
		$aveReportSizeSelected= array(3,6,12);
		$masterDataAndAveData = insertMonthlyAveragesIntoMonthArray($masterMonthData, $monthArray, $aveReportSizeSelected);
        	// $masterDataAndAveData[0] = $masterMonthData;
        	// $masterDataAndAveData[1] = $AverageReportSize;
		$masterDataAndAveData[0] = setMissingAcctNumbersToZero($masterDataAndAveData[0], $monthArray);
		return $masterDataAndAveData;

        } catch (Exception $e) {
                $msg = "ERROR: (File: " . $e->getFile() . "  Line: " . $e->getLine() . "): " . $e->getMessage();
                $log->lwrite(__FILE__, $msg);
        }

	return $monthTest;
	// end test_class($beginMonth, $endMonth)
}


function parseObject($myObject) {
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        foreach($myObject as $key => $value) {
		if (is_object($value)) {
			$log->lwrite(__FILE__, sprintf("***  Value is object - Key: %s", $key));
			parseObject($value);
			continue;
		} elseif (is_object($key)) {
			$log->lwrite(__FILE__, sprintf("***  Key is object"));
			parseObject($key);
			continue;
		}
		if ( $key == "dataType" || $key == "Amount_mo_disp" || $key == "FullName" ) {
			$msg = sprintf("***  [%s]  %s  =>  %s", gettype($value), $key, $value);
			$log->lwrite(__FILE__, $msg);
		}
	}
}
		


function generateFunctionList() {
	// Read the functions.inc file and generate a functionList file
	//   that lists all the functions in that file

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, sprintf("Entering generateFunctionList"));
	$log->lwrite(__FILE__, sprintf(""));

	$path = "/home/rickchurch11/www/include/functions.inc";
	$fh = fopen($path, 'r');
	$cntr = 1;
	while (!feof($fh)) {
		$myLine = fgets($fh);
		//if ("function" in $myLine) {
		if ( substr($myLine, 0, 8) == "function" ) {
			$myLine = trim($myLine);
			$myLine = rtrim($myLine, "{");
			$myLine = trim($myLine);
			$log->lwrite(__FILE__, sprintf("%s  %s",$cntr, $myLine));
			$cntr++;
			//
			// Write $myLine to a file named FunctionList
			//
		}
	}
	fclose($fh);
	$log->lwrite(__FILE__, "");
	// end generateFunctionList
}

function sendMail($recipient, $subject, $msg) {
        date_default_timezone_set('America/Denver');

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, sprintf("Entering sendMail"));

	$sender = "From:rschurch85@gmail.com\r\n";
	mail($recipient, $subject, $msg, $sender);

	$log->lwrite(__FILE__, sprintf("Sent email to: %s", $recipient));
	}


function generateGraph($graphAcctArray) {
        date_default_timezone_set('America/Denver');

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, sprintf("Entering generateGraph()"));

	$beginMonth = $_SESSION['beginMonth'];
	$endMonth = $_SESSION['endMonth'];
	$log->lwrite(__FILE__, sprintf("beginMonth: %s     endMonth: %s", $beginMonth, $endMonth));

	$acctNumArray = getAcctNumberArray("Preston");
	foreach ($acctNumArray as $item) {
		if (in_array($item['LI_AcctNumber'],$graphAcctArray)) {
			$log->lwrite(__FILE__,sprintf("Need AcctNum: %s    AcctName: %s", $item['LI_AcctNumber'], $item['LI_FullName']));
			$acctNameLookup[$item['LI_AcctNumber']] = $item['LI_FullName'];
		}
	}

	include "/home/rickchurch11/www/libchart/libchart/classes/libchart.php";
	//$chart = new LineChart(500, 250);
	$chart = new LineChart(1300, 450);

	$aveReportSizeSelected = array();
	$masterDataAndAveData = getFinancialsMultMonths($beginMonth, $endMonth, $aveReportSizeSelected);
	//$masterMonthData['201103']['5015']['Amount_mo_disp']  would give 
        //               the month amount for acct 5015(asset Mgr) for Mar 2011

	$MasterMonthData = $masterDataAndAveData[0];
	//$displayAcctNumArray = getAcctNumberArray("Preston");
	$monthArray = convertDateRangeToArray($beginMonth, $endMonth);
	
	$seriesArray = array();
	foreach ($graphAcctArray as $acctNum ) {
		// instantiate a new dataSet for each acct (each line graph)
		//$seriesArray[$acctNum] = new XYDataSet();
		$seriesArray[$acctNameLookup[$acctNum]] = new XYDataSet();
	}

	foreach ($monthArray as $month) {
	        $log->lwrite(__FILE__,sprintf("Setting data points for: %s", $month['dateValue']));
		foreach ($graphAcctArray as $acctNum ) {
			//$serie1->addPoint(new Point("06-01", 273));
			$value = $MasterMonthData[$month['dateValue']][$acctNum]['Amount_mo_real'];

			// if there is no value in db for this, then convert the resulting empty str to zero
			if ($value == "") {
				$value = 0;
				//$log->lwrite(__FILE__, "Found empty string for graph data value, so convert to zero.");
			}

			$dateToDisplay = $month['dateDisplay'];
			$log->lwrite(__FILE__,sprintf("Date: %s    Value: [%s]", $dateToDisplay, $value));

			//  create new data point for this acct using date and value
			//$seriesArray[$acctNum]->addPoint(new Point($dateToDisplay, $value));
			$seriesArray[$acctNameLookup[$acctNum]]->addPoint(new Point($dateToDisplay, $value));
		}
	}

	$dataSet = new XYSeriesDataSet();
	foreach ($graphAcctArray as $acctNum ) {
		//$dataSet->addSerie($acctNum, $seriesArray[$acctNum]);
		$dataSet->addSerie($acctNameLookup[$acctNum], $seriesArray[$acctNameLookup[$acctNum]]);
	}

	/*
	$chart = new LineChart(500, 250);

	$serie1 = new XYDataSet();
	$serie1->addPoint(new Point("06-01", 273));
	$serie1->addPoint(new Point("06-02", 421));
	$serie1->addPoint(new Point("06-03", 642));
	$serie1->addPoint(new Point("06-04", 799));
	$serie1->addPoint(new Point("06-05", 1009));
	$serie1->addPoint(new Point("06-06", 1106));
	
	$serie2 = new XYDataSet();
	$serie2->addPoint(new Point("06-01", 280));
	$serie2->addPoint(new Point("06-02", 300));
	$serie2->addPoint(new Point("06-03", 212));
	$serie2->addPoint(new Point("06-04", 542));
	$serie2->addPoint(new Point("06-05", 600));
	$serie2->addPoint(new Point("06-06", 850));

	$dataSet = new XYSeriesDataSet();
	$dataSet->addSerie("Product 1", $serie1);
	$dataSet->addSerie("Product 2", $serie2);
	*/

	$chart->setDataSet($dataSet);

	$beginMonthDisplay = $monthArray[0]['dateDisplay'];
	$endMonthDisplay = $monthArray[count($monthArray)-1]['dateDisplay'];

	$chartTitle = sprintf("Financial trends for months %s thru %s", $beginMonthDisplay, $endMonthDisplay);
	$chart->setTitle($chartTitle);
	$rndm = rand();
	$file = sprintf("libchart/demo/generated/rcGraph%d.png", $rndm);
	$chart->render($file);

	return $file;
	// end generateGraph()
}


function getAcctNumberArray($mgmntCo) {
        $monthArray = array();
	//  LI_DisplayPosition < 450 will include all line items
	$query = "SELECT LI_AcctNumber, LI_FullName, LI_Category, LI_Type FROM LINE_ITEMS 
	      WHERE LI_MgmntCompany = ? AND LI_DisplayPosition < 450 ORDER BY LI_DisplayPosition";
        $params = array($mgmntCo);
        $dbh = new pders_pdo();
        $result = $dbh->Query($query, $params);
        return $result;
}


function convertDateRangeToArray($beginMonth, $endMonth) {
	//  keys available;
	//     $monthArray['dateValue']
	//     $monthArray['DateDisplay']

        date_default_timezone_set('America/Denver');

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, sprintf("Entering convertDateRangeToArray(%s, %s)", $beginMonth,$endMonth));

        //$monthArray = array();
        $query = "SELECT DISTINCT EN_Date FROM (SELECT * FROM ENTRIES P1 
	     WHERE P1.EN_Date >= ? AND P1.EN_Date <= ?) AS Q1 ORDER BY EN_Date";
        $params = array($beginMonth, $endMonth);
        $dbh = new pders_pdo();
        $result = $dbh->Query($query, $params);

	$reportDates = getList_reportDates();
	$monthArray = array();
	foreach ($result as $month) {
		$curMonth = $month['EN_Date'];
		$log->lwrite(__FILE__, sprintf("cur month value: %s", $curMonth));

		foreach ($reportDates as $dates) {
			if ($dates['DateValue'] == $curMonth) {
				//$log->lwrite(__FILE__, sprintf("We have a match for %s", $curMonth));
				//$log->lwrite(__FILE__, sprintf("Saving display value of %s for  %s", $dates['DateDisplay'],$curMonth));
				$monthArray[]= array('dateValue'=>$curMonth, 'dateDisplay'=>$dates['DateDisplay']);
			}
		}
	}
	foreach ($monthArray as $tMonth ) {
		//$log->lwrite(__FILE__, sprintf("date val: %s      date displ: %s", $tMonth['dateValue'], $tMonth['dateDisplay']));
	}
	return $monthArray;
	// end convertDateRangeToArray()
}



function getFinancialsMultMonths($beginMonth, $endMonth, $aveReportSizeSelected) {
	// 
	// This function will return an multi level array ($masterDataAndAveData)
	//   There are 2 main components of $masterDataAndAveData.  The first component
	//       $masterDataAndAveData[0] is $masterMonthData which contains the primary data
	//       for the monthly historical data and the 3, 6, & 12 monthly average data.  
	//       The second component is $masterDataAndAveData[1] or $AverageReportSize which is the 
	//       'Qualified' monthly averages the user selected - see below comments for more details.
	//
	// $masterDataAndAveData :
	//      $masterDataAndAveData[0] = $masterMonthData    (primary monthly historical data AND 3, 6, & 12 monthly average data)
	//      $masterDataAndAveData[1] = $AverageReportSize    simple array like (3, 6, 12) - see below comments for more details.
	//
	// $masterMonthData:
	//      $masterMonthData[month][acct Number]['Amount_mo_disp']    where month is like '201008' (Aug 2010) or '201112' (Dec 2011)
	//      $masterMonthData[month][acct Number]['Amount_mo_real']
	//      $masterMonthData[month][acct Number]['Amount_ytd_disp']
	//      $masterMonthData[month][acct Number]['Amount_ytd_real']
	//      $masterMonthData[month][acct Number]['FullName']
	//      $masterMonthData[month][acct Number]['Category']    -not yet implemented
	//      $masterMonthData[month][acct Number]['Type']    -not yet implemented
	//      $masterMonthData[$key][acct Number]     where $key is '3moAve', '6moAve', or '12moAve' (value is display type num format)
	//
	// $AverageReportSize takes the passed in $aveReportSizeSelected array which looks like (0=>3, 1=>6, 2=>12) if user
	//    selected all 3 averages (3, 6, 12) and then qualifies each of the 3 values to ensure the month range selected
	//    has enough months to properly calculate those averages.  If we pass in (3, 6, 12) but only have month range
	//    Jan 2011 thru Apr 2011 selected, then we can't calc 6 and 12 mo averages and therefore the final
	//    $AverageReportSize  array will be an array with a single value of (3).
	//
	// Example;
        //      $masterMonthData['201103']['5015']['Amount_mo_disp']  would give 
        //               the month amount for acct 5015(asset Mgr) for Mar 2011
        //                      or
        //      $masterMonthData['201103']['5015']['Amount_mo_real']  would give actual numbers for numerical calcs


        date_default_timezone_set('America/Denver');

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, sprintf("Entering getFinancialsMultMonths(%s, %s)", $beginMonth,$endMonth));

	//
	// convert begin and end months to an array of months that span that time period
	//
	$monthArray = convertDateRangeToArray($beginMonth, $endMonth);

	//
	// Cycle thru each month requested and generate an array for each month
	//    that contains the primary data (ea acct# which then contains month & YTD 
	//    numbers (real & display format)
	// Save this array that contains the month data into a master array - $masterMonthData
	//
	foreach ($monthArray AS $item) {
		$month = $item['dateValue'];
		$masterMonthData[$month] = getFinancials($month);
	}
	//  Next we add monthly averages to $masterMonthData so that it is accessable as such;
	//        $masterMonthData['6moAve'][$acctNum]  returns the (display) average number  (6moAve could also be 3moAve, ect)
	$monthArraySize = count($monthArray);
	$log->lwrite(__FILE__, sprintf("Number of months to retrieve: %d", $monthArraySize));

	$dbh = new pders_pdo();
	// Generate a db query result that contains all line items (ie: all acct numbers)
	$query = "SELECT * FROM LINE_ITEMS";
        //$params = array($lineItem);
        $result = $dbh->Query($query);

	// $aveReportSizeSelected (passed into our function) will define what averages we calc and (try to) display
        // $aveReportSizeSelected=array('one'=>3, 'two'=>6, 'three'=>12);    This is passed into the function - if user selected all 3
	$AverageReportSize = array();

	foreach ($aveReportSizeSelected as $monAve){
		$log->lwrite(__FILE__, sprintf("Number of months to average out: %s", $monAve));
		$log->lwrite(__FILE__, sprintf("first month to get for %d mo average: %s", $monAve, $monthArray[$monthArraySize-$monAve]['dateValue']));
		//  skip the calcs and saving data if total months to be displayed are less than average report (ie:3,6,12)
                if ( $monAve <= $monthArraySize ) {
			// $result is the db query result containing all the acct numbers
			foreach ($result as $lineItem) {
				$li_id = $lineItem['LI_Id'];
				$acctNum = $lineItem['LI_AcctNumber'];
				$cntr2 = 0;
				$aveTot = 0;
				//  Loop thru ea month to get current acct number amount - we'll use this to get an average number
				//    for each acct and save to masterMonthData main array object. Thus we will end up with 
				//    $masterMonthData['6moAve'][$acctNum]  average amount - will also have 3mo and 12mo amounts if applicable.
			        for ( $moCntr = $monthArraySize - $monAve; $moCntr < $monthArraySize; $moCntr++ ) {
					$month = $monthArray[$moCntr]['dateValue'];
					$moAmt = $masterMonthData[$month][$acctNum]['Amount_mo_real'];
					//$log->lwrite(__FILE__, sprintf("Month: %s     acctNum: %s       Tot accumulated: %s", $month, $acctNum, $aveTot));
					//$log->lwrite(__FILE__, sprintf("Monthly amt to add into ave tot: %s", $moAmt));
					$aveTot = $aveTot + $moAmt;
					if ($cntr2 > 50 ) {
						$log->lwrite(__FILE__, sprintf("ERROR - Loop exeeded 50 iterations for acct num: %s", $acctNum));
						break;
					}
				}
				//$log->lwrite(__FILE__, sprintf("Total %d mo amt for %s: %d", $monAve, $acctNum, $aveTot ));
				$key = sprintf("%dmoAve", $monAve);
				//  save averages with the following format;
				//       $masterMonthData['6moAve'][$acctNum]  returns the display number  (6moAve could also be 3moAve, ect)
				$masterMonthData[$key][$acctNum] = addDecimalComma($aveTot / $monAve);
			}
			$AverageReportSize[] = $monAve;
		} else {
			$log->lwrite(__FILE__, sprintf("Total months displayed %d is less than %d months needed to calc %d ave", $monthArraySize, $monAve, $monAve));
			//  Need to also remove the array item from $aveReportSizeSelected array
		}
	}
	
	$masterDataAndAveData[0] = $masterMonthData;
	$masterDataAndAveData[1] = $AverageReportSize;

	return $masterDataAndAveData;
	// end getFinancialsMultMonths($beginMonth, $endMonth, $aveReportSizeSelected) 
}



function getFinancials($month) {
	// This function will return an multi level array ($moData) with the following structure
	//  $moData[acct num] = ('LineItem', 
	//                       'Amount_mo_disp'     monthly amt with commas & decimal pt for display
	//			 'Amount_ytd_disp'   ytd amt with commas & decimal pt for display
        //                       'Amount_mo_real'    monthly amt - numerical format for calc manipulation
        //                       'Amount_ytd_real'    ytd amt - numerical format for calc manipulation
	//			 'FullName',
	//			 'Category',
	//			 'Type')

        date_default_timezone_set('America/Denver');

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

        $log->lwrite(__FILE__, sprintf("Entering getFinancials(%s)", $month));

	$dbh = new pders_pdo();
	$query = "SELECT * FROM ENTRIES WHERE EN_Date = ?";
	$params = array($month);
	$result = $dbh->Query($query, $params);
	$log->lwrite(__FILE__, sprintf("Records found for %s: %d", $month, count($result)));
	$moData = array();
	foreach ($result AS $row) {
		$lineItem = $row['EN_LineItem'];
		$amount_mo = addDecimalComma($row['EN_Amount_mo']);
		$amount_ytd = addDecimalComma($row['EN_Amount_ytd']);
		// I have month and YTD info, now get acct num, acct name, category, and type
		$query2 = "SELECT * FROM LINE_ITEMS WHERE LI_Id  = ?";
		$params2 = array($lineItem);
		$result2 = $dbh->Query($query2, $params2);
		if ( count($result2) > 1 ) {
			$msg = sprintf("Error: Unexpected number of results.  Count: %d", count($result2));
			$log->lwrite(__FILE__, $msg);
			return;
		}
		foreach ( $result2 AS $liRow ) {
			$acctNumber = $liRow['LI_AcctNumber'];
			$fullName = $liRow['LI_FullName'];
			$category = $liRow['LI_Category'];
			$type = $liRow['LI_Type'];
		}

		$msg = sprintf("%s    %s     %s    %s    %s    %s    %s", $acctNumber, $lineItem,
		   $amount_mo, $amount_ytd,$fullName,$category,$type);
		//$log->lwrite(__FILE__, $msg);

		$moData[$acctNumber] = array('LineItem'=>$lineItem, 'Amount_mo_disp'=>$amount_mo, 'Amount_ytd_disp'=>$amount_ytd,
		    'Amount_mo_real'=>$row['EN_Amount_mo'], 'Amount_ytd_real'=>$row['EN_Amount_ytd'],
		    'FullName'=>$fullName, 'Category'=>$category, 'Type'=>$type);
	}
	$moDataCnt = count($moData);
	//$msg = sprintf("Array count for moData: %d", $moDataCnt);
	$log->lwrite(__FILE__, sprintf("Array count for moData: %d", $moDataCnt));
	return $moData;
	// end getFinancials($month)
}



function getList_reportDates() {
	// get unique dates from DB reflecting the reports we have (by year & month)
	// Will return an array of dates that look like;
	//     "DateDisplay"=> "2011-Mar", "DateValue"=>"201103"

	$dates = array("01"=>"Jan", "02"=>"Feb", "03"=>"Mar", "04"=>"Apr", "05"=>"May", "06"=>"Jun", "07"=>"Jul", "08"=>"Aug", "09"=>"Sept", "10"=>"Oct", "11"=>"Nov", "12"=>"Dec");

        date_default_timezone_set('America/Denver');

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');

	$log->lwrite(__FILE__, "Entering getList_reportDates()");

        $db = new pders_pdo();

        $query = "select distinct EN_Date from ENTRIES ORDER BY EN_Date";
        $result = $db->Query($query);
        $disDates = array();
        try {
                //echo sprintf("Num of Resutls: %d<br>", count($result));
                $log->lwrite(__FILE__, sprintf("Num of Resutls: %d<br>", count($result)));
                foreach ($result AS $row) {
                        $year = substr($row[0], 0, 4);
                        $mo = substr($row[0], 4, 2);
                        $monMod = $dates[$mo];
                        //$disDates[] = $row[0];
                        $disDates[] = array("DateDisplay"=>sprintf("%s-%s", $year, $monMod), "DateValue"=>$row[0]);
                        //$log->lwrite(__FILE__, sprintf("Year mo: %s", sprintf("%s-%s", $year, $monMod)));
                }
        } catch (Exception $e) {
                $log->lwrite(__FILE__, sprintf("DB ERROR: %s", $e->getMessage()));
                $log->lwrite(__FILE__, sprintf("Num of Resutls: %d", count($result)));
        }
	return $disDates;
}


function monthToLoadIsValid($monthToLoad) {
	$year = substr($monthToLoad, 0, 4);
	$mo = substr($monthToLoad, 4, 2);
	if ($year > 2000 && $year < 2020) {
		if ($mo > 0 && $mo < 13) return True;
	}
	return False;
}


function LoadMonthly($reportDate) {
	//  This function will parse the monthly report text file such that
	//     it will insert each line item (that is appropriate) into DB.
	//  To create the financial monthly text file open the Preston pdf
	//    and begin selecting data with the first line item - acct 4000
	//    and continue hightlighting on thru to last line item in 
	//    Other accts.  That will highlite most all line items so that
	//    you can copy and then paste into Wordpad doc.  Then also copy
	//    the line for Total Income and paste that below what was copied
	//    in prior step. Save the file with the name as YYYYMM.   This 
	//    function should now be able to parse the file and insert the 
	//    data in to the DB.



        require_once "include/logger.inc";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');
        // write message to the log file 
        $log->lwrite(__FILE__, 'Entering LoadMonthly');

	$log->lwrite(__FILE__, sprintf("Month to load: %s", $reportDate));

	if (! monthToLoadIsValid($reportDate)) return;
	//$reportDate = "201103";
	$filePath = sprintf("/home/rickchurch11/PrestonFiles/CF%s.txt", $reportDate);
	//$file=fopen("/home/rickchurch11/CF201103.txt","r");
	$file=fopen($filePath, "r");
	$cntr = 0;
	$specCntr = 0;
	$specCntr2 = 0;
	$totOprInc_ytd = 0;
	while(!feof($file)) {
		$cntr += 1;
		if ($cntr > 200){
			$log->lwrite(__FILE__, "ERROR !!     hit the max counter of 200 - Problem ??");
			 break;
		}
		$line = fgets($file). "<br />";
		$line = trim($line, "<br />");
		//echo"<br />LINE: ".$line."<br />";
		$log->lwrite(__FILE__, "================================================");

		// Lets first parse out these lines;
		//   Beginning cash balance - needs extra work to get
		//   Ending cash balance
		//   Total Operating Income - needs extra work to get
		//   Total Operating Expenditure
                $pos = strpos(strtolower($line), "total operating expenditure");
                if ($pos === false) {
                        // do nothing
                } else {
                        // I want the 2nd line after this line which will contain
                        //    the Beginning cash balance.  that number ends up as 
                        //    a single item on the line.
                        $specCntr = $cntr + 2;
			$parts = explode("$", $line);
			$totOprExp_mo = StrToFloat($parts[1]);
			$totOprExp_ytd = StrToFloat($parts[2]);
			$log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
                        $log->lwrite(__FILE__, sprintf("Total Operating Expenditure (mo): %s",$totOprExp_mo));
			$log->lwrite(__FILE__, sprintf("Total Operating Expenditure (ytd): %s",$totOprExp_ytd));
                        $log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
			saveToDB($reportDate, "999995", $totOprExp_mo, $totOprExp_ytd);
			continue;
                }
                if ($specCntr != 0 && $cntr == $specCntr) {
			$begCashBal = StrToFloat($line);
                        $log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
			$log->lwrite(__FILE__, sprintf("line: [%s]", $line));
                        $log->lwrite(__FILE__, sprintf("Beginning cash balance: %s", $begCashBal));
			$log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
			if ( $endCashBal != 0 && $begCashBal != 0 ) {
				$cashBalDiff = $endCashBal - $begCashBal;
				$log->lwrite(__FILE__, sprintf("Cash balance difference: %s", $cashBalDiff));
				saveToDB($reportDate, "999990", $cashBalDiff);
			} else {
				$log->lwrite(__FILE__, "Don't have both begin & end cash bal, so not saving cash bal diff");
			}
			saveToDB($reportDate, "999998", $begCashBal);
			continue;
                }
		$endbal = "ending cash balance";
		$pos = strpos(strtolower($line), $endbal);
		if ($pos === false) {
                        // do nothing
                } else {
			$parts = explode("$", $line);
			$endCashBal = StrToFloat($parts[1]);
			$log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
                        $log->lwrite(__FILE__, sprintf("Ending Cash Balance: %s", $endCashBal));
			$log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
                        if ( $endCashBal != 0 && $begCashBal != 0 ) {
                                $cashBalDiff = $endCashBal - $begCashBal;
                                $log->lwrite(__FILE__, sprintf("Cash balance difference: %s", $cashBalDiff));
                                saveToDB($reportDate, "999990", $cashBalDiff);
                        } else {
                                $log->lwrite(__FILE__, "Don't have both begin & end cash bal, so not saving cash bal diff");
                        }
			saveToDB($reportDate, "999999", $endCashBal);
			continue;
		}
		$pos = strpos(strtolower($line), "total operating income");
		if ($pos === false) {
                        // do nothing
                } else {
                        // I want the 2nd line after this line which will contain
                        //    total operating income.  that number ends up as 
                        //    a single item on the line.
                        $specCntr2 = $cntr + 2;
			// get the YTD number and save till we have montly amt
			$dollarParts = explode("$", $line);
			if (count($dollarParts) == 2) $totOprInc_ytd = $dollarParts[1];
		}
		if ($specCntr2 != 0 && $cntr == $specCntr2) {
			$totOpInc_mo = StrToFloat($line);
			
			$totOprInc_ytd = StrToFloat($totOprInc_ytd);
                        $log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
			$log->lwrite(__FILE__, sprintf("line: [%s]", $line));
                        $log->lwrite(__FILE__, sprintf("Total Operating Income (mo): %s", $totOpInc_mo));
			$log->lwrite(__FILE__, sprintf("Total Operating Income (ytd): %s", $totOprInc_ytd));
                        $log->lwrite(__FILE__, "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=");
			saveToDB($reportDate, "999997", $totOpInc_mo, $totOprInc_ytd);
                        continue;
                }

		//
		// split/explode line on single space, get acct num as the first part
		//     and then re-combine the remaining parts into a modified 'line' string object.

                $parts = explode(" ", $line);
		$arrayCnt = count($parts);
		$acctNum = $parts[0];
		$line_m = "";
		$partsCntr = 0;
		while (list($key, $val) = each($parts)) {
			//echo"explode     key: ".$key."    val: ".$val."<br>";
			if ($partsCntr > 0) {
				$line_m = sprintf("%s %s", $line_m, $val);
			}
			$partsCntr += 1;
	
		}
		$parts2 = explode("$", $line_m);

		if (count($parts2) > 2) {
			$log->lwrite(__FILE__, sprintf("LINE: %s", $line));
			$log->lwrite(__FILE__, sprintf("acct num: %s", $acctNum));
			$log->lwrite(__FILE__, sprintf("Item Name: %s", $parts2[0]));
			// Check monthly and YTD for negative values
			$moAmt = StrToFloat($parts2[1]);
			$ytdAmt = StrToFloat($parts2[2]);
			$log->lwrite(__FILE__, sprintf("Month: %s", $moAmt));
			$log->lwrite(__FILE__, sprintf("YTD: %s", $ytdAmt));
			// before I insert do the following;
			//   verify that the acct num exists in db
			saveToDB($reportDate, $acctNum, $moAmt, $ytdAmt);
			//exit("quitting");
		}
	}
	fclose($file);
	//  end LoadMonthly()
}

function saveToDB($reportDate, $acctNum, $moAmt, $ytdAmt=NULL) {
	// This function is called by LoadMonthly() to save monthly
	//    financial report data to DB.  It will first verify that
	//    the line item data is the type wanted (ie: we don't want
	//    Building or Land or Equipment numbers) and will also 
	//    verify that the data item is not already in DB.  It 
	//    checks that by looking at the Line Item id and current
	//    month and if that combination exists, will not insert.

        require_once "include/logger.inc";

        // Logging class initialization 
        $log = new Logging();
        // set path and name of log file (optional) 
        $log->lfile('/home/rickchurch11/logs/rdc.log');
        // write message to the log file 
        $log->lwrite(__FILE__, 'Entering saveToDB');

	// 
	// GET EXISTING MAX ID FROM ENTRIES TABLE
	//
	$dbh = new pders_pdo();
	$query = "select max(EN_EntryId) FROM ENTRIES";
        $result = $dbh->Query($query);
	$whatisit = gettype($result);
	//echo "db result type: ".$whatisit."<br>";
	if ($whatisit == "object" || $whatisit == "array") {
        	// We have a normal result set returned
                $parseResult = True;
	} else {
        	// Had an error executing the query
                $parseResult = False;
        }
	if (!$parseResult) {
        	$log->lwrite(__FILE__, sprintf("Error getting max(id): %s", $result));
		//exit("done");
		return;
	}
	foreach ($result AS $row) {
		$id = $row[0];
		$nextId = $id +1;
		$log->lwrite(__FILE__, sprintf("max id: %s", $id));
		$log->lwrite(__FILE__, sprintf("next Id: %s", $nextId));
	}

	//
	// get the line item id from LINE_ITEMS table	
	//
	$query = sprintf("select LI_Id FROM LINE_ITEMS WHERE LI_AcctNumber = '%s'", $acctNum);
        $result = $dbh->Query($query);
        $whatisit = gettype($result);
        //echo "db result type: ".$whatisit."<br>";
        if ($whatisit == "object" || $whatisit == "array") {
                // We have a normal result set returned
                $parseResult = True;
        } else {
                // Had an error executing the query
                        select>
                $parseResult = False;
        }
        if (!$parseResult) {
                $log->lwrite(__FILE__, sprintf("Error getting max(id): %s", $result));
                //exit("done");
		return;
	}
	$log->lwrite(__FILE__, sprintf("count records (qry for line Id): %d", count($result)));
	if (count($result) == 0) {
		$log->lwrite(__FILE__, "");
		$log->lwrite(__FILE__, "");
		$log->lwrite(__FILE__, "Don't want this item - aborting the insert               **********");
		$log->lwrite(__FILE__, "");
		$log->lwrite(__FILE__, "");
		return;
	}
        foreach ($result AS $row) {
                $LI_Id = $row['LI_Id'];
		$log->lwrite(__FILE__, sprintf("LI_Id: %s", $LI_Id));
	}

	//
	// verify we are not putting a duplicate entery in the db
	//
	//$query = "SELECT COUNT(*) FROM ENTRIES WHERE EN_Date = ? AND EN_LineItem = ?";
	//$params = array($reportDate, $LI_Id);
	$query = sprintf("SELECT COUNT(*) FROM ENTRIES WHERE EN_Date = %s AND EN_LineItem = %s", $reportDate, $LI_Id);
        //$result = $dbh->Query($query, $params);
	$result = $dbh->Query($query);
        $whatisit = gettype($result);
        //echo "db result type: ".$whatisit."<br>";
        if ($whatisit == "object" || $whatisit == "array") {
                // We have a normal result set returned
                $parseResult = True;
        } else {
                // Had an error executing the query
                $parseResult = False;
        }
        if (!$parseResult) {
                $log->lwrite(__FILE__, sprintf("Error getting max(id): %s", $result));
                //exit("done");
		return;
        }
        foreach ($result AS $row) {
                $existingCnt = $row[0];
                $log->lwrite(__FILE__, sprintf("existingCnt: %s", $existingCnt));
        }
	if ($existingCnt > 0) {
		$log->lwrite(__FILE__, "Already inserted this into db - aborting the insert");
		return;
	}
	//exit("stop for now again.");

	//
	//  INSERT DATA INTO DATABASE
	//
	$log->lwrite(__FILE__, "Inserting into DB");
	$table = "ENTRIES";
	$data = array('EN_EntryId'=>$nextId, 'EN_Date'=>$reportDate, 'EN_LineItem'=>$LI_Id, 'EN_Amount_mo'=>$moAmt, 'EN_Amount_ytd'=>$ytdAmt);
	$dbh->InsertRow($table, $data);
	
	// end saveToDB()
}


function DecimalToStr($num) {
	// convert decimal value to dollar string value
	//  ie converts 5000 to $5,000 or -15000 to ($15,000)
	$len = strlen($num);
	$neg = False;
	if (substr($num, 0, 1) == "-") {
		$len = $len -1;
		$neg = True;
		$num = substr($num, 1, $len);
	}
	$nuNum = "";
	for ($cntr = 0;$cntr < $len; $cntr++) {
		$pos = $len - $cntr - 1;
		$threes = $cntr % 3;
		if ($cntr > 0 && $threes == 0 ) {
			$nuNum = sprintf("%s,%s", substr($num, $pos, 1), $nuNum);
		} else {
			$nuNum = sprintf("%s%s", substr($num, $pos, 1), $nuNum);
		}
	}
	$nuNum = sprintf("$%s", $nuNum);
	if ($neg) {
		$nuNum = sprintf("(%s)", $nuNum);
	}
	return $nuNum;

}

function StrToFloat($oldnum) {

	// this assumes that the sender verified we have all numeric values
	//    except for "." or "," or ")"  - ie:  1,234.56 or 1,234.56)
	// This function takes the string number with comma and dec point and converts to Float
	// This function also checks to see if we have a negative number as denoted by the 
	//      last chr being ")".

	// check for $ in value passed in
	/*
	$pos = strpos(strtolower($oldnum), "$");
	if ($pos === false) {
		// do nothing
	} else {
		$dllrParts = explode("$", $oldnum);
		$oldnum = $dllrParts[1];
	}
	*/
	$dllrParts = explode("$", $oldnum);
	if (count($dllrParts) > 1) $oldnum = $dllrParts[1];

	$negNum = False;
	$parenParts = explode(")", $oldnum);
	if (count($parenParts) > 1) $negNum = True;
	$oldnum = $parenParts[0];

	$nuVal = "";
	$numparts = explode(",", $oldnum);
	while (list($key, $val) = each($numparts)){
		$nuVal = sprintf("%s%s", $nuVal, $val);
	}
	$nuVal = (float)$nuVal;
	if ($negNum) $nuVal = -$nuVal;
	if ($negNum) {
		//echo "<br>=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=<br>";
		//echo "!!!  Negative Amount !!  ".$nuVal."<br>";
		//echo "=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=-~=<br><br>";
	}

	return $nuVal;
	// end StrToFloat()
}
 


function LoadFile()
{
  // This function does the inital load of data into
  //   the LINE_ITEMS table which consists of acct numbers,
  //   acct names, category, type, ect.  This should only 
  //   need to be done once for each management company.

  $masterData = array(0);
  $file=fopen("/home/rickchurch11/DB_Schema3","r");
  $masterData = array();
  $data = array();
  while(!feof($file))
  {
    //echo "------------------------------------<br />";
    //echo fgets($file). "<br />";
    $line = fgets($file). "<br />";
    //echo $line;
    $token = strtok($line, "$");
    $cntr = 0;
    while ($token != false)
    {
      // tokens are how you parse/split the line read.  I have '$' that I want
      //    to split the line on.
      $token = trim($token); 
      if ($cntr == 3)
      {
        $token = trim($token, "<br />");
        if ($token == "")
        {
          $token = "Maintenance & Repairs";
        }
        else
        {
          $token = trim($token);
        }
      
      }
      if ($cntr == 1)
      {
        for ($i=0; $i < strlen($dummy); $i++)
        {
          //echo $i . "     [" . $dummy[$i] . "]";
        }
    
      }
      if ($cntr == 0)
      {
        $data['LI_AcctNumber'] = $token;
      }
      if ($cntr == 1)
      {
        $data['LI_FullName'] = $token;
      }
      if ($cntr == 2)
      {
        $data['LI_AcctNickName'] = $token;
      }
      if ($cntr == 3)
      {
        $data['LI_Category'] = $token;
      }

      //  this was printing ok ::: echo "[$token]<br />";
      //$masterData = $data;    
      $token = strtok("$");
      $cntr += 1;
    }
    $firstBadRecord = trim($data['LI_AcctNumber'], "<br />");
    //echo " acct# [". $firstBadRecord . "]<br />";
    if ($firstBadRecord != "")
    {
      $query = "select max(LI_Id) from LINE_ITEMS";
      $db = new pders_pdo();
      $result = $db->Query($query);
      foreach ($result AS $row) {
        $maxid = $row[0];
        //echo "maxid: ".$row[0]."<br>";
      }
      $maxid += 1;
      //echo "NextId: ".$maxid."<br>";
      $data['LI_Id']=$maxid;
      echo "printing one entry<br>";
      $query = "select LI_AcctNumber from LINE_ITEMS"; 
      $result = $db->Query($query);
      $skip = 0;
      foreach ($result AS $row) {
        $acctnum = $row['LI_AcctNumber'];
        if ($acctnum == $data['LI_AcctNumber']) {
          $skip = 1;
          break;
        }
      }
      echo "skip: ".$skip."<br>";
      if ($skip == 0) {
        $table = 'LINE_ITEMS';
        $result = $db->InsertRow($table, $data);
        echo "DB Insert Successfull !!<br>";
      }

      PrintData($data);
      array_push($masterData, $data);
    }
  }
  fclose($file);
  echo "+++++++++++++++++++++++++++++++++++++++<br />";
  echo "Parsing masterData object<br />";
  //print_r($masterData);
  ////PrintData2($masterData);

  // end LoadFile()
}

/*
//LoadFile();
//LoadMonthly();

exit("done");


$data['one'] = 1;
$data['two'] = 2;
$data['three'] = 3;
$data['four'] = 4;

//PrintData($data);

*/

function PrintData($data)
{
  while (list($key, $val) = each($data))
  {
    echo "$key => $val<br />";
    //echo "key: $key       val: $val<br />";
  }
}

function PrintData2($data)
{
  // parses what I had as a list of dict objects or 
  //   2 level array
  while (list($key, $val) = each($data))
  {
    echo "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=<br />";
    while (list($key2, $val2) = each($val))
    {
      echo "$key2 => $val2<br />";
      //echo "key: $key       val: $val<br />";
    }
  }
}

?>

