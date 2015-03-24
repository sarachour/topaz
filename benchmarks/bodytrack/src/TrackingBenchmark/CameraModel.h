//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//               
//	 © 2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : CameraModel.h
//  author : Jean-Yves Bouguet - jean-yves.bouguet@intel.com
//			 Scott Ettinger - scott.m.ettinger@intel.com
//  description : Camera Model description (single and multiple).
//  modified : 
//--------------------------------------------------------------


#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#include <vector>
#include "SmallVectors.h"
#include "DMatrix.h"

#define CAMERA_COUNT	4

class Point{
public:
	float d[2];
	Point(){
		//FPUREL(d[0]);FPUREL(d[1]);
	};
	float x() const{return d[0];}
	float y() const{return d[1];}
	Point(float vx, float vy){
		d[0] = vx; 
		d[1]=vy;
		//FPUREL(d[0]); FPUREL(d[1]);
	};
	~Point(){};
	inline void Set(float vx, float vy){
		d[0] = vx; d[1]=vy;
		//FPUREL(d[0]); FPUREL(d[1]);
	};
	std::vector<float> toPrimitive();
	int getPrimitiveLength();
	
	int getPrimitiveSize(){
		return 2;
	};
	float * getPrimitive(float * dest){
		dest[0] = d[0];
		dest[1] = d[1];
		dest+=2;
		return dest;
	}
	float * setPrimitive(float * src){
		d[0] = src[0];
		d[1] = src[1];
		src+=2;
		//FPUREL(d[0]);FPUREL(d[1]);
		return src;
	}
	void print(){
		printf("%f,%f\n", d[0],d[1]);
	}
};

//Single camera model:
//For more information, check http://www.vision.caltech.edu/bouguetj/calib_doc
class Camera{
public:
	Point fc;												//Focal length
	Point cc;												//Principal point
	float alpha_c;											//skew coefficient
	float kc[5];											//Distortion parameters 
	float Rc_ext[3][3];										//Rotation matrix (extrinsics)
	Vector3f omc_ext;										//Rotation vector (extrinsics)
    Vector3f Tc_ext;										//Translation vector (extrinsics)
	DMatrixf mc_ext;									//Full extrinsics (=[Rc_ext Tc_ext])
    Vector3f eye;										//Position of the camera in the world reference frame
	Camera(){};
	~Camera(){};
	bool LoadParams(const char *fname);						//Load the camera parameters from a file
	
	int getPrimitiveSize(){
		int size=0;
		size += fc.getPrimitiveSize();
		size += cc.getPrimitiveSize();
		size += 1; //alpha_c
		size += 5; //kc_c
		size += 9; //Rc_ext
		size += omc_ext.getPrimitiveSize();
		size += Tc_ext.getPrimitiveSize();
		size += mc_ext.getPrimitiveSize();
		size += eye.getPrimitiveSize();
		return size;
	}
	float * getPrimitive(float * dest){
		dest = fc.getPrimitive(dest);
		dest = cc.getPrimitive(dest);
		dest = omc_ext.getPrimitive(dest);
		dest = Tc_ext.getPrimitive(dest);
		dest = mc_ext.getPrimitive(dest);
		dest = eye.getPrimitive(dest);
		
		int k=0;
		dest[k] = alpha_c; k++;
		for(int i=0; i < 5; i++){
			dest[k] = kc[i];
			k++;
		}
		for(int i=0; i < 3; i++){
			for(int j=0; j < 3; j++){
				dest[k] = Rc_ext[i][j];
				k++;
			}
		}
		dest += k;
		return dest;
	}
	float * setPrimitive(float * src){
		src = fc.setPrimitive(src);
		src = cc.setPrimitive(src);
		src = omc_ext.setPrimitive(src);
		src = Tc_ext.setPrimitive(src);
		src = mc_ext.setPrimitive(src);
		src = eye.setPrimitive(src);
		
		int k=0;
		alpha_c = src[k]; k++;
		//FPUREL(alpha_c);
		for(int i=0; i < 5; i++){
			kc[i] = src[k];
			//FPUREL(kc[i]);
			k++;
		}
		for(int i=0; i < 3; i++){
			for(int j=0; j < 3; j++){
				Rc_ext[i][j] = src[k];
				//FPUREL(Rc_ext[i][j]);
				k++;
			}
		}
		src += k;
		return src;
	}
	void print(){
		printf("## fc\n");
		fc.print();
		printf("## cc\n");
		cc.print();
		printf("## omc\n");
		omc_ext.print();
		printf("## Tc\n");
		Tc_ext.print();
		printf("## mc\n");
		mc_ext.print();
		printf("## eye\n");
		eye.print();
		printf("## alpha_c: %f\n", alpha_c);
		printf("### kc\n");
		for(int i=0; i < 5; i++){
			printf("%f ", kc[i]);
		}
		printf("\n### Rc_ext\n");
		for(int i=0; i < 3; i++){
			for(int j=0; j < 3; j++){
				printf("%f ", Rc_ext[i][j]);
			}
			printf("\n");
		}
	}
};


//Multiple Cameras:
class MultiCamera{
private:
	std::vector<Camera> mCameras;							//Multiple cameras
public:
	MultiCamera() {};
	~MultiCamera(){};
	void SetCameras(int n) {mCameras.resize(n); };			//set number of cameras
	Camera &operator()(int i){return mCameras[i];};			//Get the ith camera
	Camera * get(int i){return &mCameras[i];}
	std::vector<Camera> &operator()(){return mCameras;};	//Get all the cameras
	int GetCameraCount(){return (int) mCameras.size();};	//returns the number cameras
	
	int getPrimitiveSize(){
		int size=0;
		for(int i=0; i < mCameras.size(); i++)
			size += mCameras[i].getPrimitiveSize();
		return size;
	}
	float * getPrimitive(float * dest){
		for(int i=0; i < mCameras.size(); i++){
			dest = mCameras[i].getPrimitive(dest);
		}
		return dest;
	}
	float * setPrimitive(float * src, int n){
		mCameras.resize(n);
		for(int i=0; i < n; i++){
			src = mCameras[i].setPrimitive(src);
		}
		return src;
	}
	void print(){
		for(int i=0; i < mCameras.size(); i++){
			mCameras[i].print();
		}
	}
};


#endif
