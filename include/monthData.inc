<?php

require_once "include/pders_db.inc";
require_once "include/logger.inc";


/*    monthData class
 * -  contains ____,  ____, and _____  methods
 * -  
 *
*/


class monthData {
	//          ***************   STRUCTURE OF monthData class   ***************
	//
	//   month               property showing month (ie: 201107 for July 2011)
	//   dataType            property that indicates what data structure we have - monthData
	//   acctNum->           subclass with the following structure
	//       - 'dataType'          acctNum dataType
	//       - 'number'            acct number
        //       - 'amount_mo_disp'     monthly amt with commas & decimal pt for display
        //       - 'amount_ytd_disp'   ytd amt with commas & decimal pt for display
        //       - 'amount_mo_real'    monthly amt - numerical format for calc manipulation
        //       - 'amount_ytd_real'    ytd amt - numerical format for calc manipulation

	public $month;
	public $dataType;

	function __construct($month) {

		require_once "include/functions.inc";

	        date_default_timezone_set('America/Denver');
        	// Logging class initialization 
	        $log = new Logging();
        	// set path and name of log file (optional) 
	        $log->lfile('/home/rickchurch11/logs/rdc.log');

        	$log->lwrite(__FILE__, sprintf("Entering Class monthData setMonthData(%s)", $month));

		$this->month = $month;
		$this->dataType = "monthData";

	        $dbh = new pders_pdo();
		//
		// Get all income & expense entries from DB for the requested month
		//
        	$query = "SELECT * FROM ENTRIES WHERE EN_Date = ?";
	        $params = array($month);
        	$result = $dbh->Query($query, $params);
	        $log->lwrite(__FILE__, sprintf("Records found for %s: %d", $month, count($result)));
        	$moData = array();
	        foreach ($result AS $row) {
        	        $lineItem = $row['EN_LineItem'];
                	$amount_mo = addDecimalComma($row['EN_Amount_mo']);
	                $amount_ytd = addDecimalComma($row['EN_Amount_ytd']);
			//
			// I have month & ytd info - now get associated acct num, acct name, category, & type
			//
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
			$msg = sprintf("%s    %s", $acctNumber,$fullName);
	                //$log->lwrite(__FILE__, $msg);

			//
			// This is the heart of the month data object - creating the acctNumber sub class object
			//
			$this->$acctNumber = new acctNum($acctNumber, $amount_mo, $amount_ytd, $row['EN_Amount_mo'],
				$row['EN_Amount_ytd'], $fullName);

			//   DEPRICATED ARRAY OBJECT CONSTRUCT  *****************
        	        //$moData[$acctNumber] = array('LineItem'=>$lineItem, 'Amount_mo_disp'=>$amount_mo, 'Amount_ytd_disp'=>$amount_ytd,
                	//    'Amount_mo_real'=>$row['EN_Amount_mo'], 'Amount_ytd_real'=>$row['EN_Amount_ytd'],
	                //    'FullName'=>$fullName, 'Category'=>$category, 'Type'=>$type);
        	}

	}
}

class acctNum {

	public $dataType;
	public $number;
        public $amount_mo_disp;
        public $amount_ytd_disp;
        public $amount_mo_real;
        public $amount_ytd_real;
        public $fullName;
        public $category;
        public $type;


	function __construct($acctNumber, $amount_mo, $amount_ytd, $amtMoReal, $amtYTDReal, $fullName) {

		$this->dataType = 'acctNum';
		$this->number = $acctNumber;
	        $this->amount_mo_disp = $amount_mo;
        	$this->amount_ytd_disp = $amount_ytd;
	        $this->amount_mo_real = $amtMoReal;
        	$this->amount_ytd_real = $amtYTDReal;
	        $this->fullName = $fullName;
		// Category and Type to be added in later
        	$this->category = "";
	        $this->type = "";
	}

	public function set_Amount_mo_disp($amt) {
		$Amount_mo_disp = $amt;
	}
	public function get_Amount_mo_disp() {
		return $Amount_mo_disp;
	}

        public function set_Amount_ytd_disp($amt) {
                $Amount_ytd_disp = $amt;
        }
        public function get_Amount_ytd_disp() {
                return $Amount_ytd_disp;
        }

        public function set_Amount_mo_real($amt) {
                $Amount_mo_real = $amt;
        }
        public function get_Amount_mo_real() {
                return $Amount_mo_real;
        }

        public function set_Amount_ytd_real($amt) {
                $Amount_ytd_real = $amt;
        }
        public function get_Amount_ytd_real() {
                return $Amount_ytd_real;
        }

        public function set_FullName($amt) {
                $FullName = $amt;
        }
        public function get_FullName() {
                return $FullName;
        }

        public function set_Category($amt) {
                $Category = $amt;
        }
        public function get_Category() {
                return $Category;
        }

        public function set_Type($amt) {
                $Type = $amt;
        }
        public function get_Type() {
                return $Type;
        }
}




?> 
