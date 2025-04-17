/* Google Form Link Format:
 * https://docs.google.com/forms/d/e/YOURLINK/
 * formResponse?usp=pp_url&
 * entry.2128414819=suhu&entry.671850365=rh&entry.713531188=pm1&entry.318084391=pm3&entry.2115180752=pm25&entry.1088528795=pm10
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecure.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
#else
  #error "This code is intended to run on ESP8266 or ESP32 platform!"
#endif

// WiFi credentials
#define WIFI_SSID     "username"
#define WIFI_PASSWORD "password"

// Google Form POST URL
#define FORM_URL "https://docs.google.com/forms/d/e/YOURLINK/formResponse"

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Simulated sensor variables
float temperature = 0.0;
float humidity = 0.0;
uint16_t pm03 = 0, pm10 = 0, pm25 = 0, pm100 = 0;

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  display.display();

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi connected.");
  display.display();

  // Seed RNG
  randomSeed(micros());
}

void loop() {
  // Simulate sensor values
  temperature = random(250, 400) / 10.0; // 25.0 - 40.0 °C
  humidity    = random(300, 900) / 10.0; // 30.0 - 90.0 %
  pm03        = random(0, 300);
  pm10        = random(0, 100);
  pm25        = random(0, 150);
  pm100       = random(0, 200);

  // Display to Serial Monitor
  Serial.println("========== SIMULATED DATA ==========");
  Serial.printf("Temperature: %.1f °C\n", temperature);
  Serial.printf("Humidity   : %.1f %%\n", humidity);
  Serial.printf("PM0.3      : %u #/0.1L\n", pm03);
  Serial.printf("PM1.0      : %u µg/m³\n", pm10);
  Serial.printf("PM2.5      : %u µg/m³\n", pm25);
  Serial.printf("PM10       : %u µg/m³\n", pm100);
  Serial.println("====================================");

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Temp : %.1f C\n", temperature);
  display.printf("Humidity: %.1f %%\n", humidity);
  display.printf("PM0.3: %u\n", pm03);
  display.printf("PM1.0: %u\n", pm10);
  display.printf("PM2.5: %u\n", pm25);
  display.printf("PM10 : %u\n", pm100);
  display.display();

  // Send data to Google Form
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, FORM_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "entry.2128414819=" + String(temperature, 1) +
                      "&entry.671850365=" + String(humidity, 1) +
                      "&entry.713531188=" + String(pm03) +
                      "&entry.318084391=" + String(pm10) +
                      "&entry.2115180752=" + String(pm25) +
                      "&entry.1088528795=" + String(pm100);

    int responseCode = http.POST(postData);

    if (responseCode == 200) {
      Serial.println("Google Form: Data submitted successfully!");
    } else {
      Serial.print("Google Form Error: ");
      Serial.println(responseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Skipping data submission.");
  }

  delay(500); // Delay between posts
}
