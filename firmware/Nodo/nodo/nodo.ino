#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <vector> // Para usar std::vector

#define SDA_PIN 21
#define SCL_PIN 22

#define LED_PIN 27
#define NUMPIXELS 3
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Colores útiles
#define OFF      pixels.Color(0, 0, 0)
#define GREEN    pixels.Color(0, 255, 0)
#define YELLOW   pixels.Color(255, 150, 0)
#define ORANGE   pixels.Color(255, 60, 0)
#define RED      pixels.Color(255, 0, 0)

#define MISO 2
#define MOSI 5
#define SCK 18
#define SS_PIN 23     // CS LoRa
#define RST_PIN 19    // RESET LoRa
#define DI0_PIN 4     // DIO0 LoRa
#define node_id "N002"

#define MQ3_PIN 34    // Pin MQ-3
const float RL = 10000.0;
const float VCC = 3.3;
float R0 = 6000.0;         // Valor estimado de R0
const float A = -0.40;
const float B = 1.35;

Adafruit_BME680 bme;

// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1
unsigned long lastGpsFix = 0;
const unsigned long gpsTimeout = 10000;

// Vector para almacenar datos
std::vector<String> dataBuffer;
const unsigned long transmissionInterval = 2000; // Intervalo de 2 segundos
const unsigned long ackTimeout = 2000; // Aumentado a 2 segundos
const int maxRetries = 3;

void calibrarR0() {
  Serial.println("🕐 Calibrando R0... Por favor, no soples alcohol cerca");
  delay(5000);

  float sumaRs = 0.0;
  for (int i = 0; i < 50; i++) {
    int raw = analogRead(MQ3_PIN);
    float Vout = raw * (VCC / 4095.0);
    float Rs = (VCC - Vout) * RL / Vout;
    sumaRs += Rs;
    delay(100);
  }

  R0 = sumaRs / 50.0;
  Serial.print("📏 R0 calibrado: ");
  Serial.println(R0);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin(SDA_PIN, SCL_PIN);
  gpsSerial.begin(9600, SERIAL_8N1, 13, 12);

  if (!bme.begin(0x76)) {
    Serial.println("❌ No se encontró BME680. Verifica conexiones.");
    while (1);
  }

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  SPI.begin(SCK, MISO, MOSI, SS_PIN);
  LoRa.setPins(SS_PIN, RST_PIN, DI0_PIN);
  if (!LoRa.begin(915E6)) {
    Serial.println("❌ Fallo al iniciar LoRa");
    while (true);
  }

  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);

  pixels.begin();
  pixels.clear();
  pixels.show();

  calibrarR0();

  Serial.println("✅ LoRa, BME680, NeoPixels y GPS listos");
}

String getSensorData() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      lastGpsFix = millis(); // Registrar el último fix válido
    }
  }

  bool gpsActivo = (millis() - lastGpsFix) < gpsTimeout;
  if (!gpsActivo) return "";

  if (!bme.performReading()) return "";

  int mq3_raw = analogRead(MQ3_PIN);
  if (mq3_raw <= 100 || mq3_raw >= 4000) return "";

  float Vout = mq3_raw * (VCC / 4095.0);
  float Rs = (VCC - Vout) * RL / Vout;
  float ratio = Rs / R0;
  float ppm = pow(10, A * log10(ratio) + B);

  float temp = bme.temperature;
  float hum = bme.humidity;
  float pres = bme.pressure / 100.0;
  float gas = bme.gas_resistance / 1000.0;

  double lat = gps.location.lat();
  double lng = gps.location.lng();

  String mensaje = "ID: " + String(millis());
  mensaje += " | Nodo: ";
  mensaje += node_id;
  mensaje += " | Temp: " + String(temp, 1) + "°C";
  mensaje += " | Hum: " + String(hum, 1) + "%";
  mensaje += " | Pres: " + String(pres, 1) + "hPa";
  mensaje += " | Gas: " + String(gas, 1) + "kΩ";
  mensaje += " | MQ3: " + String(ppm, 1) + " ppm";
  mensaje += " | Lat: " + String(lat, 6);
  mensaje += " | Lon: " + String(lng, 6);

  return mensaje;
}

void sendBufferedData() {
  if (dataBuffer.empty()) {
    Serial.println("🗑 Buffer de datos vacío");
    return;
  }

  for (size_t i = 0; i < dataBuffer.size(); i++) {
    bool success = false;
    String msgId = dataBuffer[i].substring(4, 19); // Extraer ID (millis)
    for (int j = 0; j < maxRetries; j++) {
      pixels.setPixelColor(0, YELLOW);
      pixels.show();
      LoRa.beginPacket();
      LoRa.print(dataBuffer[i]);
      LoRa.endPacket();
      unsigned long startTime = millis();
      while (millis() - startTime < ackTimeout) {
        if (LoRa.parsePacket() > 0) {
          String ack = LoRa.readString();
          if (ack.indexOf("ACK: " + msgId) >= 0) {
            success = true;
            Serial.println("📡 Datos enviados con éxito - ID: " + msgId);
            break;
          }
        }
      }
      if (success) break;
      Serial.println("❌ Intento " + String(j + 1) + " fallido para ID: " + msgId);
      delay(500);
    }
    if (success) {
      dataBuffer.erase(dataBuffer.begin() + i);
      i--; // Ajustar índice después de borrar
    } else {
      Serial.println("❌ Fallo al enviar datos buffered: ID: " + msgId);
    }
  }
}

void loop() {
  String sensorData = getSensorData();
  if (sensorData == "") {
    pixels.setPixelColor(0, ORANGE);
    pixels.show();
    delay(1000);
    return;
  }

  bool success = false;
  for (int i = 0; i < maxRetries; i++) {
    pixels.setPixelColor(0, YELLOW);
    pixels.show();
    LoRa.beginPacket();
    LoRa.print(sensorData);
    LoRa.endPacket();
    unsigned long startTime = millis();
    while (millis() - startTime < ackTimeout) {
      if (LoRa.parsePacket() > 0) {
        String ack = LoRa.readString();
        if (ack.indexOf("ACK: " + sensorData.substring(4, 19)) >= 0) {
          success = true;
          break;
        }
      }
    }
    if (success) break;
    pixels.setPixelColor(0, ORANGE);
    pixels.show();
    delay(500);
  }

  float ppm = sensorData.substring(sensorData.indexOf("MQ3: ") + 5, sensorData.indexOf(" ppm")).toFloat();
  if (ppm > 100) pixels.setPixelColor(1, RED);
  else if (ppm > 35) pixels.setPixelColor(1, ORANGE);
  else pixels.setPixelColor(1, GREEN);

  float temp = sensorData.substring(sensorData.indexOf("Temp: ") + 6, sensorData.indexOf("°C")).toFloat();
  if (temp >= 32.0) pixels.setPixelColor(2, RED);
  else if (temp >= 28.0) pixels.setPixelColor(2, YELLOW);
  else pixels.setPixelColor(2, GREEN);

  pixels.show();

  if (success) {
    pixels.setPixelColor(0, GREEN);
    Serial.println("📡 Enviado por LoRa con éxito: " + sensorData);
    sendBufferedData(); // Intentar enviar todos los datos buffered
  } else {
    dataBuffer.push_back(sensorData); // Almacenar en el buffer
    pixels.setPixelColor(0, RED);
    Serial.println("❌ Fallo al enviar, datos almacenados en buffer: " + sensorData);
    sendBufferedData(); // Intentar enviar datos buffered incluso tras fallo
  }

  delay(transmissionInterval);
}