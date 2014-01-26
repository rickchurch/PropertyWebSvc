<html>
  <head>
      <title>{$title}</title>
  </head>
  <body>

	<p style="color:red;">{$errmsg}</p>	

	<form action = "adminMenu.php" method = "post">
	<!--    <form action = "buildMonthlyReport.php" method = "post">   used thru 1/1/2012    -->
	<!--	<form action = "index.php" method = "post">    -->

      <p>
      </p><br>

	<!--    
	<select name="selDate">
	{section name=date loop=$dates}
		<option value={$dates[date].DateValue}>{$dates[date].DateDisplay}</option>
	{/section}
	</select>
	    -->


      <br>
	<br><br>
	<!--    <input type="submit" style="margin-left:10px" name="SubmitBtn" value="Show Monthly Financials">   -->
	<br><br><br>
	<input type="submit" style="margin-left:10px" name="SubmitBtn" value="continue">
	<!--    <span style="font-size:8pt;"> Authorized Users only</span>   -->
      <br>
  </body>
</html>

