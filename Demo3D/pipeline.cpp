/*  pipeline.cpp
 *   
 *      Implementation of the 3D line drawing pipeline. For a description
 *  of the clipping algorithm see here:
 *  
 *  http://chaosinmotion.com/blog/?p=1001
 */

#include <math.h>
#include <stdint.h>
#include "math3d.h"
#include "pipeline.h"

/************************************************************************/
/*                                                                      */
/*  Construction                                                        */
/*                                                                      */
/************************************************************************/

/*  Pipeline::Pipeline
 *   
 *      Set up a pipeline to draw at the window provided
 */

Pipeline::Pipeline(Arduboy &ctx, uint8_t xmin, uint8_t xmax, uint8_t ymin, uint8_t ymax) : arduboy(ctx)
{
    minx = xmin;
    maxx = xmax;
    miny = ymin;
    maxy = ymax;
}

/************************************************************************/
/*                                                                      */
/*  MDL1: Move/Draw Line level 1                                        */
/*                                                                      */
/************************************************************************/

void Pipeline::mdl1(bool draw, uint8_t x, uint8_t y)
{
    if (draw) {
        arduboy.drawLine(xpos,ypos,x,y,WHITE);
    }
    xpos = x;
    ypos = y;
}

/************************************************************************/
/*                                                                      */
/*  MDL2: Move/Draw Line level 2                                        */
/*                                                                      */
/************************************************************************/

void Pipeline::mdl2(bool draw, const Vector3D &v)
{
    char buffer[64];

    float tmp = (1.0f + v.x / v.w) / 2.0f;
    uint8_t x = minx + (uint8_t)((maxx - minx) * tmp);
    tmp = (1.0f + v.y / v.w) / 2.0f;
    uint8_t y = maxy - (uint8_t)((maxy - miny) * tmp);

    mdl1(draw,x,y);
}

/************************************************************************/
/*                                                                      */
/*  MDL3: Move/Draw Line level 3: Clipper                               */
/*                                                                      */
/************************************************************************/

static uint8_t OutCode(const Vector3D &v)
{
    uint8_t m = 0;

    if (v.x < -v.w) m |= 1;
    if (v.x > v.w) m |= 2;
    if (v.y < -v.w) m |= 4;
    if (v.y > v.w) m |= 8;
    if (v.z < -v.w) m |= 16;
    if (v.z > 0) m |= 32;

    return m;
}

static void Lerp(const Vector3D &a, const Vector3D &b, float alpha, Vector3D &out)
{
    float a1 = 1.0f - alpha;
    out.x = a1 * a.x + alpha * b.x;
    out.y = a1 * a.y + alpha * b.y;
    out.z = a1 * a.z + alpha * b.z;
    out.w = a1 * a.w + alpha * b.w;
}

void Pipeline::mdl3(bool draw, const Vector3D &v)
{
    uint8_t newOutCode = OutCode(v);
    Vector3D lerp;
    if (draw) {
        uint8_t mask = newOutCode | oldOutCode;

        /*
         *  Fast accept/reject
         */

        if (0 == (newOutCode & oldOutCode)) {
            if (0 == mask) {
                // Fast accept. Both points are inside; we assume
                // the previous point was already passed upwards, 
                // so we only draw to the current vector location
                mdl2(true,v);
            } else {
                // At this point we have a line that crosses
                // a boundary. We calculate the alpha between
                // 0 and 1 for each point along the line.
                //
                // (This is the Liang-Barsky optimization of
                // the Cohen-Sutherland algorithm)

                float aold = 0;     // (1 - alpha) * old + alpha * new = v
                float anew = 1;     // in the above, 0 == old, 1 == new.
                float alpha;

                uint8_t m = 1;
                uint8_t i;
                for (i = 0; i < 6; ++i) {
                    if (mask & m) {
                        // Calculate alpha; the intersection along the line
                        // vector intersecting the specified edge
                        // 
                        // These are specific cases of the general equation
                        // alpha = (c - old)/(new - old), which yields
                        // alpha == 0 if c == old, and alpha == 1 if c == new,
                        // and with alpha as a linear scale with the intersection
                        // point sliding from old to new.

                        switch (i) {
                            default:
                            case 0:         // clip (1,0,0,1)
                                alpha = oldPos.x + oldPos.w;
                                alpha = alpha/(alpha - (v.x + v.w));
                                break;
                            case 1:         // clip (1,0,0,-1)
                                alpha = oldPos.x - oldPos.w;
                                alpha = alpha/(alpha - (v.x - v.w));
                                break;
                            case 2:         // clip (0,1,0,1)
                                alpha = oldPos.y + oldPos.w;
                                alpha = alpha/(alpha - (v.y + v.w));
                                break;
                            case 3:         // clip (0,1,0,-1)
                                alpha = oldPos.y - oldPos.w;
                                alpha = alpha/(alpha - (v.y - v.w));
                                break;
                            case 4:         // clip (0,0,1,1)
                                alpha = oldPos.z + oldPos.w;
                                alpha = alpha/(alpha - (v.z + v.w));
                                break;
                            case 5:         // clip (0,0,1,0)
                                alpha = oldPos.z;
                                alpha = alpha/(alpha - v.z);
                                break;
                        }

                        if (oldOutCode & m) {
                            if (aold < alpha) aold = alpha;
                        } else {
                            if (anew > alpha) anew = alpha;
                        }

                        if (aold > anew) {
                            // We have a case where the line is not visible
                            // because it's outside the visible frustrum.
                            // abort.
                            break;
                        }
                    }
                    m <<= 1;
                }

                if (i >= 6) {
                    // Ran all clipping edges.
                    if (oldOutCode) {
                        Lerp(oldPos,v,aold,lerp);
                        mdl2(false,lerp);
                    }

                    // Draw to the new point
                    if (newOutCode) {
                        Lerp(oldPos,v,anew,lerp);
                        mdl2(true,lerp);
                    } else {
                        mdl2(true,v);
                    }
                }
            }
        }
    } else {
        if (newOutCode == 0) {
            mdl2(draw,v);
        }
    }

    oldOutCode = newOutCode;
    oldPos = v;
}


/************************************************************************/
/*                                                                      */
/*  MDL3: Move/Draw Line level 4                                        */
/*                                                                      */
/************************************************************************/

void Pipeline::mdl4(bool draw, const Vector3D &v)
{
    Vector3D t;

    t.multiply(transformation,v);
    mdl3(draw,t);
}

void Pipeline::mdl4(bool draw, float x, float y, float z)
{
    Vector3D t;

    t.x = transformation.a[0][0] * x + transformation.a[0][1] * y + transformation.a[0][2] * z + transformation.a[0][3];
    t.y = transformation.a[1][0] * x + transformation.a[1][1] * y + transformation.a[1][2] * z + transformation.a[1][3];
    t.z = transformation.a[2][0] * x + transformation.a[2][1] * y + transformation.a[2][2] * z + transformation.a[2][3];
    t.w = transformation.a[3][0] * x + transformation.a[3][1] * y + transformation.a[3][2] * z + transformation.a[3][3];

    mdl3(draw,t);
}


