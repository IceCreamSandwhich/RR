const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=0.7, maximum-scale=1, user-scalable=no">
<style type="text/css">
body { text-align: center; font-size: 24px;}
button { text-align: center; font-size: 24px;}
#container {
    margin-right: auto;
    margin-left: auto;
    width: 400px; 
    height: 400px;
    position: relative;
    margin-bottom: 10px;
    display: grid;
    grid-template-rows: auto auto auto;
    grid-template-columns: auto auto auto;
}
.button_up {
  grid-row: 1;
  grid-column: 2;
}

.button_down {
  grid-row: 3;
  grid-column: 2;
}

.button_right {
  grid-row: 2;
  grid-column: 3;
}

.button_left {
  grid-row: 2;
  grid-column: 1;
    
}

.button_stop {
  grid-row: 2;
  grid-column: 2;
}
</style>
<script>
var CMD_STOP     = 0;
var CMD_FORWARD  = 1;
var CMD_BACKWARD = 2;
var CMD_LEFT     = 4;
var CMD_RIGHT    = 8;
var img_name_lookup = {
  [CMD_STOP]:     "stop",
  [CMD_FORWARD]:  "up",
  [CMD_BACKWARD]: "down",
  [CMD_LEFT]:     "left",
  [CMD_RIGHT]:    "right"
}
var ws = null;

function init() 
{
  var container = document.querySelector("#container");
  const buttons = document.querySelectorAll(".control-button");
  buttons.forEach(button => {
    button.addEventListener('click', (event) =>
    {
      console.log(event.target);
      send_command(event.target.id);
    });
  });
}
function ws_onmessage(e_msg)
{
    e_msg = e_msg || window.event; // MessageEvent
    //alert("msg : " + e_msg.data);
}
function ws_onopen()
{
  document.getElementById("ws_state").innerHTML = "OPEN";
  document.getElementById("wc_conn").innerHTML = "Disconnect";
}
function ws_onclose()
{
  document.getElementById("ws_state").innerHTML = "CLOSED";
  document.getElementById("wc_conn").innerHTML = "Connect";
  console.log("socket was closed");
  ws.onopen = null;
  ws.onclose = null;
  ws.onmessage = null;
  ws = null;
}
function wc_onclick()
{
  if(ws == null)
  {
    //(80 is default)
    ws = new WebSocket("ws://" + window.location.host + "/ws");
    document.getElementById("ws_state").innerHTML = "CONNECTING";
    
    ws.onopen = ws_onopen;
    ws.onclose = ws_onclose;
    ws.onmessage = ws_onmessage; 
  }
  else
    ws.close();
}

function send_command(cmd) 
{   
  if(ws != null)
    if(ws.readyState == 1)
      ws.send(cmd + "\r\n");   
}

window.onload = init;
</script>
</head>
<body>
<div id="container">
  <button id="0" class="control-button button_stop"> STOP</button>
  <button id="1" class="control-button  button_up">UP</button>
  <button id="2" class="control-button button_down">DOWN</button>
  <button id="8" class="control-button button_right">RIGHT</button>
  <button id="4" class="control-button button_left">LEFT</button>
</div>
<p>
WebSocket : <span id="ws_state" style="color:blue">closed</span><br>
</p>
<button id="wc_conn" type="button" onclick="wc_onclick();">Connect</button>
<br>
<br>
</body>
</html>
)=====";

#endif // __WIRELESSDRIVE_WEBSITE_H__