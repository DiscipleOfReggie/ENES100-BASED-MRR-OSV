#ifndef BlackBoxOSV_h
#define BlackBoxOSV_h

#include <Arduino.h>

class BlackBoxOSV {
  private:
    int _pin_right_dir1, _pin_right_dir2, _pin_left_dir1, _pin_left_dir2, _pin_pwm,ir_Pin,
	_right_motor_pwm, _left_motor_pwm, _right_motor_dir, _left_motor_dir,_pin_trig_Pin,_pin_echo_Pin;
  public:
    BlackBoxOSV(int pin_right_dir1, int pin_right_dir2, int pin_left_dir1, int pin_left_dir2, int pin_pwm, int trigPin,
 int echoPin, int irPin);
    void init();
    void turnOffMotors();
    void setRightMotorPWM(int pwm);
    void setLeftMotorPWM(int pwm);
    void turnLeft(int pwm);
    void turnRight(int pwm);
    void drive(int pwm);
	void driveP(int pwm, int pause);
    bool  IRsignal();
	bool obstacle(double distance);
    double getDistance();
    int getLeftMotorPWM(){
      return _left_motor_pwm;
    };
    int getRightMotorPWM(){
      return _right_motor_pwm;
    };
};

#endif

