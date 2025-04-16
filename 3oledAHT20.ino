#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PM25AQI.h>
#include <Adafruit_AHTX0.h>
#include <SoftwareSerial.h>

// ====== OLED Display Setup ======
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ====== PMS Sensor Setup ======
SoftwareSerial pmSerial(16, 17); // RX=16 (sensor TX), TX=17 (not used)
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// ====== AHT Sensor Setup ======
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ===== OLED INIT =====
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 OLED init failed!"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // ===== PMS SENSOR INIT =====
  pmSerial.begin(9600);
  if (!aqi.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM2.5 sensor!");
    while (1) delay(10);
  }
  Serial.println("PM2.5 sensor initialized");

  // ===== AHT SENSOR INIT =====
  if (!aht.begin()) {
    Serial.println("Could not find AHT sensor! Check wiring.");
    while (1) delay(10);
  }
  Serial.println("AHT10/AHT20 sensor initialized");
}

void loop() {
  // ===== Read Temperature and Humidity =====
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  // ===== Read PM2.5 Data =====
  PM25_AQI_Data data;
  bool pm_ok = aqi.read(&data);

  // ===== Serial Output =====
  Serial.println(F("----------- Sensor Readings -----------"));
  Serial.print("Temperature: ");
  Serial.print(temp.temperature, 1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity.relative_humidity, 1);
  Serial.println(" %");

  if (pm_ok) {
    Serial.print("PM0.3: ");
    Serial.println(data.particles_03um);

    Serial.print("PM1.0 (std): ");
    Serial.print(data.pm10_standard);
    Serial.print(" | PM2.5 (std): ");
    Serial.print(data.pm25_standard);
    Serial.print(" | PM10 (std): ");
    Serial.println(data.pm100_standard);
  } else {
    Serial.println("PM sensor read error!");
  }

  // ===== OLED Display =====
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("T:");
  display.print(temp.temperature, 1);
  display.print((char)247); // Degree symbol
  display.print("C H:");
  display.print(humidity.relative_humidity, 1);
  display.println("%");

  if (pm_ok) {
    display.print("PM0.3:");
    display.println(data.particles_03um);

    display.print("PM1.0:");
    display.print(data.pm10_standard);
    display.print(" PM2.5:");
    display.println(data.pm25_standard);

    display.print("PM10:");
    display.println(data.pm100_standard);
  } else {
    display.println("PM Read Error");
  }

  display.display();
  delay(1000);
}
