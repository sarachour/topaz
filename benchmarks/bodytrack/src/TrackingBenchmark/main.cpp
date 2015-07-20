#include <topaz.h> 
#include <task.h> 
#include <checker.h> 
//-------------------------------------------------------------
 //      ____                        _      _
 //     / ___|____ _   _ ____   ____| |__  | |
 //    | |   / ___| | | |  _  \/ ___|  _  \| |
 //    | |___| |  | |_| | | | | |___| | | ||_|
 //     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
 //                         
 //	  2006, Intel Corporation, licensed under Apache 2.0 
 //
 //  file : main.cpp
 //  author : Scott Ettinger - scott.m.ettinger@intel.com
 //  description : Top level body tracking code.  Takes image
 //				  inputs from disk and runs the tracking
 //				  particle filter.
 //
 //				  Currently contains 3 versions :
 //				  Single threaded, OpenMP, and Posix threads.
 //				  They are kept separate for readability.
 //
 //				  Thread methods supported are selected by the 
 //				  #defines USE_OPENMP, USE_THREADS or USE_TBB. 
 //
 //  modified : 
 //--------------------------------------------------------------
 
#if defined(HAVE_CONFIG_H)
 # include "config.h"
 #endif
 
#include <vector>
 #include <sstream>
 #include "relax_lib.h"
 #include <fstream>
 #include <iomanip>
 
//Add defines USE_OPENMP, USE_THREADS or USE_TBB for threaded versions if not using config file (Windows).
 //#define USE_OPENMP
 //#define USE_THREADS
 //#define USE_TBB
 

#include "ParticleFilter.h"
 #include "TrackingModel.h"
 #include "system.h"
 
using namespace std; 


//calculate particle weights (mWeights) and find highest likelihood particle. 
 //computes an optimal annealing factor and scales the likelihoods. 
 //Also removes any particles reported as invalid by the model.
 
#define MAX_MODEL_SIZE 512
 #define MAX_IMAGE_SIZE 1382400
 #define MAX_PROJECTED_BODY_SIZE 512
 
MultiCameraProjectedBody g_projections; 
ImageMeasurements g_measurements; 
vector<FlexImage8u> g_mEdgeMaps; 
vector<BinaryImage> g_mFGMaps; 

float internal_CalcWeights (float * v_proj,float * v_mdl, char * v_img, int ncameras,int w,int h, int b )
{
	
	//unpack
 v_proj = g_projections.setPrimitive (v_proj,ncameras ); 
	v_mdl = g_measurements.setPrimitive (v_mdl ); 
	g_mFGMaps.resize (ncameras ); 
	g_mEdgeMaps.resize (ncameras ); 
	for (int i=0; i < g_mEdgeMaps.size (); i++ ){
		v_img = g_mEdgeMaps[i].setPrimitive (v_img,w,h,b ); 
	}
	for (int i=0; i < g_mFGMaps.size (); i++ ){
		v_img = g_mFGMaps[i].setPrimitive (v_img,w,h ); 
	}
	FlexImage8u * t_mEdgeMaps = &g_mEdgeMaps[0]; 
	BinaryImage * t_mFGMaps = &g_mFGMaps[0]; 
	//compute projected 2D points into each camera image for each body part
 float err = g_measurements.ImageErrorEdgeTask (t_mEdgeMaps, &g_projections, ncameras ); //compute cylinder edge map term
 err += g_measurements.ImageErrorInsideTask (t_mFGMaps, &g_projections, ncameras ); //compute silhouette term
 return -err; 
}

TrackingModel g_TrackingModel; 

#define BATCH 2 
 float* gparticle[BATCH]; 
