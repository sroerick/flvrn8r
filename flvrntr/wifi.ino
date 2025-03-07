#include "WiFi.h" // ESP32 WiFi include
// #include "WiFiConfig.h" // My WiFi configuration.
#include <PubSubClient.h>
#include <ArduinoJson.h>



const char* SSID = "FLVRTWN";
const char* password = "dripcoffee";
const char* mqtt_server = "192.168.1.143";
const char* mqtt_username = "espresso";
const char* mqtt_password = "coffee";

const int pin = 0;
const int numberOfPins = 16; 
int pins[numberOfPins] = {15,2,4,16,17,5,18,19,13,12,14,27,26,25,33,32};


// extract the pins which move the pumps forward

int oddPins[numberOfPins / 2]; // Assuming there's an even number of pins, so half will be odd-numbered pins

unsigned long activationTime[numberOfPins] = {0};
unsigned long activationDuration[numberOfPins] = {0}; // in milliseconds
bool isPinActive[numberOfPins] = {false};


void setupOddPins() {
    int oddPinIndex = 0;
    for (int i = 0; i < numberOfPins; i+=2) {
        oddPins[oddPinIndex++] = pins[i];
    }
}




// use 21+22 for LCD 
// Modify this based on the box 
const char* box_prefix = "BOX3";

// LCD Code from Chat Gippity 
// #include <Wire.h>  // Built-in
// #include <LiquidCrystal_I2C.h>

// Initialize the LCD object with the I2C address (commonly 0x27 or 0x3F)
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// void setup(){
  // Initialize the LCD connected to pins SDA and SCL:
  // lcd.begin();
  // Print a message to the LCD.
  // lcd.setCursor(0, 0);  // Cursor at (column, row).
  // lcd.print("Hello, world!");
// }


// You can install the LiquidCrystal_I2C library from the Arduino IDE by going to Sketch -> Include Library -> Manage Libraries... and then searching for "LiquidCrystal_I2C".

// Summary
// You won't use UART0 or any other UART for this type of LCD. You'll use the I2C interface.
// Only two data pins (SDA, SCL) are needed, plus power (VCC, GND).
// Libraries like LiquidCrystal_I2C make it easy to send text to the display.








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
      digitalWrite(pins[i], LOW); // Turn off the pin
      isPinActive[i] = false; // Reset the pin's active flag
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
    if (strcmp(topic, "FLVR") == 0) {
        payload[length] = '\0'; // Null-terminate the payload
        
        // Parse the payload using ArduinoJSON library
        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload);

        int boxValue = doc["box"];
        
        if (boxValue == 1) {
            int podValue = doc["pod"];
            if (podValue > 0 && podValue <= numberOfPins) {  // Validate the pod value
                int targetPin = oddPins[podValue - 1];
                
                float amountValue = doc["amount"];
                int pinIndex = podValue - 1; // Array is 0-indexed
                activationTime[pinIndex] = millis();
                activationDuration[pinIndex] = amountValue * 1000; // Convert seconds to milliseconds
                isPinActive[pinIndex] = true;

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
      client.subscribe("BOX1");
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

}

