/*  math3d.h
 *
 *        3D math routines
 */

#ifndef __MATH3D_H
#define __MATH3D_H

/************************************************************************/
/*                                                                      */
/*  Matrix Routines                                                     */
/*                                                                      */
/************************************************************************/

#define AXIS_X          0
#define AXIS_Y          1
#define AXIS_Z          2

/*  Matrix3D
 *
 *      A 3D matrix. THis is a 64 byte object which stores the full
 *  3D representation of the transformation matrix
 */

class Matrix3D
{
    public:
                        Matrix3D();

        // Initialize matrix
        void            setIdentity();
        void            setTranslate(float x, float y, float z);
        void            setScale(float x, float y, float z);
        void            setScale(float x);
        void            setRotate(uint8_t axis, float angle);
        void            setPerspective(float fov, float aspect, float near);

        // Inline multiply transformation matrix
        void            multiply(const Matrix3D &m);
        
        float           a[4][4];
};

/*  Vector3D
 *    
 *      A homogeneous coordinate vector. Those contain an additional w
 *  term which gives the 'scale' after transformation
 */
struct Vector3D
{
    double x,y,z,w;

    void                multiply(const Matrix3D &m, const Vector3D &v);
};



#endif

