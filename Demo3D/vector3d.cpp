/*  vector3d.cpp
 *   
 *      Implementation of a 3d vector
 */

#include <math.h>
#include <stdint.h>
#include "math3d.h"


/************************************************************************/
/*                                                                      */
/*  Matrix multiplication                                               */
/*                                                                      */
/************************************************************************/

/*  Vector3D:multiply
 *
 *      Set this vector to the multiplcation of the provided vector and
 *  matrix
 */

void Vector3D::multiply(const Matrix3D &m, const Vector3D &v)
{
    x = m.a[0][0] * v.x + m.a[0][1] * v.y + m.a[0][2] * v.z + m.a[0][3] * v.w;
    y = m.a[1][0] * v.x + m.a[1][1] * v.y + m.a[1][2] * v.z + m.a[1][3] * v.w;
    z = m.a[2][0] * v.x + m.a[2][1] * v.y + m.a[2][2] * v.z + m.a[2][3] * v.w;
    w = m.a[3][0] * v.x + m.a[3][1] * v.y + m.a[3][2] * v.z + m.a[3][3] * v.w;
}

