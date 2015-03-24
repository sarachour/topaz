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
//			 Jean-Yves Bouguet - jean-yves.bouguet@intel.com 
//  description : Body geometry description.
//  modified : 
//--------------------------------------------------------------


#ifndef BODYGEOMETRY_H
#define BODYGEOMETRY_H

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#include <vector>
#include <string>

#include "BodyPose.h"
#include "DMatrix.h"
#include "SmallVectors.h"
#include "pin_util.h"

#define N_PARTS 10
#define N_LENGTHS 18

//Preset rotation matrices
const float XRot90[12]  = {1.00f,  0.00f, 0.00f,  0.00f, 0.00f, 0.00f,  1.00f,  0.00f, 0.00f, -1.00f, 0.00f,  0.00f};
const float XRot180[12] = {1.00f,  0.00f, 0.00f,  0.00f, 0.00f, -1.00f, 0.00f,  0.00f, 0.00f, 0.00f,  -1.00f, 0.00f};
const float XRot270[12] = {1.00f,  0.00f, 0.00f,  0.00f, 0.00f, 0.00f,  -1.00f, 0.00f, 0.00f, 1.00f,  0.00f,  0.00f};
const float YRot90[12]  = {0.00f,  0.00f, -1.00f, 0.00f, 0.00f, 1.00f,  0.00f,  0.00f, 1.00f, 0.00f,  0.00f,  0.00f};
const float YRot180[12] = {-1.00f, 0.00f, 0.00f,  0.00f, 0.00f, 1.00f,  0.00f,  0.00f, 0.00f, 0.00f,  -1.00f, 0.00f};
const float ZRot180[12] = {-1.00f, 0.00f, 0.00f,  0.00f, 0.00f, -1.00f, 0.00f,  0.00f, 0.00f, 0.00f,  1.00f,  0.00f};

//Body geometry parameters:
class BodyParameters{
public:
	float limbs[N_PARTS][4];									//Radius of each body part
	float lengths[N_LENGTHS];									//Length of each body parts								
	
	BodyParameters(){
		
	};
	~BodyParameters(){};
	
	//Load the body shape parameters from a file
	bool InitBodyParameters(const std::string fname);	
	//TODO
	int getPrimitiveSize(){
		int size=0;
		size += N_PARTS*4;
		size += N_LENGTHS;
		return size;
	}		
	float * getPrimitive(float * dest){
		int k=0;
		for(int i=0; i < N_PARTS; i++){
			for(int j=0; j < 4; j++){
				dest[k] = limbs[i][j];
				k++;
			}
		}
		for(int i=0; i < N_LENGTHS; i++){
			dest[k] = lengths[i];
			k++;
		}
		dest += k;
		return dest;
	}
	float * setPrimitive(float * src){
		int k=0;
		for(int i=0; i < N_PARTS; i++){
			for(int j=0; j < 4; j++){
				limbs[i][j] = src[k];
				//FPUREL(limbs[i][j]);
				k++;
			}
		}
		for(int i=0; i < N_LENGTHS; i++){
			lengths[i] = src[k];
			//FPUREL(lengths[i]);
			k++;
		}
		src += k;
		return src;
	}
	void print(){
		printf("## Limbs\n");
		for(int i=0; i < N_PARTS; i++){
			for(int j=0; j < 4; j++){
				printf("%f ",limbs[i][j]);
				
			}
			printf("\n");
		}
		printf("## Lengths\n");
		for(int i=0; i < N_LENGTHS; i++){
			printf("%f ",lengths[i]);
		}
		printf("\n");
	}			
};


//primitive cylinder object:
class KTCylinder{
public:
	float bottom, top, length;									//bottom,top radius and length of the cylinder
	DMatrixf pose;										//3D pose of the cylinder
	
	KTCylinder(){
		//FPUREL(bottom); FPUREL(top); FPUREL(length);
	};
	~KTCylinder(){};
	
	//Construct a cylinder given top and bottom radii, and length
	KTCylinder(float b, float t, float l) {
		top = t; 
		bottom = b; 
		length = l; 
	};
	
	//Set cylinder values given top and bottom radii, and length
	void Set(float b, float t, float l) {
		top = t; 
		bottom = b; 
		length = l; 
		
		//FPUREL(bottom); FPUREL(top); FPUREL(length);
	};
	//TODO
	//TODO
	int getPrimitiveSize(){
		int size=0;
		//10
		size += 3;
		size += pose.getPrimitiveSize();
		return size;
	}		
	float * getPrimitive(float * dest){
		dest[0] = bottom;
		dest[1] = top;
		dest[2] = length;
		dest += 3;
		dest = pose.getPrimitive(dest);
		return dest;
	}
	float * setPrimitive(float * src){
		bottom = src[0];
		top = src[1];
		length = src[2];
		
		//FPUREL(bottom); FPUREL(top); FPUREL(length);
		
		src += 3;
		src = pose.setPrimitive(src);
		return src;
	}
	void print(){
		printf("bot: %f, top %f, length: %f\n", bottom, top, length);
		pose.print();
	}
};

//primitive body geometry object:
class BodyGeometry{
private:
	std::vector<KTCylinder> mCylinders;							//Each body part is modeled as a conic cylinder
	BodyParameters mParams;										//Body shape parameters
	
	//TEMPORARY VARIABLES, DO NOT COPY
	std::vector<Vector3f> mCentersA, mCentersB;					//centers of spheres used to approximate the conic cylinders during intersection test
	std::vector<float> mRadiiA, mRadiiB;						//radii of spheres
	//TEMPORARY VARIABLES END
	
	bool IntersectingCylinders(KTCylinder &cylA, KTCylinder &cylB);

public:
	BodyGeometry() { mCylinders.resize(N_PARTS); };
	~BodyGeometry(){};

	//Load Body Parameters from a file
	bool InitBodyShape(std::string fname) {return mParams.InitBodyParameters(fname); };
	
	//Get/set the ith body part
	KTCylinder &operator()(int i) {return mCylinders[i]; };	
	const KTCylinder &operator()(int i) const {return mCylinders[i]; };
	KTCylinder * get(int i){return &mCylinders[i]; };
	//Get body parameters
	BodyParameters &Parameters() {return mParams; };
	
	//Generate body geometry from a given pose
	void ComputeGeometry(BodyPose &angles, BodyParameters &params);	
	void ComputeGeometryTask(BodyPose *angles, BodyParameters *params);	

	//returns the number of body parts
	int GetBodyPartCount() const {return (int)mCylinders.size();};		

	//returns true if no pair of body parts intersect
	bool Valid();		
	//TODO
	int getPrimitiveSize(){
		int size=0;
		//10
		for(int i=0; i < N_PARTS; i++){
			size += mCylinders[i].getPrimitiveSize();
		}
		size += mParams.getPrimitiveSize();
		return size;
	}		
	float * getPrimitive(float * dest){
		for(int i=0; i < N_PARTS; i++){
			dest= mCylinders[i].getPrimitive(dest);
		}
		dest = mParams.getPrimitive(dest);
		return dest;
	}
	float * setPrimitive(float * src){
		mCylinders.resize(N_PARTS);
		for(int i=0; i < N_PARTS; i++){
			src= mCylinders[i].setPrimitive(src);
		}
		src = mParams.setPrimitive(src);
		return src;
	}	
	void print(){
		printf("# Geometry");
		printf("## Params\n");
		mParams.print();
		printf("## Cylinders\n");
		for(int i=0; i < N_PARTS; i++){
			mCylinders[i].print();
		}
	}				
};



#endif

