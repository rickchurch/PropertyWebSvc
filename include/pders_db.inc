<?php



class pders_db {
        private $dbuser = 'rickchurch11';
        private $dbpwd = 'noelcat01';
        private $dbname = 'rickchurch11';

        //print_r "in pders_db class<br>";
        
        function Query($qry)
        {
                //print_r "db user is " . $dbuser;
                $db = new mysqli('localhost', $this->dbuser, $this->dbpwd, $this->dbname);
                if ($db->errno) {
			printf("Database ERROR: %s", $db->error);
		}
                $result = $db->query($qry, MYSQLI_STORE_RESULT);
                $db->close();
                return $result;       
        }        
}


class pders_pdo {
        private $dbuser = 'rickchurch11';
        private $dbpwd = 'noelcat01';
        private $dbname = 'rickchurch11';
        private $hostname = 'localhost';
	//private $hostDbNames = sprintf("mysql:host=%s ;dbname=%s", $hostname, $dbname);

	public function Query($qry, $params=NULL) {
                try {
                        /*
                        $qry needs to be in one of the following formats;
                              "select UserId, FirstName, LastName from USERS where LastName = ?"
                              "select UserId, FirstName, LastName from USERS"
                        $params (if passed in) needs to be an array (even if only one value)

                        */
                        $hostDbNames = sprintf("mysql:host=%s;dbname=%s", $this->hostname, $this->dbname);
                        $dbh = new PDO($hostDbNames, $this->dbuser, $this->dbpwd);

                        $db_query = $dbh->prepare($qry);
                        $db_query->execute($params);
                        $result = $db_query->fetchAll();

                } catch (PDOException $exception) {
                        return $exception->getMessage();
                }
                //$result = $dbh->query($qry);
                $db_query = NULL;
                $dbh = NULL;
                return $result;

	}

	public function InsertRow($table, $data) {
		// This function assumes that $data is an associative array
		require_once "include/functions.inc";
		$log = new Logging();
		$log->lfile('/home/rickchurch11/logs/rdc.log');
		$subqry = "";
		// Modify the data array keys so we can use Prepared statements.  In other words, the
		//   keys (column names) must follow syntax of being a Named Parameter which means
		//   the key/column name must be prepended with colon.  So modify the array keys (saving
		//   as data_m) and also create subquery containing same (Named Parameters).
		foreach( $data as $key => $value){
			$log->lwrite(__FILE__, sprintf("Key: %s      Value: %s", $key, $value));
			$data_m[sprintf(":%s",$key)] = $value;
			if ($subqry == "") {
				$subqry = sprintf("%s = :%s", $key, $key);
			} else {
				$subqry = sprintf("%s, %s = :%s", $subqry, $key, $key);
			}
		}
		try {
			// log new array key/values
			foreach( $data_m as $key => $value){
				$msg = sprintf("data_m   Key: %s      Value: %s", $key, $value);
				$log->lwrite(__FILE__, $msg);
			}
                        $hostDbNames = sprintf("mysql:host=%s;dbname=%s", $this->hostname, $this->dbname);
                        $dbh = new PDO($hostDbNames, $this->dbuser, $this->dbpwd);
			$qry = sprintf("INSERT INTO %s SET %s", $table, $subqry);
			$log->lwrite(__FILE__, sprintf("Query: %s", $qry));
			// first step after making the connection is to 'prepare' and then we will
			//    execute.
			$stmnt = $dbh->prepare($qry);
			$result = $stmnt->execute($data_m);
			//echo "insert result data type: ".gettype($result)."<br>";
			// result looks like it is type bool - not int for rows affected
			$result = settype($result, string);
			$log->lwrite(__FILE__, sprintf("Insert result (1 or 0): %s", $result));
			$stmnt = null;
                        $dbh = null;
			return $result;
                } catch (PDOException $exception) {
                        return $exception->getMessage();
                }
	
	}
}





?>
