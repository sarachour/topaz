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
//  description : 
//  modified : 
//--------------------------------------------------------------

#ifndef VECTOR3_H
#define VECTOR3_H

#include "relax_lib.h"

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#include <math.h>
#include <iostream>

#define Vector3f Vector3<float>
#define Vector3d Vector3<double>
#define Vector3i Vector3<int>

//3D vector 
template<class T> 
class Vector3 {
public:
	T d[3];
	///constructors
	Vector3() {
		//UNKUREL(d[0],T); UNKUREL(d[1],T); UNKUREL(d[2],T);
	};
	Vector3(const T xv, const T yv, const T zv)	{ 
		d[0] = xv;  d[1] = yv; d[2] = zv; 
		//UNKUREL(d[0],T); UNKUREL(d[1],T); UNKUREL(d[2],T);
	};
	Vector3(const Vector3 &v)	{ 
		d[0] = v.d[0]; d[1] = v.d[1]; d[2] = v.d[2]; 
		//UNKUREL(d[0],T); UNKUREL(d[1],T); UNKUREL(d[2],T);
	};		//copy constructor

	~Vector3() {
		
	};

	///basic vector operations

	void Set(const T xv, const T yv, const T zv)  { 
		d[0] = xv;  d[1] = yv; d[2] = zv; 
		//UNKUREL(d[0],T); UNKUREL(d[1],T); UNKUREL(d[2],T);
	};

	inline const Vector3 operator-(const Vector3 &v) 	//subtraction
	{	Vector3 r(d[0] - v.d[0], d[1] - v.d[1], d[2] - v.d[2]); 
		return(r);
	};

	inline Vector3 operator+(const Vector3 &v)			//addition
	{	Vector3 r(d[0] + v.d[0], d[1] + v.d[1], d[2] + v.d[2]); 
		return(r);
	}

	inline void operator+=(const Vector3 &v)			//in place addition 
	{	d[0] += v.d[0]; d[1] += v.d[1]; d[2] += v.d[2];
	}

	inline void operator-=(const Vector3 &v)			//in place subtraction
	{	d[0] -= v.d[0]; d[1] -= v.d[1]; d[2] -= v.d[2];
	}

	inline Vector3 operator*(const T s)					//scalar multiplication
	{	Vector3 r(d[0] * s, d[1] * s, d[2] * s); 
		return(r);
	}

	inline Vector3 operator/(const T s)					//scalar division
	{	Vector3 r(d[0] / s, d[1] / s, d[2] / s); 
		return(r);
	}

	inline void operator*=(const T s)					//in place scalar multiplication 
	{	d[0] *= s; d[1] *= s; d[2] *= s;
	}

	inline void operator/=(const T s)					//in place scalar division
	{	d[0] /= s; d[1] /= s; d[2] /= s;
	}


	inline T Dot(const Vector3 &v)						//dot product with another vector
	{	return(v.d[0] * d[0] + v.d[1] * d[1] + v.d[2] * d[2]);
	}

	inline Vector3 Norm()								//return normalized vector
	{	T mag = t_sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
		return(*this * (1/mag));
	}

	inline Vector3 operator*(const Vector3 &v)			//cross product with another vector
	{	return(Vector3( d[1] * v.d[2] - d[2] * v.d[1], 
						 d[2] * v.d[0] - d[0] * v.d[2],
						 d[0] * v.d[1] - d[1] * v.d[0]) );
	}

	inline T Mag()										//magnitude
	{	return t_sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
	}

	inline T MagSq()									//magnitude squared
	{	return d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
	}

	void Print(std::ostream &s)
	{	s << "(" << d[0] << ", " << d[1] << ", " << d[2] << ")";
	}

	inline T &operator[](const int i)									//vector access by index
	{	
		T *p = &d[0];
		return p[i];
	}
};

//print vector to a stream
template<class T>
inline std::ostream &operator<<(std::ostream &s, Vector3<T> &v)
{	v.Print(s);
	return s;
}


#endif

