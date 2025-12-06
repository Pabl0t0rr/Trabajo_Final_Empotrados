#include <WiFi.h>
#include <WebServer.h>
#include "src/secrets.h"
#include "DHT.h"
#include "Ticker.h"

//Ticker para manejar el DHT
Ticker tickerDHT;
Ticker tickerWiFi;

// Pines
#define Led_H 10
#define Led_T 8
#define Led_B 7
#define DHTPin 4

// Sensor DHT
#define DHTTYPE DHT22
DHT dht(DHTPin, DHTTYPE);
//Para poder acceder a ellos de forma global
float temperatura = 0.0;
float humedad = 0.0;
//Valores maximos pra que salten
float tempMax = 24.0;
float humMax = 90.0;

// Servidor HTTP
WebServer server(80);

//Metricas OPCIONAL medinate latencias
unsigned long lastRead = 0;
unsigned long tiempoLedTEncendido = 0;
unsigned long tiempoLedHEncendido = 0;
int lecturasTotales = 0;
int erroresDHT = 0;
unsigned long lastServerTime = 0;

//Funcion mostrar metricas obtenidas
void printMetrics(unsigned long dtDHT, unsigned long latenciaDHTtoLED, unsigned long serverLatency) {
    Serial.println("===== METRICAS =====");
    Serial.print("Temperatura actual: "); Serial.println(temperatura);
    Serial.print("Humedad actual: "); Serial.println(humedad);
    Serial.print("Tiempo desde última lectura DHT (ms): "); Serial.println(dtDHT);
    Serial.print("Latencia DHT→LED (ms): "); Serial.println(latenciaDHTtoLED);
    Serial.print("Lecturas totales DHT: "); Serial.print(lecturasTotales);
    Serial.print(" | Errores DHT: "); Serial.println(erroresDHT);
    Serial.print("Tiempo total LED_T encendido (ms): "); Serial.println(tiempoLedTEncendido);
    Serial.print("Tiempo total LED_H encendido (ms): "); Serial.println(tiempoLedHEncendido);
    Serial.print("Latencia servidor web (ms): "); Serial.println(serverLatency);
}

//Funcion para consegurir los datos del DHT22
void getDataDHT() {
    unsigned long now = millis();
    unsigned long dt = now - lastRead;
    lastRead = now;
    lecturasTotales++;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
        erroresDHT++;
        Serial.println("Error leyendo el DHT22");
    } else {
        temperatura = t;
        humedad = h;
    }

    // Latencia evento→acción
    unsigned long t0 = millis();

    // Control LEDs
    if(temperatura >= tempMax){
        digitalWrite(Led_T, HIGH);
        tiempoLedTEncendido += dt;
    } else {
        digitalWrite(Led_T, LOW);
    }

    if(humedad >= humMax){
        digitalWrite(Led_H, HIGH);
        tiempoLedHEncendido += dt;
    } else {
        digitalWrite(Led_H, LOW);
    }

  unsigned long t1 = millis();
  unsigned long latenciaDHTtoLED = t1 - t0;
  // Imprimir métricas
  printMetrics(dt, latenciaDHTtoLED, lastServerTime);
}

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reintentando conexión...");
    WiFi.reconnect();
  }
}

// Página web prueba
String getPage(float h, float t, bool ledState) {

  String btnLabel = ledState ? "Apagar LED" : "Encender LED";
  String btnClass = ledState ? "off" : "on";
  String btnColor = ledState ? "linear-gradient(135deg,#f44336,#d32f2f)" : "linear-gradient(135deg,#4caf50,#66bb6a)";

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

  html += String(t);

  html += R"rawliteral(</div>
  <div class="card-unit">°C</div>
  </div>

  <div class="card">
  <div class="card-icon">💧</div>
  <div class="card-label">Humedad</div>
  <div class="card-value">)rawliteral";

  html += String(h);

  html += R"rawliteral(</div>
  <div class="card-unit">%</div>
  </div>
  </div>
  <div class="led-control">
    <a href=")rawliteral";

  html += ledState ? "/led/off" : "/led/on";

  html += R"rawliteral("><button class="led-button" style="background:)rawliteral";

  html += btnColor;

  html += R"rawliteral(">)rawliteral";

  html += btnLabel;

  html += R"rawliteral(</button></a>
  </div>
  </div>
  </body>
  </html>
  )rawliteral";

  return html;
}

void handleRoot() {
  bool estadoLed = digitalRead(Led_B);
  server.send(200, "text/html", getPage(humedad, temperatura, estadoLed));
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

//Para poder tratar los eventos wifi
void wifiCallback(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.print("Conectado a ");
      Serial.println(WIFI_SSID);
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("WiFi OK - IP asignada: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi desconectado. Intentando reconectar...");
      WiFi.reconnect();
      break;
    default:
      Serial.print("Evento WiFi desconocido: ");
      Serial.println(event);
      break;
  }
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
  tickerDHT.attach(1.0, getDataDHT); //Conseguir la info cad 1seg

  // WIFI
  WiFi.onEvent(wifiCallback);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  tickerWiFi.attach(5.0, checkWiFi); //Revisar que sigas teniendo conexion en mitad del programa cada 5seg

  // Rutas servidor web
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  // Inicio servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  unsigned long t0 = millis();
  server.handleClient();
  unsigned long t1 = millis();
  lastServerTime = t1 - t0; // guardamos latencia para métricas
}
