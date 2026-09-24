#include <esp_now.h>
#include <WiFi.h>

// Replace with the MAC address of the OTHER ESP32 board
uint8_t broadcastAddress[] = {
  0xF4, 0x65, 0x0B, 0x33, 0x22, 0xCC
};

// Define a data structure to send/receive
typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

struct_message myData;
struct_message incomingReadings;

// Callback when data is sent
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status: ");
  Serial.println(
    status == ESP_NOW_SEND_SUCCESS
      ? "Delivery Success"
      : "Delivery Fail"
  );
}

// Callback when data is received
void OnDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *incomingData,
  int len
) {
  // Make sure we don't copy more data than our structure can hold
  if (len != sizeof(incomingReadings)) {
    Serial.print("Unexpected data size: ");
    Serial.println(len);
    return;
  }

  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("String: ");
  Serial.println(incomingReadings.a);

  Serial.print("Int: ");
  Serial.println(incomingReadings.b);
}

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register send and receive callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer (the other ESP32)
  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW initialized");
}

void loop() {
  // Set values to send
  strcpy(myData.a, "Hello from ESP32!");
  myData.b = random(1, 100);

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(
    broadcastAddress,
    (uint8_t *)&myData,
    sizeof(myData)
  );

  if (result == ESP_OK) {
    Serial.println("Message sent");
  } else {
    Serial.print("Error sending message: ");
    Serial.println(result);
  }

  delay(5000);
}
