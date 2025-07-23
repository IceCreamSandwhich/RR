#ifndef WIRELESS_DRIVE_WEBSITE_H
#define WIRELESS_DRIVE_WEBSITE_H

const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=0.7, maximum-scale=1, user-scalable=no">
<title>Robot Wireless Control</title>
<style type="text/css">
body { 
    text-align: center; 
    font-family: Arial, sans-serif;
    background-color: #f5f5f5;
    margin: 0;
    padding: 20px;
}
button { 
    font-size: 24px;
    padding: 15px 25px;
    margin: 5px;
    border-radius: 8px;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
}
#container {
    margin: 0 auto;
    width: 400px; 
    height: 400px;
    position: relative;
    margin-bottom: 20px;
    display: grid;
    grid-template-rows: auto auto auto;
    grid-template-columns: auto auto auto;
    gap: 10px;
}
.button_forward {
  grid-row: 1;
  grid-column: 2;
  background-color: #4CAF50;
}
.button_backward {
  grid-row: 3;
  grid-column: 2;
  background-color: #f44336;
}
.button_right {
  grid-row: 2;
  grid-column: 3;
  background-color: #2196F3;
}
.button_left {
  grid-row: 2;
  grid-column: 1;
  background-color: #2196F3;
}
.button_stop {
  grid-row: 2;
  grid-column: 2;
  background-color: #ff9800;
}
.button_connect {
  background-color: #607d8b;
  color: white;
  padding: 10px 20px;
}
.button_autonomous {
  position: fixed;
  left: 20px;
  bottom: 20px;
  background-color: #9C27B0;
  z-index: 100;
}
.active {
  filter: brightness(85%);
  transform: scale(0.98);
}
.active-mode {
  box-shadow: 0 0 15px #9C27B0;
  border: 2px solid white;
  font-weight: bold;
}
#status-panel {
  background-color: white;
  padding: 15px;
  border-radius: 8px;
  margin: 20px auto;
  max-width: 400px;
  box-shadow: 0 2px 5px rgba(0,0,0,0.1);
}
</style>
</head>
<body>
<h1>Rescue Roller Controller</h1>
<div id="container">
  <button id="0" class="control-button button_stop">STOP</button>
  <button id="1" class="control-button button_forward">FORWARD</button>
  <button id="2" class="control-button button_backward">BACKWARD</button>
  <button id="8" class="control-button button_right">RIGHT</button>
  <button id="4" class="control-button button_left">LEFT</button>
  <button id="9" class="control-button button_autonomous">AUTONOMOUS</button>
</div>

<div id="status-panel">
  <p>WebSocket Status: <span id="ws_state" style="font-weight:bold;">DISCONNECTED</span></p>
  <button id="wc_conn" class="button_connect" type="button">Connect to Robot</button>
  <p id="command-status">Current Command: None</p>
  <p id="mode-status">Current Mode: Manual</p>
</div>

<script>
// Command Constants
const CMD_STOP     = 0;
const CMD_FORWARD  = 1;
const CMD_BACKWARD = 2;
const CMD_LEFT     = 4;
const CMD_RIGHT    = 8;
const CMD_AUTONOMOUS = 9;

// UI State
let ws = null;
let isConnected = false;
let activeCommand = CMD_STOP;
let isAutonomousMode = false;
let isTouchDevice = ('ontouchstart' in window);

// Initialize the controller

function init() {
  // Set up event listeners
  if (isTouchDevice) {
    setupTouchControls();
  } else {
    setupMouseControls();
  }
  
  document.getElementById("wc_conn").addEventListener("click", connectWebSocket);
  document.addEventListener('keydown', handleKeyDown);
  document.addEventListener('keyup', handleKeyUp);
  
  // Initialize autonomous control
  autonomous_control();
}

function autonomous_control() {
  // Autonomous button handling
  const autoButton = document.getElementById("9");
  const container = document.getElementById("container");

  const toggleAutonomousMode = (e) => {
    e.preventDefault();
    isAutonomousMode = !isAutonomousMode;
    autoButton.classList.toggle("active-mode", isAutonomousMode);
    document.getElementById("mode-status").textContent = 
      `Current Mode: ${isAutonomousMode ? "Autonomous" : "Manual"}`;
    sendCommand(CMD_AUTONOMOUS);
    
    if (!isAutonomousMode) {
      sendCommand(CMD_STOP);
    }
  };

  // Set up event listeners
  autoButton.addEventListener("click", toggleAutonomousMode);
  autoButton.addEventListener("touchstart", toggleAutonomousMode);

  // Update container touch handler to exclude autonomous button
  if (isTouchDevice) {
    container.addEventListener('touchstart', (e) => {
      e.preventDefault();
      const touch = e.touches[0];
      const element = document.elementFromPoint(touch.clientX, touch.clientY);
      if (element && element.classList.contains('control-button') && element.id !== "9") {
        element.classList.add('active');
        sendCommand(element.id);
      }
    });
  }
}

function setupTouchControls() {
  const container = document.getElementById("container");
  container.addEventListener('touchstart', (e) => {
    e.preventDefault();
    const touch = e.touches[0];
    const element = document.elementFromPoint(touch.clientX, touch.clientY);
    if (element && element.classList.contains('control-button') && element.id != "9") {
      element.classList.add('active');
      sendCommand(element.id);
    }
  });
  
  container.addEventListener('touchend', (e) => {
    e.preventDefault();
    document.querySelectorAll('.control-button').forEach(btn => {
      btn.classList.remove('active');
    });
    if (!isAutonomousMode) {
      sendCommand(CMD_STOP);
    }
  });
}