void ParticleFilter::CalcWeights (std::vector<Vectorf > &particles )
{
	mBestParticle = 0; 
	fpType total = 0, best = 0, minWeight = 1e30f, annealingFactor = 1; 
	mWeights.resize (particles.size ()); 
	
	//allocate
 float * mdl_prim = new float[MAX_MODEL_SIZE]; 
	char * img_prim = new char[MAX_IMAGE_SIZE]; 
	float * part_proj = new float[MAX_PROJECTED_BODY_SIZE*BATCH]; 
	int particle_size = 0; 
	bool valid[BATCH]; 
	int m_ncams = mModel->NCameras (); 
	int m_nbits = mModel->getBytesPerPixel (); 
	int m_width = mModel->getWidth (); 
	int m_height = mModel->getHeight (); 
	
	mModel->getAbbPrimitive (mdl_prim ); 
	mModel->getImagePrimitive (img_prim ); 
	
	float* tproj ; 
	int* t_valid ; 
	float* tmodel ; 
	char* timg ; 
	int nCams ; 
	int nBits ; 
	int width ; 
	int height ; 
	float* tweight ; 
	char tvalid ; 
	Topaz::topaz->start (0 ); 
	for (int i = 0 ; i <= (particles.size())-1 ; i += BATCH )
	{
		for (int k=0; k < BATCH; k++ ){
			gparticle[k] = &particles[i+k][0]; 
			mModel->SetupPose (particles[i+k], m_ncams, &valid[k], &part_proj[MAX_PROJECTED_BODY_SIZE*k] ); 
		}
		//printf("particle %d\n", i);
 
		Topaz::topaz->send (0 , i , (float*)part_proj , valid , (float*)mdl_prim , (char*)img_prim , m_ncams , m_nbits , m_width , m_height ); 
		Topaz::topaz->unpack_inputs (0 , &i , &tproj , &t_valid , &tmodel , &timg , &nCams , &nBits , &width , &height ); 
		if (Topaz::topaz->receive (0 , &i , &tweight , &tvalid )){
			for (int k=0; k < BATCH; k++ ){
				bool c_isvalid = (bool )t_valid[k]; 
				mWeights[i+k] = tweight[k]; 
				//printf("weight[%d]: %f\n",i+k,mWeights[i+k]);
 //printf("combine %d/%d : %f %s\n", i,particles.size(), tweight, c_isvalid? "y":"n");
 //don't skip particle like that.
 if (!c_isvalid )//if not valid(model prior), remove the particle from the list
 {
					particles[i+k].clear (); 
				}
				else {
					minWeight = std::min (mWeights[i+k], minWeight ); // TODO:  postincrement maybe an issue.
 particle_size++; 
				}
			}
			
		}
	}
	delete mdl_prim; 
	delete img_prim; 
	delete part_proj; 
	
	if (particle_size < mMinParticles )return; //bail out if not enough valid particles
 mWeights -= minWeight; //shift weights to zero for numerical stability
 if (mModel->StdDevs ().size ()> 1 )
	annealingFactor = BetaAnnealingFactor (mWeights, 0.5f ); //calculate annealing factor if more than 1 step
 
	for (uint i = 0; i < mWeights.size (); i++ )
	{
		if (particles[i].size ()==0 ){
			particles[i] = particles[particles.size ()- 1]; 
			mWeights[i] = mWeights[mWeights.size ()-1]; 
			mWeights.pop_back (); 
			particles.pop_back (); 
			i--; 
			continue; 
		}
		double wa = annealingFactor * mWeights[i]; 
		//printf("annealing %d/%d\n", i, mWeights.size());
 mWeights[i] = (float )t_exp (wa ); 
		total += mWeights[i]; //save sum of all weights
 if (i == 0 || mWeights[i] > best )//find highest likelihood particle
 {best = mWeights[i]; 
			mBestParticle = i; 
			printf ("anneal: %f\n" , mWeights[i] ); 
		}
	}
	mWeights *= fpType (1.0 )/ total; 
	//normalize weights
 
}


//calculate particle weights (mWeights) and find highest likelihood particle. 
 //computes an optimal annealing factor and scales the likelihoods. 
 //Also removes any particles reported as invalid by the model.
 
