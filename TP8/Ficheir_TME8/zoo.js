// Create the canvas
var canvas = document.getElementById('canvas');
var ctx = canvas.getContext("2d");
var sensor_obj=[];

// Background image
var bgReady = false;
var bgImage = new Image();
bgImage.onload = function () {
	bgReady = true;
};
bgImage.src="zoo-miami.jpg";

// Draw everything
var render = function () {
	if (bgReady) {
		ctx.drawImage(bgImage, 0, 0);
	}

        ctx.fillStyle = "rgb(250, 0, 0)";
        ctx.font = "44px Helvetica";
        ctx.textAlign = "left";
        ctx.textBaseline = "top";
	for (i=0;i<sensor_obj.length;i++)
	{
        	ctx.fillText(String(sensor_obj[i][1]), 32, 32*2+i*44);
	}

};

var getdata = function() {
	var xhttp=new XMLHttpRequest();

        xhttp.onreadystatechange = function()
        {
                if (xhttp.readyState == 4 && xhttp.status == 200)
                {
                        sensor_text = xhttp.responseText;
                        sensor_obj=JSON.parse(sensor_text);
                        console.log(sensor_text);
                        console.log(sensor_obj);
			for (i=0;i<sensor_obj.length;i++)
			{
				console.log(sensor_obj[i][0]);
				console.log(sensor_obj[i][1]);
			}
                }
        };
        xhttp.open("GET", "getData.php", true);
        xhttp.send();
};

var myprint = function() {
	console.log("myprint");
};

// The main loop
var main = function () {
	var now = Date.now();
	var delta = now - then;

	render();

	then = now;

	// Request to do this again ASAP
	requestAnimationFrame(main);
};

// Cross-browser support for requestAnimationFrame
var w = window;
requestAnimationFrame = w.requestAnimationFrame || w.webkitRequestAnimationFrame || w.msRequestAnimationFrame || w.mozRequestAnimationFrame;

var then = Date.now();
setInterval(getdata,2000);
main();
