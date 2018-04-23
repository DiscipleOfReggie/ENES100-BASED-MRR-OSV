/* Sweep
  by BARRAGAN <http://barraganstudio.com>
  This example code is in the public domain.

  modified 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object
}

void loop() {


 //myservo.writeMicroseconds(800);                  // ClockWise/ Rise Arm 700-1499
 //delay(8000);

 
  myservo.writeMicroseconds(1500);                  // Stop 1500
  delay(100);
  myservo.writeMicroseconds(1600);                  // Counter Clockwise/ Drops Arm 1501-2300
  delay(5000);
  delay(100);



 
}
