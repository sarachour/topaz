//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//                  
//	  2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : TrackingModel.h
//  author : Scott Ettinger - scott.m.ettinger@intel.com
//  description : Observation model for kinematic tree body 
//				  tracking.  

//				  Derived classes implement multi-threaded 
//				  versions of this class, however some of the
//				  capability for multithreading is implemented
//				  here.  Memory is allocated for separate threads
//				  if necessary and the likelihood function takes 
//				  a thread id.  Defaults are for 1 thread.
//				  
//  modified : 
//--------------------------------------------------------------

#ifndef TRACKINGMODEL_H
#define TRACKINGMODEL_H

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#include <vector>
#include <string>
#include <FlexImage.h>
#include "ImageMeasurements.h"
#include "ImageProjection.h"
#include "CameraModel.h"
#include "BodyGeometry.h"
#include "BinaryImage.h"

#define FlexImage8u  FlexImage<Im8u,1>
#define FlexImage32f FlexImage<Im32f,1>

class TrackingModel{
protected:
	BinaryImage * t_mFGMaps;
	FlexImage8u * t_mEdgeMaps;
	//= # cameras
	std::vector<BinaryImage >				mFGMaps;			// Background segmented images from each camera 
	//= # cameras
	std::vector<FlexImage8u>				mEdgeMaps;			// edge processed images from each camera
	std::vector<std::vector<float> >		mStdDevs;			// standard deviations for each layer of annealed particle filtering
	BodyPose					mPoses;				// Body poses and displacement parameters
	BodyGeometry				mBodies;			// Body geometry objects
	
	MultiCamera								mCameras;			// All cameras
	MultiCameraProjectedBody	mProjections;		// Image projections of the body on all cameras
	ImageMeasurements			mImageMeasurements;	// Image measurement objects for error computation
	int										mNCameras;			// number of cameras used
	std::string								mPath;				// dataset path

	//Load Camera(s) parameters from configuration files
	bool InitCameras(std::vector<std::string> &calibFiles);

	//Initialize body model geometry parameters and initial pose from configuration file
	bool InitGeometry(const std::string &fname) {return mBodies.InitBodyShape(fname); };

	//Load initial state from a file
	bool LoadInitialState(const std::string &fname) {return mPoses.Initialize(fname); };

	//Load Body Pose Parameters from a file
	bool LoadPoseParameters(const std::string &fname) {return mPoses.InitParams(fname); };

	//Generate an edge map from the original camera image
	virtual void CreateEdgeMap(const FlexImage8u &src, FlexImage8u &dst);
public:

	TrackingModel();
	virtual ~TrackingModel() {};

	int SetupPose(std::vector<float> & v, int ncams, bool*valid, float * proj);
	int getAbbPrimitive(float * prim);
	//Get function for standard deviations of pose motion
	std::vector<std::vector<float> > &StdDevs() {return mStdDevs; };

	//Load body parameters, camera calibrations, and initial state from dataset
	bool Initialize(const std::string &path, int cameras, int layers);

	//Load and process new observation data from image files for a given time(frame).  Generates edge maps from the raw image files.
	virtual bool GetObservation(float timeval);

	//Returns body pose state - required by particle filter for initialization only
	std::vector<float> InitialState() {return mPoses.Pose(); };

	//Calculates Log likelihood for a given set of body pose angles (and translation - all are in the vector)
	float LogLikelihood(float *v, int n, bool *valid);
	float LogLikelihoodMain(std::vector<float> & v, bool &valid);

	//Draw body geometry onto camera image and save as BMP
	bool OutputBMP(const std::vector<float> &pose, int frame);
	
	int getPrimitiveSize();
	void getPrimitive(float * dest);
	void setPrimitive(float * src, int nCams);
	int getImagePrimitiveSize();
	void getImagePrimitive(char * dest);
	void setImagePrimitive(char * src, int nCams, int w, int h, int b);
	int getWidth(){
		return mFGMaps[0].Width();
	}
	int getHeight(){
		return mFGMaps[0].Height();
	}
	int getBytesPerPixel(){
		return mEdgeMaps[0].BytesPerPixel();
	}
	int NCameras(){
		return mNCameras;
	}
};


#endif
