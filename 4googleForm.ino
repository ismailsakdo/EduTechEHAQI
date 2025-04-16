#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_PM25AQI.h>
#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
#define WIFI_SSID "XXXXX"
#define WIFI_PASSWORD "XXXXX"
#define FORM_URL "https://docs.google.com/forms/d/e/INSERTYOURSXXXXX/formResponse?usp=pp_url&"

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// AHT20 (Temp & Humidity)
Adafruit_AHTX0 aht;

// PMSA003I
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(16, 17); // RX, TX
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// Variables to store sensor data
float temperature = 0;
float humidity = 0;
uint16_t pm03 = 0, pm10 = 0, pm25 = 0, pm100 = 0;

void setup() {
  Serial.begin(115200);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // AHT Sensor
  if (!aht.begin()) {
    Serial.println("AHT not found");
    while (1) delay(10);
  }

  // PMS Sensor
  pmSerial.begin(9600);
  if (!aqi.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM sensor!");
    while (1) delay(10);
  }
}

void loop() {
  sensors_event_t humidityEvent, tempEvent;
  aht.getEvent(&humidityEvent, &tempEvent);
  temperature = tempEvent.temperature;
  humidity = humidityEvent.relative_humidity;

  PM25_AQI_Data data;
  if (aqi.read(&data)) {
    pm03 = data.particles_03um;
    pm10 = data.pm10_standard;
    pm25 = data.pm25_standard;
    pm100 = data.pm100_standard;
  } else {
    Serial.println("Failed to read PM sensor");
    return;
  }

  // Serial Monitor Output
  Serial.println("========== SENSOR DATA ==========");
  Serial.printf("Temperature: %.2f °C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("PM0.3: %u #/0.1L\n", pm03);
  Serial.printf("PM1.0: %u µg/m³\n", pm10);
  Serial.printf("PM2.5: %u µg/m³\n", pm25);
  Serial.printf("PM10 : %u µg/m³\n", pm100);
  Serial.println("=================================");

  // OLED Output
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Temp: %.1f C\n", temperature);
  display.printf("Humidity: %.1f %%\n", humidity);
  display.printf("PM0.3: %u\n", pm03);
  display.printf("PM1.0: %u\n", pm10);
  display.printf("PM2.5: %u\n", pm25);
  display.printf("PM10 : %u\n", pm100);
  display.display();

  // Send to Google Form (Update Your Number)
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postData = "entry.1468817812=" + String(temperature) +
                      "&entry.1496521866=" + String(humidity) +
                      "&entry.1637395709=" + String(pm03) +
                      "&entry.1843721027=" + String(pm10) +
                      "&entry.882263007=" + String(pm25) +
                      "&entry.806421355=" + String(pm100);  // replace these with your correct Google Form field IDs

    http.begin(FORM_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int response = http.POST(postData);

    if (response == 200) {
      Serial.println("Google Form: Data submitted successfully!");
    } else {
      Serial.print("Google Form Error: ");
      Serial.println(response);
    }
    http.end();
  }

  delay(10000); // 10s delay between readings
}
