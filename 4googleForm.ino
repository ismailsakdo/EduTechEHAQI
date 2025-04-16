
#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "XXXX"
#define WIFI_PASSWORD "XXXX"

// Google Form URL for submitting responses
#define FORM_URL "https://docs.google.com/forms/d/e/XXXXOihBtbKA/formResponse?usp=pp_url&"

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  delay(10000); // Wait for 10 seconds

  // Generate dummy temperature and humidity values
  float temperature = random(20, 60);
  float humidity = random(30, 90);

  // Construct the query parameters with dummy sensor data
  String postData = "entry.352215316=" + String(temperature) + "&entry.629053361=" + String(humidity);

  // Print the complete URL link before submitting
  Serial.print("Sending data to URL: ");
  Serial.println(FORM_URL);

  // Send HTTP POST request to submit data to Google Form
  HTTPClient http;
  http.begin(FORM_URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(postData);
  if (httpResponseCode == 200) {
    Serial.println("Data submitted successfully!");
  } else {
    Serial.print("Error during HTTP request. Response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
