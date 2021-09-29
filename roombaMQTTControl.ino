
#include "roombaDefines.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>
//
//int sensors[] = {22};//{7, 8, 21, 22, 23, 24, 25, 26, 35, 46, 47, 48, 49, 50, 51};
//int sensorCount = sizeof(sensors);

const char *ssid = "Robot";               //robot creates wifi hotspot when wifi connection is not configured
const char *outTopic = "tank/out";        //MQTT topic for robot telemetry messages
const char *inTopic = "tank/in";          //MQTT topic for control messages
const char *mqtt_server = "192.168.0.19"; //my defauld MQTT server
const char *mqtt_server1 = "test.mosquitto.org";
const char *streamTopic = "tank/stream";
const char *debug = "tank/debug";

char buffer1[20]; //multiusage
WiFiClient espClient;
PubSubClient client(espClient); //MQTT

void setup()
{
  pinMode(D5, OUTPUT);
  Serial.begin(115200);

  wakeUp();
  startFull();
  playSound(2);
  stop();
  delay(2000);

  startSafe();
  // playSound(2);

  setDebrisLED(ON);
  writeLEDs('W', 'I', 'F', 'I');

  WiFiManager wifiManager;
  wifiManager.autoConnect("Robot");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //OTA
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  //OTA END

  ////////////////////

  // playSound(3);
  setPowerLED(255, 255);
  setDebrisLED(ON);
  setDockLED(OFF);
  setSpotLED(OFF);
  setWarningLED(OFF);
  writeLEDs('R', 'E', 'D', 'Y');
}

void loop()
{
  ArduinoOTA.handle();

  //MQTT
  if (!client.connected())
    reconnect();
  else
    client.loop();

  //send telemetry every 200ms
  if (millis() % 1000 == 0)
  {
    int voltage = 0; //getSensorData(22);
    sprintf(buffer1, "T;%d;RSSI=%d;", millis() / 1000, WiFi.RSSI());
    client.publish(outTopic, buffer1);
  }

  int b = Serial.available();
  if (b > 170)
  {
    byte buffer[b];
    Serial.readBytes(buffer, b);
    client.publish(streamTopic, buffer, b);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  //control is done by 6 byte messages : [$][len][ch1][ch2][ch3][ch4]
  // [$] - start of the frame
  // [len] - number of bytes to read. in this case always 4
  // [ch1][ch2][ch3][ch4] - control channels. Each channel has a range from 0 to 200. Middle is at 100 - servo stop
  if (length > 0)
  {
    if ((char)payload[0] == '$')
    {

      int len = payload[1];
      int ch1 = payload[2] - 100;
      int ch2 = payload[3] - 100;
      int ch3 = payload[4] - 100;
      int ch4 = payload[5] - 100;

      //chanel mixer
      int m1 = ch2 - ch1;
      int m2 = ch2 + ch1;
      if (m1 > 100)
        m1 = 100;
      if (m1 < -100)
        m1 = -100;
      if (m2 > 100)
        m2 = 100;
      if (m2 < -100)
        m2 = -100;

      m1 = map(m1, -100, 100, -500, 500);
      m2 = map(m2, -100, 100, -500, 500);
      driveWheels(m2, m1);

      int m3 = map(ch3, -100, 100, -100, 100);
      int m4 = map(ch4, -100, 100, -100, 100);

      switch (ch3)
      {
        case 1:
          stop();
          //powerOff();
          break;

        case 2:
          wakeUp();
          startFull();
          // playSound(2);
          setPowerLED(255, 255);
          setDebrisLED(ON);
          setDockLED(OFF);
          setSpotLED(OFF);
          setWarningLED(OFF);
          writeLEDs('R', 'E', 'D', 'Y');
          break;

        case 3:
          seekDock();
          break;

        case 4:
          //undock
          wakeUp();
          clean();
          break;

        case 5:
          powerOff();
          break;

        case 10:
          enableBrushes(true, false, true, true, true);
          break;

        case 11:
          enableBrushes(true, true, false, false, false);
          break;

        case 12:
          writeLEDs('-', 'G', 'O', '-');
          clean();
          break;
        case 20:
          //start stream
          Serial.write(148);
          Serial.write(18); //get one sensor

          Serial.write(7); //BUMPS wheeldops 1
          Serial.write(8); //wall 1
          Serial.write(21);//charging state 1
          Serial.write(22); //voltage  2
          Serial.write(23); //current  2
          Serial.write(24); //temperature 1
          Serial.write(25); //battery charge 2
          Serial.write(26); //battery capacity 2
          Serial.write(28);//cliff left
          Serial.write(29);//cliff front left
          Serial.write(30);//cliff fromt right
          Serial.write(31);//cliff right signal
          Serial.write(46);//light bump left
          Serial.write(47);//frontleft
          Serial.write(48);//center left
          Serial.write(49);//center right
          Serial.write(50);//front right
          Serial.write(51);//right
          break;


        case 21:
          //pause stream
          Serial.write(150);//set stream state
          Serial.write(0); //pause stream
          break;
      }
    }
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    // Attempt to connect
    if (client.connect(ssid))
    {
      // playSound(1);
      client.publish(outTopic, "READY");
      client.subscribe(inTopic);
    }
    else
    {
      // Serial.print(client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
