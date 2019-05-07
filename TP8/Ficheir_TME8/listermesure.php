<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
  <title>Page listant les mesures</title>
  <link rel="stylesheet" href="monstyle.css">
  <meta charset="UTF-8"> 
</head>

<body>

<!-- Menu de navigation du site -->
<ul class="navbar">
  <li><a href="accueil.html">Accueil</a>
  <li><a href="listermesure.php">Lister toutes les mesures</a>
</ul>

<!-- Contenu principal -->
<h1>Lister les mesures</h1>

<?php
  require_once 'login.php';
  $conn = new mysqli($hn, $un, $pw, $db);
  if ($conn->connect_error) die($conn->connect_error);

  $query  = "SELECT * FROM mesure";
  $result = $conn->query($query);
  if (!$result) die ("Database access failed: " . $conn->error);

  $rows = $result->num_rows;
?>
  
	<table>
	<tr>
		<th>idmesure</th>
		<th>valeur</th>
		<th>date</th>
	</tr>
  
<?php
  for ($j = 0 ; $j < $rows ; ++$j)
  {
    $result->data_seek($j);
    $row = $result->fetch_array(MYSQLI_NUM);

    echo "<tr>"; 
	echo "<td>"; echo $row[0]; echo "</td>"; 
	echo "<td>"; echo utf8_encode($row[1]); echo "</td>"; 
	echo "<td>"; echo utf8_encode($row[2]); echo "</td>"; 
    echo "</tr>";
  }
?>

	</table>
  
<?php
  $result->close();
  $conn->close();
?>

</body>
</html>
