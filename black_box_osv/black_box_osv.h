#ifndef BlackBoxOSV_h
#define BlackBoxOSV_h

#include <Arduino.h>

class BlackBoxOSV {
  private:
    int _pin_right_dir1, _pin_right_dir2, _pin_left_dir1, _pin_left_dir2, _pin_pwm,
	_right_motor_pwm, _left_motor_pwm, _right_motor_dir, _left_motor_dir;
  public:
    BlackBoxOSV(int pin_right_dir1, int pin_right_dir2, int pin_left_dir1, int pin_left_dir2, int pin_pwm);
    void init();
    void turnOffMotors();
    void setRightMotorPWM(int pwm);
    void setLeftMotorPWM(int pwm);
    void turnLeft(int pwm);
    void turnRight(int pwm);
    void drive(int pwm);
    int getLeftMotorPWM(){
      return _left_motor_pwm;
    };
    int getRightMotorPWM(){
      return _right_motor_pwm;
    };
};

#endif

