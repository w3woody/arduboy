/*  Demo3D
 *  
 *      A simple 3D demo
 */
 
/*  Demo3D: A simple 3D demo.
 *
 *  Copyright © 2016 by William Edward Woody
 *
 *  This program is free software: you can redistribute it and/or modify it 
 *  under the terms of the GNU General Public License as published by the 
 *  Free Software Foundation, either version 3 of the License, or (at your 
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along 
 *  with this program. If not, see <http://www.gnu.org/licenses/>
 */

#include "Arduboy.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include "pipeline.h"

/****************************************************************************/
/*                                                                          */
/*  Constants                                                               */
/*                                                                          */
/****************************************************************************/

#define STATE_DISPLAY       0
#define STATE_SKIPDISPLAY   1

/****************************************************************************/
/*                                                                          */
/*  Globals                                                                 */
/*                                                                          */
/****************************************************************************/

Arduboy arduboy;

static uint8_t GState;              // Current play state.

static uint8_t oldBtnState;         // Track button state to find down transitions
static uint8_t curBtnState;

// Tank location
static float GAngle;
static float GXPos;
static float GYPos;

static float GXAngle;
static float GYAngle;
static float GDistance;

static Pipeline pipeline(arduboy,0,128,0,64);

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

/*  ButtonPressed
 *   
 *      Determine if the button is pressed
 */
static boolean ButtonPressed(uint8_t flag)
{
    return (0 != (flag & curBtnState));
}

/****************************************************************************/
/*                                                                          */
/*  Globals                                                                 */
/*                                                                          */
/****************************************************************************/

/*  setup
 *
 *      Initial setup
 */
void setup() 
{
    arduboy.beginNoLogo();
    arduboy.setFrameRate(50);
    arduboy.clear();
    arduboy.display();

    GDistance = 6;
    GXAngle = 0;
    GYAngle = 0;

    GState = STATE_DISPLAY;

}

void updateCTM()
{
    Matrix3D m;

    pipeline.transformation.setPerspective(1.0f,2.0f,0.5f);

    m.setTranslate(0,0,-GDistance);
    pipeline.transformation.multiply(m);

    m.setRotate(AXIS_X,GXAngle);
    pipeline.transformation.multiply(m);

    m.setRotate(AXIS_Y,GYAngle);
    pipeline.transformation.multiply(m);
}

void drawBox(int x, int y, int z)
{
    pipeline.moveTo(x-1,y-1,z-1);
    pipeline.lineTo(x+1,y-1,z-1);
    pipeline.lineTo(x+1,y+1,z-1);
    pipeline.lineTo(x-1,y+1,z-1);
    pipeline.lineTo(x-1,y-1,z-1);
    pipeline.lineTo(x-1,y-1,z+1);
    pipeline.lineTo(x+1,y-1,z+1);
    pipeline.lineTo(x+1,y+1,z+1);
    pipeline.lineTo(x-1,y+1,z+1);
    pipeline.lineTo(x-1,y-1,z+1);
    pipeline.moveTo(x+1,y-1,z-1);
    pipeline.lineTo(x+1,y-1,z+1);
    pipeline.moveTo(x+1,y+1,z-1);
    pipeline.lineTo(x+1,y+1,z+1);
    pipeline.moveTo(x-1,y+1,z-1);
    pipeline.lineTo(x-1,y+1,z+1);
}

/*  loop
 *   
 *      Run rendering loop
 */
void loop() 
{
    if (!arduboy.nextFrame()) return;

    UpdateButtonState();
    
    switch (GState) {
        case STATE_DISPLAY:
            arduboy.clear();
            updateCTM();

            drawBox(0,0,0);
            drawBox(3,0,0);
            drawBox(-3,0,0);
            drawBox(0,3,0);
            drawBox(0,-3,0);
            drawBox(0,0,3);
            drawBox(0,0,-3);
            arduboy.display();
            
            GState = STATE_SKIPDISPLAY;
            break;
            
        case STATE_SKIPDISPLAY:
            if (ButtonPressed(UP_BUTTON)) {
                GXAngle -= 0.05;
                if (GXAngle < -M_PI/2) GXAngle = -M_PI/2;
                GState = STATE_DISPLAY;
            } else if (ButtonPressed(DOWN_BUTTON)) {
                GXAngle += 0.05;
                if (GXAngle > M_PI/2) GXAngle = M_PI/2;
                GState = STATE_DISPLAY;
            } else if (ButtonPressed(LEFT_BUTTON)) {
                GYAngle -= 0.05;
                if (GYAngle < -M_PI) GYAngle += 2 * M_PI;
                GState = STATE_DISPLAY;
            } else if (ButtonPressed(RIGHT_BUTTON)) {
                GYAngle += 0.05;
                if (GYAngle > M_PI) GYAngle -= 2 * M_PI;
                GState = STATE_DISPLAY;
            } else if (ButtonPressed(A_BUTTON)) {
                GDistance += 0.05;
                GState = STATE_DISPLAY;
            } else if (ButtonPressed(B_BUTTON)) {
                GDistance -= 0.05;
                if (GDistance < 1) GDistance = 1;
                GState = STATE_DISPLAY;
            }
            break;
    }
}
