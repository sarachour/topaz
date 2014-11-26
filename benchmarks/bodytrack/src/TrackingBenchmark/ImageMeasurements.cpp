//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//                         
//	 © 2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : ImageMeasurements.cpp
//  author : Scott Ettinger - scott.m.ettinger@intel.com
//			 Jean-Yves Bouguet - jean-yves.bouguet@intel.com
//			 
//  description : Image Measurements (silhouette and edges)
//  modified : 
//--------------------------------------------------------------

#if defined(HAVE_CONFIG_H)
# include "config.h"
#endif

#include "ImageMeasurements.h"
#include "relax_lib.h"
#include <math.h>

using namespace std;

//2D vector magnitude
inline double mag(Point * p)
{	return t_sqrt((double)(p->x * p->x + p->y * p->y));
}

inline double mag_rel(Point * p){
	float ms_a = mag(p);
	float ms_b = mag(p);
	float ms_c = mag(p);
	if(ms_a == ms_b) return ms_a;
	else if(ms_b == ms_c) return ms_b;
	else if(ms_a == ms_c) return ms_c;
	else return 0;
}

//accumulate error at a given edge sample point (round to nearest integral point)
inline void SampleEdgePointTask(float xf, float yf, FlexImage8u * EdgeMap, int * error, int * samplePoints)
{
	int x = int(xf + 0.5f), y = int(yf + 0.5f);
	if((x >= 0) && (x < EdgeMap->Width()) && (y >= 0) && (y < EdgeMap->Height())) //check image bounds
	{	int e = 255 - (*EdgeMap)(x,y);												//get value from image map and compute difference
		*error += (e * e);														//sum squared error values
		*samplePoints+=1;															//count points sampled
	}
}

//Generate Samples for points along the non-joint edges of the cylinder
void ImageMeasurements::EdgeErrorTask(ProjectedCylinder * ProjCyl, FlexImage8u * EdgeMap, float * error, int * samplePoints)
{
	int ErrorSSD = 0;
	Point p1 = ProjCyl->mPts[0];
	Point s1;	
	Point x1 = ProjCyl->mPts[1];																//get direction vector of side 1 of the 2D cylinder projection
	s1.Set(x1.x - p1.x, x1.y - p1.y);
	int n1 = max((int)(mag_rel(&s1) / mStep + 0.5), 4);							//compute number of points sampled (sample at least 4)
	float d1 = 1.0f / (float)n1++;											//get fraction of side length per sample

	Point p2 = ProjCyl->mPts[2];
	Point s2;			
	Point x2 = ProjCyl->mPts[3];													//repeat for side 2 of cylinder
	s2.Set(x2.x - p2.x, x2.y - p2.y);
	int n2 = max((int)(mag_rel(&s2) / mStep + 0.5), 4);					
	float d2 = 1.0f / (float)n2++;
	if(n1 > MAX_ITERS) n1 = MAX_ITERS;
	if(n2 > MAX_ITERS) n2 = MAX_ITERS;
	float delta = 0;
	//printf("loop %d %d (%f,%f) (%f,%f)\n", n1, n2, s1.x, s1.y, s2.x, s2.y);
	for(int i = 0; i < n1; i++)												//generate sample points along each side of cylinder projection
	{	float x = p1.x + delta * s1.x;
		float y = p1.y + delta * s1.y;
		SampleEdgePointTask(x, y, EdgeMap, &ErrorSSD, samplePoints);				//accumulate error at computed edge points on side 1
  		delta += d1;
	}
	delta = 0;
	for(int i = 0; i < n2; i++)
	{	float x = p2.x + delta * s2.x;
		float y = p2.y + delta * s2.y;
		SampleEdgePointTask(x, y, EdgeMap, &ErrorSSD, samplePoints);				//accumulate error at comptued edge points on side 2
		delta += d2;
	}
	*error += (float)ErrorSSD / (255.0f * 255.0f);
}

//compute edge map error term for all cameras given the set of 2D body geometry projections
float ImageMeasurements::ImageErrorEdgeTask(FlexImage8u * ImageMaps, MultiCameraProjectedBody * ProjBodies, int nimgmaps) 
{
	int samples = 0;
	float error = 0;
	for(int i = 0; i < nimgmaps; i++)							//for each camera, compute the edge map error term
	{	
		int nParts = (*ProjBodies)(i).Size();
		for(int j = 0; j < nParts; j++)										//accumulate edge error for each body part, counting samples
		{
			EdgeErrorTask(&(*ProjBodies)(i)(j), &ImageMaps[i], &error, &samples);
		}
	}
	//cout << "Samples = " << samples << endl;
	return (float)error / samples;											//normalize to number of samples
}

