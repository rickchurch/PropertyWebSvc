<?php


/*
 *  * Logging class: 
 * - contains lfile, lopen and lwrite methods 
 * - lfile sets path and name of log file 
 * - lwrite will write message to the log file 
 * - first call of the lwrite will open log file implicitly 
 * - message is written with the following format: hh:mm:ss (script name) message  
*/
class Logging
	{
	// define default log file
	private $log_file = '/tmp/logfile.log';
	// define file pointer
	private $fp = null;
	// set log file (path and name)
	//date_default_timezone_set('America/Denver');
	//echo date('D,F j, Y, h:i:s A');
	public function lfile($path) 
	{
		$this->log_file = $path;
	}
	// write message to the log file 
	public function lwrite($script_path, $message)
	{
		// if file pointer doesn't exist, then open log file
		if (!$this->fp) $this->lopen();
		
		// define script name - 01Dec11 don't seem to work so use alt method
		//$script_name = pathinfo($_SERVER['PHP_SELF'], PATHINFO_FILENAME);
                $baseName = basename($script_path);
                $parts = explode(".", $baseName);
                $script_name = $parts[0];

		// define current time 
		$time = date('H:i:s');
		// write current time, script name and message to the log file
		fwrite($this->fp, "$time ($script_name) $message\n"); 
	}
	// open log file
	private function lopen()
	{
		// define log file path and name
		$lfile = $this->log_file;
		// define the current date (it will be appended to the log file name)
		$today = date('Y-m-d');
		// open log file for writing only; place the file pointer at the end of the file
		// if the file does not exist, attempt to create it
		$this->fp = fopen($lfile . '_' . $today, 'a') or exit("Can't open $lfile!");     
		try {
			chmod($lfile . '_' . $today, 0777);
			//$tree = 5;
		} catch(Exception $e) {
			$msg = sprintf("ERROR: (File: %s  Line: %s): %s", $e->getFile(), $e->getLine(), $e->getMessage() );
		        $log->lwrite(__FILE__, $msg);
		}

	} 
}




?>
