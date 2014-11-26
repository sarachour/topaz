//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//                           
//	 © 2006, Intel Corporation, licensed under Apache 2.0 
//
//  file :	 Dmatrix.h
//  author : Scott Ettinger - scott.m.ettinger@intel.com
//  description : Displacement matrix and vector math 
//				  operations.
//  modified : 
//--------------------------------------------------------------


#ifndef DMATRIX_H
#define DMATRIX_H

#if defined(HAVE_CONFIG_H)
# include "config.h"

#endif
#include "relax_lib.h"
#include "stdio.h"
#include <cstring>
#include <iostream>
#include "SmallVectors.h"

typedef enum {X, Y, Z} DMAxis;
typedef enum {XYZ, ZYX, YXZ, ZXY} DMOrder;

//simple 3D Displacement Matrix class
#define DMatrixf DMatrix


class DMatrix;
//-------------- Related functions ----------------------------------------------------------

//Displacement matrix inverse

DMatrix Inverse(const DMatrix &m);

//Frobenius norm squared of matrix

float FrobNorm2(const DMatrix &m);

//Create a rotation matrix around a given axis

void AxisRotation(DMatrix &m, DMAxis axis, float angle);

//Create a rotation matrix given Euler angles and axis order

void EulerRotation(DMatrix &m, float theta, float phi, float tau, DMOrder order);



class DMatrix{
protected:
	
public:
	float data[3][4];
	
	DMatrix() {};
	
	//constructor to initialize diagonal elements 
	DMatrix(float d1, float d2, float d3)
	{	
		Clear();
		data[0][0] = d1; data[1][1] = d2; data[2][2] = d3;
	}

	//constructor to initialize diagonal and translation elements
	DMatrix(float d1, float d2, float d3, float t1, float t2, float t3)
	{	Clear();
		data[0][0] = d1; data[1][1] = d2; data[2][2] = d3;
		data[0][3] = t1; data[1][3] = t2; data[2][3] = t3;
	}

	//constructor given a pointer to data (copies the data)
	DMatrix(const float *p) { memcpy(data, p, 12 * sizeof(float)); };

	//constructor given an orthogonal axis and an angle in radians
	DMatrix(DMAxis axis, float angle)
	{	
		AxisRotation(*this, axis, angle);
	}

	//constructor given a set of Euler angles in radians
	DMatrix(float theta, float phi, float tau, DMOrder order)
	{	
		EulerRotation(*this, theta, phi, tau, order);
	}

	//destructor
	~DMatrix() {};

	///set to zero
	void Clear() {memset(data, 0, sizeof(float) * 12); };
	
	///set to identity
	void SetIdentity() {Clear(); data[0][0] = (float)1; data[1][1] = (float)1; data[2][2] = (float)1; };
	
	///element access
	inline float &operator()(int r, int c) {return data[r][c]; };
	
	///set to given data
	void Set(const float *p) { memcpy(data, p, 12 * sizeof(float)); };
	
	///set translation vector of Displacement matrix
	void SetTranslation(const float t1, const float t2, const float t3) { data[0][3] = t1; data[1][3] = t2; data[2][3] = t3; };
	
	///display matrix
	void Print(std::ostream &s) {s << data[0][0] << " " << data[0][1] << " " << data[0][2] << " " << data[0][3] << std::endl;
				  s << data[1][0] << " " << data[1][1] << " " << data[1][2] << " " << data[1][3] << std::endl;
				  s << data[2][0] << " " << data[2][1] << " " << data[2][2] << " " << data[2][3] << std::endl; };
	
	int getPrimitiveSize()				//equality
	{	
		return 12;
	}
	float * getPrimitive(float * dest){
		int k=0;
		for(int i=0; i < 3; i++){
			for(int j=0; j < 4; j++){
				dest[k] = data[i][j];
				k++;
			}
		}
		dest += k;
		return dest;
	}
	float * setPrimitive(float * src){
		int k=0;
		for(int i=0; i < 3; i++){
			for(int j=0; j < 4; j++){
				data[i][j] = src[k];
				k++;
			}
		}
		src += k;
		return src;
	}
	void print(){
		for(int i=0; i < 3; i++){
			for(int j=0; j < 4; j++){
				printf("%f ", data[i][j]);
			}
			printf("\n");
		}
	}
};

//------------- Operators Supported ---------------

//between Dmatrices				:	+   -   *   += 
//between Dmatrix and scalar	:	*   *=
//between Dmatrix and Vector3	:	*





//--------------------------------- Arithmetic Operators --------------------------------------

//Matrix-vector product with normalized coordinates (3x4 Matrix * 3x1 vector)
inline Vector3 operator*(const DMatrix &dm, const Vector3 &v)
{
	Vector3 r;
	DMatrix &m = *((DMatrix *)&dm);
	r.x = m(0,0) * v.x + m(0,1) * v.y + m(0,2) * v.z + m(0,3);
	r.y = m(1,0) * v.x + m(1,1) * v.y + m(1,2) * v.z + m(1,3);
	r.z = m(2,0) * v.x + m(2,1) * v.y + m(2,2) * v.z + m(2,3);
	return r;
}

//Matrix Subtraction
 
