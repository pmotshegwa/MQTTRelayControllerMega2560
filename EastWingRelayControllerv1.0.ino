

#include <SPI.h>                  // For networking
#include <Ethernet.h>             // For networking
#include <PubSubClient.h>         // For MQTT
#include <Wire.h>                 // For temp sensor and OLED
#include <DallasTemperature.h>
#include <Ethernet.h>
#include <Adafruit_GFX.h>         // For OLED display
#include <Adafruit_SSD1306.h>     // For OLED display

/**
 * SDA - pin 20
 * SDL - pin 21
 */
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


/* MQTT Settings */
#define MQTT_SERVER_IP "192.168.2.204"
#define MQTT_SERVER_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_CLIENT_ID "EastWingRelayController"

const char statusTopic[]  = "home/events";    // MQTT topic to publish status reports

//MQTT topic for the outputs
const char commandTopic1[] = "home/east/relay01/power"; 
const char commandTopic2[] = "home/east/relay02/power"; 
const char commandTopic3[] = "home/east/relay03/power"; 
const char commandTopic4[] = "home/east/relay04/power"; 
const char commandTopic5[] = "home/east/relay05/power"; 
const char commandTopic6[] = "home/east/relay06/power"; 
const char commandTopic7[] = "home/east/relay07/power"; 
const char commandTopic8[] = "home/east/relay08/power"; 
const char commandTopic9[] = "home/east/relay09/power"; 
const char commandTopic10[] = "home/east/relay10/power"; 
const char commandTopic11[] = "home/east/relay11/power"; 
const char commandTopic12[] = "home/east/relay12/power"; 
const char commandTopic13[] = "home/east/relay13/power"; 
const char commandTopic14[] = "home/east/relay14/power"; 
const char commandTopic15[] = "home/east/relay15/power"; 
const char commandTopic16[] = "home/east/relay16/power"; 
const char temperatureTopic[] = "home/garage/temperature"; //MQTT topic to publish tempreture
const String relayStatusTopic = "home/east/relay"; //prefix MQTT topic to publish relay status 


const int SENSOR_PIN = 8; // Arduino pin connected to DS18B20 sensor's DQ pin
const int RELAY_PIN_NUM = 16;
const int RELAY_1_PIN = 38;
const int RELAY_2_PIN = 22;
const int RELAY_3_PIN = 40;
const int RELAY_4_PIN = 24;
const int RELAY_5_PIN = 42;
const int RELAY_6_PIN = 26;
const int RELAY_7_PIN = 44;
const int RELAY_8_PIN = 28;
const int RELAY_9_PIN = 46;
const int RELAY_10_PIN = 30;
const int RELAY_11_PIN = 48;
const int RELAY_12_PIN = 32;
const int RELAY_13_PIN = 47;
const int RELAY_14_PIN = 34;
const int RELAY_15_PIN = 53;
const int RELAY_16_PIN = 36;

char messageBuffer[100];
char topicBuffer[100];
//char cmdTopicBuffer[100]
char clientBuffer[50];
String tempString;
String ipAddress;
//short lastToggledRelay = 0;
//short previousToggledRelay = 0;
bool publishStatus = false;
String lastActivatedRelayStatus = "OFF";
String lastActivatedRelay = "00";

OneWire oneWire(SENSOR_PIN);         // setup a oneWire instance
DallasTemperature tempSensor(&oneWire); // pass oneWire to DallasTemperature library
IPAddress broker(192,168,2,204);        // MQTT broker


// Instantiate MQTT client
//PubSubClient client(broker, 1883, callback);
EthernetClient ethclient;
PubSubClient client(ethclient);

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
short relayPinsModule1[] = { 
  RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN,     
  RELAY_5_PIN, RELAY_6_PIN, RELAY_7_PIN, RELAY_8_PIN,     
  RELAY_9_PIN, RELAY_10_PIN, RELAY_11_PIN, RELAY_12_PIN,  
  RELAY_13_PIN, RELAY_14_PIN, RELAY_15_PIN, RELAY_16_PIN, 
};

bool relayStateModule1[RELAY_PIN_NUM];
/**
 * MQTT callback
 */
void callback(char* topic, char* data, unsigned int length)
{
  data[length] = '\0'; // Null terminator used to terminate the char array
  String message = String(data);
  String cmdTopic = String(topic);
  short digitalPin;
  //Serial.println(data);
  //Serial.println("topic: "+ cmdTopic);

  // expected topic format: home/east/relayXX/power
  // extractin XX e.g 01 or 15
  String strPin = cmdTopic.substring(15,17);
  //Serial.println("extract pin: "+ strPin);
  
  short relayPin = (int)strPin.toInt();
  if (relayPin == 0 || relayPin > RELAY_PIN_NUM){
    Serial.println("Invaid topic format received");
  }
  else if (message != "OFF" && message != "ON") 
  {
     Serial.println("Invaid message received: "+message);
  }
  else {
     digitalPin = relayPinsModule1[relayPin-1];
     if (message == "ON") {
        digitalWrite(digitalPin, LOW);
        relayStateModule1[relayPin-1] = true;
     }
     else if (message =="OFF"){
        digitalWrite(digitalPin, HIGH);
        relayStateModule1[relayPin-1] = false;
     }
    lastActivatedRelayStatus = message;
    lastActivatedRelay = strPin;
    //publishRelayStatus(data, strPin);
    publishStatus = true;

  }
}