void ParticleFilter::MainCalcWeights (std::vector<Vectorf > &particles )
{
	bool valid; 
	mBestParticle = 0; 
	fpType total = 0, best = 0, minWeight = 1e30f, annealingFactor = 1; 
	mWeights.resize (particles.size ()); 
	uint i = 0; 
	
	while (i < particles.size ())//compute likelihood weights for each particle
 {
		mWeights[i] = mModel->LogLikelihoodMain (particles[i], valid ); 
		
		if (!valid )//if not valid(model prior), remove the particle from the list
 {particles[i] = particles[particles.size ()- 1]; 
			particles.pop_back (); mWeights.pop_back (); 
		}
		else 
		minWeight = std::min (mWeights[i++], minWeight ); //find minimum weight
 }
	if ((int )particles.size ()< mMinParticles )return; 
	//printf("finished modelling\n");					//bail out if not enough valid particles
 mWeights -= minWeight; //shift weights to zero for numerical stability
 if (mModel->StdDevs ().size ()> 1 )
	annealingFactor = BetaAnnealingFactor (mWeights, 0.5f ); //calculate annealing factor if more than 1 step
 for (uint i = 0; i < mWeights.size (); i++ )
	{
		double wa = annealingFactor * mWeights[i]; 
		mWeights[i] = (float )t_exp (wa ); 
		total += mWeights[i]; //save sum of all weights
 if (i == 0 || mWeights[i] > best )//find highest likelihood particle
 {best = mWeights[i]; 
			mBestParticle = i; 
		}
	}
	//printf("finished annealing\n");		
 mWeights *= fpType (1.0 )/ total; //normalize weights
 }


//templated conversion from string
 template<class T> 
bool num (const string s, T &n )
{istringstream ss (s ); 
	ss >> n; 
	return !ss.fail (); 
}

//write a given pose to a stream
 inline void WritePose (ostream &f, vector<float> &pose )
{for (int i = 0; i < (int )pose.size (); i++ )
	f << pose[i] << " " ; 
	f << endl; 
}

bool ProcessCmdLine (int argc, char **argv, string &path, int &cameras, int &frames, int &particles, int &layers, int &threads, int &threadModel, bool &OutputBMP )
{
	string usage ("Usage : Track (Dataset Path) (# of cameras) (# of frames to process)\n" ); 
	usage += string ("              (# of particles) (# of annealing layers) \n" ); 
	usage += string ("              [thread model] [# of threads] [write .bmp output (nonzero = yes)]\n\n" ); 
	usage += string ("        Thread model : 0 = Auto-select from available models\n" ); 
	usage += string ("                       1 = Intel TBB                 " ); 
	#ifdef USE_TBB
 usage += string ("\n" ); 
	#else
 usage += string ("(unavailable)\n" ); 
	#endif
 usage += string ("                       2 = Posix / Windows threads   " ); 
	#ifdef USE_THREADS
 usage += string ("\n" ); 
	#else
 usage += string ("(unavailable)\n" ); 
	#endif
 usage += string ("                       3 = OpenMP                    " ); 
	#ifdef USE_OPENMP
 usage += string ("\n" ); 
	#else
 usage += string ("(unavailable)\n" ); 
	#endif
 usage += string ("                       4 = Serial\n" ); 
	
	string errmsg ("Error : invalid argument - " ); 
	if (argc < 6 || argc > 9 )//check for valid number of arguments
 {cout << "Error : Invalid number of arguments" << endl << usage << endl; 
		return false; 
	}
	path = string (argv[1] ); //get dataset path and add backslash if needed
 if (path[path.size ()- 1] != DIR_SEPARATOR[0] )
	path.push_back (DIR_SEPARATOR[0] ); 
	if (!num (string (argv[2] ), cameras ))//parse each argument
 {cout << errmsg << "number of cameras" << endl << usage << endl; 
		return false; 
	}
	if (!num (string (argv[3] ), frames ))
	{cout << errmsg << "number of frames" << endl << usage << endl; 
		return false; 
	}
	if (!num (string (argv[4] ), particles ))
	{cout << errmsg << "number of particles" << endl << usage << endl; 
		return false; 
	}
	if (!num (string (argv[5] ), layers ))
	{cout << errmsg << "number of annealing layers" << endl << usage << endl; 
		return false; 
	}
	threads = -1; 
	threadModel = 0; 
	if (argc < 7 )//use default single thread mode if no threading arguments present
 return true; 
	if (!num (string (argv[6] ), threadModel ))
	{cout << errmsg << "Thread Model" << endl << usage << endl; 
		return false; 
	}
	if (argc > 7 )
	if (!num (string (argv[7] ), threads ))
	{cout << errmsg << "number of threads" << endl << usage << endl; 
		return false; 
	}
	int n; 
	OutputBMP = true; //do not output bmp results by default
 if (argc > 8 )
	{if (!num (string (argv[8] ), n ))
		{cout << errmsg << "Output BMP flag" << endl << usage << endl; 
			return false; 
		}
		OutputBMP = (n != 0 ); 
	}
	return true; 
}


