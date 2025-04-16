#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_AHTX0 aht;

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin not used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Clear the buffer and display startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  delay(1000);

  // Initialize AHT sensor
  if (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("AHT Sensor Error!");
    display.display();
    while (1) delay(10);
  }
  
  Serial.println("AHT10 or AHT20 found");
  display.clearDisplay();
  display.println("AHT Sensor Found");
  display.display();
  delay(1000);
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  
  // Print to serial monitor
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  // Display on OLED
  display.clearDisplay();
  
  // Display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature:");
  
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(temp.temperature);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167); // degree symbol
  display.setTextSize(2);
  display.print("C");

  // Display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity:");
  
  display.setTextSize(2);
  display.setCursor(0,45);
  display.print(humidity.relative_humidity);
  display.print(" %");
  
  display.display();

  delay(2000); // Update every 2 seconds
}
