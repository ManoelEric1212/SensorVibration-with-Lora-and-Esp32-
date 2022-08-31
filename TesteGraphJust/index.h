const char MAIN_page[] PROGMEM = R"webpage(
<!DOCTYPE HTML><html>
<head>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css">
<link rel="stylesheet" type="text/css" href="style.css" media="screen" />
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<script src="code/highcharts.js"></script>
<script src="code/modules/exporting.js"></script>
<script src="code/modules/export-data.js"></script>
<script src="code/modules/accessibility.js"></script>

<style>
@import 'https://fonts.googleapis.com/css?family=Ubuntu:300, 400, 500, 700';

*, *:after, *:before {
  margin: 0;
  padding: 0;
}

.svg-container {
  position: absolute;
  top: 0;
  right: 0;
  left: 0;
  z-index: -1;
}

svg {
  path {
    transition: .1s;
  }

  &:hover path {
    d: path("M 800 300 Q 400 250 0 300 L 0 0 L 800 0 L 800 300 Z");
  }
}

body {
  background: #fff;
  color: #333;
  font-family: 'Ubuntu', sans-serif;
}

h3 {
  font-weight: 400;
}

header {
  color: #fff;
  text-align: center;
}

main {
  background: linear-gradient(to bottom, #ffffff 0%, #dddee1 100%);
  border-bottom: 1px solid rgba(0, 0, 0, .2);
  padding: 30vh 0 35vh;
  position: relative;
  text-align: center;
  overflow: hidden;


}

footer {
  background: #dddee1;
  padding: 1vh 0;
  text-align: center;
  position: relative;
}

small {
  opacity: .5;
  font-weight: 300;

  a {
    color: inherit;
  }
}

.normal{
     max-width: 250px;
     min-height: 50px;
     background: #e0e0e0 ;
   padding-left: 10px;
    padding-right: 10px;
     box-sizing: border-box;
     color: #FFF;
     margin: 20px;
     box-shadow: 0px 2px 18px -4px rgba(0, 0, 0, 0.75);
   border-radius: 70px;
  }

.disturbio{
     max-width: 250px;
     min-height: 50px;
     background: #e0e0e0;
   padding-left: 10px;
  padding-right: 10px;
     box-sizing: border-box;
     color: #FFF;
     margin: 20px;
     box-shadow: 0px 2px 18px -4px rgba(0, 0, 0, 0.75);
   border-radius: 70px;
  }
.indica_dist{
  position: absolute;
  top:100px;
  right:30px;
  }
.card{
  border-radius: 70px;
  position: absolute;
  top:0px;
  left:500px;
  background:#dddee1;
  box-sizing: border-box;
  color: #FFF;
  box-shadow: 10px 10px 10px -4px rgba(0, 0, 0, 0.75);
  padding-left: 10px;
  padding-right: 10px;
  
}

.container{
  min-width: 300px;
  max-width: 1000px;
  height: 400px;
  position: absolute;
  top:100px;
  left:10px;
  }
</style>
</head>
<body>
  <div class="svg-container">

    <svg viewbox="0 0 800 400" class="svg">
      <path id="curve" fill="#50c6d8" d="M 800 300 Q 400 350 0 300 L 0 0 L 800 0 L 800 300 Z">
      </path>
    </svg>
  </div>

  <header>
    <h1>Sensor de vibração 4.0</h1>

  </header>

  <main>
	<div class="card"><font color="#212121"> <h5>Equipamento conectado:<span id="id_maq"> ------------ </span></h5> </font> <br> </div>
	<div id="Acx-chart" class="container"></div>
	<div class = "indica_dist">
		<div id="normal_bloco" class="normal"> <h4><font color="#212121">Normal</font></h4> </div>
		<div id="dist_bloco" class="disturbio"> <h4><font color="#212121">Distúrbio!</font></h4> </div>
	</div>
    
  
  </main>

  <footer>
    <p>Proposta de sensor de vibração para o monitoramento em tempo real de motores</p>
    <small>🕷 Desenvolvido por: NovTec (Novas Tecnologias).</small>
  </footer>
</body>

<script>
    (function() {
  // Variables
  var $curve = document.getElementById("curve");
  var last_known_scroll_position = 0;
  var defaultCurveValue = 350;
  var curveRate = 3;
  var ticking = false;
  var curveValue;

  // Handle the functionality
  function scrollEvent(scrollPos) {
    if (scrollPos >= 0 && scrollPos < defaultCurveValue) {
      curveValue = defaultCurveValue - parseFloat(scrollPos / curveRate);
      $curve.setAttribute(
        "d",
        "M 800 300 Q 400 " + curveValue + " 0 300 L 0 0 L 800 0 L 800 300 Z"
      );
    }
  }

  // Scroll Listener
  // https://developer.mozilla.org/en-US/docs/Web/Events/scroll
  window.addEventListener("scroll", function(e) {
    last_known_scroll_position = window.scrollY;

    if (!ticking) {
      window.requestAnimationFrame(function() {
        scrollEvent(last_known_scroll_position);
        ticking = false;
      });
    }

    ticking = true;
  });
})();


var chartT = new Highcharts.Chart({
    chart:{ renderTo : 'Acx-chart' },
    title: { text: 'Monitoramento em tempo real sensor' },
    series: [{
    name: 'Acx',
    color: '#0277bd', 
    data: []
  }, {
    name:'Acy',
    color: '#64dd17 ',
    data: []
  }, {
    name:'Acz',
    color: '#b71c1c',
    data: []  
  }],

  xAxis: { type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: {
    title: { text: 'Acelerômetro dados' }
  },
  credits: { enabled: false }
});

          setInterval(function () {
          var xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            var sensors = JSON.parse(this.responseText);
            var x = (new Date()).getTime();
            var y = sensors.Acx;
            var z = sensors.Acy;
            var w = sensors.Acz;
          if(chartT.series[0].data.length > 600) {
             chartT.series[0].addPoint([x, y], true, true, true);
             chartT.series[1].addPoint([x, z], true, true, true);
             chartT.series[2].addPoint([x, w], true, true, true); 
          } else {
            chartT.series[0].addPoint([x, y], true, false, true);
            chartT.series[1].addPoint([x, z], true, false, true);
             chartT.series[2].addPoint([x, w], true, false, true);
          }
          }
          };
          xhttp.open("GET", "/acx", true);
          xhttp.send();
          }, 10 ) ;

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/botoes", true);
  xhttp.send();
  xhttp.onload = function() {
    if (this.status == 200) {
      var jsonResponse = JSON.parse(this.responseText);
      if (jsonResponse.Indica_normal == '1'){
        document.getElementById("normal_bloco").style.backgroundColor = '#02b875';
        document.getElementById("dist_bloco").style.backgroundColor = '#e0e0e0';
        }
      if (jsonResponse.Indica_dist1 == '1'){
        document.getElementById("dist_bloco").style.backgroundColor = '#d84315';
        document.getElementById("normal_bloco").style.backgroundColor = '#e0e0e0';
        }
    }
    else {
      console.log(this.status);
    }
  };
}, 10); 


setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/id", true);
  xhttp.send();
  xhttp.onload = function() {
    if (this.status == 200) {
      var jsonId = JSON.parse(this.responseText);
      document.getElementById("id_maq").innerHTML = jsonId.Id_equipamento;
    }
    else {
      console.log(this.status);
    }
  };
}, 10);


</script>
</html>)webpage";
