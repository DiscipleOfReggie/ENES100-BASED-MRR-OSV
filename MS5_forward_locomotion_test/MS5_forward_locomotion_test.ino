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
BlackBoxOSV osv(2,4,6,7,3);
int speed = 100;


void setup() {
  Serial.begin(9600);
  osv.init();
}

void loop() {
  osv.setRightMotorPWM(speed);
  osv.setLeftMotorPWM(speed);
  Serial.print("Current PWM:");
  Serial.println(speed);
  delay(50);
}
