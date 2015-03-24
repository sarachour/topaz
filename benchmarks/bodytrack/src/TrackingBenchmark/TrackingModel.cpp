//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//                  
//	  2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : TrackingModel.cpp
//  author : Scott Ettinger - scott.m.ettinger@intel.com
//  description : Observation model for kinematic tree body 
//				  tracking.
//				  
//  modified : 
//--------------------------------------------------------------

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include "TrackingModel.h"
#include "CovarianceMatrix.h"
#include "FlexLib.h"
#include "system.h"

#ifndef uint
#define uint unsigned int
#endif

using namespace std;

//templated conversion to string with field width
template<class T>
inline string str(T n, int width = 0, char pad = '0')
{	stringstream ss;
	ss << setw(width) << setfill(pad) << n;
	return ss.str();
}

// -------------------------------- Initialization ----------------------------

//Constructor initializes for single thread code
TrackingModel::TrackingModel()
{	
	
}



//Load camera calibration parameters from files
bool TrackingModel::InitCameras(vector<string> &fileNames)
{	mCameras.SetCameras((int)fileNames.size());									//set number of cameras
	for(int i = 0; i < (int)fileNames.size(); i++)
		if(!mCameras(i).LoadParams(fileNames[i].c_str()))						//load each camera calibration 
			return false;
	return true;
}

//Initialize the tracking model parameters
bool TrackingModel::Initialize(const string &path, int cameras, int layers)
{
	mPath = path;
	mNCameras = cameras;
	mFGMaps.resize(cameras);
	mEdgeMaps.resize(cameras);
	vector<string> calibFiles(cameras);											//set camera calibration file paths
	for(int i = 0; i < cameras; i++)
		calibFiles[i] = path + "CALIB" + DIR_SEPARATOR + "Camera" + str(i + 1) + ".cal";
	if(!InitCameras(calibFiles)) return false;									//initialize camera calibration parameters				
	if(!InitGeometry(path + "BodyShapeParameters.txt")) return false;			//initialize geometry parameters
	if(!LoadInitialState(path + "InitialPose.txt")) return false;				//initialize body pose angles and translations
	if(!LoadPoseParameters(path + "PoseParameters.txt")) return false;			//initialize pose statistics
	GenerateStDevMatrices(layers, mPoses.Params(), mStdDevs);				//generate annealing rates for particle filter using pose parameters
	return true;
}

// ------------------------- Likelihood computation ---------------------------

//Calculate the likelihood for the current observation
int TrackingModel::getAbbPrimitive(float * v)
{
	mImageMeasurements.getPrimitive(v);
	return 512;
}
int TrackingModel::SetupPose(vector<float> & v, int ncameras, bool * valid, float * vproj)
{
	BodyPose *pose = &mPoses;										//get workspace
	BodyGeometry *body = &mBodies;
	MultiCameraProjectedBody *projections = &mProjections;
	//printf("made it into loglik\n");
	pose->Set(v);															//set pose angles and translation
	*valid = false;

	if(!pose->ValidTask(&pose->Params()))										//test for a valid pose (reject impossible body angles)
		return -1;
	body->ComputeGeometryTask(pose, &body->Parameters());		
	if(!body->Valid())														//test for valid geometry (reject poses with intersecting body parts)
		return -1;
	//body is ok, cameras - unsure.
	projections->ImageProjectionTask(body, &mCameras);
	projections->getPrimitive(vproj);
	*valid = true;
	return 64;
}

//Calculate the likelihood for the current observation
float TrackingModel::LogLikelihood(float * v, int n, bool * valid)
{
	BodyPose *pose = &mPoses;										//get workspace
	BodyGeometry *body = &mBodies;
	MultiCameraProjectedBody *projections = &mProjections;
	ImageMeasurements *measurements = &mImageMeasurements;
	//printf("made it into loglik\n");
	printf("TR SET\n");
	pose->Set(v, n);															//set pose angles and translation
	*valid = false;
	
	printf("TR VALID\n");
	if(!pose->ValidTask(&pose->Params()))										//test for a valid pose (reject impossible body angles)
		return -1;
	
	printf("TR COMP\n");
	body->ComputeGeometryTask(pose, &body->Parameters());		
	if(!body->Valid())														//test for valid geometry (reject poses with intersecting body parts)
		return -1;
	//body is ok, cameras - unsure.
	projections->ImageProjectionTask(body, &mCameras);
	//compute projected 2D points into each camera image for each body part
	float err = measurements->ImageErrorEdgeTask(t_mEdgeMaps, projections, mNCameras);		//compute cylinder edge map term
	err += measurements->ImageErrorInsideTask(t_mFGMaps, projections, mNCameras);				//compute silhouette term
	*valid = true;
	return -err;
}

