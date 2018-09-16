
/*
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Streaming.h>
#include "DHT.h"

#define DHTPIN 14       // what pin we're connected to D5
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define R0 5            // Relay 0 D1
#define R1 4            // Relay 1 D2
#define R2 0            // Relay 2 D3
#define R3 2            // Relay 3 D4

#define SLEEP_DELAY_IN_SECONDS  10

//const char* ssid = "Nygaard";
//const char* password = "julelysbandit";
const char* ssid = "jodle65salig39masai";
const char* password = "7ea4f4120e1ee0226282f12639";

//const char* mqtt_server = "m20.cloudmqtt.com";
const char* mqtt_server = "192.168.87.108";
const char* mqtt_username = "ust03";
const char* mqtt_password = "Alfex999";
const char* mqtt_topic = "/Gasmads/Outdoor/03/json/";

const int AnalogIn  = A0;
int readingIn = 0;
float t_correct = 2.3;

//const int ESP_BUILTIN_LED = 2;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // setup serial port
  Serial.begin(115200);
  // setup WiFi
  setup_wifi();
  //client.setServer(mqtt_server, 17229);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();
  pinMode(R0, OUTPUT);    
  pinMode(R1, OUTPUT);    
  pinMode(R2, OUTPUT);    
  pinMode(R3, OUTPUT);    
  digitalWrite(R0, HIGH);
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);
  digitalWrite(R3, HIGH);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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

void callback(char* topic, byte* payload, unsigned int length) {
  char message_buff[100];
  int i = 0;
  int x = 0;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    message_buff[i] = payload[i];
    x++;
  }
  message_buff[x] = '\0';
  String msgString = String(message_buff);
  Serial.println("Payload: " + msgString);

  if (strstr(topic, "correction")){
     Serial.print("It's a correction: ");
     float ff = msgString.toFloat();
     t_correct = ff;
     Serial.println(t_correct);
  }
   switch (msgString[0]){
    case '0':     //Det er et alle kald
    if(msgString[2]=='1'){
      digitalWrite(R0, LOW);
      digitalWrite(R1, LOW);
      digitalWrite(R2, LOW);
      digitalWrite(R3, LOW);
    }
    else{
      digitalWrite(R0, HIGH);
      digitalWrite(R1, HIGH);
      digitalWrite(R2, HIGH);
      digitalWrite(R3, HIGH);
    }
    case '1':
    if(msgString[2]=='1'){
      digitalWrite(R0, LOW);
    }
    else{
      digitalWrite(R0, HIGH);
    }
    break;
    case '2':
    if(msgString[2]=='1'){
      digitalWrite(R1, LOW);
    }
    else{
      digitalWrite(R1, HIGH);
    }
    break;
    case '3':
    if(msgString[2]=='1'){
      digitalWrite(R2, LOW);
    }
    else{
      digitalWrite(R2, HIGH);
    }
    break;
    case '4':
    if(msgString[2]=='1'){
      digitalWrite(R3, LOW);
    }
    else{
      digitalWrite(R3, HIGH);
    }
    break;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Gasmads03", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/Gasmads/Outdoor/03/input/#");
      Serial.println("subscribe");
      //client.publish(mqtt_topic, "Så er vi igang");
    }
    else {
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
  char result[8]; // Buffer big enough for 7-character float 
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    //Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" correction: ");
  Serial.print(t_correct);
  Serial.print(" Ressult: ");
  t-=t_correct;
  Serial.print(t);
  Serial.print(" *C ");
  
  dtostrf(t, 6, 2, result); // Leave room for too large numbers!  
//  mqtt_topic = "/Gasmads/Outdoor/03/Temp/";
//  client.publish(mqtt_topic, result);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" *H ");
  dtostrf(h, 6, 2, result); // Leave room for too large numbers!  
//  mqtt_topic = "/Gasmads/Outdoor/03/Humid/";
//  client.publish(mqtt_topic, result);
 
  readingIn = analogRead(AnalogIn);    // read the input pin
  sprintf(result,"%d",readingIn);
  Serial.print("Analog value: ");
  Serial.println(readingIn);           // debug value
//  mqtt_topic = "/Gasmads/Outdoor/03/Analog/";
//  client.publish(mqtt_topic, result);


//  Convert data to JSON string 
  String json =
  "{"
  "\"humidity\": \"" + String(h) + "\","
  "\"temperature\": \"" + String(t) + "\","
  "\"analog\": \"" + String(readingIn) + "\","
  "\"relay_1\": \"" + digitalRead(R0) + "\","
  "\"relay_2\": \"" + digitalRead(R1) + "\","
  "\"relay_3\": \"" + digitalRead(R2) + "\","
  "\"relay_4\": \"" + digitalRead(R3) + "\"}";
// Convert JSON string to character array
// Serial.print("json length: ");
//  Serial.println(json.length()+1);
  char jsonChar[200];
  json.toCharArray(jsonChar, json.length()+1);
  // Publish JSON character array to MQTT topic
//  mqtt_topic = "/Gasmads/Outdoor/03/json/";

  if( client.publish(mqtt_topic,jsonChar)){
    Serial.println(json); 
  }
  else{
    Serial.println("Ikke Sendt");      
  }

  
    //client.disconnect();
    //WiFi.disconnect();
    //ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  //ESP.deepSleep(10 * 1000, WAKE_NO_RFCAL);
  //ESP.deepSleep(30e6); 
  delay(10000);   
}
