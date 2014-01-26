<html>
  <head>
      <title>{$title}</title>
  </head>
  <body>


	<form action = "main.php" method = "post">

      <p>

	<table border="1">
		<tr>
		    <th> Acct Number </th>
		    <th> Acct Name </th>
		    <th> Amount </th>
		</tr>


		


		<!-- This works or fills web page correctly, but also gives an error on web page saying cant use object as array   -->
		
		{foreach from=$tester  item=acctNum_obj}
			{if $acctNum_obj->dataType == 'acctNum'}
				<tr>
					<td>{$acctNum_obj->number}</td><td>{$acctNum_obj->FullName}</td><td>{$acctNum_obj->Amount_mo_disp}</td>
				</tr>
			{/if}
		{/foreach}
		

		<!--
		{foreach from=$tester  item=acctNum_obj}
			{foreach from=$acctNum_obj  item=acctNum_property} 
				{if $acctNum_property == 'acctNum'}
				
				{/if}
			{/foreach}
		{/foreach}
		-->



		<!--
	        {section name=atter loop=$tester}
			{if $tester[atter]->rc_dataType == 'acctNum'}
				<tr><td>{$tester[atter]->number}</td>
				    <td>{$tester[atter]->FullName}</td>
				    <td>{$tester[atter]->Amount_mo_disp}</td></tr>
			{/if}
		{sectionelse}
        	        <tr><td>No Data Found</td></tr>
	        {/section}
		-->


		<tr></tr><tr></tr>
	</table>

      </p><br>


      <br>
	<br><br>
	<input type="submit" style="margin-left:10px" name="SubmitBtn" value="go">

      <br>
  </body>
</html>

