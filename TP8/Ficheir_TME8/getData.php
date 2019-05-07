<?php
  require_once 'login.php';
  $conn = new mysqli($hn, $un, $pw, $db);
  if ($conn->connect_error) die($conn->connect_error);

  $query  = "SELECT * FROM mesure order by date desc limit 1";
  $result = $conn->query($query);
  if (!$result) 
	die ("Database access failed: " . $conn->error);

  
  $finalarray = array();
  $rows=$result->num_rows;
  for ($j = 0 ; $j < $rows ; ++$j)
  {
    $result->data_seek($j);
    $row = $result->fetch_array(MYSQLI_NUM);

    //$finalarray[]=array('id' => $row[0], 'nom' => utf8_encode($row[1]));
    $finalarray[]=array($row[0], utf8_encode($row[1]));
  }

  $json=json_encode($finalarray);

  echo $json;

?>
