<html>
  <head>
      <title>{$title}</title>
  </head>
  <body>
      <p>Brought to you by Rick Church
      </p><br>

      <p><select name="action">
             <option value="WeeklyRpts">Weekly Reports</option>
             <option value="MnthlyRpts">Monthly Reports</option>
             <option value="pics">Pictures</option>
             <option value="suresBeads">Shirley's Beads</option>
          </select>

	{section name=lineitem loop=$info}
	   <p>UserId: {$info[lineitem].id}<br>
	   First Name: {$info[lineitem].fname}<br>
	   Last Name: {$info[lineitem].lname}</p>
	{/section}
      <br>
      <br>
  </body>
</html>

