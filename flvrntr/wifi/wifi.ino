#include "WiFi.h" // ESP32 WiFi include
// #include "WiFiConfig.h" // My WiFi configuration.
#include <PubSubClient.h>
#include <ArduinoJson.h>


// CHANGE THIS FOR EACH BOX
const int boxNumber = 3;
const char* box_prefix = "BOX3";

// vars 
const char* SSID = "FLVRTWN";
const char* password = "dripcoffee";
const char* mqtt_server = "192.168.1.143";
const char* mqtt_username = "espresso";
const char* mqtt_password = "coffee";

const int pin = 0;
// const int numberOfPins = 16; 
// int pins[numberOfPins] = {15,2,4,16,17,5,18,19,13,12,14,27,26,25,33,32};
const int numberOfPins = 8; 
int pins[numberOfPins] = {15,4,18,21,13,14,26,33};




// extract the pins which move the pumps forward


// int oddPins[numberOfPins / 2]; // Assuming there's an even number of pins, so half will be odd-numbered pins
unsigned long lastUpdateTime = 0;
unsigned long activationTime[numberOfPins] = {0};
unsigned long activationDuration[numberOfPins] = {0}; // in milliseconds
bool isPinActive[numberOfPins] = {false};

WiFiClient espClient;
PubSubClient client(espClient);

void ConnectToWiFi()
{
 
  WiFi.mode(WIFI_STA);

  // THESE ARE THE SAME 
  WiFi.begin(SSID, password);

  Serial.print("Connecting to "); Serial.println(SSID);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}

void updatePinStates() {
  for(int i = 0; i < numberOfPins; i++) {
    if (isPinActive[i] && (millis() - activationTime[i] >= activationDuration[i])) {
      Serial.print("Turning off pin");
      Serial.print(i);
      digitalWrite(pins[i], LOW); // Turn off the pin
      isPinActive[i] = false; // Reset the pin's active flag
      int pinState = digitalRead(pins[i]);  
    }      
  }
}

void printActivePins() {
  for(int i = 0; i < numberOfPins; i++) {
    int pinState = digitalRead(pins[i]); 
    if(pinState == HIGH) {
        Serial.println("The pin is HIGH");
    } 

    
  }
}



void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Check if the topic matches
    if (strcmp(topic, box_prefix) == 0) {
        payload[length] = '\0'; // Null-terminate the payload
        
        // Parse the payload using ArduinoJSON library
        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload);

        int boxValue = doc["box"];
        
        if (boxValue == boxNumber) {
            int podValue = doc["pod"];
            if (podValue > 0 && podValue <= numberOfPins) {  // Validate the pod value
                float amountValue = doc["amount"];
                int pinIndex = podValue - 1; // Array is 0-indexed
                int targetPin = pins[pinIndex];
                activationTime[pinIndex] = millis();
                activationDuration[pinIndex] = amountValue * 1000; // Convert seconds to milliseconds
                isPinActive[pinIndex] = true;
                Serial.print("activating pin ");
                Serial.print(targetPin);
                Serial.print(activationDuration[pinIndex]);
                digitalWrite(targetPin, HIGH);
            } else {
                Serial.println("Invalid pod number received");
            }
        }
    }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(box_prefix);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Welcome To FLVRTOWN");
  for (int i = 0; i < numberOfPins; i++) {
    pinMode(pins[i], OUTPUT); 
    digitalWrite(pins[i], LOW);
  }
  ConnectToWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  updatePinStates();
  if (millis() - lastUpdateTime >= 10000) { 
    lastUpdateTime = millis(); // Update the last update time
    printActivePins();
  }
}

