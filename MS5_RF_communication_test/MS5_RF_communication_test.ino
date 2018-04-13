#include "Enes100.h"

/* Create a new Enes100 object
 * Parameters:
 *  string teamName
 *  int teamType
 *  int markerId
 *  int rxPin
 *  int txPin
 */
Enes100 enes("BASED-MRR", BLACK_BOX, 23, 8, 9);

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // Update the OSV's current location
    if (enes.updateLocation()) {
        enes.println("Huzzah! Location updated!");
        enes.print("My x coordinate is ");
        enes.println(enes.location.x);
        enes.print("My y coordinate is ");
        enes.println(enes.location.y);
        enes.print("My theta is ");
        enes.println(enes.location.theta);
    } else {
        enes.println("Sad trombone... I couldn't update my location");
    }

    enes.navigated();
}
