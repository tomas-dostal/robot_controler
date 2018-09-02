#include "math.h"
//#define VOLTS_PER_UNIT    .0049F        // (.0049 for 10 bit A-D) -> for IR sensor;
#include <Servo.h>     //zahrnutí knihovny pro ovládání servo motoru


// use following commands to receive serial comunication ftrought the terminal 
//stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
//screen /dev/ttyUSB0 9600
// and replace ttyUSB0 with device's location

// 0: trig, 1: echo
// ultrasonic -> sensor type 0
int ultrasonic_trig[] =   { 20, 22, 24, 26, 28, 30, 32, 34, 36, 38};
int ultrasonic_echo[] =   { 21, 23, 25, 27, 29, 31, 33, 35, 37, 39};
// infrared -> sensor type 1
int infrared[] =    { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};

Servo motor;         //každý motor má svou instanci třídy Servo
Servo back_rotate;         //každý motor má svou instanci třídy Servo
Servo front_rotate;         //každý motor má svou instanci třídy Servo


bool debug = true; // send to serial line also comments

int ultrasound_last[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int infrared_last[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned long start;
//##################################################


// setup exerything
void setup() 
{

  // setup serial line
  Serial.begin(115200);
  if(debug)
  {
    Serial.println("SETUP...");
    
  }
  // for comunication with ESP8266 module connected with rx tx 
  Serial1.begin(115200);


  // setup infrared ports
  
  // consider changing "i < 10" to  sizeof(infrared) / sizeof (int)
  for(int i = 0; i < (sizeof(infrared) / sizeof (int)); i+= 1)
  {
    //pinMode(infrared[i], INPUT);
  }
  // setup echo ports
  for(int i = 0; i < (sizeof(ultrasonic_echo) / sizeof (int)); i+= 1)
  {
    pinMode(ultrasonic_echo[i], INPUT);
  }
  // setup trig ports
  for(int i = 0; i < (sizeof(ultrasonic_trig) / sizeof (int)); i+= 1)
  {
    pinMode(ultrasonic_trig[i], OUTPUT);
    //digitalWrite(ultrasonic_trig[i], LOW);
  }
  // motors
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  // go forwards, backwards
  motor.attach(12);   
  back_rotate.attach(13); // back rotate servo
  front_rotate.attach(11); // fron rotate servo


  /*// check if servo is attached
  Serial.print("Servo 'motor' is ");
  Serial.println(motor.attached());
*/

  if(debug) Serial.println("Setup done!");
    

}

//##################################################

// loop - this part repats to infinity
// to simplify it I moved most of the content info functions

void loop()
{
  serial_control();
  
  //wasd_serial_control();  
  
  //ultra();
  //Serial.print("\n\nTimediff: ");
  //Serial.print(start - millis());
  //Serial.println("s. \n\n\n");
  /*
  for(int i = 0; i < 10; i+= 1)
  {
    // by the way these functions returns sensor's distance value. Measurment errors already filtred.
    // US Range: 2cm to ~4.5m
    
    float dist = ultrasound_getdistance(i); 
    
    if(dist < 150 && dist > 20)       // IR: from 20cm to 150cm 
    {
       //Serial.print(i);
       //Serial.print(": ");
       Serial.println(infrared_getdistance(i));
       //delay(1000);
       
    }
  }

  // control the car
  // 7,8,9,0
  int min_distance = 40;
  if(infrared_last[7] > min_distance && infrared_last[8] > min_distance && infrared_last[9] > min_distance && infrared_last[0] > min_distance)
  {
    motor.write(5);
    back_rotate.write(0);
    front_rotate.write(0);

  }

  
  // turn left if there is an obstacle 7,8,9,0
  if(infrared_last[7] <= min_distance && infrared_last[8] <= min_distance && infrared_last[9] <= min_distance && infrared_last[0] <= min_distance)
  {
   // go back
    infrared_getdistance(3);
    infrared_getdistance(4);
    delay(1000);
    
    if(infrared_last[3] > min_distance && infrared_last[4])
    {
      // go back 
      motor.write(-2);
      back_rotate.write(0);
      front_rotate.write(0);
      infrared_getdistance(3);
      infrared_getdistance(4);
      infrared_getdistance(1);
      infrared_getdistance(2);

   
      motor.write(-5);
      back_rotate.write(0);
      front_rotate.write(0);
      delay(500);
      

    }   
  */
 }



// check_value( byte sensor_index //index of sensor (0 to 9)
//              byte sensor_type  // 0 = ultrasound, 1 = infrared
//              int value // value returned by specific sensor 
bool check_value(byte sensor_index, byte sensor_type, int value) 
{
  if(sensor_type == 0) // ultrasound
  {
    // Measurement Range: 2cm to ~4.5m
    // Measurement Resolution: 0.3cm
    
    if(abs(ultrasound_last[sensor_index] - value) > 500)  // possibly invalid value
    {
      if(debug)
      {
        Serial.print("Ultrasound: ");
        Serial.print(sensor_index);
        Serial.print("; Possibly invalid sensor value received. Last value: ");
        Serial.print(infrared_last[sensor_index]);
        Serial.print(", current measured value: ");
        Serial.println(value);
      }
      return false;
      
    }
    else
    {
      return true;
    }
  }
  // Measurment from 20cm to 150cm 
  else if (sensor_type == 1) // infrared
  {
    if(abs(infrared_last[sensor_index] - value) > 130) // possibly invalid value
    {
      if(debug)
      {
        Serial.print("IR: ");
        Serial.print(sensor_index);
        Serial.print("; Possibly invalid sensor value received. Last value: ");
        Serial.print(infrared_last[sensor_index]);
        Serial.print(", current measured value: ");
        Serial.println(value);
      }
      return false;
    }
    else
    {
      return true;
    }
  }
  else // error
  {
    if(debug)
    {
      Serial.print("check_value(): sensor_type ");
      Serial.print(sensor_type);
      Serial.println(" is not defined. Use: 0: Ultrasonic, 1: infrared");
    }
  }
  return false;
}


//##################################################

// IR
// Used IR sensors are GP2Y0A02
// These sensors measures the distance from 20cm to 150cm 
// Average current consumption: 33 mA
// period: 38 ± 10 ms

// Official datasheeet: https://www.sparkfun.com/datasheets/Sensors/Infrared/gp2y0a02yk_e.pdf 
float infrared_getdistance(int sensor_index )
{
  int val = analogRead(infrared[sensor_index]);    // reads the value of the sharp sensor
  if(debug)
  {
    Serial.print("IR_");
    Serial.print(sensor_index);
    Serial.print(": ");
  }
  float distance = 9462/(val - 16.92);//12343.85 * pow((val),-1.15);

  //float v = analogRead(infrared[i])*0.0048828125;   
  //float va = 65*pow(v, -1.10); 
  //distance=val;  
  //distance=4800/(analogRead(val)-20);

  if(debug)
    Serial.println(distance);
  //Serial.print("; value: ");
  //Serial.print(val);       
  //float volts = (float)val*(float)(5.0/1024.0);
  //Serial.print("; ");
  //Serial.print(volts);
  //Serial.println("V");

  // check if distance is OK
  //ultrasonic -> sensor type 0 ; infrared -> sensor type 1; check_value(byte sensor_index, byte sensor_type, int value)
  if(check_value(sensor_index, 1, distance))
  {
     infrared_last[sensor_index] = distance;
     return distance;
  }
  else
  {
    return infrared_last[sensor_index];   
  } 
}
void set_speed(int a)  // speed has to be in range -80 to 80
{
  // to modify range
  if(a < 8 && a > 0)
  {
    a += 7;
  }
  else if(a < -80)
    a = -80;
  else if (a > 80)
    a = 80;
  // To fit in range (-90, 90) 
  a -= 90;
  a = a*(-1);
  motor.write((a)); //read int or parseFloat for ..float...
}
void set_front_rotation(int angle) // from -55 to -55
{
  if(angle < -55)
    angle = -55;
  else if (angle > 55)
    angle = 55;
  angle -= 90;
  angle = angle*(-1);
  front_rotate.write((angle));  
}
void set_back_rotation(int angle) // from -55 to -55
{
  if(angle < -55)
    angle = -55;
  else if (angle > 55)
    angle = 55;
  angle -= 90;
  angle = angle*(-1);
  back_rotate.write((angle));  
}
//##################################################
// Ultraosnic sensors used: SRF05
// These sensors has 5 pins: GND, VCC, ECHO, TRIG ... and the last one. 
// Trig and echo are used: 
// trig: ultrasonic_trig[] = {20, 22, 24, 26, 28, 30, 32, 34, 36, 38};
// echo: ultrasonic_echo[] = {21, 23, 25, 27, 29, 31, 33, 35, 37, 39};
// indexes if sensors are    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9}
// Measurement Range: 2cm to ~4.5m
// Measurement Resolution: 0.3cm
// Measurement Angle: up to 15 deg
// Measurement Rate: 40 Hz = 1 measurment per 25 milliseconds
// Supply Current: 10 to 40mA

int ultrasound_getdistance(int sensor_index)
{
  long duration;
  long distance;

  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(ultrasonic_trig[sensor_index], LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonic_trig[sensor_index], HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonic_trig[sensor_index], LOW);
  duration = pulseIn(ultrasonic_echo[sensor_index],HIGH);

  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  distance = duration / 29.1 / 2 ;
  if(debug)
  {
    Serial.print("US_");  // like UltraSonic
    Serial.print(sensor_index); // index of the sensor
    Serial.print(": ");
  }
  if (distance <= 0)
    if(debug)
      Serial.println("Out of range");
  else 
  {
    if(debug)
    {
      Serial.print(distance);  
      Serial.print("cm");
      Serial.print(";   ");
    }
  }

  // check if distance is OK
  //ultrasonic -> sensor type 0 ; infrared -> sensor type 1; check_value(byte sensor_index, byte sensor_type, int value)
  if(check_value(sensor_index, 0, distance))
  {
     ultrasound_last[sensor_index] = distance;
     return distance;
  }
  else
  {
    return ultrasound_last[sensor_index];
    
  } 
}

int ultra()
{
  long duration;
  long distance;
  long t = millis();

  for(int i = 0; i < 10; i++)
  {
      digitalWrite(ultrasonic_trig[i], LOW); 
  }
  Serial.print("Duration: "); 
  
  delayMicroseconds(2);
  for(int i = 0; i < 10; i++)
  {
      digitalWrite(ultrasonic_trig[i], HIGH); 
  }

  delayMicroseconds(10);
  for(int i = 0; i < 10; i++)
  {
    duration = pulseIn(ultrasonic_echo[i],HIGH);
    Serial.print("US_");  // like UltraSonic
    Serial.print(i); // index of the sensor
    Serial.print(": ");
    distance = duration / 29.1 / 2 ;
    if (distance <= 0)
      Serial.println("Out of range");
    else 
    {
        Serial.print(distance);  
        Serial.print("cm");
        Serial.print(";   ");
    }

    // check if distance is OK
    //ultrasonic -> sensor type 0 ; infrared -> sensor type 1; check_value(byte sensor_index, byte sensor_type, int value)
    if(check_value(i, 0, distance))
    {
       ultrasound_last[i] = distance;
    }
  }
  
}
 

void wasd_serial_control()
{
 char byte = 0;
  // press q to cancel and exit
  while (byte != 'p') {
    Serial.readBytes(&byte, 1);
    // press w to move forward
     if (byte == 's') {
       set_speed(0);
      set_front_rotation(0);
      set_back_rotation(0);
      
      Serial.print("stop\n");
      byte = 0;
    }
    else if (byte == 'w') {
       set_speed(50);
      set_front_rotation(0);
      set_back_rotation(0);
      
      Serial.print("move forward \n");
      byte = 0;
    }
    // press w to move forward-left
    else if (byte == 'q') {
       set_speed(30);
      set_front_rotation(-20);
      set_back_rotation(20);
      
      Serial.print("move forward-left \n");
      byte = 0;
    }
     // press w to move forward-right
    else if (byte == 'e') {
       set_speed(30);
      set_front_rotation(20);
      set_back_rotation(-20);
      
      Serial.print("move forward-right \n");
      byte = 0;
    }
    // press s to move backward
    else if (byte == 'x') {
       set_speed(-20);
       set_front_rotation(0);
       set_back_rotation(0);
      Serial.print("move back \n");
      byte = 0;
    }
     // press s to move backward
    else if (byte == 'y') {
       set_speed(-20);
       set_front_rotation(-20);
       set_back_rotation(20);
      Serial.print("move back left \n");
      byte = 0;
    }
    if (byte == 'c') {
       set_speed(-20);
       set_front_rotation(20);
       set_back_rotation(-20);
      Serial.print("move back right \n");
      byte = 0;
    }
    // press a to turn left
    else if (byte == 'a') {
      set_speed(20);

      set_front_rotation(-50);
      set_back_rotation(50);
      
      Serial.print("move left \n");
      byte = 0;
    }
    // press d to turn right
    else if (byte == 'd') {
       set_speed(20);

      set_front_rotation(50);
      set_back_rotation(-50);
      
      Serial.print("move right \n");
      byte = 0;
    }
    // press e to default stop
    if (byte == 'p' || byte == 's') 
    {
      set_speed(0);

      set_front_rotation(0);
      set_back_rotation(0);
      
      Serial.print("full stop \n");
      byte = 0;
    }  
}
}
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void serial_control()
{
 // read from port 1, send to port 0:
  if (Serial1.available()) 
  {
  String readString = "";
  while (Serial1.available()) {
  
    //delay(3);  //delay to allow buffer to fill 
    if (Serial1.available() >0) {
      char c = Serial1.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    } 
  }

  if (readString.length() >0) 
  {
     
      //Serial.println(readString); //see what was received
      
      // expect a string like 07002100 containing the two servo positions      
      String A = getValue(readString, ':', 1);
      String B = getValue(readString, ':', 2);
      String C = getValue(readString, ':', 3);

      /*Serial.print("A: ");
      Serial.print(A);  //print to serial monitor to see parsed results
      Serial.print("; B: ");
      Serial.print(B);
      Serial.print("; C: ");
      Serial.println(C);
      */
      int a = A.toInt();
      int b = B.toInt();
      int c = C.toInt();
      Serial.print("Set speed: ");
      Serial.print(a);
      Serial.print(" Set back rotation: "); 
      Serial.print(b);
      Serial.print(" Set front rotation: "); 
      Serial.println(c);
      
      set_speed(a);
      set_back_rotation(b);
      set_front_rotation(c);
      
    readString="";
  }
   /* switch(inByte)
    {

    case '0'  : // stop 
       Serial.println(" : stop") ;
       set_speed(0);
       set_front_rotation(0);
       set_back_rotation(0);
       break; 
    case '1'  : // forward 
       Serial.println(" : forward") ;
       set_speed(10);
       set_front_rotation(0);
       set_back_rotation(0);
      break;
    case '2'  : // backward 
      Serial.println(" : backward") ;
      set_speed(-10);
      set_front_rotation(0);
      set_back_rotation(0);
      break; 
     case '3'  : // left 
      Serial.println(" : left") ;
      set_speed(8);
      set_front_rotation(-50);
      set_back_rotation(50);
      break;
    case '4'  : // right 
      Serial.println(" : right") ;
      set_speed(8);
      set_front_rotation(50);
      set_back_rotation(-50);
      break; */
      
  
    }
 }

  

