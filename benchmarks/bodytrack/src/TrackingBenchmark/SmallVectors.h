//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//
//	 © 2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : 
//  author : Scott Ettinger - scott.m.ettinger@intel.com
//  description : // simple templated 2D and 3D vector classes
//  modified : 
//--------------------------------------------------------------


#ifndef SMALL_VECTORS_H
#define SMALL_VECTORS_H

#include <math.h>
#include <iostream>
#include "stdio.h"
#include "relax_lib.h"
//-- defines for frequently used instantiations 

#define Vector2f Vector2
#define Vector3f Vector3

//------------------ 3D vector ------------------- 

//3D vector 
 
class Vector3 {
public:
	float x, y, z;
	//c onstructors
	Vector3() {};
	inline Vector3(float xv, float yv, float zv)  { x = xv;  y = yv; z = zv; };
	inline Vector3(float *p) {x = p[0]; y = p[1]; z = p[2]; };

	~Vector3() {};

	// functions
	inline void Set(float xv, float yv, float zv)  { x = xv;  y = yv; z = zv; };

	// simple vector operations
	
	inline Vector3 operator-(const Vector3 &v) const		//subtraction
	{	return Vector3(x - v.x, y - v.y, z - v.z); 
	};

	inline Vector3 operator+(const Vector3 &v) const		//addition
	{	return Vector3(x + v.x, y + v.y, z + v.z); 
	}

	inline void operator+=(const Vector3 &v)				//in place addition 
	{	x += v.x; y += v.y; z += v.z;
	}

	inline void operator-=(const Vector3 &v)				//in place subtraction
	{	x -= v.x; y -= v.y; z -= v.z;
	}

	inline Vector3 operator*(const float s)	const				//scalar multiplication
	{	return Vector3(x * s, y * s, z * s); 
	}

	inline Vector3 operator/(const float s)	const				//scalar division
	{	return Vector3(x / s, y / s, z / s); 
	}

	inline void operator*=(const float s)						//in place scalar multiplication 
	{	x *= s; y *= s; z *= s;
	}

	inline void operator/=(const float s)						//in place scalar division
	{	x /= s; y /= s; z /= s;
	}

	inline float Dot(const Vector3 &v) const					//dot product with another vector
	{	return(v.x * x + v.y * y + v.z * z);
	}

	inline Vector3 Norm() const 							//return normalized vector
	{	float mag = t_sqrt(x*x + y*y + z*z);
		return(*this * (1/mag));
	}

	inline Vector3 operator*(const Vector3 &v) const		//cross product with another vector
	{	return(Vector3( y * v.z - z * v.y, 
						 z * v.x - x * v.z,
						 x * v.y - y * v.x) );
	}

	inline float Mag() const									//magnitude
	{	return t_sqrt(x * x + y * y + z * z);
	}

	inline float MagSq() const									//magnitude squared
	{	return x * x + y * y + z * z;
	}

	inline void Print() const
	{	std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
	}

	inline float &operator[](const int i)						//vector access by index
	{	
		float *p = &x;
		return p[i];
	}

	inline bool operator==(const Vector3 &v)				//equality
	{	return v.x == x && v.y == y && v.z == z;
	}
	int getPrimitiveSize()				//equality
	{	
		return 3;
	}
	float * getPrimitive(float * dest){
		dest[0] = x;
		dest[1] = y;
		dest[2] = z;
		dest += 3;
		return dest;
	}
	float * setPrimitive(float * src){
		x = src[0];
		y = src[1];
		z = src[2];
		src += 3;
		return src;
	}
	void print(){
		printf("%f %f %f\n", x,y,z);
	}
};


//multiplication by a prefix scalar 

inline Vector3 operator*(const float s, const Vector3 &v)
{	return Vector3(s * v.x, s * v.y, s * v.z);
}


//absolute value

inline Vector3 abs(Vector3 &v)
{	return Vector3(fabs(v.x), fabs(v.y), fabs(v.z));
}


//------------------ 2D vector ------------------- 

 
class Vector2 {
public:
	float x, y;
	//constructors
	Vector2() {};
	inline Vector2(float xv, float yv)  { x = xv;  y = yv; };
	inline Vector2(float *p) {x = p[0]; y = p[1]; };
	~Vector2() {};
	
	//functions

	inline void Set(float xv, float yv)  { x = xv;  y = yv; };

	//simple vector operations
	
	inline Vector2 operator-(const Vector2 &v)	const		//subtraction
	{	return Vector2(x - v.x, y - v.y); 
	};

	inline Vector2 operator+(const Vector2 &v)	const		//addition
	{	return Vector2(x + v.x, y + v.y); 
	}

	inline Vector2 operator*(const float s)	const				//scalar multiplication
	{	return Vector2(x * s, y * s); 
	}

	inline Vector2 operator/(const float s)	const				//scalar division
	{	return Vector2(x / s, y / s); 
	}

	inline float Dot(const Vector2 &v)	const					//dot product with another vector
	{	return(v.x * x + v.y * y);
	}

	inline Vector2 Norm()	const							//return normalized vector
	{	float mag = t_sqrt(x*x + y*y);
		return(*this * (1/mag));
	}

	inline float operator*(const Vector2 &v) const				//cross product with another vector
	{	return(x * v.y - y * v.x);
	}

	inline float Mag()	const									//magnitude
	{	return t_sqrt(x * x + y * y);
	}

	inline float MagSq() const									//magnitude squared
	{	return x * x + y * y;
	}

	float &operator[](int i) 									//vector access by index
	{	float *p = &x;
		return p[i];
	}

	bool operator==(Vector2 &v) const
	{	return (x == v.x && y == v.y);
	}

	inline void Print() const
	{	std::cout << "(" << x << ", " << y << ")" << std::endl;
	}
	

};

//multiplication by a prefix scalar 

inline Vector2 operator*(const float s, const Vector2 &v)
{	return Vector2(s * v.x, s * v.y);
}

//absolute value

inline Vector2 abs(Vector2 &v)
{	return Vector2(fabs(v.x), fabs(v.y));
}

#endif
