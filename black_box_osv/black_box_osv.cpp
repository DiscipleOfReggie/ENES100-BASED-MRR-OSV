/*
 * BlackBoxOSV.cpp - library for ENES100 Black Box mission OSV.
 * Designed for Section 0502, Team BASED-MRR.
 * Adapted from "dfr_tank" library.
 * Created by Michael Lum, April 2, 2018.
 */

#include "Arduino.h"
#include "black_box_osv.h"

BlackBoxOSV::BlackBoxOSV(int pin_right_dir1, int pin_right_dir2, int pin_left_dir1, int pin_left_dir2, int pin_pwm) {
  _pin_right_dir1 = pin_right_dir1;
  _pin_right_dir2 = pin_right_dir2;
  _pin_left_dir1 = pin_left_dir1;
  _pin_left_dir2 = pin_left_dir2;
  _pin_pwm = pin_pwm;
};

void BlackBoxOSV::init(){
  pinMode(_pin_right_dir1, OUTPUT);
  pinMode(_pin_right_dir2, OUTPUT);
  pinMode(_pin_left_dir1, OUTPUT);
  pinMode(_pin_left_dir2, OUTPUT);
  pinMode(_pin_pwm, OUTPUT);
  //pinMode(_pin_left_pwm, OUTPUT);
}

void BlackBoxOSV::setLeftMotorPWM(int pwm) {
  
  analogWrite (_pin_pwm, abs(pwm));      //PWM Speed Control
  
  if(pwm >= 0.0f) {
    digitalWrite(_pin_left_dir1,HIGH); 
    digitalWrite(_pin_left_dir2,LOW); 

  } 
  else {
    digitalWrite(_pin_left_dir1,LOW); 
    digitalWrite(_pin_left_dir2,HIGH);
  }
  //_left_motor_pwm = pwm;
};

void BlackBoxOSV::setRightMotorPWM(int pwm) {
  analogWrite (_pin_pwm, abs(pwm));      //PWM Speed Control
  
   if(pwm >= 0.0f) {
    digitalWrite(_pin_right_dir1,LOW); 
    digitalWrite(_pin_right_dir2,HIGH); 

  } 
  else {
    digitalWrite(_pin_right_dir1,HIGH); 
    digitalWrite(_pin_right_dir2,LOW);
  }

 // _right_motor_pwm = pwm;
};

void BlackBoxOSV::turnLeft(int pwm) {
  analogWrite (_pin_pwm , abs(pwm));      

   if(pwm >= 0.0f) {
   this->setRightMotorPWM(pwm);
   this->setLeftMotorPWM(-pwm);  
  } 
 // _right_motor_pwm = pwm;
 // _left_motor_pwm = pwm;
};

void BlackBoxOSV::turnRight(int pwm) {
  analogWrite (_pin_pwm, abs(pwm));      

   if(pwm >= 0.0f) {
   this->setRightMotorPWM(-pwm);
   this->setLeftMotorPWM(pwm);  
  } 
 // _right_motor_pwm = pwm;
 // _left_motor_pwm = pwm;
};
void BlackBoxOSV::drive(int pwm) {
  setRightMotorPWM(pwm);
  setLeftMotorPWM(pwm);
};
void BlackBoxOSV::turnOffMotors(){
  setRightMotorPWM(0);
  setLeftMotorPWM(0);
  _pin_pwm = 0;
  //_right_motor_pwm = 0;
  //_left_motor_pwm = 0;
};
