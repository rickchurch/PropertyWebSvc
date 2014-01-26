<html>
  <head>
      <title>{$title}</title>
  </head>
  <body>

	<p style="color:red;">{$errmsg}</p><br>

	<form action = "adminMenu.php" method = "post">
	<!--	<form action = "index.php" method = "post">    -->

	<!-- <img style="margin-left:150px;" src="/images/LeasingOffice.jpg" alt="Legacy Leasing Office" width="638px" height="400px" />  -->
	<img style="margin-left:150px;" src="/images/LeasingOffice.jpg" alt="Legacy Leasing Office" width="678px" height="430px" />


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


	<br><br><br>
	<!--   <input type="submit" style="margin-left:10px" name="SubmitBtn" value="admin"> -->
	<!--   <span style="font-size:8pt;"> Authorized Users only</span> -->
      <br>
  </body>
</html>

