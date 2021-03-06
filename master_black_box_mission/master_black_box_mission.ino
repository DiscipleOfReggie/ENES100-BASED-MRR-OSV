#include <black_box_osv.h>
#include "Enes100.h"
#include <Servo.h>

enum phase {
  phase_0, // Exit the landing zone
  phase_1, // Navigate to center of mission area
  phase_2, // Search for Black Box
  phase_3, // Navigate to Black Box
  phase_4, // Transmit Black Box coordinates
  phase_5, // Secure Black Box
  phase_6, // RTB
  phase_7  //Adjust location to reset phase_2
};

const double landing_zone_x = 1.475, thresh = .2, angle_thresh = .2, obstacle_dist = 22.0,
             pi = 3.14159265358, two_pi = pi * 2;
const int tag = 31, power = 255;

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
Enes100 enes("BASED-MRR", BLACK_BOX, tag, 8, 9);
Coordinate landing_coordinate, black_box_coordinate, mission_center(1, 2.7);
Servo myServo;
phase cur_phase;
bool landing_stored, arrived;

void setup() {
  Serial.begin(9600);
  osv.init();
  cur_phase = phase_0;
  landing_stored = false;
  arrived = false;
  myServo.attach(5);
  //Lock servo arm upright
  myServo.writeMicroseconds(1500);
  delay(1000);
  myServo.writeMicroseconds(800);

}

