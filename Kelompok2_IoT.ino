#include "secret.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MLX90614.h>
#include "DHT.h"
#include <time.h>
#include <MQ135.h>  // Menambahkan pustaka MQ135

#define LED_RED_PIN    15   // Pin untuk LED merah (danger)
#define LED_GREEN_PIN  7    // Pin untuk LED hijau (safe)

// Pin dan tipe DHT
#define DHTPIN 4
#define DHTTYPE DHT22

// Pin untuk sensor MQ135
#define MQ135_PIN 17  // Gunakan pin analog yang tersedia (misalnya GPIO34)

// Inisialisasi sensor
MAX30105 particleSensor;
Adafruit_BMP280 bmp;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
DHT dht(DHTPIN, DHTTYPE);
MQ135 gasSensor(MQ135_PIN);  // Inisialisasi sensor MQ135

// Variabel untuk data sensor
float spo2 = 0, heartRate = 0;
float ambientTemp = 0, objectTemp = 0;
float bmpTemp = 0, pressure = 0, altitude = 0;
float humidity = 0, dhtTemp = 0;
float co2 = 0;  // Variabel untuk CO2

// NTP Server dan pengaturan zona waktu
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // GMT +7
const int daylightOffset_sec = 0;

// Interval pembacaan sensor
unsigned long lastMaxReadTime = 0;
unsigned long lastMlxReadTime = 0;
unsigned long lastBmpReadTime = 0;
unsigned long lastDhtReadTime = 0;
const unsigned long maxReadInterval = 1500;
const unsigned long mlxReadInterval = 100;
const unsigned long bmpReadInterval = 100;
const unsigned long dhtReadInterval = 100;

// AWS IoT Core configurations
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi connected!");

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setCallback(messageHandler);

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("AWS IoT connected!");
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
}

void initTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
}

String getTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01T00:00:00Z";
  }
  char timestamp[25];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(timestamp);
}

void publishMessage() {
  StaticJsonDocument<1024> doc;

  // Tentukan status berdasarkan data sensor
  String status = "safe";

  if (pressure < 950 || pressure > 1050) {
    status = "pressure_danger";
  } else if (heartRate < 40 || heartRate > 150) {
    status = "heartRate_danger";
  } else if (co2 > 1000) {
    status = "co2_danger";
  } else if (ambientTemp < 30 || ambientTemp > 38) {
    status = "ambient_temp_danger";
  } else if (dhtTemp < 15 || dhtTemp > 35 || humidity < 20 || humidity > 80) {
    status = "environment_danger";
  }

  // Tambahkan data sensor dan status ke JSON payload
  doc["timestamp"] = getTimestamp();
  doc["heartRate"] = heartRate;
  doc["ambientTemp"] = ambientTemp;
  doc["objectTemp"] = objectTemp;
  doc["pressure"] = pressure;
  doc["humidity"] = humidity;
  doc["dhtTemp"] = dhtTemp;
  doc["co2"] = co2;
  doc["status"] = status;  // Menambahkan status ke payload

  // Serialize JSON payload
  char jsonBuffer[1024];
  serializeJson(doc, jsonBuffer);
  
  // Debug print the JSON payload
  Serial.print("Payload JSON: ");
  Serial.println(jsonBuffer);

  // Publish data to AWS IoT Core
  if (client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer)) {
    Serial.println("Data successfully published to AWS IoT!");
  } else {
    Serial.println("Failed to publish data to AWS IoT.");
  }
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<512> doc;
  deserializeJson(doc, payload);

  const char* status = doc["status"];
  const char* timestamp = doc["timestamp"];

  Serial.print("Received Command - Status: ");
  Serial.println(status);
  Serial.print("Timestamp: ");
  Serial.println(timestamp);

  // Mengubah warna LED berdasarkan status
  if (strcmp(status, "pressure_danger") == 0 || 
      strcmp(status, "heartRate_danger") == 0 || 
      strcmp(status, "co2_danger") == 0 || 
      strcmp(status, "ambient_temp_danger") == 0 || 
      strcmp(status, "environment_danger") == 0) {
    // Status bahaya, LED merah
    digitalWrite(LED_RED_PIN, HIGH);   // Nyalakan LED Merah
    digitalWrite(LED_GREEN_PIN, LOW);  // Matikan LED Hijau
    digitalWrite(10, HIGH);  // Aktifkan relay
    digitalWrite(15, HIGH);  // Aktifkan buzzer
  } else {
    // Status aman, LED hijau menyala
    digitalWrite(LED_RED_PIN, LOW);    // Matikan LED Merah
    digitalWrite(LED_GREEN_PIN, HIGH); // Nyalakan LED Hijau
    digitalWrite(10, LOW);   // Matikan relay
    digitalWrite(15, LOW);   // Matikan buzzer
  }
}


