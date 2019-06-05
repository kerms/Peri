<?php include_once ('include/header.php'); ?>
<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>

<div class="jumbotron text-center">
  <h1>Projet Peri</h1>
  <p></p>
</div>

<?php
header("Refresh:3");
$servername = "localhost";
$username = "root";
$password = "peri2019";
$dbname = "pperi";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT value, capteur_id,time FROM capteur_value WHERE capteur_id=1  ORDER BY time DESC LIMIT 60;";
$result = $conn->query($sql);

/*
$chart_array_capteur_1 = array();
if ($result->num_rows > 0) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        array_push($chart_array_capteur_1, array(date_format(date_create($row["time"]), 'Y-m-d H:i:s'), $row["value"]));

        echo "capteur_id: " . $row["capteur_id"]. " - value: " . $row["value"]. " - time : " . ($row["time"]) . "<br>";
    }
} else {
    echo "0 results";
}*/


$dataPoints = array();

?> 

<?php
    while($row = mysqli_fetch_assoc($result)){
        //echo "['".$row["time"]."', ".$row["value"]."],";
    	array_push($dataPoints, array("label" => "'".$row["time"]."'", "y"=>$row["value"]));
    }
?>

<?php $conn->close(); ?>


<script>
window.onload = function() {
 
var dataPoints = <?php echo json_encode($dataPoints, JSON_NUMERIC_CHECK); ?>;
 
var chart = new CanvasJS.Chart("chartContainer", {
	theme: "light2",
	title: {
		text: "distance"
	},
	axisX:{
		title: "date time"
	},
	axisY:{
		includeZero: false,
		suffix: " mm"
	},
	data: [{
		type: "line",
		dataPoints: dataPoints
	}]
});
chart.render();
 

 
var updateInterval = 1000;
var xValue = dataPoints.length;
var yValue = dataPoints[dataPoints.length - 1].y;

function updateChart() {

	<?php
		$conn = new mysqli($servername, $username, $password, $dbname);
		// Check connection
		if ($conn->connect_error) {
		    die("Connection failed: " . $conn->connect_error);
		}

		$sql = "SELECT value, capteur_id,time FROM capteur_value WHERE capteur_id=1  ORDER BY time DESC LIMIT 60;";
		$result = $conn->query($sql);
		$dataPoints = array();
	    while($row = mysqli_fetch_assoc($result)){
	    	array_push($dataPoints, array("label" => "'".$row["time"]."'", "y"=>$row["value"]));
	    }
		$conn->close();
	?>

	dataPoints =  <?php echo json_encode($dataPoints, JSON_NUMERIC_CHECK); ?>;
	chart.render();
};

setInterval(function () { updateChart() }, updateInterval);

}
</script>



<div id="chartContainer" style="height: 370px; width: 100%;"></div>

<?php include_once ('include/footer.php'); ?>