//Body tracking Single Threaded
 int mainSingleThread (string path, int cameras, int frames, int particles, int layers, bool OutputBMP )
{
	cout << endl << "Running Single Threaded" << endl << endl; 
	
	TrackingModel model; 
	if (!model.Initialize (path, cameras, layers ))//Initialize model parameters
 {cout << endl << "Error loading initialization data." << endl; 
		return 0; 
	}
	model.GetObservation (0 ); //load data for first frame
 ParticleFilter pf; //particle filter instantiated with body tracking model type
 pf.SetModel (model ); //set the particle filter model
 pf.InitializeParticles (particles ); //generate initial set of particles and evaluate the log-likelihoods
 
	cout << "Using dataset : " << path << endl; 
	cout << particles << " particles with " << layers << " annealing layers" << endl << endl; 
	ofstream outputFileAvg ((path + "poses.txt" ).c_str ()); 
	
	vector<float> estimate; //expected pose from particle distribution
 
	#if defined(ENABLE_PARSEC_HOOKS)
 __parsec_roi_begin (); 
	#endif
 for (int i = 0; i < frames; i++ )//process each set of frames
 {cout << "Processing frame " << i << endl; 
		cout << "Updating" << endl; 
		if (!pf.Update ((float )i ))//Run particle filter step
 {cout << "Error loading observation data" << endl; 
			return 0; 
		}
		cout << "Estimating.." << endl; 
		pf.Estimate (estimate ); 
		cout << "Estimated." << endl; //get average pose of the particle distribution
 WritePose (outputFileAvg, estimate ); 
		cout << "Outputted Pose" << endl; 
		if (OutputBMP )
		pf.Model ().OutputBMP (estimate, i ); 
		cout << "Outputted BMP" << endl; //save output bitmap file
 }
	#if defined(ENABLE_PARSEC_HOOKS)
 __parsec_roi_end (); 
	#endif
 
	return 1; 
}

