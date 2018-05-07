#include <Servo.h>

#include <Enes100.h>

#include <black_box_osv.h>


enum phase {
  phase_0, // Exit the landing zone
  phase_1, // Navigate to center of mission area
  phase_2, // Search for Black Box
  phase_3, // Navigate to Black Box
  phase_4, // Transmit Black Box coordinates
  phase_5, // Secure Black Box
  phase_6,  // RTB
  defaultr  // Put something here
};

const double landing_zone_x = 1.375, thresh = .5, angle_thresh = .2, obstacle_distance = 23.5;

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
Enes100 enes("BASED-MRR", BLACK_BOX, 41, 8, 9);
Coordinate landing_coordinate, black_box_coordinate, mission_center(0, 0, 0);
Servo myservo;
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
  myservo.attach(5);//////////////////Whats the continuous servo pin
}

/****************************************************************************************///Helper Methods
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
  if (osv.IRsignal()) {
    cur_phase = phase_3;
    return true;
  } else {
    return false;
  }
};

//Orients OSV in angle specified by theta
//-3.14 <= theta <= 3.14
void orient(double theta) {
  bool success = false;
  double diff, cur;

  while (!success) {
    if (updateAndPrintLocation()) {
      diff = angle(enes.location.theta, theta);

      if (diff < 180 && diff > thresh) {// turn left
        osv.turnLeft(power);
        delay(100);
        osv.turnOffMotors();
        delay(300);
      } else if ((2 * 3.14 - diff) > thresh) {
        osv.turnRight(power);
        delay(100);
        osv.turnOffMotors();
        delay(300);
      } else {
        success = true;
      }
    }
  }
};

// returns double indicating angle from a to b, clockwise (
double angle(double a, double b) {
  double diff;

  diff = fmod(b - a, 2 * 3.14);
  if (diff < 0) {
    diff += (2 * 3.14);
  }
  return diff;
};
/****************************************************************************///Helper Method end


void loop() {
  int counter = 0;
  double starting_theta, theta_sum = 0, theta_temp = 0;

  //Phases for panic search, default?


  // update location
  updateAndPrintLocation();

  switch (cur_phase) {

    /* EXIT THE LANDING ZONE
        Preconditions: N/A
        Postconditions: OSV has stored landing coordinate in landing_coordinate and is now at a
        coordinate outside of the landing zone, i.e., the OSV has a coordinate with X value
        greater than the bound defined by landing_zone_x
        Moves to: Phase 1
    */
    case phase_0:
      {
        enes.println("PHASE 0");
        myservo.writeMicroseconds(1500);                  // ClockWise/ Rise Arm 700-1499
        delay(1000);
        myservo.writeMicroseconds(800);                     // ClockWise/ Rise Arm 700-1499

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
        while (enes.location.x < landing_zone_x + thresh) {
          orient(3.14);
          //If osv is pointing in reverse direction
          osv.driveP(-power, 200);
          updateAndPrintLocation();
        }

        //orient to theta 0
        orient(0);
        cur_phase = phase_1;
        break;
      }
    /************************************************************************************************/
    /* NAVIGATE TO CENTER OF MISSION AREA
        Preconditions: OSV outside of landing zone
        Postconditions: OSV at center of mission area (X coordinate, Y coordinate)
        Moves to: Phase 2
    */
    case phase_1:
      {
        enes.println("PHASE 1");


        //While current coordinate not within threahold values compared with center of mission area,

        //If current x coordinate not within threshold value for mission area center
        //turn in correct x direction facing mission area center (+x or -x)

        //proceed along this path (maybe adjust as necessary) until within threshold of x value
        //for mission area center OR until obstacle reached (in which case increment obstacle
        //counter)

        //If current y coordinate not within threshold value for mission area center
        //turn in correct y direction facing mission area center (+y or -y)

        //proceed along this path (maybe adjust as necessary) until within threshold of y value
        //for mission area center OR until obstacle reached (in which case increment obstacle
        //counter)

        //WHERE TO RESET COUNTER?

        //If obstacle counter == 2  (OSV blocked in two directions)
        //go around

        double angle = 0.0;
        while (enes.location.x <= 2.7 + thresh) {
          //If osv is not at mission area center
          if (enes.location.y <= 1 + thresh || enes.location.y >= 1 - thresh) {
            osv.driveP(power, 500);
          } else

            angle = ((atan((2.7 - enes.location.x) / (abs(enes.location.y - 1)))) / (180)) * 3.14;
          if (enes.location.y > 1) {
            orient(1.57 - angle);
          } else
            orient(abs(1.57 - angle));
        }
        osv.driveP(power, 500);
      }

      while (osv.obstacle(obstacle_distance)) {
        osv.turnRight(power);
        osv.driveP(power, 500);
      }

      cur_phase = phase_2;

      break;
  }


}
