# Trabajo Final - Sistemas Empotrados con ESP32-S3

## Descripción del Proyecto

Este proyecto es el **Trabajo Final de Sistemas Empotrados**. Consiste en un montaje con **ESP32-S3** que:

* Mide **temperatura y humedad** usando un **sensor DHT22**.
* Monta un **servidor web** accesible desde cualquier navegador en la misma red Wi-Fi.
* Permite **visualizar los datos en tiempo real** y **controlar LEDs** mediante botones en la web.

El objetivo principal es demostrar cómo integrar hardware y software en un sistema empotrado interactivo y conectado.

---

## Tabla de Contenidos

* [Descripción](#descripción-del-proyecto)
* [Características](#características)
* [Materiales](#materiales)
* [Diagrama de Conexiones](#diagrama-de-conexiones)
* [Instalación](#instalación)
* [Uso](#uso)
* [Capturas](#capturas)

---

## Características

* Lectura de **temperatura y humedad** con DHT22.
* Servidor web **integrado en la ESP32-S3**.
* Interfaz web **interactiva** para control de LEDs.
* Actualización de datos en **tiempo real**.
* Compatible con **navegadores modernos** sin necesidad de instalar software adicional.

---

## Materiales

* ESP32-S3
* Sensor DHT22
* LEDs (3)
* Resistencias (220Ω)
* Cables y protoboard

---

## Diagrama de Conexiones

```
ESP32-S3       DHT22       LED_T         LED_H       LED_B
--------      -------     -------       -------     --------
GPIO4  -------- DATA
GND    -------- GND        GND
3V3    -------- VCC
GPIO7 ------------------- Anodo (+) LED_B
GPIO8 ------------------- Anodo (+) LED_T
GPIO10 ------------------ Anodo (+) LED_H
GND    ------------------ Catodo LED (-) con resistencia
```

## Instalación

1. Clonar el repositorio:

```bash
git clone https://github.com/Pabl0t0rr/Trabajo_Final_Empotrados.git
```

2. Abrir el proyecto en **Arduino IDE**.
3. Instalar librerías necesarias:

   * `DHT sensor library`
   * `WiFi`
   * `WebServer`
   * #include "Ticker"

4. Configurar tu **SSID** y **password** de Wi-Fi en el código.
5. Compilar y subir el código a tu ESP32-S3.

---

## Uso

1. Conecta la ESP32-S3 a tu fuente de alimentación.
2. Revisa la **IP asignada** por el monitor serial.
3. Abre tu navegador e ingresa la IP de la ESP32-S3.
4. Visualiza temperatura y humedad en tiempo real.
5. Usa los botones de la web para encender/apagar los LEDs.

---

## Capturas

<div align="center">
  <img src="https://raw.githubusercontent.com/Pabl0t0rr/Trabajo_Final_Empotrados/main/assets/montajePlaca.jpeg" width="500" />
</div>

*Montaje basado en ESP32-S3 con sensor DHT22 y control de LEDs vía web.*


<div align="center">
  <img src="https://raw.githubusercontent.com/Pabl0t0rr/Trabajo_Final_Empotrados/main/assets/Interfaz_Final.png" width="500" />
</div>
*Interfaz web para visualizar y controlar la placa.*