/*
 * 
 */
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientString = "Reconnecting - " + String(Ethernet.localIP());
    clientString.toCharArray(clientBuffer, clientString.length()+1);
    
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected to MQTT");
      oled.println("Connected to MQTT");
      oled.display();
      delay(2000);
      
      // Once connected, publish an announcement...
      clientString.toCharArray(clientBuffer, clientString.length() + 1);
      client.publish(statusTopic, clientBuffer);
      // ... and subscribe
      client.subscribe(commandTopic1);
      client.subscribe(commandTopic2);
      client.subscribe(commandTopic3);
      client.subscribe(commandTopic4);
      client.subscribe(commandTopic5);
      client.subscribe(commandTopic6);
      client.subscribe(commandTopic7);
      client.subscribe(commandTopic8);
      client.subscribe(commandTopic9);
      client.subscribe(commandTopic10);
      client.subscribe(commandTopic11);
      client.subscribe(commandTopic12);
      client.subscribe(commandTopic13);
      client.subscribe(commandTopic14);
      client.subscribe(commandTopic15);
      client.subscribe(commandTopic16);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      oled.println("failed, rc=");
      oled.print(client.state());
      oled.println(" try again in 5 seconds");     
      oled.display();
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/**
 * Initial configuration
 */
void setup()
{
  Serial.begin(9600);  // Use the serial port to report back readings

  // initialize digital pin as an output.
  initRelayPins();
  
  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000);         // wait for initializing
  oled.clearDisplay(); // clear display
  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display

  tempSensor.begin();     // initialize the sensor
  tempString.reserve(10); // to avoid fragmenting memory when using String
  ipAddress.reserve(16);
  
  oled.println("   Motshegwa.com     ");
  oled.println(" Relay Controller  ");
  oled.println("Getting IP address ");
  oled.print  ("  ");
  oled.display();  
  // initialize the Ethernet shield using DHCP:
  Serial.println("Obtaining an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtaining an IP address");

    // check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
      Serial.println("Ethernet shield was not found");

    // check for Ethernet cable
    if (Ethernet.linkStatus() == LinkOFF)
      Serial.println("Ethernet cable is not connected.");

    while (true);
  }


  // print out Arduino's IP address, subnet mask, gateway's IP address, and DNS server's IP address
  Serial.print("- Arduino's IP address   : ");
  Serial.println(Ethernet.localIP());
  oled.clearDisplay(); // clear display
  oled.setCursor(0, 1);        // position to display
  oled.print("IP: ");
  oled.println(Ethernet.localIP());
  oled.display();
  
  /* Connect to MQTT broker */
  Serial.println("connecting to MQTT..");
  oled.print("connecting to MQTT..");
  oled.display();
  
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
  oled.display();
  delay(5000);
  Serial.println("Ready.");
}


void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  tempSensor.requestTemperatures();             // send the command to get temperatures
  float tempCelsius = tempSensor.getTempCByIndex(0);  // read temperature in Celsius
  tempString  = String(tempCelsius, 2); // two decimal places

  Serial.println(tempString); // print the temperature in Celsius to Serial Monitor
  oled.clearDisplay(); // clear display
  oled.setCursor(0, 1);        // position to display
  oled.print("IP: ");
  oled.println(Ethernet.localIP());
  oled.println();
  oled.print("Temp: ");
  oled.println(tempString);
  
  // Check if any relays have been activated since boot up
  if (lastActivatedRelay != "00") 
  {
    oled.println("Last activity:");
    oled.println("Relay "+lastActivatedRelay+" Powered "+lastActivatedRelayStatus);
  }
  publishTemperature(tempString);
  delay(500);

  if (publishStatus)
  {
    publishRelayStatus(lastActivatedRelayStatus, lastActivatedRelay);
    publishStatus = false;
  }
  oled.display();
  
  //rippleRelays();
}

/** 
 *  Publish the relay status
 *  Relay pin already zero padded e.g 01 instead of 1
 */
void publishRelayStatus(String currStatus, String str_relay_pin) 
{
  char statusBuffer[3];
  String topicString = relayStatusTopic + ""+str_relay_pin;
  topicString.toCharArray(topicBuffer, topicString.length()+1);
  currStatus.toCharArray(statusBuffer, currStatus.length() + 1);
  client.publish(topicBuffer, statusBuffer, true);
  Serial.println("publish topic: "+topicString+"message: "+currStatus);
}

// Publish current temp
void publishTemperature(String temp) 
{
  char tempBuffer[10];
  temp.toCharArray(tempBuffer, temp.length() + 1);
  client.publish(temperatureTopic, tempBuffer, true);
}
/*
 * Low level triggered relay
 */
void rippleRelays()
{
  oled.clearDisplay(); // clear display
  oled.setCursor(0, 0);        // position to display
    
  for (byte i = 0; i < RELAY_PIN_NUM; i++)
  {
    digitalWrite(relayPinsModule1[i], LOW);
    oled.print(String(i+1)+" ");
    oled.print("");
    oled.display();
    delay(10);
  }
  oled.println("");
  oled.println("All Relays ON");
  oled.display();
  delay(500);
  
  oled.clearDisplay(); // clear display
  oled.setCursor(0, 0);        // position to display
  for (byte i = 0; i < RELAY_PIN_NUM; i++)
  {
    digitalWrite(relayPinsModule1[i], HIGH);
    oled.print(String(i+1)+" ");
    oled.print("");
    oled.display();
    delay(10);
  }
   oled.println("");
   oled.println("All Relays OFF");
   oled.display();
   delay(500);
}
void initRelayPins()
{
  for (byte i = 0; i < RELAY_PIN_NUM; i++)
  {
    pinMode(relayPinsModule1[i], OUTPUT);
    digitalWrite(relayPinsModule1[i], HIGH);
    relayStateModule1[i] = false;
  }
}
