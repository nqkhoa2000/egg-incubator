#include <Arduino.h>
#include <WiFi.h> // tv wifi
#include <PubSubClient.h>  // thu vien kn MQTT
#include "Wire.h"
#include "DHT.h"

#define DHTPIN 27 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float Temp = 0, Hum = 0;
// Replace the next variables with your SSID/Password combination
const char* ssid = "Fuvitech";
const char* password = "fuvitech.vn";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "103.130.212.157";
const char* mqtt_server = "fuvitech.vn"; //khong sua

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup_wifi();
void reconnect();
void callback(char* topic, byte* message, unsigned int length);
void readsensor();
void setup() {
 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  Wire.begin();
  dht.begin();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("", "demo", "fuvitech.vn")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("anhdo/sn/ss001/");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    readsensor();
    Serial.println("da gui");
    client.publish("anhdo/sn/ss001/ss/1", String(Temp).c_str());// gưi dữ liệu nhiệt độ không khí lên MQTT
    client.publish("anhdo/sn/ss001/ss/6", String(Hum).c_str()); // gưi dữ liệu độ ẩm không khí lên MQTT
  }
}
void readsensor(){
  Hum = dht.readHumidity();
  Temp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  if (isnan(Hum) || isnan(Temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}
