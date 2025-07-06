#include <WiFi.h>
#include <HTTPClient.h>
#include <LoRa.h>
#include <SPI.h>

#define WIFI_SSID "STARLINK"
#define WIFI_PASSWORD "Jazi1423.."
#define FIREBASE_PROJECT_ID "dashboard-1ef79"
#define FIREBASE_API_KEY "AIzaSyAgT_5JjnWeVLCGUZ4CvieD5nuBRbjbjRY"
#define FIREBASE_COLLECTION_PATH "mensajes"
#define GATEWAY_ID "gateway_001"

#define MISO 2
#define MOSI 5
#define SCK 18
#define SS_PIN 23
#define RST_PIN 19
#define DIO0_PIN 4

void updateGatewayStatus(bool isConnected) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String gatewayUrl = "https://firestore.googleapis.com/v1/projects/" + String(FIREBASE_PROJECT_ID) +
                       "/databases/(default)/documents/gateways/" + GATEWAY_ID + "?key=" + FIREBASE_API_KEY;

    String gatewayPayload = "{\"fields\":{\"isReady\":{\"booleanValue\":\"true\"},\"isWifiConnected\":{\"booleanValue\":\"" + String(isConnected ? "true" : "false") + "\"},\"timestamp\":{\"timestampValue\":\"" + getISO8601Time() + "\"}}}";

    http.begin(gatewayUrl);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.sendRequest("PATCH", gatewayPayload);

    if (httpResponseCode > 0) {
      Serial.println("✅ Estado del Gateway actualizado en Firestore");
    } else {
      Serial.print("❌ Error al actualizar estado del Gateway: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  configTime(0, 0, "pool.ntp.org");
  delay(2000); // Esperar sincronización NTP

  SPI.begin(SCK, MISO, MOSI, SS_PIN);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa no iniciado");
    while (true);
  }

  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);

  updateGatewayStatus(true);

  Serial.println("📡 LoRa iniciado");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String mensaje = "";
    while (LoRa.available()) {
      mensaje += (char)LoRa.read();
    }
    int rssi = LoRa.packetRssi();
    Serial.print("📩 Recibido: ");
    Serial.println(mensaje);
    Serial.print("RSSI: ");
    Serial.println(rssi);

    bool isWifiConnected = WiFi.status() == WL_CONNECTED;
    updateGatewayStatus(isWifiConnected);

    // Extraer msgId
    int msgIdStart = mensaje.indexOf("ID: ") + 4;
    int msgIdEnd = mensaje.indexOf(" | Nodo: ");
    String msgId = mensaje.substring(msgIdStart, msgIdEnd);

    // Enviar ACK al Nodo
    String ack = "ACK: " + msgId;
    LoRa.beginPacket();
    LoRa.print(ack);
    LoRa.endPacket();
    Serial.println("📡 Enviado ACK: " + ack);

    // Subir a Firebase
    if (isWifiConnected) {
      HTTPClient http;
      String url = "https://firestore.googleapis.com/v1/projects/" + String(FIREBASE_PROJECT_ID) +
                   "/databases/(default)/documents/" + FIREBASE_COLLECTION_PATH + "?key=" + FIREBASE_API_KEY;

      String jsonPayload = "{\"fields\":{";
      jsonPayload += "\"mensaje\":{\"stringValue\":\"" + mensaje + "\"},";
      jsonPayload += "\"rssi\":{\"integerValue\":\"" + String(rssi) + "\"},";
      jsonPayload += "\"msgId\":{\"stringValue\":\"" + msgId + "\"},";
      jsonPayload += "\"timestamp\":{\"timestampValue\":\"" + getISO8601Time() + "\"}";
      jsonPayload += "}}";

      http.begin(url);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonPayload);

      if (httpResponseCode > 0) {
        Serial.println("✅ Enviado a Firestore");
      } else {
        Serial.print("❌ Error HTTP: ");
        Serial.println(httpResponseCode);
        Serial.println(http.getString());
      }
      http.end();
    }
  }

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 10000) {
    bool isWifiConnected = WiFi.status() == WL_CONNECTED;
    if (isWifiConnected != (WiFi.status() == WL_CONNECTED)) {
      updateGatewayStatus(isWifiConnected);
    }
    lastCheck = millis();
  }
}

String getISO8601Time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01T00:00:00Z";
  }
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buf);
}