void setup() {
  Serial.begin(115200);
  connectAWS();
  client.setCallback(messageHandler);

    // Konfigurasi pin untuk aktuator
  pinMode(10, OUTPUT);  // Relay
  pinMode(15, OUTPUT);  // Buzzer

  // Subscribe ke topic command
  client.subscribe("esp32/sub");  // Topic untuk menerima command dari AWS IoT Core
  
  initTime();

  // Initialize sensors
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD, 0x57)) {
    Serial.println("MAX30105 sensor not detected!");
  } else {
    particleSensor.setup(60, 4, 2, 100, 411, 4096);
    Serial.println("MAX30105 sensor detected!");
  }

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 sensor not detected!");
  } else {
    Serial.println("BMP280 sensor detected!");
  }

  if (!mlx.begin()) {
    Serial.println("MLX90614 sensor not detected!");
  } else {
    Serial.println("MLX90614 sensor detected!");
  }

  dht.begin();
}

void loop() {
  unsigned long currentTime = millis();

  // Reading MAX30102
  if (currentTime - lastMaxReadTime >= maxReadInterval) {
    lastMaxReadTime = currentTime;
    readMAX30102();
  }

  // Reading MLX90614
  if (currentTime - lastMlxReadTime >= mlxReadInterval) {
    lastMlxReadTime = currentTime;
    readMLX90614();
  }

  // Reading BMP280
  if (currentTime - lastBmpReadTime >= bmpReadInterval) {
    lastBmpReadTime = currentTime;
    readBMP280();
  }

  // Reading DHT22
  if (currentTime - lastDhtReadTime >= dhtReadInterval) {
    lastDhtReadTime = currentTime;
    readDHT22();
  }

  // Reading MQ135
  readCO2();  // Membaca data CO2 dari sensor MQ135
  
  // Publish the message to AWS IoT
  publishMessage();

  // MQTT client loop
  client.loop();
  delay(1000);
}

// Fungsi membaca MAX30102
void readMAX30102() {
  uint32_t irBuffer[100], redBuffer[100];
  int32_t bufferLength = 100;
  int32_t heartRateLocal = 0, spo2Local = 0;
  int8_t validSPO2 = 0, validHeartRate = 0;

  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) {
      particleSensor.check();
    }
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2Local, &validSPO2, &heartRateLocal, &validHeartRate);

  heartRate = heartRateLocal;
  spo2 = spo2Local;
  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  Serial.print("SPO2: ");
  Serial.println(spo2);
}

void readMLX90614() {
  ambientTemp = mlx.readAmbientTempC();
  objectTemp = mlx.readObjectTempC();
  Serial.print("Ambient Temp: ");
  Serial.println(ambientTemp);
  Serial.print("Object Temp: ");
  Serial.println(objectTemp);
}

void readBMP280() {
  bmpTemp = bmp.readTemperature();
  pressure = bmp.readPressure() / 100.0;
  altitude = bmp.readAltitude(1013.25);
  Serial.print("BMP Temp: ");
  Serial.println(bmpTemp);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  Serial.print("Altitude: ");
  Serial.println(altitude);
}

void readDHT22() {
  humidity = dht.readHumidity();
  dhtTemp = dht.readTemperature();

  if (isnan(humidity) || isnan(dhtTemp)) {
    Serial.println("Failed to read DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("DHT Temp: ");
  Serial.println(dhtTemp);
}

void readCO2() {
  co2 = gasSensor.getPPM();  // Nilai CO2 dalam PPM
  Serial.print("CO2: ");
  Serial.println(co2);
}
