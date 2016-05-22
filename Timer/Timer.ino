/*  Timer
 *  
 *      3D Tanks
 */

#include "Arduboy.h"
#include <stdint.h>
#include <avr/pgmspace.h>

/****************************************************************************/
/*                                                                          */
/*  States                                                                  */
/*                                                                          */
/****************************************************************************/

#define STATE_RESET         0
#define STATE_STOP          1
#define STATE_RUN           2

/****************************************************************************/
/*                                                                          */
/*  Globals                                                                 */
/*                                                                          */
/****************************************************************************/

Arduboy arduboy;

static uint8_t GState;              // Current play state.

static uint32_t GStartTime;         // Start time when timer is running
static uint32_t GElapsedTime;       // Elapsed time
static uint32_t GDispTime;          // Displayed Time

static uint32_t GDispQueue[3];      // Queue of display times

static uint8_t oldBtnState;         // Track button state to find down transitions
static uint8_t curBtnState;

/****************************************************************************/
/*                                                                          */
/*  ButtonStates                                                            */
/*                                                                          */
/****************************************************************************/

/*  UpdateButtonState
 *
 *      Update the button state
 */

static void UpdateButtonState()
{
    oldBtnState = curBtnState;
    curBtnState = ArduboyCore::buttonsState();
}

static void DisplayTimeAt(int line, uint32_t t)
{
    char buffer[20];
    
    int f = t % 10;
    t /= 10;
    int s = t % 60;
    t /= 60;
    int m = t % 60;
    int h = t / 60;

    if ((h == 0) && (m == 0)) {
        sprintf(buffer,"%d.%d",s,f);
    } else if (h == 0) {
        sprintf(buffer,"%d:%02d.%d",m,s,f);
    } else {
        sprintf(buffer,"%d:%02d:%02d.%d",h,m,s,f);
    }

    int x = 128 - strlen(buffer)*12;
    arduboy.setCursor(x,line*16);
    
    arduboy.print(buffer);
}

static void DisplayTime()
{
    arduboy.clear();
    arduboy.setTextSize(2);

    DisplayTimeAt(0,GDispTime);
    for (int i = 0; i < 3; ++i) {
        if (GDispQueue[i]) {
            DisplayTimeAt(i+1,GDispQueue[i]);
        }
    }

    arduboy.display();
}

/*  ButtonKeyDown
 *   
 *      Determine if a button was just pressed. This tests the transition
 *  from the previous state. Requires that UpdateButtonState was called.
 */
static boolean ButtonKeyDown(uint8_t flag)
{
    if (flag & oldBtnState) return false;
    if (0 == (flag & curBtnState)) return false;
    return true;
}

void setup() 
{
    // Now start up with a cleared screen
    arduboy.beginNoLogo();
    arduboy.setFrameRate(60);

    // And Reset the timer
    GState = STATE_RESET;
    GDispTime = 0;
    GDispQueue[0] = 0;
    GDispQueue[1] = 0;
    GDispQueue[2] = 0;
    DisplayTime();
}

void loop() 
{
    long t;
    
    if (!arduboy.nextFrame()) return;

    UpdateButtonState();
    
    switch (GState) {
        case STATE_RESET:
            GStartTime = 0;
            GElapsedTime = 0;
            GState = STATE_STOP;
            t = 0;
            break;

        case STATE_STOP:
            if (ButtonKeyDown(A_BUTTON)) {
                GStartTime = millis();
                GState = STATE_RUN;
            } else if (ButtonKeyDown(B_BUTTON)) {
                if (GElapsedTime == 0) {
                    GDispQueue[0] = 0;
                    GDispQueue[1] = 0;
                    GDispQueue[2] = 0;
                    DisplayTime();
                } else {
                    GElapsedTime = 0;
                }
            }
            t = GElapsedTime;
            break;

        case STATE_RUN:
            t = GElapsedTime + millis() - GStartTime;

            if (ButtonKeyDown(A_BUTTON)) {
                GElapsedTime = t;
                GStartTime = 0;
                GState = STATE_STOP;
            } else if (ButtonKeyDown(B_BUTTON)) {
                // Reset? Or store time for secondary display?
                GDispQueue[2] = GDispQueue[1];
                GDispQueue[1] = GDispQueue[0];
                GDispQueue[0] = t/100;
                DisplayTime();
            }
            break;
    }

    t /= 100;
    if (t != GDispTime) {
        GDispTime = t;
        DisplayTime();
    }
}

