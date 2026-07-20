#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "your-project-id-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YOUR_DATABASE_SECRET"

FirebaseData fbdata;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read sensor values
  int waterLevelPct = map(analogRead(34), 0, 4095, 0, 100);
  int rainDetectedPct = map(analogRead(35), 0, 4095, 0, 100);
  float tempValue = 28.5; // Example DHT sensor read
  
  // 1. Update live sensor nodes
  Firebase.setInt(fbdata, "SmartFloodSystem/Sensors/waterLevel", waterLevelPct);
  Firebase.setInt(fbdata, "SmartFloodSystem/Sensors/rainIntensity", rainDetectedPct);
  Firebase.setFloat(fbdata, "SmartFloodSystem/Sensors/temperature", tempValue);

  // 2. Broadcast active device availability
  String path = "SmartFloodSystem/Devices/ESP32-Ojo-Alpha";
  double currentTimestamp = millis(); // Swap with NTP Client epoch timestamp in production
  Firebase.setDouble(fbdata, path + "/lastSeen", currentTimestamp); 
  Firebase.setInt(fbdata, path + "/battery", 88);
  Firebase.setInt(fbdata, path + "/wifiSignal", WiFi.RSSI());
  Firebase.setString(fbdata, path + "/sensorStatus", "nominal");

  // 3. Optional: Write logging points on an interval to populate historical charts
  // String histPath = "SmartFloodSystem/History/" + String(currentTimestamp);
  // Firebase.setDouble(fbdata, histPath + "/timestamp", currentTimestamp);
  // Firebase.setInt(fbdata, histPath + "/waterLevel", waterLevelPct);
  // Firebase.setInt(fbdata, histPath + "/rainIntensity", rainDetectedPct);

  delay(5000); // 5-second broadcast interval
}