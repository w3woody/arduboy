/*  Popeline.h
 *   
 *      Defines a vector pipeline to render 3D points
 */

#ifndef __PIPELINE_H
#define __PIPELINE_H

#include <stdint.h>
#include <arduboy.h>
#include "math3d.h"

/************************************************************************/
/*                                                                      */
/*  Rendering pipeline                                                  */
/*                                                                      */
/************************************************************************/

class Pipeline
{
    public:
                    Pipeline(Arduboy &ctx, uint8_t minx, uint8_t maxx, uint8_t miny, uint8_t maxy);

        void        moveTo(const Vector3D &v)
                        {
                            mdl4(false,v);
                        }
        void        lineTo(const Vector3D &v)
                        {
                            mdl4(true,v);
                        }
        void        moveTo(float x, float y, float z)
                        {
                            mdl4(false,x,y,z);
                        }
        void        lineTo(float x, float y, float z)
                        {
                            mdl4(true,x,y,z);
                        }

        /*
         *  The transformation matrix is exposed; adjusting this adjusts the
         *  transformation used to convert 3D vectors to screen space
         */
        Matrix3D    transformation;

    private:
        /*
         *  We hold a handle to the arduboy context. I'm not a fan of this waste
         *  of pointer space, but it's either that, or simply embedding the context
         *  within our pipeline object..
         */

        Arduboy     &arduboy;
        
        /*
         *  Move/Draw level 1: lowest level, draw lines
         */

        uint8_t     xpos;
        uint8_t     ypos;
        void        mdl1(bool draw, uint8_t x, uint8_t y);

        /*
         *  Move/Draw level 2: screen space drawing. Converts homogeneous
         *  coordinates into pixel space. Assumes we've been clipped.
         */

        uint8_t     minx;
        uint8_t     maxx;
        uint8_t     miny;
        uint8_t     maxy;
        void        mdl2(bool draw, const Vector3D &v);

        /*
         *  Move/Draw level 3: screen space clipper. Clips against the
         *  screen space bounding box at (-1,1)/(-1,1)/(-1,0). See the
         *  discussion at http://chaosinmotion.com/blog/?p=555 for 
         *  more information about the clipping planes used, and 
         *  https://en.wikipedia.org/wiki/Cohen–Sutherland_algorithm
         *  for a description of the algorithm used.
         */

        Vector3D    oldPos;
        uint8_t     oldOutCode;
        void        mdl3(bool draw, const Vector3D &v);

        /*
         *  Move/Draw level 4: transformation.
         */

        void        mdl4(bool draw, const Vector3D &v);
        void        mdl4(bool draw, float x, float y, float z);
};

#endif // __PIPELINE_H

