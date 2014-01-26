
<html>
  <head>
      <title>Legacy Apts Pictures</title>
  </head>
  <body>


	<form action = "adminMenu.php" method = "post">
	<!--	<form action = "index.php" method = "post">    -->

        <br><br>

	<h1 style="text-align:center;">Legacy Apartment Pictures </h1>

	<br><br>

	{section name=pic loop=$PicArray}

		<!--   <img border="0" src={$PicArray[pic]} width="678" height="430" /> -->
		<!--   <img border="0" src={$PicArray[pic]} width="430" height="573" /> -->
		<div style="text-align:center;">
		<img style="border-style:solid;border-width:2px;" src={$PicArray[pic].file} 
		                 width={$PicArray[pic].width} height={$PicArray[pic].height} />
		
		<br>
		<span>{$PicArray[pic].caption} </span>
		</div>
		<br><br><br>

	{/section}


	<br><br><br>

	<input type="submit" style="margin-left:50px" name="SubmitBtn" value="Continue">

	<!--   <span style="font-size:8pt;"> Authorized Users only</span> -->
      <br>
  </body>
</html>

