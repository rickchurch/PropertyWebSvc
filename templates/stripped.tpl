<html>
  <head>
      <title>{$title}</title>
  </head>
  <body>


	<form action = "adminMenu.php" method = "post">
	<!--	<form action = "index.php" method = "post">    -->

      <p>
      </p><br>


		<fieldset style="width:600px;margin-left:150px;border-style: solid;border:#DAA520 1px solid;border-width:medium;">
                        <legend id="legend"> Please Login </legend>
 			<br><span style="margin-left:100px;"><b>Name:</b><input type="text" name="username" style="margin-left:25px;"></span><br>
			<span style="margin-left:100px;"><b> Password: </b><input type="password" name="pwd"></span>
			<input type="submit" style="margin-left:80px" name="SubmitBtn" value="continue">
                        <br /><br />
                        </p>
                </fieldset>




			Beginning Month: <select name="selStartMonth">
                        {section name=date loop=$dates}
                        <option value={$dates[date].DateValue}>{$dates[date].DateDisplay}</option>
                        {/section}
                        </select>


	<br><br><br>
	<!--   <input type="submit" style="margin-left:10px" name="SubmitBtn" value="admin"> -->
	<!--   <span style="font-size:8pt;"> Authorized Users only</span> -->
      <br>
  </body>
</html>