//Calculate the likelihood for the current observation
float TrackingModel::LogLikelihoodMain(vector<float> & v, bool & valid)
{
	BodyPose &pose = mPoses;										//get workspace
	BodyGeometry &body = mBodies;
	MultiCameraProjectedBody &projections = mProjections;
	ImageMeasurements &measurements = mImageMeasurements;

	pose.Set(v);															//set pose angles and translation
	valid = false;

	if(!pose.Valid(mPoses.Params()))										//test for a valid pose (reject impossible body angles)
		return 0;
	body.ComputeGeometry(pose, mBodies.Parameters());		
	if(!body.Valid())														//test for valid geometry (reject poses with intersecting body parts)
		return 0;
	//body is ok, cameras - unsure.
	projections.ImageProjection(body, mCameras);
								//compute projected 2D points into each camera image for each body part
	float err = measurements.ImageErrorEdge(mEdgeMaps, projections);		//compute cylinder edge map term
	err += measurements.ImageErrorInside(mFGMaps, projections);				//compute silhouette term
	valid = true;
	return -err; // negative score
}


//------------------------ Observation processing -----------------------------

//Separable 7x7 gaussian filter
inline void GaussianBlur(FlexImage8u &src, FlexImage8u &dst)
{
	float k[] = {0.12149085090552f, 0.14203719483447f, 0.15599734045770f, 0.16094922760463f, 0.15599734045770f, 0.14203719483447f, 0.12149085090552f};
	FlexImage8u tmp;
	FlexFilterRowV(src, tmp, k, 7);											//separable gaussian convolution using kernel k
	FlexFilterColumnV(tmp, dst, k, 7);
}

//Calculate gradient magnitude and threshold to binarize
inline FlexImage8u GradientMagThreshold(const FlexImage8u &src, float threshold)
{
	FlexImage8u r(src.Size());
	ZeroBorder(r);
	for(int y = 1; y < src.Height() - 1; y++)																					//for each pixel
	{	Im8u *p = &src(1,y), *ph = &src(1,y - 1), *pl = &src(1,y + 1), *pr = &r(1,y);
		for(int x = 1; x < src.Width() - 1; x++)
		{	float xg = -0.125f * ph[-1] + 0.125f * ph[1] - 0.250f * p[-1] + 0.250f * p[1] - 0.125f * pl[-1] + 0.125f * pl[1];	//calc x and y gradients
			float yg = -0.125f * ph[-1] - 0.250f * ph[0] - 0.125f * ph[1] + 0.125f * pl[-1] + 0.250f * pl[0] + 0.125f * pl[1];
			float mag = xg * xg + yg * yg;																						//calc magnitude and threshold
			*pr = (mag < threshold) ? 0 : 255;
			p++; ph++; pl++; pr++;
		}
	}
	return r;
}

//Generate an edge map from the original camera image
void TrackingModel::CreateEdgeMap(const FlexImage8u &src, FlexImage8u &dst)
{
	FlexImage8u gr = GradientMagThreshold(src, 16.0f);							//calc gradient magnitude and threshold
	GaussianBlur(gr, dst);														//Blur to create distance error map
}

//load and process all images for new observation at a given time(frame)
bool TrackingModel::GetObservation(float timeval)
{
	int frame = (int)timeval;													//generate image filenames
	int n = mCameras.GetCameraCount();
	vector<string> FGfiles(n), ImageFiles(n);
	for(int i = 0; i < n; i++)													
	{	FGfiles[i] = mPath + "FG" + str(i + 1) + DIR_SEPARATOR + "image" + str(frame, 4) + ".bmp";
		ImageFiles[i] = mPath + "CAM" + str(i + 1) + DIR_SEPARATOR + "image" + str(frame, 4) + ".bmp";
	}
	FlexImage8u im;
	for(uint i = 0; i < FGfiles.size(); i++)
	{	if(!FlexLoadBMP(FGfiles[i].c_str(), im))								//Load foreground maps and raw images
		{	cout << "Unable to load image: " << FGfiles[i].c_str() << endl;
			return false;
		}	
		mFGMaps[i].ConvertToBinary(im);											//binarize foreground maps to 0 and 1
		if(!FlexLoadBMP(ImageFiles[i].c_str(), im))
		{	cout << "Unable to load image: " << ImageFiles[i].c_str() << endl;
			return false;
		}
		CreateEdgeMap(im, mEdgeMaps[i]);										//Create edge maps

	}
	return true;
}

// ----------------------- Output results as a bitmap -------------------------

