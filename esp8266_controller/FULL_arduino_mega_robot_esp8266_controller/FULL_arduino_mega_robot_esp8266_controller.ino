// Copyright of the ESP8266WebServer library author: 
/* 
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//********** This is a place for libraries ************

#include <Arduino.h>
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//********** Variables ************

// Setup Wi-fi AccessPoint

  ESP8266WebServer server(80);
  #define WIFI 0
  #define AP 1

  const char* ssid = "AP";
  const char* pw = "Tomik1999";
  int max_speed = 99;


// ############### Functions for WWW ################

void set()
{
  // auto redirect not needed anymore root (/) page after a set request
  //String auto_redirect = "";//"<!DOCTYPE html><html><head><script language='JavaScript' type='text/JavaScript'>function redirect(){  window.location.href = '/'; }onload=redirect;</script></head><body>";
 
  String message = "Number of args received: "; // just for debug in case something goes wrong
  message += server.args(); //Get number of parameters
  message += "\n";  //Add a new line

  // in these variables  the information sent by client is stored
  int motor_speed, back_rotate, front_rotate;
  bool is_response_ok = false; // by deafult false; Is at least one of the parameters in the response from client? 
  
  for (int i = 0; i < server.args(); i++)
  {
    message += "Argument number " + (String)i + " : ";   //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter

    if(server.argName(i) == "speed")
    {
            motor_speed = server.arg(i).toInt();
            is_response_ok = true;
    }
    else if(server.argName(i) == "back_rotate")
    {
            back_rotate = server.arg(i).toInt();
            is_response_ok = true;
    }

    else if(server.argName(i) == "front_rotate")
    {
            front_rotate = server.arg(i).toInt();
            is_response_ok = true;
    }

            
    /*set?speed=21&back_rotate=8&front_rotate=12*/
  }
  if(is_response_ok == true)
  {
      send_serial(motor_speed, back_rotate, front_rotate);
      server.send(200, "text/html", message + "\nOK");       //Response to the HTTP request
  }
  else
  {
      send_serial(0, 0, 0);
      server.send(200, "text/html", "<h1>What the f***?</h1> <p>\nYou have to use at least one parameter of following: 'speed', 'back_rotate', 'front_rotate'. Does not matter on order.\nYou can send values apx -50 to 50, if you send higher value, the controller code will modifiy your number to fit to the range. \nThe request should look like: http://192.168.4.1/set?speed=21&back_rotate=8&front_rotate=12\n\nJust check what the f*** you sent me." + message + "</p></body></html> ");       //Response to the HTTP request

  }
}
// following functions are used when specific name is inserted to URL (like 192.168.4.1/stop)
void stop_me()
{
 server.send(200, "text/html", "stop");
  send_serial(0, 0, 0);
}
void forward_me()
{
 server.send(200, "text/html", "forward");
 //Serial.write("1"); //Write the serial data
 send_serial(50, 0, 0);

}
void backward_me()
{
 server.send(200, "text/html", "backward");
 //Serial.write("2"); //Write the serial data
 send_serial(-50, 0, 0);

}  
void left_me()
{
 server.send(200, "text/html", "left");
 //Serial.write("3"); //Write the serial data
 send_serial(30, -30, 30);

}
void right_me()
{
 server.send(200, "text/html", "right");
 //Serial.write("4"); //Write the serial data
 send_serial(30, 30, -30);

}

void root()
{
  server.send(200, "text/html", "root");
  //Serial.write("0"); //Write the serial data
  //send_serial(0, 0, 0);

}
void not_found() {
  String message = "Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

// ############ Send variables by serial ##############
// send obtained information by a serial line to arduino mega
void send_serial(int motor_speed, int back_rotate, int front_rotate)
{
  String send_string = "";
  send_string += "A:";
  if(motor_speed < -max_speed)
    send_string += -max_speed;
  else if(motor_speed > max_speed)
    send_string += max_speed;
  else send_string += String(motor_speed);


  send_string += "B:";

  if(back_rotate < -max_speed)
    send_string += -max_speed;
  else if(back_rotate > max_speed)
    send_string += max_speed;
  else send_string += String(back_rotate);

  send_string += "C:";


  if(front_rotate < -max_speed)
    send_string += -max_speed;
  else if(front_rotate > max_speed)
    send_string += max_speed;
  else send_string += String(front_rotate);


  Serial.println(send_string);
}

// ################## SETUP code is called first #################
void setup()
{
  
  //############# serial communication #############
  Serial.begin (115200);
   // Leonardo: wait for serial port to connect
  while (!Serial) {}


    //############# Wi-Fi Access point #############
    #if AP == 1 && WIFI == 0
      WiFi.softAPdisconnect();
      WiFi.disconnect();
      Serial.println("AP mode");
      Serial.println("\n\nConfiguring AP...\nSSID: ");
      Serial.print(ssid);
      Serial.print(" password: ");
      Serial.print(pw);
      WiFi.mode(WIFI_AP);
      // WiFi.softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4);
      WiFi.softAP(ssid, pw);
      WiFi.begin();
  
      IPAddress ip = WiFi.softAPIP();
      Serial.print("\nServer is running on: http://");
      Serial.println(ip);
    #endif 
    //############# Connect to a Wi-Fi#############
    #if AP == 0 && WIFI == 1
      WiFi.softAPdisconnect();
      WiFi.disconnect();
      Serial.println(" Wi-FI mode");
      Serial.print("\n\Connecting to Wi-FI ");
      Serial.print(ssid);
      Serial.print(" with password: ");
      Serial.print(pw);
      WiFi.mode ( WIFI_STA );
      WiFi.begin ( ssid, pw );
      Serial.println ( "" );
    
      // Wait for connection
      while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
      }
    
      Serial.println ( "" );
      Serial.print ( "Connected to " );
      Serial.println ( ssid );
      Serial.print("\nServer is running on: http://");
      Serial.println(WiFi.localIP());
    #endif 
    
    // here you can optionally add/modify reachable pages

    // server.on("/mypage", mypage_asdf());
    // This means, that when 192.168.1.10/mypage is typed, function (void) mypage_asdf() is called.

    server.on("/", root);
    // /stop, /forward, /backward. /right and /left are here for backward compatibility with android BASIC app
    server.on("/stop", stop_me);
    server.on("/forward", forward_me);
    server.on("/backward", backward_me);
    server.on("/right", right_me);
    server.on("/left", left_me);
    // This option is used by FULL android app
    server.on("/set", set);
    server.onNotFound ( not_found );


    server.begin();
}
// ############## LOOP is called to infinity ############## 
void loop()
{
    server.handleClient();
}
