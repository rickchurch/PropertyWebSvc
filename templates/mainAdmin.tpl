<?php
session_start();
?>
<html>
  <head>
      <title>{$title}</title>
      <script type="text/javascript">
         function show_alert()
         {
         alert("Testing javascript Alert box");
         }
      </script>
  </head>
  <body>


	<form action = "adminAction.php" method = "post">
	<!--	<form action = "index.php" method = "post">    -->

      <!--    <p> Select One Action Below;
      </p><br>    -->

	{if $login == 'adminpass'}

                <fieldset style="width:600px;margin-left:150px;border-style: solid;border:#DAA520 1px solid;border-width:medium;background:Beige;">
                        <legend id="legend"> Parse & Load Data </legend>
			<br />


	                <!--    <input type="radio" name="action" value="loadMonthFinancialData" style="margin-left:10px;"> 
					Load financial data (use flat file to load data into DB)<br /><br />    -->
			<span style="margin-left:30px;">Enter the month to load (ie:201103) </span>
                        <input type="text" name="monthOfDataToLoad"> <br><br>

			<input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Load financial data - use flat file"><br /><br />
			<br /><br />

			<span style="margin-left:30px;">Enter the year to load Budget (ie:2011) </span>
                        <input type="text" name="budgetYearToLoad"> <br><br>

                        <input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Load BUDGET data - use flat file"><br /><br />


                        </p>
                </fieldset>

                <br><br><br>


                <fieldset style="width:600px;margin-left:150px;border-style: solid;border:#DAA520 1px solid;border-width:medium;background:Beige;">
                        <legend id="legend"> MISC </legend>
                        <br />

                        <input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Generate Function List File"><br /><br />
                        <br /><br />

                        <input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Test new class"><br /><br />
                        <br /><br />

                        </p>
                </fieldset>


	
		<br><br><br>

                <fieldset style="width:600px;margin-left:150px;border-style: solid;border:#DAA520 1px solid;border-width:medium;background:Beige;">
                        <legend id="legend"> Review SINGLE Month Data </legend>

				<br />

			        <select name="selDate" style="margin-left:170px;" onchange="show_alert()">
			        {section name=date loop=$dates}
		                <option value={$dates[date].DateValue}>{$dates[date].DateDisplay}</option>
			        {/section}
			        </select>

			<br /><br />
                        <!--    <input type="radio" name="action" value="ReadMoFinData_eaEntry" style="margin-left:10px;"> 
                                       Show each entry<br><br>    -->
			<input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Show each entry"><br><br>

			<!--    <input type="radio" name="action" value="ShowLineItemRpt" style="margin-left:10px;"> 
			               Show Line Item Rpt (like a reg monthly rpt) <br><br>    -->
			<input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Show Line Item Rpt (like a reg monthly rpt)"><br><br>

			<!--    <input type="radio" name="action" value="ShowReconcilliation" style="margin-left:10px;"> 
			               Show Reconcilliation <br><br>    -->
			<input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Show Reconcilliation"><br><br>
                        </p>
                </fieldset>

	{/if}


	<!--    MAIN     REVIEW MONTHLY DATA   SECTION        -->


                <br><br><br>

                <fieldset style="width:750px;margin-left:50px;border-style: solid;border:#DAA520 1px solid;border-width:medium;background:Beige;">
                        <legend id="legend" style="font-size:20;"><b> Review Monthly Financial Data </b></legend>

                        <br />
			<span style="margin-left:20px;color:blue;">Select a beginning and ending month and then</span><br />
			<span style="margin-left:30px;color:blue;">click on "Show Selected Months" button.</span>
			<br /><br />

                        <span style="margin-left:20px;font-size:18;"> Beginning Month: </span><select name="selStartMonth">
                        {section name=date loop=$dates}
                        <option value={$dates[date].DateValue}>{$dates[date].DateDisplay}</option>
                        {/section}
                        </select>

                        <span style="margin-left:60px;font-size:18;">Ending Month: </span><select name="selEndMonth">
                        {section name=date loop=$dates}
			{if $maxDate == $dates[date].DateValue}
				<option value={$dates[date].DateValue} selected="selected">{$dates[date].DateDisplay}</option>
			{else}
                        	<option value={$dates[date].DateValue}>{$dates[date].DateDisplay}</option>
			{/if}
                        {/section}
                        </select>


			<p style="margin-left:100px;border-bottom-style:solid;border-bottom-width:2px;border-bottom-color:GoldenRod;margin-right:200px"></p>


			<input style="margin-left:20px;" type="checkbox" name="3moave" value="yes" checked> 
				<span style="margin-left:20px;font-size:18;">  Show last 3 month Average </span>
				<span style="margin-left:20px;font-size:12;color:blue"> (Shows the average of the last 3 months - within the date range selected) </span><br />

			<input style="margin-left:20px;" type="checkbox" name="6moave" value="yes"> 
				<span style="margin-left:20px;font-size:18;">  Show last 6 month Average </span>
				<span style="margin-left:20px;font-size:12;color:blue"> (Shows the average of the last 6 months - within the date range selected) </span><br />

			<input style="margin-left:20px;" type="checkbox" name="12moave" value="yes"> 
				<span style="margin-left:20px;font-size:18;">   Show last 12 month Average </span>
				<span style="margin-left:20px;font-size:12;color:blue"> (Shows the average of the last 12 months - within the date range selected) </span><br /><br />

			<span style="margin-left:80px;font-size:12;color:blue;"> Note: The 3, 6, 12 month average is not displayed if the date range 
				doesn't include that number of </span><br />
			<span style="margin-left:80px;font-size:12;color:blue;"> months (ie: if you selected Mar 2011 thru Aug 2011, that would 
				contain 7 months - enough </span><br />
			<span style="margin-left:80px;font-size:12;color:blue;"> to calculate the last 3 or last 6 month average, but not enough for a 12 month average) </span><br /><br />

			<p style="margin-left:100px;border-bottom-style:solid;border-bottom-width:2px;border-bottom-color:GoldenRod;margin-right:200px"></p>


			<span style="margin-left:20px;font-size:18;"> Adjust Report width </span><input type="text" name="reportWidth" style="margin-left:10px;" size=5 value=0>
				<span style="margin-left:10px;font-size:10pt;color:blue;"> (Defaults to 0. Max is 400 to make report wider) </span><br>

                        <br />
                        <!--   <input type="radio" name="action" value="showMultMonthsData" style="margin-left:10px;"> 
                                       Show Multiple Months <br><br>    -->


			<p style="margin-left:100px;border-bottom-style:solid;border-bottom-width:2px;border-bottom-color:GoldenRod;margin-right:200px"></p><br />


			<input type="submit" style="background=LightCyan;" name="SubmitBtn" value="Show Selected Months">
			
			<br><br>
                        </p>
                </fieldset>

                <br><br><br>

        <!--    DISPLAY LEGACY PICTURES     SECTION        -->

                <fieldset style="width:750px;margin-left:50px;border-style: solid;border:#DAA520 1px solid;border-width:medium;background:Beige;">
                        <legend id="legend" style="font-size:20;"><b> View Legacy Pictures  </b></legend>

                        <br />
			<input type="submit" style="background=LightCyan;margin-left:50px;" name="SubmitBtn" value="Show Pictures">
                        <br><br>

                </fieldset>

                <br><br><br>



	<br><br><br>
	<input type="submit" style="margin-left:200px;background=LightCyan;" name="SubmitBtn" value="Logout">
	<!--   <span style="font-size:8pt;"> Authorized Users only</span> -->
      <br>
  </body>
</html>

