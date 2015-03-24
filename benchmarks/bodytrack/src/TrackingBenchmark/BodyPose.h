//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//                          
//	 © 2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : BodyPose.h
//  author : Jean-Yves Bouguet - jean-yves.bouguet@intel.com
//			 Scott Ettinger - scott.m.ettinger@intel.com
//  description : Body pose description.
//  modified : 
//--------------------------------------------------------------


#ifndef BODYPOSE_H
#define BODYPOSE_H

#if defined(HAVE_CONFIG_H)
# include "config.h"
#include "stdio.h"

#endif

#include <string>
#include <vector>
#include "pin_util.h"

#define N_ANGLES 31

//Pose parameters
class PoseParams{
private:
	std::vector<float> stdAngle;							//Standard deviations on the pose angles
	std::vector<float> minAngles;							//minimum values of the pose angles
	std::vector<float> maxAngles;							//maximum values of the pose angles
public:
	PoseParams(){};
	~PoseParams(){};
	bool Initialize(std::string fname);						//Initialize the pose parameters from a file
	float &PoseStd(int i){return stdAngle[i];};				//ith entry of the standard deviation vector
	float &PoseMin(int i){return minAngles[i];};			//ith entry of the minimum vector
	float &PoseMax(int i){return maxAngles[i];};			//ith entry of the maximum vector
	std::vector<float> &stdVec(){return stdAngle;};			//the standard deviation vector
	std::vector<float> &minVec(){return minAngles;};		//the minimum vector
	std::vector<float> &maxVec(){return maxAngles;};		//the maximum vector
	//TODO
	int getPrimitiveSize(){
		int size=0;
		//10
		size += stdAngle.size();
		size += minAngles.size();
		size += maxAngles.size();
		return size;
	}		
	float * getPrimitive(float * dest){
		for(int i=0; i < N_ANGLES; i++){
			dest[i] = stdAngle[i];
		}
		dest += N_ANGLES;
		for(int i=0; i < N_ANGLES; i++){
			dest[i] = minAngles[i];
		}
		dest += N_ANGLES;
		for(int i=0; i < N_ANGLES; i++){
			dest[i] = maxAngles[i];
		}
		dest += N_ANGLES;
		return dest;
	}
	float * setPrimitive(float * src){
		stdAngle.resize(N_ANGLES);
		for(int i=0; i < N_ANGLES; i++){
			stdAngle[i] = src[i];
			//FPUREL(stdAngle[i]);
		}
		src += N_ANGLES;
		minAngles.resize(N_ANGLES);
		for(int i=0; i < N_ANGLES; i++){
			minAngles[i] = src[i];
			//FPUREL(minAngles[i]);
		}
		src += N_ANGLES;
		maxAngles.resize(N_ANGLES);
		for(int i=0; i < N_ANGLES; i++){
			maxAngles[i] = src[i];
			//FPUREL(maxAngles[i]);
		}
		src += N_ANGLES;
		return src;
	}
	void print(){
		for(int i=0; i < N_ANGLES; i++){
			printf("std %d: %f\n", i, stdAngle[i]);
		}
		for(int i=0; i < N_ANGLES; i++){
			printf("min %d: %f\n", i, minAngles[i]);
		}
		for(int i=0; i < N_ANGLES; i++){
			printf("max %d: %f\n", i, maxAngles[i]);
		}
		
	}
};

//Body pose
class BodyPose{
private:
	PoseParams mParams;										//Body Pose parameters
	std::vector<float> mAngles;								//Angles describing the body pose
public:
	BodyPose(){};
	~BodyPose(){};
	bool Initialize(std::string fname);						//Initialize the body pose from a file
	bool InitParams(std::string fname)
	{	return mParams.Initialize(fname);};					//Initialize the pose parameters from a file
	float &operator()(int i){return mAngles[i];};			//Get or set the ith body pose angle entry
	float get(int i){return mAngles[i];};			//Get or set the ith body pose angle entry
	std::vector<float> Pose() {return mAngles;};			//Get or set the entire set of angles
	PoseParams &Params() {return mParams; };				//Get the pose parameters
	void Set(float *angle_values,int n);					//Set the set of angles from a float* and the size
	void Set(const std::vector<float> &v) {mAngles = v;};
	int Size(){return (int) mAngles.size();};				//returns the number of body parts
	PoseParams	&PoseParameters(){return mParams;};
	bool Valid(PoseParams &params);	
	bool ValidTask(PoseParams *params);	
	//TODO
	int getPrimitiveSize(){
		int size=0;
		//10
		size += mAngles.size();
		size += mParams.getPrimitiveSize();
		return size;
	}		
	float * getPrimitive(float * dest){
		dest = mParams.getPrimitive(dest);
		for(int i=0; i < N_ANGLES; i++){
			dest[i] = mAngles[i];
		}
		dest += N_ANGLES;
		return dest;
	}
	float * setPrimitive(float * src){
		src = mParams.setPrimitive(src);
		mAngles.resize(N_ANGLES);
		for(int i=0; i < N_ANGLES; i++){
			mAngles[i] = src[i];
			//FPUREL(mAngles[i]);
		}
		src += N_ANGLES;
		return src;
	}
	void print(){
		printf("#BodyPose\n");
		printf("## Params\n");
		mParams.print();
		printf("## Angles\n");
		for(int i=0; i < N_ANGLES; i++){
			printf("  %d] %f\n", i, mAngles[i]);
		}
	}
};

#endif