//accumulate error at a given edge sample point (round to nearest integral point)
inline void SampleEdgePoint(float xf, float yf, const FlexImage8u &EdgeMap, int &error, int &samplePoints)
{
	int x = int(xf + 0.5f), y = int(yf + 0.5f);
	if((x >= 0) && (x < EdgeMap.Width()) && (y >= 0) && (y < EdgeMap.Height())) //check image bounds
	{	int e = 255 - EdgeMap(x,y);												//get value from image map and compute difference
		error += (e * e);														//sum squared error values
		samplePoints++;															//count points sampled
	}
}

//Generate Samples for points along the non-joint edges of the cylinder
void ImageMeasurements::EdgeError(const ProjectedCylinder &ProjCyl, const FlexImage8u &EdgeMap, float &error, int &samplePoints)
{
	int ErrorSSD = 0;
	const Point &p1 = ProjCyl.mPts[0];
	Point s1;																//get direction vector of side 1 of the 2D cylinder projection
	s1.Set(ProjCyl.mPts[1].x - p1.x, ProjCyl.mPts[1].y - p1.y);
	int n1 = max((int)(mag(&s1) / mStep + 0.5), 4);							//compute number of points sampled (sample at least 4)
	float d1 = 1.0f / (float)n1++;											//get fraction of side length per sample

	const Point &p2 = ProjCyl.mPts[2];
	Point s2;																//repeat for side 2 of cylinder
	s2.Set(ProjCyl.mPts[3].x - p2.x, ProjCyl.mPts[3].y - p2.y);
	int n2 = max((int)(mag(&s2) / mStep + 0.5), 4);					
	float d2 = 1.0f / (float)n2++;

	float delta = 0;
	for(int i = 0; i < n1; i++)												//generate sample points along each side of cylinder projection
	{	float x = p1.x + delta * s1.x;
		float y = p1.y + delta * s1.y;
		SampleEdgePoint(x, y, EdgeMap, ErrorSSD, samplePoints);				//accumulate error at computed edge points on side 1
  		delta += d1;
	}
	delta = 0;
	for(int i = 0; i < n2; i++)
	{	float x = p2.x + delta * s2.x;
		float y = p2.y + delta * s2.y;
		SampleEdgePoint(x, y, EdgeMap, ErrorSSD, samplePoints);				//accumulate error at comptued edge points on side 2
		delta += d2;
	}
	error += (float)ErrorSSD / (255.0f * 255.0f);
}

//check bounds of nearest integral point and accumulate error value
inline void SampleInsidePointTask(float xf, float yf, const BinaryImage *FGmap, int *error, int *samplePoints)
{
	int x = int(xf + 0.5f), y = int(yf + 0.5f);
	if((x >= 0) && (x < FGmap->Width()) && (y >= 0) && (y < FGmap->Height())) //check image bounds
	{	int e = 1 - (*FGmap)(x,y);												//get value from image map and compute difference
		*error += e;															//sum squared error values (since err = {1,0} same as sum of errors)
		*samplePoints+=1;														//count points sampled
	}
}


//Sample points inside the projected cylinder
void ImageMeasurements::InsideErrorTask(ProjectedCylinder *ProjCyl, BinaryImage *FGmap, int * error, int * samplePoints)
{
	Point p1 = ProjCyl->mPts[0];
	Point p2 = ProjCyl->mPts[3];
	Point x1 = ProjCyl->mPts[1];
	Point x2 = ProjCyl->mPts[2];
	Point s1, s2;
	s1.Set(x1.x - p1.x, x1.y - p1.y);				//get vectors along sides
	s2.Set(x2.x - p2.x, x2.y - p2.y);
	Point m(p1.x + s1.x / 2.0f - (p2.x + s2.x / 2.0f), p1.y + s1.y / 2.0f - (p2.y + s2.y / 2.0f));
	int n1 = max((int)(mag_rel(&s1) / mVstep + 0.5), 4);							//compute number of points sampled on each side (sample at least 4)
	int n2 = max((int)(mag_rel(&m) / mHstep + 0.5f), 4);							//compute number of interior samples using the midpoint length
	float d1 = 1.0f / n1++;													//get fraction of side lengths per sample
	float d2 = 1.0f / n2;
	float delta1 = 0;
	Point e1, e2;
	if(n1 > MAX_ITERS) n1 = MAX_ITERS;
	if(n2 > MAX_ITERS) n2 = MAX_ITERS;
	//printf("loop %d %d (%f,%f) (%f,%f)\n", n1, n2, s1.x, s1.y, s2.x, s2.y);
	for(int i = 0; i < n1; i++)												//generate sample points along each side of cylinder projection
	{   e1.Set(p1.x + delta1 * s1.x, p1.y + delta1 * s1.y);
		e2.Set(p2.x + delta1 * s2.x, p2.y + delta1 * s2.y);
		m.Set(e2.x - e1.x, e2.y - e1.y);									//get vector between edge points
		delta1 += d1;
		float delta2 = 0;
		for(int j = 0; j < n2; j++)											//generate interior samples
		{	
			SampleInsidePointTask(e1.x + delta2 * m.x, e1.y + delta2 * m.y, FGmap, error, samplePoints);
			delta2 += d2;
		}
	}
}

