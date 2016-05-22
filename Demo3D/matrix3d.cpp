/*  matrix3d.cpp
 *   
 *      Implementation of the 3D transformation matrix
 */

#include <math.h>
#include <stdint.h>
#include "math3d.h"

/************************************************************************/
/*                                                                      */
/*  Matrix Routines                                                     */
/*                                                                      */
/************************************************************************/

/*  Matrix3D::Matrix3D
 *   
 *      Construct. This initializes with setIdentity
 */

Matrix3D::Matrix3D()
{
    setIdentity();
}

/************************************************************************/
/*                                                                      */
/*  Matrix Creation                                                     */
/*                                                                      */
/************************************************************************/

/*  Matrix3D::setIdentity
 *   
 *      Set to identity matrix
 */

void Matrix3D::setIdentity()
{
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 4; ++j) {
            a[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

/*  Matrix3D::setTranslate
 *   
 *      Create a translation matrix
 */

void Matrix3D::setTranslate(float x, float y, float z)
{
    setIdentity();
    a[0][3] = x;
    a[1][3] = y;
    a[2][3] = z;
}

/*  Matrix3D::setScale
 *   
 *      Scale matrix
 */

void Matrix3D::setScale(float x, float y, float z)
{
    setIdentity();
    a[0][0] = x;
    a[1][1] = y;
    a[2][2] = z;
}

/*  Matrix3D::setScale
 *   
 *      Scale matrix
 */

void Matrix3D::setScale(float s)
{
    setIdentity();
    a[0][0] = s;
    a[1][1] = s;
    a[2][2] = s;
}

/*  Matrix3D::setRotate
 *   
 *      Rotational matrix. Axis is 0 (x), 1 (y) or 2 (z)
 */

void Matrix3D::setRotate(uint8_t axis, float angle)
{
    setIdentity();
    float c = cos(angle);
    float s = sin(angle);
    
    switch (axis) {
        case AXIS_X:
            a[1][1] = c;
            a[2][2] = c;
            a[1][2] = -s;
            a[2][1] = s;
            break;
        case AXIS_Y:
            a[0][0] = c;
            a[2][2] = c;
            a[0][2] = s;
            a[2][0] = -s;
            break;
        case AXIS_Z:
            a[0][0] = c;
            a[1][1] = c;
            a[0][1] = s;
            a[1][0] = -s;
            break;
        default:
            return;
    }
}

/*  Matrix3D::setPerspective
 *   
 *      Set the perspective matrix. See https://github.com/w3woody/clipping
 */

void Matrix3D::setPerspective(float fov, float aspect, float near)
{
    setIdentity();

    a[0][0] = fov/aspect;
    a[1][1] = fov;
    a[2][2] = 0.0f;
    a[3][3] = 0.0f;
    a[2][3] = -1.0f;
    a[3][2] = -near;
}


/************************************************************************/
/*                                                                      */
/*  Matrix Math                                                         */
/*                                                                      */
/************************************************************************/

/*  Matrix3D::multiply
 *   
 *      Multiply the provided matrix into this one. This is done via pre-
 *  multiplication, and allows us to chain transformations. The way this
 *  is done implies you must start with the last (perspective) matrix first,
 *  pushing earlier transformations later. This allows you (if you wish)
 *  to build a push stack of matrices, though in the limited confines of
 *  an embedded processor that stack cannot be very large...
 */

void Matrix3D::multiply(const Matrix3D &m)
{
    float tmp[4];
    float n;
    
    for (uint8_t i = 0; i < 4; ++i) {
        /*
         * Comlumn by column multiply and replace.
         */
        for (uint8_t j = 0; j < 4; ++j) {
            n = 0.0f;
            for (uint8_t k = 0; k < 4; ++k) {
                n += a[i][k] * m.a[k][j];
            }
            tmp[j] = n;
        }

        for (uint8_t j = 0; j < 4; ++j) {
            a[i][j] = tmp[j];
        }
    }
}