bool TrackingModel::OutputBMP(const std::vector<float> &pose, int frame)
{	
	vector<string> ImageFiles(mNCameras);
	for(int i = 0; i < mNCameras; i++)													
		ImageFiles[i] = mPath + "CAM" + str(i + 1) + DIR_SEPARATOR + "image" + str(frame, 4) + ".bmp";

	Im8u yellow[3] = {0, 255, 255}, cyan[3] = {255, 255, 0}, magenta[3] = {255, 0, 255};

	mPoses.Set(pose);														//set pose angles and translation
	mBodies.ComputeGeometry(mPoses, mBodies.Parameters());				//compute 3D model geometry from pose (generate conic cylinders and their transforms)
	mProjections.ImageProjection(mBodies, mCameras);						//compute projected 2D points into each camera image for each body part
	int levels = int(ImageFiles.size() - 1) / 2 + 1;
	int w = mEdgeMaps[0].Width() / 2, h = mEdgeMaps[0].Height() / 2;				
	FlexImage<Im8u,3> result(w * 2, h * levels), dstImage;						//create result image and sub-image object

	for(int camera = 0; camera < (int)ImageFiles.size(); camera++)				//create new image for each camera view
	{	FlexImage<Im8u,1> srcImage, imds;
		FlexLoadBMP(ImageFiles[camera].c_str(), srcImage);						//load raw image from file
		FlexDownSample2(srcImage, imds);										//downsample image by factor of 2 
		dstImage = result((camera % 2) * w , int(camera / 2) * h, w, h);		//create sub-image in result image
		FlexGrayToRGB(imds, dstImage, false);									//convert downsampled image to RGB into sub-image of result
		ProjectedBody &pb = mProjections(camera);
		for(int i = 0; i < pb.Size(); i++)										//draw projected cylinders colored by body part
		{	
			ProjectedCylinder &c = pb(i);
			Im8u *color = cyan;
			if(i == 7 || i == 8 || i == 3 || i == 4)
				color = yellow;
			else if(i == 9 || i == 0)
				color = magenta;
			FlexLine(dstImage, (int)c.mPts[0].x() / 2, (int)c.mPts[0].y() / 2, (int)c.mPts[1].x() / 2, (int)c.mPts[1].y() / 2, color);
			FlexLine(dstImage, (int)c.mPts[1].x() / 2, (int)c.mPts[1].y() / 2, (int)c.mPts[2].x() / 2, (int)c.mPts[2].y() / 2, color);
			FlexLine(dstImage, (int)c.mPts[2].x() / 2, (int)c.mPts[2].y() / 2, (int)c.mPts[3].x() / 2, (int)c.mPts[3].y() / 2, color);
			FlexLine(dstImage, (int)c.mPts[3].x() / 2, (int)c.mPts[3].y() / 2, (int)c.mPts[0].x() / 2, (int)c.mPts[0].y() / 2, color);
		}
	}
	string outFname = mPath + "Result" + str(frame, 4) + ".bmp";
	return FlexSaveBMP(outFname.c_str(), result);
}


int TrackingModel::getPrimitiveSize(){
	int size=0;
	size += mPoses.getPrimitiveSize();
	size += mBodies.getPrimitiveSize();
	size += mCameras.getPrimitiveSize();
	//size += mProjections.getPrimitiveSize();
	size += mImageMeasurements.getPrimitiveSize();
	return size;
}

int TrackingModel::getImagePrimitiveSize(){
	int size=0;
	for(int i=0; i < mFGMaps.size(); i++){
		int isiz= mFGMaps[i].getPrimitiveSize();
		size += isiz;
	}
	for(int i=0; i < mEdgeMaps.size(); i++){
		int isiz = mEdgeMaps[i].getPrimitiveSize();
		size += isiz;
	}
	//mFGMaps //size of one map times number of cameras
	//mEdgeMaps //size of one map times number of cameras
	return size;
}
#define N_CAMS 4

void TrackingModel::getPrimitive(float * dest){
	dest = mPoses.getPrimitive(dest);
	dest = mBodies.getPrimitive(dest);
	dest = mCameras.getPrimitive(dest);
	//dest = mProjections.getPrimitive(dest);
	dest = mImageMeasurements.getPrimitive(dest);
}
void TrackingModel::setPrimitive(float * src, int n){
	mNCameras = n;
	//resize maps
	src = mPoses.setPrimitive(src);
	src = mBodies.setPrimitive(src);
	src = mCameras.setPrimitive(src,n);
	//src = mProjections.setPrimitive(src,n);
	src = mImageMeasurements.setPrimitive(src);
}

void TrackingModel::getImagePrimitive(char * dest){
	for(int i=0; i < mEdgeMaps.size(); i++){
		dest = mEdgeMaps[i].getPrimitive(dest);
	}
	for(int i=0; i < mFGMaps.size(); i++){
		dest = mFGMaps[i].getPrimitive(dest);
	}
}
void TrackingModel::setImagePrimitive(char * src, int n, int w,int h, int b){
	mNCameras = n;
	//resize maps
	mFGMaps.resize(mNCameras);
	mEdgeMaps.resize(mNCameras);
	for(int i=0; i < mNCameras; i++){
		src = mEdgeMaps[i].setPrimitive(src,w,h,b);
	}
	for(int i=0; i < mNCameras; i++){
		src = mFGMaps[i].setPrimitive(src,w,h);
	}
	t_mFGMaps = &mFGMaps[0];
	t_mEdgeMaps = &mEdgeMaps[0];
}
