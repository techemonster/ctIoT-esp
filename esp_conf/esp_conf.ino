#include<ESP8266WiFi.h>
#include<ESP8266mDNS.h>
#include<WiFiUdp.h>
#include<ArduinoOTA.h>
#include<PubSubClient.h>
#include <Wire.h>
//#include <Adafruit_INA219.h>

#include "DHT.h"

#define DHTTYPE 11

/////change the variable to your Raspberry Pi IP address so it connects to your MQTT broker
const char* mqtt_server="192.168.0.83";  //rpi wire
//const char* mqtt_server1="192.168.0.86"; //rpi wifi
//const char* mqtt_server2="192.168.43.57"; //linga hotspot
//const char* mqtt_username="linga";
//const char* mqtt_password="b1211141bx3u";

//////Initializes the espClient
void callback(String topic, byte* payload, unsigned int length);
WiFiClient espClient;
PubSubClient client(mqtt_server,1883,espClient);

///////connect a bulb to each of GPIOS's of your ESP8266
const int pin16=16;
const int pin5=5;
const int pin4=4;
const int pin2=2;
const int pin14=14;
const int pin12=12;
const int pin13=13;
const int pin15=15;

///////DHT sensor
const int DHTpin=0;
float t;//to read temperature
float h;//to read humidity

//////Intialize DHT sensor
DHT dht(DHTpin,DHTTYPE);

//////Dont change the function below this function your esp to your router

    String network;
    ///////////set the  parameters according to the the AP settings
    const char* ssid="placeholder";//set this as your main SSID
    const char* password="placeholder";//set this as your main AP passkey
    int connect_retry_flag=0;//varibale to find out how many times esp tried to connect to wifi
    int connect_retry_limit=5;//set the retry count limit

    int connect_retry_flag1=0;
    int connect_retry_limit1=10;
    /////////////////////////////////////////////////////


    const char* mainnetworkSSID="Cosmic Tech";
    const char* mainnetworkPASS="$3nt!n3l";

    
    //////////set these parameters acording to cosmic Tech service AP settings////
    const char* cosmicServiceSSID="linga";
    const char* cosmicServicePASS="linga1114";
    //////////////////////////////////////////////////////


void setup_wifi()
{   connect_retry_flag=0;
    ssid = mainnetworkSSID;
    password = mainnetworkPASS;
    WiFi.mode(WIFI_OFF);    
    delay(2000);
    Serial.println();
    Serial.print("trying to connect main AP:");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid,password);
    while(WiFi.status() != WL_CONNECTED & connect_retry_flag!=connect_retry_limit ) //if the connection is not made and the connection failed 
    {   
              Serial.println("connection failed!");
              delay(2000);
              connect_retry_flag++; //Increment the connection retry flag
        
    }
   
    
    //Serial.println(connect_retry_flag);
    if(connect_retry_flag==connect_retry_limit & WiFi.status() != WL_CONNECTED)
    {  
      connect_retry_flag1=0;
        network = "cosmicnetwork";
        WiFi.mode(WIFI_OFF);
        delay(2000);
        ///if the connection coneection failed flag is  set at 5 login to the CosmicTech AP
        Serial.println("");
        ssid=cosmicServiceSSID;
        password=cosmicServicePASS;//change the parameters to cosmicService parameters
        Serial.print("trying to connect Cosmictech service AP:");
        Serial.println(ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid,password);
        while(WiFi.status()!=WL_CONNECTED)
        {
            Serial.println("Connection To CosmicTech service Ap failed Reconnecting");
            delay(2000);
            connect_retry_flag1++;
            if(connect_retry_flag1==connect_retry_limit1)
            {setup_wifi();
            }
            
        }
        Serial.println("connected to CosmicTech service AP");
        //Serial.println(WiFi.localIP());
    }
    else
    {
    ///////if it doesnt exceeds connect_retry_limit and connects to main AP
        Serial.print("connected to main AP: ");
        Serial.println(ssid);
        network="mainnetwork";
        //Serial.println(WiFi.localIP());
    }

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname((const char *)"ESP_ConferanceRoom");

    // No authentication by default
    ArduinoOTA.setPassword((const char *)"sentinel");
    // Password can be set with it's md5 value as well
    // MD5(OTA) = 21232f297a57a5a743894a0e4a801fc3;
    //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    
    ArduinoOTA.onStart([](){
        String type;
        if(ArduinoOTA.getCommand() == U_FLASH)
        {
            type="sketch";
        }
        else
        {
            type="filesystem";
        }
        
        //NOTE:if updating SPIFFS this would be place to unmount SPIFFS using SPIFFS.end()
        Serial.println("start updating "+type);
   
  });
  
  ArduinoOTA.onEnd([](){
        Serial.println("\nEnd");
        delay(5000);
        Serial.println("Rebooting once before next OTA");
        ESP.restart();
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
          Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
          Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
          Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
          Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
          Serial.println("End Failed");
        }
  });
  
  ArduinoOTA.begin();
  Serial.println("Ready for OTA update");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    
   
////////////DO NOT CHANGE ANY THING ABOVE THIS LINE/////////////////////////////////

}

///////////YOUR PROGRAM STARTS BELOW THIS LINE////////////////////////////////

//////this function is excecuted when some device publishes a message to a topic that your ESP8266 is subscribed to

//////change the function below to add logic to your program, so when a device publishes a message to a topic that

