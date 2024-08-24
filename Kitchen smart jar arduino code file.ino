#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED D0
const char* ssid = "JioFi2021";
const char* password =  "bnj70bnj70";
const char* mqtt_server = "broker.hivemq.com";

//String apiKey = "ILGWQIJ6LR1C08E7"; 
String apiKey = "IAC2HNVZN6JZX2GR";
const char *server = "api.thingspeak.com"; 

const int mqtt_port = 1883;
WiFiClient espClient;
WiFiClient client2;
PubSubClient client(espClient);

//Distance code defines pins numbers
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3
// defines variables
long duration;
int distance;
int grams;
int lookup_unit1 = 100;  // Meaning of 1 cm 
String myStr;
char buf[10];

void setup() 
{
  pinMode(LED, OUTPUT);
  pinMode(D1,INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Setup the sensor HCSRC 04
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    delay(1000);
}

void loop() 
{
  
   // Clears the trigPin for distance measurement
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   // Sets the trigPin on HIGH state for 10 micro seconds
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   // Reads the echoPin, returns the sound wave travel time in microseconds
   duration = pulseIn(echoPin, HIGH);
   // Calculating the distance
   distance= duration*0.034/2;
   // Prints the distance on the Serial Monitor
   Serial.print("Distance: ");
   //Serial.println(distance);
   distance = 14-distance;
   grams = distance * lookup_unit1;
   myStr = String(grams);
   Serial.println(grams);
   myStr.toCharArray(buf,myStr.length()+1);
   client.publish("myhome/kitchen/moogdaal", buf);
   
    if (client2.connect(server, 80)) // api.thingspeak.com
    {
     String postStr = apiKey;
     postStr += "&field1=";
     postStr += String(grams);
     postStr += "r\n";
     client2.print("POST /update HTTP/1.1\n");
     client2.print("Host: api.thingspeak.com\n");
     client2.print("Connection: close\n");
     client2.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
     client2.print("Content-Type: application/x-www-form-urlencoded\n");
     client2.print("Content-Length: ");
     client2.print(postStr.length());
     client2.print("\n\n");
     client2.print(postStr);
     client2.stop();
    }
    

   delay(10000);
   //Now check if MQTT still connected
   while (!client.connected()) 
   {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

} //end of Loop
