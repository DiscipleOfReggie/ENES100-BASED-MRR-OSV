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

const double landing_zone_x = 1, ;

/* Initialize osv pins  
     Parameters:
       int pin_right_dir1, 
       int pin_right_dir2, 
       int pin_left_dir1, 
       int pin_left_dir2, 
       int pin_pwm, 
       int trigPin,
       int echoPin, 
       int irPin
*/
BlackBoxOSV osv(2, 4, 6, 7, 3, 10, 11, 13);
Enes100 enes("BASED-MRR", BLACK_BOX, 0, 8, 9);
phase cur_phase;
Coordinate landing_coordinate, black_box_coordinate;

void setup() {
  Serial.begin(9600);
  //osv.init();
  cur_phase = phase_0;
}

void loop() {
  //How frequently should we check the IR sensor for a signal?
  //Phases for panic search, default?
  //Navigate to center and back to landing coordinate with same code?


  // update current location
  
  switch (cur_phase) {

    /* EXIT THE LANDING ZONE
        Preconditions: N/A
        Postconditions: OSV has stored landing coordinate in landing_coordinate and is now at a
        coordinate outside of the landing zone, i.e., the OSV has a coordinate with X value
        greater than the bound defined by landing_zone_x
        Moves to: Phase 1
    */
    case phase_0:
      Serial.println("Phase 0");
      //store landing coordinate
      //rotate to face correct theta for exit
      //move in correct theta, readjusting as necessary until past landing_zone_x
      break;



    /************************************************************************************************/
    /* NAVIGATE TO CENTER OF MISSION AREA
        Preconditions: OSV outside of landing zone
        Postconditions: OSV at center of mission area (X coordinate, Y coordinate)
        Moves to: Phase 2
    */
    case phase_1:
      Serial.println("Phase 1");
      //Update coordinate
      if (enes.updateLocation()) {
        printLocation();
        
      }
      //If current coordinate not within threahold values compared with center of mission area,
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
      break;


      
    /************************************************************************************************/
    /* SEARCH FOR BLACK BOX
        Preconditions: OSV at center of mission area
        Postconditions: OSV has either completed X full rotations in search of black box IR signal,
        or has detected the black box IR signal and is facing the source.
        Moves to: Phase 3 if successful, Phase INSERT if unsuccessful
    */
    case phase_2:
      Serial.println("Phase 2");
      //rorate in set increments, checking IR sensor at each step
      //if IR signal found, proceed to phase 3
      //else if two full rotations are completed, proceed to phase INSERT
      break;


      
    /************************************************************************************************/
    /* NAVIGATE TO BLACK BOX
        Preconditions: OSV has Line Of Sight (LOS) with black box, as defined by detection of the
        IR signal by the forward-facing IR sensor
        Postconditions: OSV has LOS with black box and is positioned INSERT meters from the
        black box
        Notes: This phase concerns dedicated searching immediately following Phase 2.
        It does not involve the constant IR sensor checks during previous phases.
        Moves to: Phase 4
    */
    case phase_3:
      Serial.println("Phase 3");
      break;


      
    /************************************************************************************************/
    /* TRANSMIT BLACK BOX COORDINATES
        Preconditions: OSV has LOS with black box and is positioned INSERT meters from the
        black box
        Postconditions: OSV has transmitted coordinates of the black box via RF communication
        Moves to: Phase 5
    */
    case phase_4:
      Serial.println("Phase 4");
      break;


      
    /************************************************************************************************/
    /* SECURE BLACK BOX
        Preconditions: OSV has LOS with black box, is positioned INSERT meters from the
        black box, and has transmitted coordinates of the black box
        Postconditions: OSV has successfully lifted black box over the sand
        Moves to: Phase 6
    */
    case phase_5:
      Serial.println("Phase 5");
      break;


      
    /************************************************************************************************/
    /* RTB
        Preconditions: OSV has successfully lifted black box over the sand, is holding black box
        Postconditions: OSV has returned to landing coordinates, as defined by the Coordinate
        landing_coordniate
        Moves to: N/A
    */
    case phase_6:
      Serial.println("Phase 6");
      break;


      
    /************************************************************************************************/
    /*
        Preconditions: N/A
        Postconditions: N/A
    */
    default:
      break;
  }
}

void printLocation() {
  enes.print("(");
  enes.print(enes.location.x);
  enes.print(", ");
  enes.print(enes.location.y);
   enes.print("), θ = ");
  enes.println(enes.location.theta);
};

bool ir_signal_check() {
  
};



