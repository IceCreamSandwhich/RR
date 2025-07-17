/*
#include <WiFi.h>
#include <WebServer.h>
#include <Encoder.h>

#include "webserver_service.h"
#include ""


// --- Motor Pins ---
const int l1_pin = A0;
const int l2_pin = A1;
const int r1_pin = A3;
const int r2_pin = A2;

// --- Encoder Pins ---
const int el_1 = D9;
const int el_2 = D10;
const int er_1 = D7;
const int er_2 = D8;

// --- Encoder Objects ---
Encoder l_enc(el_1, el_2);
Encoder r_enc(er_1, er_2);

// --- Wi-Fi Credentials ---
const char* ssid = "ResR";
const char* password = "rescueroller";

// --- Network Config ---
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// --- Web Server ---
WebServer server(80);

// --- Movement State Enum ---
enum MovementState { STOP, FORWARD, LEFT, RIGHT, BACKWARD };
volatile MovementState currentMovement = STOP;

// --- Web Page HTML ---
String page = HTML_CONTENT;
String page2 = "";

// --- Setup ---
void setup() {
  Serial.begin(115200);

  // Motor Initialization
  init_motor(l1_pin, l2_pin, l_enc);
  init_motor(r1_pin, r2_pin, r_enc);

  // Setup web server endpoints
  setupEndpoints();
  server.begin();
}

// --- Main Loop ---
void loop() {
  server.handleClient();
  handleCurrentMovement();
}

// --- Motor and Encoder Setup ---
void init_motor(int a, int b, Encoder& e) {
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  e.write(0);
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
}

// --- Web Server Endpoints ---
void setupEndpoints() {
  server.on("/", []() {
    server.send(200, "text/html", page + page2);
  });

  server.on("/Forward", []() {
    currentMovement = FORWARD;
    page2 = "<center><p>Status: Forward</p></center>";
    server.send(200, "text/html", page + page2);
  });

  server.on("/Left", []() {
    currentMovement = LEFT;
    page2 = "<center><p>Status: Left</p></center>";
    server.send(200, "text/html", page + page2);
  });

  server.on("/Right", []() {
    currentMovement = RIGHT;
    page2 = "<center><p>Status: Right</p></center>";
    server.send(200, "text/html", page + page2);
  });

  server.on("/Backward", []() {
    currentMovement = BACKWARD;
    page2 = "<center><p>Status: Backward</p></center>";
    server.send(200, "text/html", page + page2);
  });

  server.on("/Stop", []() {
    currentMovement = STOP;
    page2 = "<center><p>Status: Stop</p></center>";
    server.send(200, "text/html", page + page2);
  });

  server.on("/ClawOpen", []() {
    claw_open();
    page2 = "<center><p>Claw Status: Open</p></center>";
    server.send(200, "text/html", page + page2);
  });

  server.on("/ClawClose", []() {
    claw_close();
    page2 = "<center><p>Claw Status: Closed</p></center>";
    server.send(200, "text/html", page + page2);
  });
}

// --- Movement Control ---
void handleCurrentMovement() {
  switch (currentMovement) {
    case STOP: stopMotors(); break;
    case FORWARD: goForward(); break;
    case BACKWARD: goBackward(); break;
    case LEFT: goLeft(); break;
    case RIGHT: goRight(); break;
  }
}

void goForward() {
  digitalWrite(r1_pin, LOW);
  digitalWrite(r2_pin, HIGH);
  digitalWrite(l1_pin, HIGH);
  digitalWrite(l2_pin, LOW);
}

void goBackward() {
  digitalWrite(r1_pin, HIGH);
  digitalWrite(r2_pin, LOW);
  digitalWrite(l1_pin, LOW);
  digitalWrite(l2_pin, HIGH);
}

void goLeft() {
  digitalWrite(r1_pin, LOW);
  digitalWrite(r2_pin, HIGH);
  digitalWrite(l1_pin, LOW);
  digitalWrite(l2_pin, LOW);
}

void goRight() {
  digitalWrite(r1_pin, LOW);
  digitalWrite(r2_pin, LOW);
  digitalWrite(l1_pin, HIGH);
  digitalWrite(l2_pin, LOW);
}

void stopMotors() {
  digitalWrite(r1_pin, LOW);
  digitalWrite(r2_pin, LOW);
  digitalWrite(l1_pin, LOW);
  digitalWrite(l2_pin, LOW);
}
*/