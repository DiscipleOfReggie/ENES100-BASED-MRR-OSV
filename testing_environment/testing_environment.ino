#include <black_box_osv.h>
#include "Enes100.h"

enum phase {
  phase_0, // Exit the landing zone
  phase_1, // Navigate to center of mission area
  phase_2, // Search for Black Box
  phase_3, // Navigate to Black Box
  phase_4, // Transmit Black Box coordinates
  phase_5, // Secure Black Box
  phase_6  // RTB
};

const double landing_zone_x = 1.375, thresh = .2, angle_thresh = .2, obstacle_distance = 23.5;

/* Initialize osv pins
     Parameters:
      int pin_right_dir1,
      int pin_right_dir2,
      int pin_left_dir1,
      int pin_left_dir2,
      int pin_pwm,
      int trigPin,
      int echoPin
      int irPin
*/
BlackBoxOSV osv(2, 4, 6, 7, 3, 10, 11, 13);
Enes100 enes("BASED-MRR", BLACK_BOX, 7, 8, 9);
Coordinate landing_coordinate, black_box_coordinate, mission_center(0,0,0);
phase cur_phase;
int power;
bool landing_stored, arrived;

void setup() {
  Serial.begin(9600);
  osv.init();
  cur_phase = phase_0;
  power = 255;
  landing_stored = false;
  arrived = false;
  
}

void loop() {
  int counter = 0;
  double starting_theta, theta_sum = 0, theta_temp = 0;
  bool arrived;

/*
  blockingUpdateAndPrintLocation();
  osv.drive(255);
  delay(300);
  osv.turnOffMotors();
  delay(1000);
 */ 
  if(irSignalCheck()){
    Serial.println("YES");
  } else {
    Serial.println("NO");
  }
  delay(100);
  
}

//Updates and prints OSV coordinates
bool updateAndPrintLocation() {
  bool success = false;
  
  if (enes.updateLocation()) {
    enes.print("(");
    enes.print(enes.location.x);
    enes.print(", ");
    enes.print(enes.location.y);
    enes.print("), Theta = ");
    enes.println(enes.location.theta);
    success = true;
  } else {
    enes.println("Location update failed.");
  }
  return success;
};

//Returns true if black box LOS found, sets current phase to 3 (Navigate to black box)
bool irSignalCheck() {
  for(int i = 0; i < 20; i++){
    if (osv.IRsignal()){
      cur_phase = phase_3;
      return true; 
    }
    delay(100);
  }
  return false;
};

//Orients OSV in angle specified by theta
//-3.14 <= theta <= 3.14
void orient(double theta){
  bool success = false, updated = false;
  double diff, cur;  
  
  while (!success) {
    updated = false;
    while(!updated){
      updated = updateAndPrintLocation();
      delay(100);
    }
    if (updated) {
      diff = angle(enes.location.theta, theta);
      enes.print("Diff: ");
      enes.println(diff);
      if (diff < 3.14 && diff > angle_thresh) {// turn left
        osv.turnLeft(power);
        enes.print("time ");
        enes.println((1 / 0.0015) * diff);
        delay(min((1 / 0.0015) * diff, 800));
        osv.turnOffMotors();
        delay(400);

        /*
         * power 255, 200ms... angle = .3 radians
         * angle = .0015t
         * t = (1/0.0015)angle
         * osv.turnLeft(power)
         * delay()
         * osv.turnOffMotors();
         */
      } else if (diff >= 3.14 && abs(diff) > angle_thresh) {// turn right
        osv.turnRight(power);
        delay(min((1 / 0.0015) * diff, 800));
        osv.turnOffMotors();
        delay(400);
      } else {
        success = true;
      }
    }
    updated = false;
  }
  enes.println("Successful orient.");
};

// returns double indicating angle from a to b, counterclockwise (
double angle(double a, double b) {
  double diff;

  diff = fmod(b - a, 2 * 3.14);
  if (diff < 0) {
    diff += (2*3.14);
  }
  return diff;
};

// Move along axis(0 = X, 1 = Y) in dir (0 = negative, 1 = positive) until obstacle or dest coordinate reached.
// Returns 0 if coordinate reached, 1 if obstacle blocked
int moveInDir(int axis, int dir, double dest) {
  double theta;
  
  switch (axis) {
    case 0: // X axis
      theta = 0? 3.14 : 0;

      // while no obstacle and not inRange
      while (!osv.obstacle(23.5) && !(inRange(enes.location.x, dest))) {
        orient(theta);
        osv.driveP(power, 200);
        updateAndPrintLocation();
      }
      break;
    case 1: // Y axis
      theta = 0? (3.14) / 2 : (3.14) * 1.5;

      // while no obstacle and not inRange
      while (!osv.obstacle(23.5) && !(inRange(enes.location.y, dest))) {
        orient(theta);
        osv.driveP(power, 200);
        updateAndPrintLocation();
      }
      break;

      /*
       * power 255, time 200ms, distance = 
       * 
       * 
       * 
       */
  }
};

bool inRange(double cur, double dest) {
  return (cur >= dest - thresh) && (cur <= dest + thresh);
};

//Alternate drive method, variable speed within
void pulse(int max_power, int duration){
  int i, segment = duration/max_power;
  
  for (i = 0; i < max_power; i++){
    osv.drive(i);
    delay(segment);
  }
  delay(duration/4);
  for (i = max_power; i >= 0; i--){
    osv.drive(i);
    delay(segment);
  }

  osv.turnOffMotors();

};

//Repeated try to update location for 10 tries
bool blockingUpdateAndPrintLocation(){
  bool updated = false;
  int counter = 0;
  
  while(!updated && counter < 10){
    updated = updateAndPrintLocation();
    counter++;
  }
  return updated;  
}

