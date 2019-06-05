<?php include_once ('include/header.php'); ?>


<div class="jumbotron text-center">
  <h1>Projet Peri</h1>
  <p></p>
</div>

<?php
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

$sql = "SELECT value, capteur_id,time FROM capteur_value WHERE capteur_id=1 LIMIT 60;";
$result = $conn->query($sql);

$chart_array_capteur_1 = array();
if ($result->num_rows > 0) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        array_push($chart_array_capteur_1, array("x"=> $row["time"], "y"=> $row["value"]));
        echo "capteur_id: " . $row["capteur_id"]. " - value: " . $row["value"]."<br>";
    }
} else {
    echo "0 results";
}
$conn->close();


?> 

<script>
window.onload = function () {

var dataPoints = [];
var chart = new CanvasJS.Chart("chartContainer", {
	title :{
		text: "Dynamic Data"
	},
	axisX:{
        title: "timeline",
    },
    axisY: {
        title: "Downloads"
    },     
	data: [{
		type: "line",
		dataPoints: dataPoints
	}]
});

var xVal = 0;
var yVal = 100; 
var updateInterval = 1000;
var dataLength = 60; // number of dataPoints visible at any point

var updateChart = function (count) {

	count = count || 1;
	
	dataPoints.push({ x: xValue, y: yValue });
	xValue++;
	chart.render();

	chart.render();
};

updateChart(dataLength);
setInterval(function(){updateChart()}, updateInterval);

}
</script>

<div id="chartContainer" style="height: 370px; width: 100%;">
</div>

<?php include_once ('include/footer.php'); ?>