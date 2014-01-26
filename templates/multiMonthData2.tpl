<html>
  <head>
      <title>{$title}</title>
  </head>
  <body>


	<form action = "adminMenu.php" method = "post">
	<!--	<form action = "index.php" method = "post">    -->

      <p>
      </p><br>

		<table style ="width:{$reportSize}%">
		

		<!--    Add table HEADERS    -->

		<tr>
                <!--    <th style ="border-bottom:5px solid LawnGreen;color:white;"> . </th>    -->
		<th style ="border-bottom:5px solid LawnGreen;text-align:center;">Show Graph</th>
		<th style ="border-bottom:5px solid LawnGreen;padding-left:15px;padding-right:15px;"> Acct # </th>
		<th style ="border-bottom:5px solid LawnGreen;padding-left:50px;"> Operating Income </th>
		{section name=month loop=$monthArray}

			<th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:50px;"> 
			    {$monthArray[month].dateDisplay} </th>
		{/section}
		{section name=averageReport loop=$AverageReportSize}
			{if $AverageReportSize[averageReport] == "TOTAL" || $AverageReportSize[averageReport] == "AVERAGE"}
				<th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:50px;color:blue">
                	           {$AverageReportSize[averageReport]} </th>
			{else}
				<th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:50px;color:blue"> 
				   Last {$AverageReportSize[averageReport]} Mo Average </th>
			{/if}
		{/section}
		</tr>

		<!--    end -  table HEADERS    -->


		{section name=acctNum loop=$displayAcctNumArray}

			<!--    Loop thru all acct numbers that were passed in    -->


			<!--    this if block only inserts/builds the header lines or adds line spacing between sections    -->

			{if $displayAcctNumArray[acctNum].LI_AcctNumber eq '5010'}
				<!-- 5010 is Management Fees (first expense item)  -->

				<!--  Added 2 empty lines to break up Income & Expense sections -->
				<tr><td style ="color:white;">.</td></tr>
				<tr><td style ="color:white;">.</td></tr>

				<!--  add the Operating Expenses  header with heavy underlines for ea col -->
				<tr>
				<th style ="border-bottom: 5px solid LawnGreen;color:white;">.</th>
				<th style ="border-bottom: 5px solid LawnGreen;color:white;">.</th>
				<th style ="border-bottom: 5px solid LawnGreen;padding-left:50px;"> Operating Expense </th>
				{section name=month loop=$monthArray}
					<th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:50px;">
					      {$monthArray[month].dateDisplay}</th>
				{/section}

				{section name=averageReport loop=$AverageReportSize}
					<th style ="text-align:right;border-bottom: 5px solid LawnGreen;color:white;">.</th>
				{/section}
				</tr>
			
			{elseif $displayAcctNumArray[acctNum].LI_AcctNumber eq '999998'}
				<!-- 999998  is Begin Cash Balance - first item for next section -  OTHER ACCTs  -->

				<!-- add 2 line spaces -->
				<tr><td style ="color:white;">.</td></tr>
				<tr><td style ="color:white;">.</td></tr>

                        {elseif $displayAcctNumArray[acctNum].LI_AcctNumber eq '3010'}
                                <!-- 3010 is Owner Draw  -->

                                <!--  Adding 2 empty lines to break up Expense & Owner Draw sections and then added the
                                     Owner Contribution/Draw  header with heavy underlines for ea col -->

                                <!-- But first check for any line items that are all zero amts for all months & don't display this section -->
                                {$showLineFlag = 0}
                                {section name=month loop=$monthArray}
                                        {if $MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_disp != "0"}
                                                {$showLineFlag = 1}
                                        {/if}
                                {/section}
                                {if $showLineFlag == 1}
					<!--  Added 2 empty lines to break up Expense & Owner Draw sections -->
	                                <tr><td style ="color:white;">.</td></tr>
        	                        <tr><td style ="color:white;">.</td></tr>

					<!--  add the Owner Contribution/Draw  header with heavy underlines for ea col -->
					<tr>
					<th style ="border-bottom: 5px solid LawnGreen;color:white;">.</th>
                	                <th style ="border-bottom: 5px solid LawnGreen;color:white;">.</th>
                        	        <th style ="border-bottom: 5px solid LawnGreen;padding-left:20px;"> Owner Contribution/Draw </th>
                                	{section name=month loop=$monthArray}
		                                <th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:10px;color:white;">.</th>
        	                        {/section}

                	                {section name=averageReport loop=$AverageReportSize}
        	                	        <th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:10px;color:white;">.</th>
                                	{/section}
	                                </tr>
				{/if}

                        {elseif $displayAcctNumArray[acctNum].LI_AcctNumber eq '1030'}
                                <!-- 1030 is Sec Bank Dep Acct  (first 'Other'  item)  -->

                                <!--  Add 2 empty lines to break up Expense & Other sections -->
                                <tr><td style ="color:white;">.</td></tr>
                                <tr><td style ="color:white;">.</td></tr>

				<!--  add the Other Expenses  header with heavy underlines for ea col -->
				<tr>
				<th style ="border-bottom: 5px solid LawnGreen;color:white;"> . </th>
                                <th style ="border-bottom: 5px solid LawnGreen;"> Acct # </th>
                                <th style ="border-bottom: 5px solid LawnGreen;padding-left:20px;"> Other Accounts </th>
                                {section name=month loop=$monthArray}
	                                <th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:10px;">
        	                              {$monthArray[month].dateDisplay}</th>
                                {/section}
	
                                {section name=averageReport loop=$AverageReportSize}


		                        {if $AverageReportSize[averageReport] == "TOTAL" || $AverageReportSize[averageReport] == "AVERAGE"}
						<th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:10px;color:blue;">
	                                            {$AverageReportSize[averageReport]} </th>
		                        {else}
	        	                        <th style ="text-align:right;border-bottom: 5px solid LawnGreen;padding-left:10px;color:blue;">
						     Last {$AverageReportSize[averageReport]} Mo Average </th>
					{/if}
                                {/section}
                                </tr>

			{/if}

			<!--  previous if block was only to insert/build the header lines or adding line spacing between sections  -->


			<!--  This next if block is where we actually insert the numbers/values into the table row  -->

			{if $displayAcctNumArray[acctNum].LI_AcctNumber eq '999995' ||
				$displayAcctNumArray[acctNum].LI_AcctNumber eq '999997' ||
				$displayAcctNumArray[acctNum].LI_AcctNumber eq '999990'}
				<!-- 999995 is TotalOperatingExpenditure -->
				<!-- 999997 is TotalOperatingIncome -->
				<!-- 999990 is cashBalDiff  -->

				<!-- make line bold and show *** for the acct num  -->
				<tr style ="font-weight:bold;">
				<td style ="text-align:center;"> <input type="checkbox" name="chkbxAcct[]" value={$displayAcctNumArray[acctNum].LI_AcctNumber}></td>
                                <td style ="border-bottom: 1px solid black;padding-left:20px;"> *** </td>
                                <td style ="border-bottom: 1px solid black;padding-left:20px;">{$displayAcctNumArray[acctNum].LI_FullName}</td>

                                {section name=month loop=$monthArray}

				<!--  if we have cashBalDiff AND it is negative, set color to red  -->
				{if $MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_real lt 0 && $displayAcctNumArray[acctNum].LI_AcctNumber eq '999990'}
					<td style ="text-align:right;border-bottom: 1px solid black;color:red">
                	                   {$MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_disp}</td>
				{else}
	                                <td style ="text-align:right;border-bottom: 1px solid black;">
					   {$MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_disp}</td>
				{/if}
                                {/section}

				<!--   Populate the columns for 3, 6, 12 mo averages as appropriate    -->
				{section name=averageReport loop=$AverageReportSize}
					{if $AverageReportSize[averageReport] == "TOTAL" || $AverageReportSize[averageReport] == "AVERAGE"}
						{$aveLable = $AverageReportSize[averageReport]}
					{else}
						{$aveLable = sprintf("%dmoAve",$AverageReportSize[averageReport])}
					{/if}
					<td style ="text-align:right;border-bottom: 1px solid black;color:blue">
					   {$MasterMonthData.{$aveLable}.{$displayAcctNumArray[acctNum].LI_AcctNumber}}</td>
				{/section}
	                        </tr>
			{elseif $displayAcctNumArray[acctNum].LI_AcctNumber eq '999998' ||
                                $displayAcctNumArray[acctNum].LI_AcctNumber eq '999999'}
				<!-- 999998 is BeginningCashBalance  -->
				<!-- 999999 is EndingCashBalance  -->

				<!-- show *** for the acct num but Don't use Bold -->
				<tr>
				<td style ="text-align:center;"> <input type="checkbox" name="chkbxAcct[]" value={$displayAcctNumArray[acctNum].LI_AcctNumber}></td>
                                <td style ="border-bottom: 1px solid black;padding-left:20px;"> *** </td>
                                <td style ="border-bottom: 1px solid black;padding-left:20px;">{$displayAcctNumArray[acctNum].LI_FullName}</td>

                                {section name=month loop=$monthArray}

                        	        <td style ="text-align:right;border-bottom: 1px solid black;">
		                                {$MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_disp}</td>
                                {/section}
				</tr>
			{else}
				<!-- std line item - show with minimal border-bottom  -->

				<!-- But first check for any line items that are all zero amts for all months & don't display those -->
				{$showLineFlag = 0}
				{section name=month loop=$monthArray}
					{if $MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_disp != "0"}
						{$showLineFlag = 1}
					{/if}
				{/section}
				{if $showLineFlag == 1}
					<tr>
					<td style ="text-align:center;"> <input type="checkbox" name="chkbxAcct[]" value={$displayAcctNumArray[acctNum].LI_AcctNumber}></td>
					<td style ="border-bottom: 1px solid black;">{$displayAcctNumArray[acctNum].LI_AcctNumber}</td>
					<td style ="border-bottom: 1px solid black;padding-left:20px;">
					     {$displayAcctNumArray[acctNum].LI_FullName}</td>

					{section name=month loop=$monthArray}
						<td style ="text-align:right;border-bottom: 1px solid black;">
						    {$MasterMonthData.{$monthArray[month].dateValue}.{$displayAcctNumArray[acctNum].LI_AcctNumber}.amount_mo_disp}</td>
					{/section}

					{section name=averageReport loop=$AverageReportSize}
						{if $AverageReportSize[averageReport] == "TOTAL" || $AverageReportSize[averageReport] == "AVERAGE"}
							{$aveLable = $AverageReportSize[averageReport]}
							<td style ="text-align:right;border-bottom: 1px solid black;color:blue">
                                        	             {$MasterMonthData.{$aveLable}.{$displayAcctNumArray[acctNum].LI_AcctNumber}}</td>
						{else}
	                	                	{$aveLable = sprintf("%dmoAve",$AverageReportSize[averageReport])}
							<td style ="text-align:right;border-bottom: 1px solid black;color:blue">
							    {$MasterMonthData.{$aveLable}.{$displayAcctNumArray[acctNum].LI_AcctNumber}}</td>
						{/if}
					{/section}

					</tr>
				{/if}
			{/if}
		{/section}
		</table>
                <br><br>        
		<span style ="margin-left:20px;font-weight:bold;"> Notes:</span><br>
		<span style ="margin-left:20px;"> 1. Subtotals (Income total and Expense totals shown are exactly as reported by Preston. </span><br>
		<span style ="margin-left:20px;"> 2. Preston moved Mortgage Interest from Operating Expenses to Other Accts beginning Aug 2011.</span><br>
		<span style ="margin-left:20px;"> 3. Due to error (primarily Lender) Property tax impounds paid with Mortgage Payment </span><br>
		<span style ="margin-left:50px;">  were $2,575 instead of $9,266 (May 2011 thru Jan 2012) resulting in a significant shortfall.  </span><br>
		<span style ="margin-left:50px;">  Beginning Feb 2012, lender increased Property tax impounds to $12,160.  (see acct# 1200 & 1220).  </span><br>
		<span style ="margin-left:20px;"> 4. It seems that the best (or easiest) way to understand real <b> CASH FLOW </b> is to look at the </span><br>
		<span style ="margin-left:50px;"> difference between Beginning cash balance and Ending cash balance which I show as Cash Balance Diff.</span><br>
		
		

	<br><br><br>
	<input type="submit" style="margin-left:10px" name="SubmitBtn" value="Continue">
	<!--   <span style="font-size:8pt;"> Authorized Users only</span> -->
      <br>
  </body>
</html>