function setupMouseControls() {
  document.querySelectorAll('.control-button').forEach(button => {
    if (button.id == "9") return; // Skip autonomous button
    
    button.addEventListener('mousedown', () => {
      if (!isAutonomousMode) {
        button.classList.add('active');
        sendCommand(button.id);
      }
    });
    
    button.addEventListener('mouseup', () => {
      button.classList.remove('active');
      if (!isAutonomousMode) {
        sendCommand(CMD_STOP);
      }
    });
    
    button.addEventListener('mouseleave', () => {
      button.classList.remove('active');
      if (!isAutonomousMode) {
        sendCommand(CMD_STOP);
      }
    });
  });
}

function handleKeyDown(e) {
  if (!isConnected || isAutonomousMode) return;
  
  switch(e.key) {
    case 'ArrowUp': case 'w': case 'W':
      document.querySelector('.button_forward').classList.add('active');
      sendCommand(CMD_FORWARD);
      break;
    case 'ArrowDown': case 's': case 'S':
      document.querySelector('.button_backward').classList.add('active');
      sendCommand(CMD_BACKWARD);
      break;
    case 'ArrowLeft': case 'a': case 'A':
      document.querySelector('.button_left').classList.add('active');
      sendCommand(CMD_LEFT);
      break;
    case 'ArrowRight': case 'd': case 'D':
      document.querySelector('.button_right').classList.add('active');
      sendCommand(CMD_RIGHT);
      break;
    case ' ': // Spacebar
      sendCommand(CMD_STOP);
      break;
    case 'm': case 'M': // Toggle autonomous mode
      document.getElementById("9").click(); // Autonomous button click
      break;
  }
}

function handleKeyUp(e) {
  if (!isConnected || isAutonomousMode) return;
  
  switch(e.key) {
    case 'ArrowUp': case 'w': case 'W':
      document.querySelector('.button_forward').classList.remove('active');
      break;
    case 'ArrowDown': case 's': case 'S':
      document.querySelector('.button_backward').classList.remove('active');
      break;
    case 'ArrowLeft': case 'a': case 'A':
      document.querySelector('.button_left').classList.remove('active');
      break;
    case 'ArrowRight': case 'd': case 'D':
      document.querySelector('.button_right').classList.remove('active');
      break;
  }
  
  if (!e.ctrlKey && !e.altKey && !e.shiftKey && !e.metaKey) {
    sendCommand(CMD_STOP);
  }
}

function connectWebSocket() {
  if (ws) {
    ws.close();
    return;
  }
  
  document.getElementById("ws_state").textContent = "CONNECTING...";
  document.getElementById("wc_conn").textContent = "Connecting...";
  
  const host = window.location.host || "192.168.4.1";
  ws = new WebSocket("ws://" + host + "/ws");
  
  ws.onopen = function() {
    isConnected = true;
    document.getElementById("ws_state").textContent = "CONNECTED";
    document.getElementById("ws_state").style.color = "green";
    document.getElementById("wc_conn").textContent = "Disconnect";
    updateCommandStatus("Ready");
  };
  
  ws.onclose = function() {
    isConnected = false;
    isAutonomousMode = false;
    document.getElementById("ws_state").textContent = "DISCONNECTED";
    document.getElementById("ws_state").style.color = "red";
    document.getElementById("wc_conn").textContent = "Connect to Robot";
    document.getElementById("9").classList.remove("active-mode");
    document.getElementById("mode-status").textContent = "Current Mode: Manual";
    updateCommandStatus("Disconnected");
    ws = null;
  };
  
  ws.onerror = function(error) {
    console.error("WebSocket Error:", error);
    document.getElementById("ws_state").textContent = "ERROR";
    document.getElementById("ws_state").style.color = "red";
  };
  
  ws.onmessage = function(e) {
    console.log("Message from robot:", e.data);
  };
}

function sendCommand(cmd) {
  if (!isConnected) return;

  // Block manual commands in autonomous mode (except stop and mode toggle)
  if (isAutonomousMode && cmd != CMD_STOP && cmd != CMD_AUTONOMOUS) {
    return;
  }
  
  activeCommand = parseInt(cmd);
  ws.send(cmd.toString());
  updateCommandStatus(getCommandName(cmd));
}

function updateCommandStatus(cmdName) {
  const statusElement = document.getElementById("command-status");
  statusElement.textContent = "Current Command: " + cmdName;
  statusElement.style.fontWeight = cmdName === "None" ? "normal" : "bold";
}

function getCommandName(cmd) {
  switch(parseInt(cmd)) {
    case CMD_STOP: return "None";
    case CMD_FORWARD: return "Forward";
    case CMD_BACKWARD: return "Backward";
    case CMD_LEFT: return "Left Turn";
    case CMD_RIGHT: return "Right Turn";
    case CMD_AUTONOMOUS: return isAutonomousMode ? "Autonomous ON" : "Autonomous OFF";
    default: return "Unknown";
  }
}

// Initialize when page loads
window.onload = init;
</script>
</body>
</html>
)=====";

#endif // WIRELESS_DRIVE_WEBSITE_H