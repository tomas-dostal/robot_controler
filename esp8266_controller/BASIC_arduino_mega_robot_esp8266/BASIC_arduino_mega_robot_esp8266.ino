// This code is a modification from "Mert Arduino and Tech" - (a YouTube channel)
// see source: 

// This code is COMPATIBILE ONLY WITH "BASIC_robot_controller.apk" app, so you can not use "FULL" version
// To use this code you need to connect to an existing wi-fi network.

#include <ESP8266WiFi.h>

// define stuff connected with Wi-fi server 
WiFiClient client;
WiFiServer server(80);
IPAddress ip(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1); // set gateway to match your (existing) network


// Select AP you want to connect to (you have to use existing wi-fi AP)
const char* ssid = "AP_to_connect";
const char* password = "password";

// data received from androud application
// note: You need to be connected to a wi-fi

String  data =""; 

void setup()
{
  // start server communication 
  server.begin();
  Serial.begin(115200);
}

void loop()
{
    /* If the server available, run the "checkClient" function */  
    client = server.available();
    if (!client) return; 
    data = checkClient ();

/************************ Run function according to incoming data from application *************************/

    /* If the incoming data is "forward", run the "MotorForward" function */
    if (data == "forward") MotorForward();
    /* If the incoming data is "backward", run the "MotorBackward" function */
    else if (data == "backward") MotorBackward();
    /* If the incoming data is "left", run the "TurnLeft" function */
    else if (data == "left") TurnLeft();
    /* If the incoming data is "right", run the "TurnRight" function */
    else if (data == "right") TurnRight();
    /* If the incoming data is "stop", run the "MotorStop" function */
    else if (data == "stop") MotorStop();
} 

/********************************************* FORWARD *****************************************************/
void MotorForward(void)   
{
  Serial.println("forward");
}

/********************************************* BACKWARD *****************************************************/
void MotorBackward(void)   
{
  Serial.println("Backward");
}

/********************************************* TURN LEFT *****************************************************/
void TurnLeft(void)   
{
  Serial.println("Left");
}

/********************************************* TURN RIGHT *****************************************************/
void TurnRight(void)   
{
  Serial.println("Right");
}

/********************************************* STOP *****************************************************/
void MotorStop(void)   
{
  Serial.println("Stop");
}

/********************************** RECEIVE DATA FROM the BASIC APP ******************************************/
String checkClient (void)
{
  //Serial.println(server.client);
  while(!client.available()) delay(1); 
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}