int topaz_main (int argc, char **argv )
{
	string path; 
	bool OutputBMP; 
	int cameras, frames, particles, layers, threads, threadModel; //process command line parameters to get path, cameras, and frames
 
	#ifdef PARSEC_VERSION
 #define __PARSEC_STRING(x) #x
 #define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
 cout << "PARSEC Benchmark Suite Version " __PARSEC_XSTRING (PARSEC_VERSION )<< endl << flush; 
	#else
 cout << "PARSEC Benchmark Suite" << endl << flush; 
	#endif //PARSEC_VERSION
 #if defined(ENABLE_PARSEC_HOOKS)
 __parsec_bench_begin (__parsec_bodytrack ); 
	#endif
 
	if (!ProcessCmdLine (argc, argv, path, cameras, frames, particles, layers, threads, threadModel, OutputBMP ))
	return 0; 
	
	//run stuff
 mainSingleThread (path, cameras, frames, particles, layers, OutputBMP ); 
	
	#if defined(ENABLE_PARSEC_HOOKS)
 __parsec_bench_end (); 
	#endif
 
	return 0; 
}
void calcweights_COMPUTE_TPZ (Task* __INPUT , Task* __OUTPUT )
{
	int i ; 
	float* tproj ; 
	int* t_valid ; 
	float* tmodel ; 
	char* timg ; 
	int nCams ; 
	int nBits ; 
	int width ; 
	int height ; 
	float tweight [ BATCH ] ; 
	char tvalid ; 
	Topaz::topaz->unpack_inputs (0 , &i , &tproj , &t_valid , &tmodel , &timg , &nCams , &nBits , &width , &height ); 
	pin_start_inject_errors (); 
	
	bool is_valid = true; 
	for (int k=0; k < BATCH; k++ ){
		is_valid = is_valid && t_valid[k]; 
		if (t_valid[k] ){
			tweight[k] = internal_CalcWeights (&tproj[MAX_PROJECTED_BODY_SIZE*k], tmodel, timg, nCams, width, height, nBits ); 
		}
	}
	tvalid = is_valid; 
	pin_stop_inject_errors (); 
	Topaz::topaz->pack_outputs (0 , i , tweight , tvalid ); 
}
bool calcweights_TRANS_TPZ (Task* __INPUT , Task* __OUTPUT ){
	
	int i ; 
	float* tproj ; 
	int* t_valid ; 
	float* tmodel ; 
	char* timg ; 
	int nCams ; 
	int nBits ; 
	int width ; 
	int height ; 
	float* tweight ; 
	char tvalid ; 
	float tv ; 
	float lweight ; 
	float bp1 ; 
	float bp2 ; 
	float bp3 ; 
	Topaz::topaz->unpack_inputs (__INPUT , 0 , &i , &tproj , &t_valid , &tmodel , &timg , &nCams , &nBits , &width , &height ); 
	Topaz::topaz->unpack_outputs (__OUTPUT , 0 , &i , &tweight , &tvalid ); 
	
	tv = tvalid; 
	//lvalid = (int) (tvalid);
 bp1 = 0; bp2 = 0; bp3 = 0; lweight = 0; 
	for (int k=0; k < BATCH; k++ ){
		if (tweight[k] < lweight || k == 0 ){
			lweight = tweight[k]; 
			bp1=gparticle[k][3]; 
			bp2=gparticle[k][4]; 
			bp3=gparticle[k][5]; 
		}
	}
	return Topaz::topaz->check (0 , i , tv , lweight , bp1 , bp2 , bp3 ); 
}
int topaz_init (int argv , char** argc ){
	Topaz::topaz->add (0 , calcweights_COMPUTE_TPZ , calcweights_TRANS_TPZ , 8 , 2 , 5 , 
		 TASK_ARG_ARRAY , FLOAT , MAX_PROJECTED_BODY_SIZE*BATCH , 
		 TASK_ARG_ARRAY , INT , BATCH , 
		 TASK_ARG_CONST_ARRAY , FLOAT , MAX_MODEL_SIZE , 
		 TASK_ARG_CONST_ARRAY , CHAR , MAX_IMAGE_SIZE , 
		 TASK_ARG_CONST_SCALAR , INT , 1 , 
		 TASK_ARG_CONST_SCALAR , INT , 1 , 
		 TASK_ARG_CONST_SCALAR , INT , 1 , 
		 TASK_ARG_CONST_SCALAR , INT , 1 , 
		 TASK_ARG_ARRAY , FLOAT , BATCH , 
		 TASK_ARG_SCALAR , CHAR , 1 ); 
	
}
int topaz_finish (int argv , char** argc ){
}
int topaz_worker (int argv , char** argc ){
}