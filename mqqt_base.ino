/******
Jada Mahfoud
EE494
******/

#include <WiFi.h> // esp WiFi connection lib
#include <PubSubClient.h> // MQTT lib
#include <Adafruit_AHTX0.h> // Temp sensor lib
#include <Wire.h> // I2C comms lib

// Constant Configs
const char* ssid = "Verizon_V9ZM9M"; // network ssid
const char* password = "jump4-mail-bet"; // network password
const char* mqtt_server = "192.168.1.154"; // Broker IP

WiFiClient espClient;// Wifi clinet object for esp
PubSubClient client(espClient);// Pubsub mqtt protocol for pi
char msg[50]; //buffer
long lastMsg = 0; // last msg timestamp
int value = 0; // msg val

//Sensor Declarations
Adafruit_AHTX0 aht; // sensor initialization
float temperatureVal = 0.0; // temp variable
float humidityVal =0.0; // humidity variable

void setup(){
  Serial.begin(115200); // Serial comms baud 

  if (!aht.begin()){ // initializes temp sensor
    Serial.println("Could not find a valid AHT sensor");
    while (1);
  }
  setup_wifi(); //calls wifi 
  client.setServer(mqtt_server, 1883); // MQTT broker address & port
  client.setCallback(callback); // callback function
}
void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { // wait until wifi connected
    delay(500); // 500ms
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); // prints esp local ip
  }

//this function will be executed whenever there is data available on subscribed topics
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect(){ // reconnects MQTT broker if lost
  while(!client.connected()){
    Serial.print("Attempting MQTT connection..."); 
    if (client.connect("Room1Client1")){ // collect with client id: Room1Client1
      Serial.println("connected");
      client.subscribe("esp1/output");
    }
    else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds");
      delay(5000); //5 second delay
    }
  }
}
void loop(){
  if (!client.connected()){
    reconnect(); // try to reconnect
  }
  client.loop();

  long now = millis(); // get time in ms
  if(now - lastMsg > 5000){
    lastMsg=now;
    sensors_event_t humidity, temp; // declare sensor event
    
   if (aht.getEvent(&humidity, &temp)){
    humidityVal = humidity.relative_humidity;
    temperatureVal = temp.temperature;

    char humidityString[8]; //Converts humidity to string
    dtostrf(humidityVal, 1, 2, humidityString);
    Serial.print("Humidity: ");
    Serial.println(humidityString);
    client.publish("esp1/humidity", humidityString); // Publish temp to esp32/humidity MQTT topic

    char tempString[8]; //Converts temp to string
    dtostrf(temperatureVal, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp1/temperature", tempString); // Publish temp to esp32/temperature MQTT topic
   }
  }
}