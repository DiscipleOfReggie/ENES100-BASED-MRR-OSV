/*
 * BlackBoxOSV.cpp - library for ENES100 Black Box mission OSV.
 * Designed for Section 0502, Team BASED-MRR.
 * Adapted from "dfr_tank" library.
 * Created by Michael Lum, April 2, 2018.
 */

#include "Arduino.h"
#include "black_box_osv.h"

BlackBoxOSV::BlackBoxOSV(int pin_right_dir1, int pin_right_dir2, int pin_left_dir1, int pin_left_dir2, int pin_pwm, int trigPin, int echoPin, int irPin) {
  _pin_right_dir1 = pin_right_dir1;
  _pin_right_dir2 = pin_right_dir2;
  _pin_left_dir1 = pin_left_dir1;
  _pin_left_dir2 = pin_left_dir2;
  _pin_pwm = pin_pwm;
  _pin_trig_Pin = trigPin;
  _pin_echo_Pin = echoPin;
  ir_Pin = irPin;

};

void BlackBoxOSV::init(){
  pinMode(_pin_right_dir1, OUTPUT);
  pinMode(_pin_right_dir2, OUTPUT);
  pinMode(_pin_left_dir1, OUTPUT);
  pinMode(_pin_left_dir2, OUTPUT);
  pinMode(_pin_pwm, OUTPUT);
  //pinMode(_pin_left_pwm, OUTPUT);

//Pins for the Ultrasonic sensor
 pinMode(_pin_trig_Pin , OUTPUT); 
 pinMode(_pin_echo_Pin , INPUT); 

//Pins for the Ir sensor
pinMode(ir_Pin, INPUT);
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

void BlackBoxOSV::turnOffMotors(){
  setRightMotorPWM(0);
  setLeftMotorPWM(0);
  _pin_pwm = 0;
  //_right_motor_pwm = 0;
  //_left_motor_pwm = 0;
};
//Ultrasonic sensor checking/ returns distance is cm
double BlackBoxOSV::getDistance(){
  digitalWrite(_pin_trig_Pin , LOW);
  delayMicroseconds(2);
  digitalWrite(_pin_trig_Pin , HIGH);
  delayMicroseconds(10);
  digitalWrite(_pin_trig_Pin , LOW);

  double duration = pulseIn(_pin_echo_Pin , HIGH);
  //Convert distance to cm
  double distance = (duration*.0343)/2; 
  return distance;
};
bool BlackBoxOSV::IRsignal() {
int val = digitalRead(ir_Pin);
if(val==0){
return true;
}else
return false;

};
void BlackBoxOSV::drive(int pwm) {
  setRightMotorPWM(pwm);
  setLeftMotorPWM(pwm);
};

