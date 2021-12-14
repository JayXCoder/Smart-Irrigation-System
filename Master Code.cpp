#define BLYNK_TEMPLATE_ID "TMPLi0iqxZuO"
#define BLYNK_DEVICE_NAME "Zati FYP"
#define BLYNK_AUTH_TOKEN "YG5ZjXwBPazD-1BC4JKRYCyDsWy1EKRv"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define DHT11PIN 25

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "z";
char pass[] = "zatisyahrin07";

// Initialize Telegram BOT
#define BOTtoken "2077278504:AAHuQV_11JXOtlBxZR8kUtO5L3bFc7NSNbM"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "362701334"
  
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//DHT11 Blynk
DHT dht(DHT11PIN, DHT11);

// define the GPIO connected with Relays and switches  
#define RelayPin1 15 //D15
#define RelayPin2 2 //D2
#define RelayPin3 4 //D4

//Water Level Declaration
#define Water_POWER_PIN  32 //D32
#define Water_SIGNAL_PIN A7 //A7
int WaterLevel = 0; // variable to store the sensor value

int SoilSensor = A0;
int Soilsensorvalue = 0;
int Soiloutputvalue = 0;


#define VPIN_BUTTON_1  V1
#define VPIN_BUTTON_2  V2
#define VPIN_BUTTON_3  V3
#define VPIN_BUTTON_4  V8

int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 0; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 0; //Define integer to remember the toggle state for relay 3

int automationSwitch = 0; //Define for Automation Toggle Control

BlynkTimer timer;

BLYNK_CONNECTED() 
{  
    // Request the latest state from the server  
    Blynk.syncVirtual(VPIN_BUTTON_1);
    Blynk.syncVirtual(VPIN_BUTTON_2);
    Blynk.syncVirtual(VPIN_BUTTON_3);
    Blynk.syncVirtual(VPIN_BUTTON_4);
}

// When App button is pushed - switch the state  
BLYNK_WRITE(VPIN_BUTTON_1) 
{
  toggleState_1 = param.asInt();  
  digitalWrite(RelayPin1, toggleState_1);  
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, toggleState_2);
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, toggleState_3);
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  automationSwitch = param.asInt();
}

void automation()
{
  //Soil Moisture Sensor Read And Print
  Soilsensorvalue=analogRead(SoilSensor);
  Soiloutputvalue=map(Soilsensorvalue, 4095, 2950, 0, 100); 

  Serial.print("Soil Moiture Level:");
  Serial.print(Soiloutputvalue);

  if (Soiloutputvalue1>65)
  {
    delay(2000);

    digitalWrite(RelayPin1, LOW); // turn Off relay 1
    toggleState_1 = 0;
    Serial.println("Pump OFF");
    bot.sendMessage(CHAT_ID, "Pump 1 is Turning Off", "");
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);   // Update Button Widget
  }

  else if (Soiloutputvalue1<25)
  {
    delay(2000);

    digitalWrite(RelayPin1, HIGH); // turn On relay 1
    toggleState_1 = 1;
    Serial.println("Pump ON");
    bot.sendMessage(CHAT_ID, "Pump 1 is turning ON", "");
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);   // Update Button Widget
  }

  delay(1000);
  Blynk.virtualWrite(V6,Soiloutputvalue);
}

void manual()
{
  //Soil Moisture Sensor Read And Print
  Soilsensorvalue=analogRead(SoilSensor);
  Soiloutputvalue=map(Soilsensorvalue, 4095, 2950, 0, 100); 

  Serial.print("Soil Moiture Level:");
  Serial.print(Soiloutputvalue);

  delay(1000);
  Blynk.virtualWrite(V6,Soiloutputvalue);
}

void setup() 
{
  Serial.begin(115200); /* Define baud rate for serial communication */

  dht.begin();

  //Relay Pin
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);

  //During Starting all Relays should TURN OFF  
  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);
  digitalWrite(RelayPin3, toggleState_3);

  //Water Level Sensor
  pinMode(Water_POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(Water_POWER_PIN, LOW); // turn the sensor OFF

  Blynk.begin(auth, ssid, pass);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop() 
{
  //This works for toggle automation and manual control for the relay module
  if(automationSwitch == 1)
  {
    automation();
  }
  else if(automationSwitch == 0)
  {
    manual();
  }


  //Water Level Sensor Read and Print
  digitalWrite(Water_POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  WaterLevel = analogRead(Water_SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(Water_POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Water Level value: ");
  Serial.println(WaterLevel);

  delay(1000);

  Blynk.virtualWrite(V7,WaterLevel);

  //To Update Serial Print With Latest Pump State
  if(toggleState_1 == 1)
  {     
    Serial.println("Pump ON");
  }

  else if(toggleState_1 == 0)
  {   
    Serial.println("Pump OFF");  
  }  
  delay(500);

  //DHT11 Sensor Read And Print
  Serial.println("Humidity (%)\tTemperature (C)");
  float humidity = dht.readHumidity();/* Get humidity value */
  float temperature = dht.readTemperature();/* Get temperature value */
  Serial.print(humidity);
  Serial.print("\t");
  Serial.print(temperature);
  Serial.print("ºC ");
  delay(1000);


  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, humidity);
  Blynk.virtualWrite(V5, temperature);


  //Blynk WiFi Status Update
  if (WiFi.status() != WL_CONNECTED)  
  {  
    Serial.println("WiFi Not Connected");  
  }  
  else  
  {  
    Serial.println("WiFi Connected");  
  }  
  Blynk.run();
}
