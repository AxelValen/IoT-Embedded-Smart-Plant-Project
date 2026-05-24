#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

#define LED_BUILTIN 2
#define WIFI_SSID     **WIFI_SSID**
#define WIFI_PASSWORD **WIFI_PASSWORD**

const char* serverIP = "172.20.10.3";  // IP local

bool isConnected = false;
int contador = 0;
unsigned long lastSend = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Conectando...");
}

void loop() {
  // --- Manejo de conexión (no bloqueante) ---
  if (WiFi.status() == WL_CONNECTED && !isConnected) {
    Serial.println("✅ Conectado!");
    Serial.print("IP del ESP32: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, HIGH);
    isConnected = true;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(1000);
    isConnected = false;
    return; // no intentes enviar si no hay conexión
  }

  // --- Envío de datos cada 2 segundos ---
  if (millis() - lastSend >= 2000) {
    lastSend = millis();

    HTTPClient http;
    String url = String("http://") + serverIP + ":3000/data";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"valor\":" + String(contador) +
                     ",\"mensaje\":\"Hola desde ESP32\"}";

    int httpCode = http.POST(payload);
    Serial.print("📤 POST | HTTP ");
    Serial.print(httpCode);
    Serial.print(" | ");
    Serial.println(payload);

    http.end();
    contador++;
  }
}