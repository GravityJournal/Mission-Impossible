#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000); // Wacht even tot de serial poort klaar is

  // Zet WiFi in Station-modus
  WiFi.mode(WIFI_STA);
  delay(100);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
}

// T