//compute silhouette error term for all cameras given the set of 2D body geometry projections
float ImageMeasurements::ImageErrorInsideTask(BinaryImage *ImageMaps, MultiCameraProjectedBody * ProjBodies, int nbodies)
{
	int samples = 0;
	int error = 0;
	for(int i = 0; i < nbodies; i++)							//for each camera, compute the edge map error term
	{	
		int nParts = (*ProjBodies)(i).Size();
		for(int j = 0; j < nParts; j++)	{		
			InsideErrorTask(&(*ProjBodies)(i)(j), &ImageMaps[i], &error, &samples);
		}
	}
	//cout << "Samples = " << samples << endl;
	return (float)error / samples;
}

//check bounds of nearest integral point and accumulate error value
inline void SampleInsidePoint(float xf, float yf, const BinaryImage &FGmap, int &error, int &samplePoints)
{
	int x = int(xf + 0.5f), y = int(yf + 0.5f);
	if((x >= 0) && (x < FGmap.Width()) && (y >= 0) && (y < FGmap.Height())) //check image bounds
	{	int e = 1 - FGmap(x,y);												//get value from image map and compute difference
		error += e;															//sum squared error values (since err = {1,0} same as sum of errors)
		samplePoints++;														//count points sampled
	}
}

//Sample points inside the projected cylinder
void ImageMeasurements::InsideError(const ProjectedCylinder &ProjCyl, const BinaryImage &FGmap, int &error, int &samplePoints)
{
	const Point &p1 = ProjCyl.mPts[0], &p2 = ProjCyl.mPts[3];
	Point s1, s2;
	s1.Set(ProjCyl.mPts[1].x - p1.x, ProjCyl.mPts[1].y - p1.y);				//get vectors along sides
	s2.Set(ProjCyl.mPts[2].x - p2.x, ProjCyl.mPts[2].y - p2.y);
	Point m(p1.x + s1.x / 2.0f - (p2.x + s2.x / 2.0f), p1.y + s1.y / 2.0f - (p2.y + s2.y / 2.0f));
	int n1 = max((int)(mag(&s1) / mVstep + 0.5), 4);							//compute number of points sampled on each side (sample at least 4)
	int n2 = max((int)(mag(&m) / mHstep + 0.5f), 4);							//compute number of interior samples using the midpoint length
	float d1 = 1.0f / n1++;													//get fraction of side lengths per sample
	float d2 = 1.0f / n2;
	float delta1 = 0;
	Point e1, e2;
	for(int i = 0; i < n1; i++)												//generate sample points along each side of cylinder projection
	{   e1.Set(p1.x + delta1 * s1.x, p1.y + delta1 * s1.y);
		e2.Set(p2.x + delta1 * s2.x, p2.y + delta1 * s2.y);
		m.Set(e2.x - e1.x, e2.y - e1.y);									//get vector between edge points
		delta1 += d1;
		float delta2 = 0;
		for(int j = 0; j < n2; j++)											//generate interior samples
		{	SampleInsidePoint(e1.x + delta2 * m.x, e1.y + delta2 * m.y, FGmap, error, samplePoints);
			delta2 += d2;
		}
	}
}

//compute edge map error term for all cameras given the set of 2D body geometry projections
float ImageMeasurements::ImageErrorEdge(std::vector<FlexImage8u> &ImageMaps, MultiCameraProjectedBody &ProjBodies) 
{
	int samples = 0;
	float error = 0;
	for(int i = 0; i < (int)ImageMaps.size(); i++)							//for each camera, compute the edge map error term
	{	int nParts = ProjBodies(i).Size();
		for(int j = 0; j < nParts; j++)										//accumulate edge error for each body part, counting samples
			EdgeError(ProjBodies(i)(j), ImageMaps[i], error, samples);
	}
	//cout << "Samples = " << samples << endl;
	return (float)error / samples;											//normalize to number of samples
}



//compute silhouette error term for all cameras given the set of 2D body geometry projections
float ImageMeasurements::ImageErrorInside(std::vector<BinaryImage> &ImageMaps, MultiCameraProjectedBody &ProjBodies)
{
	int samples = 0;
	int error = 0;
	for(int i = 0; i < (int)ImageMaps.size(); i++)							//for each camera, compute the edge map error term
	{	int nParts = ProjBodies(i).Size();
		
		for(int j = 0; j < nParts; j++)	{		
			InsideError(ProjBodies(i)(j), ImageMaps[i], error, samples);
		}
	}
	//cout << "Samples = " << samples << endl;
	return (float)error / samples;
}
