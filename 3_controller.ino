#include <WiFi.h>
#include <WebServer.h>

// ===== WIFI =====
const char* ssid = "Airtel_maya_2297";
const char* password = "Air@89168";

WebServer server(80);

// ===== MOTOR PINS =====
#define L1  12
#define L2  13
#define R1 14
#define R2 26

// ===== HTML PAGE (UPDATED) =====
String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Robot Control</title>
<style>
body { text-align:center; font-family:Arial; }
button { padding:15px; margin:10px; font-size:16px; }
input { padding:10px; margin:5px; }
</style>
</head>

<body>

<h2>ESP32 Robot Control (Smart)</h2>

<!-- Buttons -->
<button onclick="sendCmd('F')">Forward</button><br>
<button onclick="sendCmd('L')">Left</button>
<button onclick="sendCmd('S')">Stop</button>
<button onclick="sendCmd('R')">Right</button><br>
<button onclick="sendCmd('B')">Backward</button>

<br><br>

<!-- Manual Command -->
<input type="text" id="cmdBox" placeholder="Enter F/B/L/R/S">

<!-- NEW: Duration input -->
<input type="number" id="durBox" placeholder="Duration(ms)" value="200">

<br><br>

<button onclick="sendText()">Send</button>

<script>
function sendCmd(c){
  let dur = document.getElementById("durBox").value;
  fetch('/cmd?val=' + c + '&dur=' + dur);
}

function sendText(){
  let val = document.getElementById("cmdBox").value;
  let dur = document.getElementById("durBox").value;
  fetch('/cmd?val=' + val + '&dur=' + dur);
}
</script>

</body>
</html>
)rawliteral";


// ===== GLOBAL CONTROL =====
String currentCmd = "S";
unsigned long lastMoveTime = 0;
int moveDuration = 200;

// ===== STOP FUNCTION =====
void stopAll() {
  analogWrite(L1, 0);
  analogWrite(L2, 0);
  analogWrite(R1, 0);
  analogWrite(R2, 0);
}

// ===== HANDLE COMMAND =====
void handleCmd() {
  currentCmd = server.arg("val");

  // NEW: duration receive
  if (server.hasArg("dur")) {
    moveDuration = server.arg("dur").toInt();
  }

  Serial.print("CMD: ");
  Serial.print(currentCmd);
  Serial.print(" | DUR: ");
  Serial.println(moveDuration);

  lastMoveTime = millis();

  server.send(200, "text/plain", "OK");
}

// ===== NON-BLOCKING MOTOR CONTROL =====
void executeMotion() {

  if (millis() - lastMoveTime > moveDuration) {
    stopAll();
    return;
  }

  if (currentCmd == "F") {
    analogWrite(L1, 150);
    analogWrite(R1, 150);
    analogWrite(L2, 0);
    analogWrite(R2, 0);
  }
  else if (currentCmd == "B") {
    analogWrite(L1, 0);
    analogWrite(L2, 150);
    analogWrite(R1, 0);
    analogWrite(R2, 150);
  }
  else if (currentCmd == "L") {
    analogWrite(L1, 0);
    analogWrite(L2, 120);
    analogWrite(R1, 120);
    analogWrite(R2, 0);
  }
  else if (currentCmd == "R") {
    analogWrite(L1, 120);
    analogWrite(L2, 0);
    analogWrite(R1, 0);
    analogWrite(R2, 120);
  }
  else {
    stopAll();
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  stopAll();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println(WiFi.localIP());

  // HTML ROUTE BACK ADDED
  server.on("/", []() {
    server.send(200, "text/html", html);
  });

  server.on("/cmd", handleCmd);

  server.begin();
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  executeMotion();
}