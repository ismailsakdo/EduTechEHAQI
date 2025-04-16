#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PM25AQI.h>
#include <Adafruit_AHTX0.h>
#include <SoftwareSerial.h>

// ===== OLED Setup =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== PM2.5 Sensor Setup =====
SoftwareSerial pmSerial(16, 17); // RX=16 (from sensor TX), TX=17 (unused)
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// ===== AHT10/20 Sensor Setup =====
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize PM2.5 sensor
  pmSerial.begin(9600);
  if (!aqi.begin_UART(&pmSerial)) {
    Serial.println("PM2.5 sensor not found!");
    while (true);
  }
  Serial.println("PM2.5 sensor initialized");

  // Initialize Temp & Humidity sensor
  if (!aht.begin()) {
    Serial.println("AHT sensor not found!");
    while (true);
  }
  Serial.println("AHT10/AHT20 sensor initialized");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  PM25_AQI_Data data;
  bool pm_ok = aqi.read(&data);

  // ==== Serial Output ====
  Serial.println(F("----------- Sensor Readings -----------"));
  Serial.print("Temperature: ");
  Serial.print(temp.temperature, 1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity, 1);
  Serial.println(" %RH");

  if (pm_ok) {
    Serial.print("PM0.3: ");
    Serial.print(data.particles_03um);
    Serial.println(" particles/0.1L");

    Serial.print("PM1.0: ");
    Serial.print(data.pm10_standard);
    Serial.println(" µg/m³");

    Serial.print("PM2.5: ");
    Serial.print(data.pm25_standard);
    Serial.println(" µg/m³");

    Serial.print("PM10 : ");
    Serial.print(data.pm100_standard);
    Serial.println(" µg/m³");
  } else {
    Serial.println("PM sensor read error!");
  }

  // ==== OLED Display ====
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Temp: ");
  display.print(temp.temperature, 1);
  display.println((char)247); // ° symbol
  display.print("C");

  display.setCursor(0, 10);
  display.print("Humidity: ");
  display.print(humidity.relative_humidity, 1);
  display.println(" %");

  if (pm_ok) {
    display.setCursor(0, 20);
    display.print("PM0.3: ");
    display.print(data.particles_03um);
    display.println(" #/0.1L");

    display.setCursor(0, 30);
    display.print("PM1.0: ");
    display.print(data.pm10_standard);
    display.println(" ug/m3");

    display.setCursor(0, 40);
    display.print("PM2.5: ");
    display.print(data.pm25_standard);
    display.println(" ug/m3");

    display.setCursor(0, 50);
    display.print("PM10 : ");
    display.print(data.pm100_standard);
    display.println(" ug/m3");
  } else {
    display.setCursor(0, 20);
    display.println("PM read error");
  }

  display.display();
  delay(1000);
}