inline DMatrix operator-(const DMatrix &m1, const DMatrix &m2)
{
	DMatrix r;
	float *pr = (float *)r.data, *p1 = (float *)m1.data, *p2 = (float *)m2.data;
	for(int i = 0; i < 12; i++)
		*(pr++) = *(p1++) - *(p2++);
	return(r); 
}

//Matrix Addition
 
inline DMatrix operator+(const DMatrix &m1, const DMatrix &m2)
{
	DMatrix r;
	float *pr = (float *)r.data, *p1 = (float *)m1.data, *p2 = (float *)m2.data;
	for(int i = 0; i < 12; i++)
		*(pr++) = *(p1++) + *(p2++);
	return(r);
}

//Matrix Addition in place +=
 
inline void operator+=(DMatrix &m1, const DMatrix &m2)
{
	float *p1 = (float *)m1.data, *p2 = (float *)m2.data;
	for(int i = 0; i < 12; i++)
		*(p1++) += *(p2++);
}

//Matrix Subtraction in place -=
 
inline void operator-=(DMatrix &m1, const DMatrix &m2)
{
	float *p1 = (float *)m1.data, *p2 = (float *)m2.data;
	for(int i = 0; i < 12; i++)
		*(p1++) -= *(p2++);
}

//Scalar multiplication in place 

inline void operator*=(DMatrix &m, float s)
{	for(int i = 0; i < 12; i++) 
		((float *)m.data)[i] *= s;
}

//Scalar multiplication

inline DMatrix operator*(DMatrix m, float s)
{	m *= s;
	return m;
}

//Matrix-Matrix product 

inline DMatrix operator*(const DMatrix &dm1, const DMatrix &dm2)
{
	DMatrix m3, &m1 = *((DMatrix *)&dm1), &m2 = *((DMatrix *)&dm2);	//get non-const references to dm1 and dm2 to allow () operator
	for(int r = 0; r < 3; r++)
		for(int c = 0; c < 4; c++)
			m3(r,c) = m1(r,0) * m2(0,c) + m1(r,1) * m2(1,c) + m1(r,2) * m2(2,c);
	m3(0,3) += m1(0,3); m3(1,3) += m1(1,3); m3(2,3) += m1(2,3);
	return m3;
}

//---------------------------- Mathematical Functions ------------------------------------------

//Frobenius norm squared of matrix

inline float FrobNorm2(DMatrix &m)
{	float *p = (float *)m.data, r = 0;
	for(int i = 0; i < 12; i++)
	{	r += *p * *p;
		p++;
	}
	return r;
}

//Displacement matrix inverse

inline DMatrix Inverse(const DMatrix &dm)
{
	DMatrix r, &m = *((DMatrix *)&dm);
	r.Clear();
	float k1 = m(1,1) * m(2,2) - m(1,2) * m(2,1);						//calculate rotation matrix determinant
	float k2 = m(1,0) * m(2,2) - m(1,2) * m(2,0);
	float k3 = m(1,0) * m(2,1) - m(1,1) * m(2,0);
	float c = (float)1.0 / (m(0,0) * k1 - m(0,1) * k2 +	m(0,2) * k3);
	r(0,0) = c * k1;												//calculate inverse of rotation matrix
	r(0,1) = c * (m(0,2) * m(2,1) - m(0,1) * m(2,2));
	r(0,2) = c * (m(0,1) * m(1,2) - m(0,2) * m(1,1));
	r(1,0) = c * -k2;
	r(1,1) = c * (m(0,0) * m(2,2) - m(0,2) * m(2,0));
	r(1,2) = c * (m(0,2) * m(1,0) - m(0,0) * m(1,2));
	r(2,0) = c * k3;
	r(2,1) = c * (m(0,1) * m(2,0) - m(0,0) * m(2,1));
	r(2,2) = c * (m(0,0) * m(1,1) - m(0,1) * m(1,0));
	Vector3 v(-m(0,3), -m(1,3), -m(2,3));						//calculate new translation vector
	Vector3 t = r * v;
	r(0,3) = t.x;  r(1,3) = t.y; r(2,3) = t.z;
	return r;
}

//--------------------------------- Rotation Matrix functions -----------------------------------

//generate a rotation matrix about a given axis

inline void AxisRotation(DMatrix &m, DMAxis axis, float angle)
{	m.Clear();
	float c = t_cos(angle), s = t_sin(angle);
	switch(axis)
	{	case X : m(0,0) = 1; m(1,1) = c; m(1,2) = -s; m(2,1) = s; m(2,2) = c;
				 break;
		case Y : m(0,0) = c; m(0,2) = s; m(1,1) = 1; m(2,0) = -s; m(2,2) = c;
				 break;
		case Z : m(0,0) = c; m(0,1) = -s; m(1,0) = s; m(1,1) = c; m(2,2) = 1;
				 break;
	}
}

//Constructor given a set of Euler angles

inline void EulerRotation(DMatrix &m, float theta, float phi, float tau, DMOrder order)
{	
	DMatrix Rx(X, theta), Ry(Y, phi), Rz(Z, tau);
	switch (order)
	{	case XYZ:
			m = (Rx * Ry) * Rz;	break;
		case ZYX:
			m = (Rz * Ry) * Rx;	break;
		case YXZ:
			m = (Ry * Rx) * Rz;	break;
		case ZXY:
			m = (Rz * Rx) * Ry;	break;
		default:
			m = (Rx * Ry) * Rz;	break;
	}
}

#endif
