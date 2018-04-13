#include <black_box_osv.h>
#include "Enes100.h"

/* Initialize osv pins
   * Parameters:
   *  int pin_right_dir1
   *  int pin_right_dir2
   *  int pin_left_dir1
   *  int pin_left_dir2
   *  int pin_pwm
   */
BlackBoxOSV osv(4,2,6,7,3);
int speed, counter;
boolean increase = true;

void setup() {
  Serial.begin(9600);
  osv.init();
  speed = 255;
  counter = 3; //loop body 3 times
}

void loop() {
  if (counter == 3) {
    delay(1000);
  }
  if (counter > 0) {
    osv.setRightMotorPWM(speed);
    osv.setLeftMotorPWM(-speed);
    Serial.print("Current PWM:");
    Serial.println(speed);
    delay(1500);
    osv.turnOffMotors();
    delay(3000);
    counter --;
  }
}