// your ESP8266 is subscribed you can actually do something

void callback(String topic,byte* payload,unsigned int length)
{     
      Serial.println();
      Serial.print("Message arrived on from the server:");
      Serial.print(topic);
      Serial.print(". Message : ");
      String msg;
      for(int i=0;i<length;i++)
      {
          msg+=(char)payload[i];
      }
      Serial.println(msg);      


///////to reset the esp after uploading the code with usb or with OTA
if(topic=="rst/esp/conf" && msg=="reset")
{
      Serial.println("resetting the esp/conf");
      ESP.restart();
}

///////for GPIO 16
if(topic=="hall/esp/gpio16")
{
      Serial.print("Changing GPIO 16 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin16,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio16","ON");

      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin16,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio16","OFF");
      }
}

///////for GPIO 5
if(topic=="hall/esp/gpio5")
{
      Serial.print("Changing GPIO 5 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin5,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio5","ON");
            
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin5,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio5","OFF");
      }
}

///////for GPIO 4
if(topic=="hall/esp/gpio4")
{
      Serial.print("Changing GPIO 4 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin4,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio4","ON");
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin4,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio4","OFF");
      }
}

///////for GPIO 2
if(topic=="hall/esp/gpio2")
{
      Serial.print("Changing GPIO 2 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin2,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio2","ON");
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin2,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio2","OFF");
      }
}

///////for GPIO 14
if(topic=="hall/esp/gpio14")
{
      Serial.print("Changing GPIO 14 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin14,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio14","ON");
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin14,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio14","OFF");
      }
}

///////for GPIO 12
if(topic=="hall/esp/gpio12")
{
      Serial.print("Changing GPIO 12 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin12,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio12","ON");
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin12,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio12","OFF");
      }
}

///////for GPIO 13
if(topic=="hall/esp/gpio13")
{
      Serial.print("Changing GPIO 13 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin13,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio13","ON");
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin13,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio13","OFF");
      }
}

///////for GPIO 15
if(topic=="hall/esp/gpio15")
{
      Serial.print("Changing GPIO 15 to : ");
      if(msg=="1" || msg=="ON")
      {
            digitalWrite(pin15,HIGH);
            Serial.print("ON");
            client.publish("office/esp/gpio15","ON");
      }
      else if(msg=="0" || msg=="OFF")
      {
            digitalWrite(pin15,LOW);
            Serial.print("LOW");
            client.publish("office/esp/gpio15","OFF");
      }
}
Serial.println();
}


// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect()
{
/////////Loop until we are connected
     while(!client.connected())
     {    if(client.connected())
                break;
          Serial.println("Attempting MQTT connection");
          client.connect("ESP8266Client");
          delay(3000);
          if(WiFi.status()!=WL_CONNECTED)
           {setup_wifi();}
     }
      ////attempt to connect
     
          if(client.connect("ESP8266Client"))
          {
                Serial.println("connected");
              
                ////Subscribe or resubscribe to a topic 
                client.subscribe("hall/esp/gpio16");
                client.subscribe("hall/esp/gpio5");
                client.subscribe("hall/esp/gpio4");
                client.subscribe("hall/esp/gpio2");
                client.subscribe("hall/esp/gpio14");
                client.subscribe("hall/esp/gpio12");
                client.subscribe("hall/esp/gpio13");
                client.subscribe("hall/esp/gpio15");
                client.subscribe("rst/esp/conf");
                client.publish("esp/signal","ready");
          }
          
          else
          {
                Serial.print("failed,rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
                ////wait 5 seconds before trying
                delay(5000);
          }  
     }


// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200

// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs

void setup() 
{
//// put your setup code here, to run once:
////set the pinmodes here
        Serial.begin(115200);
        setup_wifi();
        Serial.println(network);
        pinMode(16,OUTPUT);
        pinMode(5,OUTPUT);
        pinMode(4,OUTPUT);
        pinMode(2,OUTPUT);
        pinMode(14,OUTPUT);
        pinMode(12,OUTPUT);
        pinMode(13,OUTPUT);
        pinMode(15,OUTPUT);
/////DHT sensor input
        pinMode(0,INPUT);
        client.setServer(mqtt_server,1883);
        client.setCallback(callback);
        
}
///////// For this project, you don't need to change anything in the loop function.
//////// Basically it ensures that you ESP is connected to your broker



void loop()
{
 if(network == "cosmicnetwork" )
  {     
        ArduinoOTA.handle();
        if(WiFi.status()!=WL_CONNECTED)
        {setup_wifi();}
    
  }
    
 
 if(network == "mainnetwork")
  {
        if(WiFi.status()!=WL_CONNECTED)
        {setup_wifi();}
        
        
        if(!client.connected())
        {
        
          Serial.println("reconnect");
          reconnect();
        }
        
      if(!client.loop())
      {
            client.connect("ESP8266Client");
      }
      unsigned long int timenow=millis();
      unsigned long int prvstime=0;
      
      if(timenow-prvstime>=5000)
      {    prvstime=timenow;
           h=dht.readHumidity();
           t=dht.readTemperature();
           Serial.print("Hall Humidity : ");
           Serial.println(h);
           Serial.print("Hall Temperature : ");
           Serial.println(t);
           
        }
      
    /////Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t))
    {
          Serial.println("Failed to read from DHT sensor!");
          delay(5000);
          return;
    }

}
}
