#include <WiFi.h>
#include <WebServer.h>
#include "src/secrets.h"
#include "DHT.h"

// Pines
#define Led_H 10
#define Led_T 8
#define Led_B 7
#define DHTPin 4

// Sensor DHT
#define DHTTYPE DHT22
DHT dht(DHTPin, DHTTYPE);

// Servidor HTTP
WebServer server(80);

// Página web prueba
String getPage(float humidity, float temperature) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Panel IoT - ESP32s3</title>
<meta http-equiv="refresh" content="2">
<style>
* {margin:0; padding:0; box-sizing:border-box;}
body {font-family:'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg,#667eea 0%,#764ba2 100%); min-height:100vh; display:flex; justify-content:center; align-items:center; padding:20px;}
.container {width:100%; max-width:500px;}
.header {text-align:center; color:white; margin-bottom:30px;}
.header h1 {font-size:2.5em; margin-bottom:10px; text-shadow:2px 2px 4px rgba(0,0,0,0.3);}
.header p {font-size:1.1em; opacity:0.9;}
.cards-grid {display:grid; grid-template-columns:1fr 1fr; gap:15px; margin-bottom:20px;}
.card {background:white; border-radius:15px; padding:25px; box-shadow:0 8px 20px rgba(0,0,0,0.15); text-align:center;}
.card-icon {font-size:3em; margin-bottom:10px;}
.card-label {color:#999; font-size:0.9em; text-transform:uppercase; letter-spacing:1px; margin-bottom:8px;}
.card-value {font-size:2.5em; font-weight:bold; color:#667eea; margin-bottom:5px;}
.card-unit {color:#999; font-size:1em;}
.led-control {background:white; border-radius:15px; padding:25px; box-shadow:0 8px 20px rgba(0,0,0,0.15); text-align:center;}
.led-button {width:100%; padding:20px; font-size:1.3em; font-weight:bold; border:none; border-radius:10px; cursor:pointer; text-transform:uppercase; letter-spacing:2px;}
.led-button.on {background:linear-gradient(135deg,#4caf50,#66bb6a); color:white; border:3px solid #2e7d32;}
</style>
</head>
<body>
<div class="container">
<div class="header">
<h1>🌡️ Panel IoT</h1>
<p>ESP32s3 Sensor Dashboard</p>
</div>

<div class="cards-grid">
<div class="card">
<div class="card-icon">🌡️</div>
<div class="card-label">Temperatura</div>
<div class="card-value">)rawliteral";
  
  html += String(temperature);
  
  html += R"rawliteral(</div>
<div class="card-unit">°C</div>
</div>

<div class="card">
<div class="card-icon">💧</div>
<div class="card-label">Humedad</div>
<div class="card-value">)rawliteral";
  
  html += String(humidity);
  
  html += R"rawliteral(</div>
<div class="card-unit">%</div>
</div>
</div>
<div class="led-control">
  <a href="/led/on"><button class="led-button on">Encender LED</button></a>
  <a href="/led/off"><button class="led-button off">Apagar LED</button></a>
</div>
</div>
</body>
</html>
)rawliteral";

  return html;
}


void handleRoot() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("⚠ Error leyendo el DHT22");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(hum);
    Serial.println(" %");
  }
  server.send(200, "text/html", getPage(hum, temp));
}

void handleLedOn() {
  digitalWrite(Led_B, HIGH);
  server.sendHeader("Location", "/"); // redirige a la página principal
  server.send(303);
}

void handleLedOff() {

  digitalWrite(Led_B, LOW);
  server.sendHeader("Location", "/"); // redirige a la página principal
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  // Leds
  pinMode(Led_H, OUTPUT);
  pinMode(Led_T, OUTPUT);
  pinMode(Led_B, OUTPUT);

  digitalWrite(Led_H, LOW);
  digitalWrite(Led_T, LOW);
  digitalWrite(Led_B, LOW);

  // DHT
  dht.begin();
  delay(2000); //Necesario para que le de tiempo al sensor a iniciarse

  // WIFI
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Rutas servidor web
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  // Inicio servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