void loop() {
  int counter = 0;
  double starting_theta, theta_sum = 0, theta_temp = 0, angle, distance;
  bool arrived;


  // update location
  //updateAndPrintLocation();

  switch (cur_phase) {

    /* EXIT THE LANDING ZONE
        Preconditions: N/A
        Postconditions: OSV has stored landing coordinate in landing_coordinate and is now at a
        coordinate outside of the landing zone, i.e., the OSV has a coordinate with X value
        greater than the bound defined by landing_zone_x
        Moves to: Phase 1
    */
    case phase_0:

      enes.println("--------PHASE 0--------");

      //store landing coordinate
      while (!landing_stored) {
        if (updateAndPrintLocation()) {

          Coordinate coor (enes.location.x, enes.location.y, enes.location.theta);
          landing_coordinate = coor;
          landing_stored = true;
        }
      }
      //print landing coordiante
      enes.print("LANDING COORDINATE STORED: ");
      enes.print("(");
      enes.print(landing_coordinate.x);
      enes.print(", ");
      enes.print(landing_coordinate.y);
      enes.print("), Theta = ");
      enes.println(landing_coordinate.theta);

      //check for obstacle immediately across rocky terrain?

      updateAndPrintLocation();

      //move in correct theta, readjusting as necessary until past landing_zone_x
      while (enes.location.x < landing_zone_x - thresh) {
        orient(pi);
        /*
           power 255, time 200ms, distance =
           d = t

        */
        //If osv is pointing in reverse direction
        osv.driveP(-power + 20, 600);
        blockingUpdateAndPrintLocation();
      }
      enes.println("EXITED LANDING AREA");
      //orient to theta 0
      orient(0);
      cur_phase = phase_1;
      irSignalCheck();
      enes.println("Moving to phase 1");
      break;

    /************************************************************************************************/
    /* NAVIGATE TO CENTER OF MISSION AREA
        Preconditions: OSV outside of landing zone
        Postconditions: OSV at center of mission area (X coordinate, Y coordinate)
        Moves to: Phase 2
    */
    case phase_1:
      enes.println("--------PHASE 1--------");

      /*
        arrived = false;
        //While current coordinate not within threahold values compared with center of mission area,
        while (!arrived) {
        //If current x coordinate not within threshold value for mission area center
        if (enes.location.x < mission_center.x - thresh  || enes.location.x > mission_center.x + thresh) {

        }
        //If current y coordinate not within threshold value for mission area center
        if (enes.location.y < mission_center.y - thresh  || enes.location.y > mission_center.y + thresh) {

        }
        if (enes.location.x >= mission_center.x + thresh &&
            enes.location.x <= mission_center.x - thresh &&
            enes.location.y >= mission_center.y + thresh &&
            enes.location.y <= mission_center.y - thresh) {
          arrived = true;
        }
        }
        while (enes.location.x <= 2.7 + thresh) {
        //If osv is not at mission area center
        while (enes.location.theta <= 0 + thresh) {
          enes.updateLocation();
          osv.turnRight(power);
          delay(500);
        }
        while (osv.obstacle(obstacle_distance)) {
          osv.turnRight(power);
          osv.driveP(power, 500);
        }
        osv.driveP(power, 1000);
        enes.updateLocation();
        }
      */

      angle = 0.0;
      while (enes.location.x <= mission_center.x - thresh && (!osv.obstacle(obstacle_dist))) {
        //If osv is not at mission area center
        if (enes.location.y <= mission_center.y + thresh || enes.location.y >= mission_center.y - thresh) {
          osv.driveP(power - 20, 300);
          updateAndPrintLocation();
        } else {
          angle = ((atan((mission_center.x - enes.location.x) / (abs(enes.location.y - mission_center.y)))) / (180)) * 3.14;
          if (enes.location.y > mission_center.y) {
            orient(1.57 - angle);
          } else {
            orient(abs(1.57 - angle));
          }
        }
        osv.driveP(power - 20, 500);
        updateAndPrintLocation();
      }

      while (osv.obstacle(obstacle_dist)) {
        osv.turnRight(power);
        osv.driveP(power - 20, 500);
        updateAndPrintLocation();
      }
      if (enes.location.x > mission_center.x - thresh && (!osv.obstacle(obstacle_dist))) {
        cur_phase = phase_2;
        break;
      }
      break;
    /************************************************************************************************/
    /* SEARCH FOR BLACK BOX
        Preconditions: OSV located at A) center of mission area B) with offset of INSERT in X/Y direction
        Postconditions: OSV has either completed X full rotations in search of black box IR signal,
        or has detected the black box IR signal and is facing the source.
        Notes: This phase concerns dedicated searching immediately following Phase 1.
        It does not involve the constant IR sensor checks during previous phases.
        Moves to: Phase 3 if successful, Phase INSERT if unsuccessful
    */
    case phase_2:

      enes.println("--------PHASE 2--------");

      //record starting theta
      starting_theta = enes.location.theta;

      //rotate in set increments, checking IR sensor at each step
      while (counter < 2) {
        //if IR signal found, proceed to phase 3
        if (irSignalCheck()) {
          break; //changing cur_phase to phase_3 covered by irSignalCheck()
        }
        theta_temp = enes.location.theta;
        osv.turnLeft(power);
        delay(100);
        osv.turnOffMotors();
        delay(200);
        updateAndPrintLocation();
        theta_sum += theta_temp;
        enes.print("Total angle rotated this rotation: ");
        enes.println(theta_sum);
        if (theta_sum >= (two_pi)) {
          counter++;
          theta_sum = 0;
          enes.print(counter);
          enes.println(" full rotation(s) completed.");
        }
        if (counter == 2) {
          cur_phase = phase_7; //Search failed
        }
      }
      break;
    /************************************************************************************************/
    /* NAVIGATE TO BLACK BOX
        Preconditions: OSV has Line Of Sight (LOS) with black box, as defined by detection of the
        IR signal by the forward-facing IR sensor
        Postconditions: OSV has LOS with black box, is positioned within INSERT meters from the
        black box, and has alerted the vision system of successful navigation.
        Moves to: Phase 4
    */

    case phase_3:
      enes.println("--------PHASE 3--------");

      if (!osv.IRsignal()) {//Verify IR signal
        cur_phase = phase_3;
      } else {
        while (!arrived) {
          osv.driveP(power - 20, 100);
          delay(400);
          if (!osv.IRsignal()) {//signal lost
            while (!osv.IRsignal()) {
              osv.turnLeft(power);
              delay(50);
              osv.turnOffMotors();
              delay(200);
              if (!osv.IRsignal()) {
                osv.turnRight(power);
              }
              delay(100);
              osv.turnOffMotors();
              delay(200);
            }
          }
          if (osv.IRsignal() && osv.obstacle(obstacle_dist)) {
            arrived = true;
          }
        }
      }
      enes.navigated();
      cur_phase = phase_4;
      break;
    /************************************************************************************************/
    /* TRANSMIT BLACK BOX COORDINATES
        Preconditions: OSV has LOS with black box and is positioned INSERT meters from the
        black box
        Postconditions: OSV has transmitted coordinates of the black box via RF communication
        Moves to: Phase 5
    */


    case phase_4:
      enes.println("--------PHASE 4--------");

      //Verify LOS via IR sensors
      if (osv.IRsignal()) {
        cur_phase = phase_3;
        break;
      }
      //Approach black box, maintaining heading and adjusting as necessary until within THRESHOLD range as determined by US sensor
      //if LOS lost, tun slightly in either direction to reacquire



      blockingUpdateAndPrintLocation();
      distance = osv.getDistance();
      black_box_coordinate = enes.location;
      black_box_coordinate.x += distance * cos(enes.location.theta);
      black_box_coordinate.y += distance * sin(enes.location.theta);
      enes.baseObjective(black_box_coordinate);

      cur_phase = phase_5;
      break;
    /************************************************************************************************/
    /* SECURE BLACK BOX
        Preconditions: OSV has LOS with black box, is positioned INSERT meters from the
        black box, and has transmitted coordinates of the black box
        Postconditions: OSV has successfully lifted black box over the sand
        Moves to: Phase 6
    */

    case phase_5:
      enes.println("--------PHASE 5--------");
      if (osv.IRsignal()) {
        cur_phase = phase_3;
        break;
      } else {
        osv.driveP(power - 20, 300);
      }
      //acquire bb with servo-powered arm
      myServo.writeMicroseconds(1600);
      delay(5000);
      myServo.writeMicroseconds(1500);                  // Stop 1500
      cur_phase = phase_6;


      break;
    /************************************************************************************************/
    /* RTB
        Preconditions: OSV has successfully lifted black box over the sand, is holding black box
        Postconditions: OSV has returned to landing coordinates, as defined by the Coordinate
        landing_coordniate
        Moves to: N/A
    */
    case phase_6:
      enes.println("--------PHASE 6--------");

      while (enes.location.x > landing_zone_x + thresh) {
        orient(0);
        osv.driveP(-power + 20, 200);
        updateAndPrintLocation();
      }
      enes.endMission();
      break;
    /************************************************************************************************/
    /*
        Preconditions: Failed phase_3
        Postconditions: OSV has moved to a new location and will restart phase_3
    */
    case phase_7:

      osv.driveP(power - 20, 600);
      cur_phase = phase_3;
      break;

    /************************************************************************************************/
    /*
        Preconditions: N/A
        Postconditions: N/A
    */
    default:
      enes.println("--------PHASE 7--------");
      //PANIC!
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//HELPER METHODS
///////////////////////////////////////////////////////////////////////////////////////////////////////



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

//Repeated try to update location for max tries
bool blockingUpdateAndPrintLocation() {
  bool updated = false;
  int counter = 0, max = 15;

  while (!updated && counter < max) {
    updated = enes.updateLocation();
    counter++;
    enes.print("Try number");
    enes.println(counter);
  }
  if (updated) {
    enes.print("(");
    enes.print(enes.location.x);
    enes.print(", ");
    enes.print(enes.location.y);
    enes.print("), Theta = ");
    enes.println(enes.location.theta);
  }
  return updated;
};

//Returns true if black box LOS found, sets current phase to 3 (Navigate to black box)
bool irSignalCheck() {
  int counter = 0, max = 20;
  bool found = false;
  
  enes.println("Checking IR");

  while (!found && counter < max) {
      counter++;
      found = osv.IRsignal();
      delay(20);
  }
  if (found) {
    cur_phase = phase_3;
    enes.println("FOUND");
    return true;
    
  } else {
    enes.println("NOT FOUND");
    return false;
  }
};

//Orients OSV in angle specified by theta
//-pi <= theta <= pi
void orient(double theta) {
  bool success = false, updated = false;
  double diff, cur;

  while (!success) {
    updated = false;
    while (!updated) {
      updated = updateAndPrintLocation();
    }
    if (updated) {
      diff = getAngle(enes.location.theta, theta);
      enes.print("Diff: ");
      enes.println(diff);
      if (diff < pi && diff > angle_thresh) {// turn left
        osv.turnLeft(power);
        enes.print("time ");
        enes.println((1 / 0.0015) * diff);
        delay(min((1 / 0.003) * diff, 500));
        osv.turnOffMotors();
        delay(400);

        /*
           power 255, 200ms... angle = .3 radians
           angle = .0015t
           t = (1/0.0015)angle
           osv.turnLeft(power)
           delay()
           osv.turnOffMotors();
        */
      } else if (diff >= pi && abs(diff) > angle_thresh) {// turn right
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
double getAngle(double a, double b) {
  double diff;

  diff = fmod(b - a, two_pi);
  if (diff < 0) {
    diff += (two_pi);
  }
  return diff;
};

// Move along axis(0 = X, 1 = Y) in dir (0 = negative, 1 = positive) until obstacle or dest coordinate reached.
// Returns 0 if coordinate reached, 1 if obstacle blocked
int moveInDir(int axis, int dir, double dest) {
  double theta;

  switch (axis) {
    case 0: // X axis
      theta = 0 ? pi : 0;

      // while no obstacle and not inRange
      while (!osv.obstacle(23.5) && !(inRange(enes.location.x, dest))) {
        orient(theta);
        osv.driveP(power - 20, 200);
        updateAndPrintLocation();
      }
      break;
    case 1: // Y axis
      theta = 0 ? (pi) / 2 : (pi) * 1.5;

      // while no obstacle and not inRange
      while (!osv.obstacle(obstacle_dist) && !(inRange(enes.location.y, dest))) {
        orient(theta);
        osv.driveP(power - 20, 200);
        updateAndPrintLocation();
      }
      break;

      /*
         power 255, time 200ms, distance =



      */
  }
};

bool inRange(double cur, double dest) {
  return (cur >= dest - thresh) && (cur <= dest + thresh);
};

//Alternate drive method, variable speed within
void pulse(int max_power, int duration) {
  int i, segment = duration / max_power;

  for (i = 0; i < max_power; i++) {
    osv.drive(i);
    delay(segment);
  }
  for (i = max_power; i >= 0; i++) {
    osv.drive(i);
    delay(segment);
  }
  osv.turnOffMotors();
};
