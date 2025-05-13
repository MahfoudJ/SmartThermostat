#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "Verizon_V9ZM9M";
const char* password = "jump4-mail-bet";

const char* host = "192.168.1.189";  
const int port = 5201;  

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  if (client.connect(host, port)) {
    Serial.println("Connected to server");
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  if (client.connected()) {
    String data = "A";
    for (int i = 0; i < 1000; i++) {
      client.print(data);
    }
    delay(100);  
  }
}

