#include <pthread.h>
#include <assert.h>

// ========================================================================
// Replicate accumulator for each thread...
// cacheline size: 64 or 128 byte
// (http://software.intel.com/en-us/forums/showthread.php?t=48480)

//#define NDEBUG

class acc_double_seq;
class acc_double_par;
class vector_seq;
class vector_par;

#ifdef _USE_PARALLEL_
typedef acc_double_par acc_double;
//typedef vector_seq vector;
typedef vector_par vector;
#define PAD_SIZE 64
#else
typedef acc_double_seq acc_double;
typedef vector_seq vector;
#define PAD_SIZE 1
#endif

//  =======================================================================

class acc_double_seq {
   double val;
public:
   acc_double_seq() {
      val = 0.0;
   }
   ~acc_double_seq() {
   }
   double readval() {
      return val;
   }
   void writeval(double d) {
      val = d;
   }
   void addval(double d) {
      val += d;
   }
   void subval(double d) {
      val -= d;
   }
   void mulval(double d) {
      val *= d;
   }
   void addvalUnsync(double d) {
      addval(d);
   }
};

//  =======================================================================

const int NDIM = 3;

class vector_seq {
public:
   double val[NDIM];
   vector_seq() {
      for (int i = 0; i < NDIM; i++) {
         val[i] = 0.0;
      }
   }
   ~vector_seq() {
   }
   void vecPrint() {
      printf("(%6.4f %6.4f %6.4f) ", val[0], val[1], val[2]);
   }
   inline double vecField(int idx) {
      return val[idx];
   }
   inline void vecFieldAdd(int idx, double v) {
      val[idx] += v;
   }
   inline void vecFieldClr(int idx) {
      val[idx] = 0.0;
   }
   inline void vecFieldSet(int idx, double v) {
      val[idx] = v;
   }
   inline void vecLoad(double *v) {
      int i;
      for (i = 0; i < NDIM; i++)
         val[i] = v[i];
   }
   inline void vecStore(double *v) {
      int i;
      for (i = 0; i < NDIM; i++)
         v[i] = val[i];
   }

   void vecMin(double *v) {
      int i;
      for (i = 0; i < NDIM; i++)
         if (val[i] > (v[i]))
            val[i] = (v[i]);
   }

   double vecDotProd(double *v) {
      int i;
      double d;
      d = 0.0;
      for (i = 0; i < NDIM; i++)
         d += (val[i] * v[i]);
      return d;
   }

   double vecNorm1() {
      int i;
      double d = 0.0;
      for (i = 0; i < NDIM; i++)
         d += val[i];
      return d;
   }

   void vecClr() {
      int i;
      for (i = 0; i < NDIM; i++)
         val[i] = 0.0;
   }

   void vecSub(double *v) {
      int i;
      for (i = 0; i < NDIM; i++)
         val[i] -= v[i];
   }

   void vecProd(double *v) {
      int i;
      for (i = 0; i < NDIM; i++)
         val[i] *= v[i];
   }

   void vecAdd(double *v) {
      int i;
      for (i = 0; i < NDIM; i++)
         val[i] += v[i];
   }

   void vecScale(double s) {
      int i;
      for (i = 0; i < NDIM; i++)
         val[i] *= s;
   }

};
//  =======================================================================

// definition of parallel routines:

#ifdef _MYPAR_LIB_
inline bool isParallelSectionWrapper() {return 0 != parlib_runtime_parallel_mode;}
inline int getThreadNumWrapper() {return parlib_runtime_thread_num;}
inline int getCurrentThreadIdWrapper() {return parlib_runtime_this_thread_id;}
#endif

//#define _OPENMP_
#ifdef _OPENMP_
#include <omp.h>
inline bool isParallelSectionWrapper() {return omp_in_parallel();}
inline int getThreadNumWrapper() {return omp_get_max_threads();}
inline int getCurrentThreadIdWrapper() {return omp_get_thread_num();}
#endif

//  =======================================================================


//check code <- don't need to generate this code!
#ifndef NDEBUG
//#define TEST_ASSERT(test, assert1, assert2) { assert (!assert1); assert(!assert2); if (!test) test = true; }
#define TEST_ASSERT(test, assert1, assert2) {}
# else
#define TEST_ASSERT(test, assert1, assert2) {}
#endif

#define PAD_REG_SIZE 6
#define MAX_PROC_SIZE 16

void* PAD_REG_acc_double = 0;
volatile int PAD_REG_IDX_acc_double = 0;
pthread_mutex_t PAD_REG_MUTEX_acc_double;
volatile bool PAD_REG_INIT_acc_double = false;

class acc_double_par {
public:
   struct double_val_pad {
      char pad0[PAD_SIZE];
      volatile double val;
      char pad1[PAD_SIZE];
   };

private:

   double_val_pad * vals;
   int vals_id;
   volatile bool isReplInitialized;
   //double_val_pad vals[MAX_PROCESSORS];

   //cached value
   char pad0[PAD_SIZE];
   volatile bool isCached;
   volatile double cachedVal;
   char pad1[PAD_SIZE];

   //mutex
   pthread_mutex_t acc_mutex, acc_repl_mutex;

   //check code <- don't need to generate this code!
#ifndef NDEBUG
   bool run_unsync, run_sync, run_repl;
#endif

   //friend void acc_double_callback_hook_after(void * object);

public:
   acc_double_par() {
      isCached = true;
      cachedVal = 0.0;
      vals = 0;
      pthread_mutex_init(&acc_mutex, 0);
      pthread_mutex_init(&acc_repl_mutex, 0);
      initPadRegister();
      isReplInitialized = false;
#ifndef NDEBUG
      run_unsync = false, run_sync = false, run_repl = false;
#endif

   }
   ~acc_double_par() {
      pthread_mutex_destroy(&acc_repl_mutex);
      pthread_mutex_destroy(&acc_mutex);
      if (isReplInitialized)
         reclaimRegisterItem(vals_id);
   }

   void initReplication() {
      if (isReplInitialized)
         return;
      pthread_mutex_lock(&acc_repl_mutex);
      if (isReplInitialized) {
         pthread_mutex_unlock(&acc_repl_mutex);
         return;
      }
      getNextRegisterItem(&vals, vals_id);
      //vals[0].val = cachedVal;
      for (int i = 0; i < getThreadNumWrapper(); i++)
         vals[i].val = 0.0;
      //cachedVal = 0.0;
      isReplInitialized = true;
      pthread_mutex_unlock(&acc_repl_mutex);
      //printf("AccDoubleReplicationInit... %p\n", this);
   }

   static void initPadRegister() {
      if (PAD_REG_INIT_acc_double)
         return;
      assert(!isParallelSectionWrapper());
      pthread_mutex_init(&PAD_REG_MUTEX_acc_double, 0);
      PAD_REG_acc_double = (void*) new double_val_pad*[PAD_REG_SIZE];
      double_val_pad** PAD_REGx = (double_val_pad**) PAD_REG_acc_double;
      PAD_REG_IDX_acc_double = 0;
      for (int i = 0; i < PAD_REG_SIZE; i++) {
         PAD_REGx[i] = new double_val_pad[MAX_PROC_SIZE];
      }
      PAD_REG_INIT_acc_double = true;
   }

   static void getNextRegisterItem(double_val_pad ** ret_ptr, int & ret_id) {
      // better implementation may include linked list for assigned
      // free elements, with additional elements produced if required
      //assert (PAD_REG_INIT_acc_double);
      if (!PAD_REG_INIT_acc_double)
         initPadRegister();

      //printf("%d   %d\n", PAD_REG_INIT_acc_double, PAD_REG_IDX_acc_double);
      //     assert (true == PAD_REG_INIT_acc_double);
      //     assert (PAD_REG_IDX_acc_double < PAD_REG_SIZE);
      pthread_mutex_lock(&PAD_REG_MUTEX_acc_double);
      int id = PAD_REG_IDX_acc_double++;
      pthread_mutex_unlock(&PAD_REG_MUTEX_acc_double);
      ret_id = id;
      *ret_ptr = ((double_val_pad**) PAD_REG_acc_double)[id];

   }

   void reclaimRegisterItem(int id) {
      // todo
   }

   double readval() {
      return readvalUnsync();
   }

   double readvalUnsync() {
      TEST_ASSERT(run_unsync, run_sync, run_repl);
      if (!isCached) __updateState();
      return cachedVal;
   }

   double readvalSync() {
      TEST_ASSERT(run_sync, false, run_repl);
      pthread_mutex_lock(&acc_mutex);
      double ret = readvalUnsync();
      pthread_mutex_unlock(&acc_mutex);
      return ret;
   }

   double readvalRepl() {
      TEST_ASSERT(run_repl, false, run_sync);
      // if the replication was not required, the value is already cached...
      if (!isCached) {
         assert (isReplInitialized);
         __updateState();
      }

      return cachedVal;
   }

   void __updateState() {
      assert (!isParallelSectionWrapper());
      double val = 0.0;
      for (int i = 0; i < getThreadNumWrapper(); i++) {
         val += vals[i].val;
         vals[i].val = 0.0;
      }
      cachedVal += val;
      isCached = true;
   }

   void writeval(double d) {
      writevalUnsync(d);
   }

   void writevalUnsync(double d) {
      TEST_ASSERT(run_unsync, run_sync, run_repl);
      if (!isCached) __updateState();
      cachedVal = d;
   }

   void writevalSync(double d) {
      TEST_ASSERT(run_sync, false, run_repl);
      pthread_mutex_lock(&acc_mutex);
      writevalUnsync(d);
      pthread_mutex_unlock(&acc_mutex);
   }

   void writevalRepl(double d) {
      TEST_ASSERT(run_repl, false, run_sync);
      //printf("wrREPL %5.3f!\n", d);
      if (!isCached) {
         assert (isReplInitialized);
         __updateState();
      }
      cachedVal = d;
   }

   void addval(double d) {
      addvalUnsync(d);
   }

   void addvalUnsync(double d) {
      TEST_ASSERT(run_unsync, run_sync, run_repl);
      if (!isCached) __updateState();
      cachedVal += d;
   }

   void addvalSync(double d) {
      TEST_ASSERT(run_sync, false, run_repl)
      pthread_mutex_lock(&acc_mutex);
      addvalUnsync(d);
      pthread_mutex_unlock(&acc_mutex);
   }

   void addvalRepl(double d) {
      TEST_ASSERT(run_repl, false, run_sync);
      //printf ("adREPL!\n");
      if (isCached) {
         if (!isReplInitialized) { initReplication(); }
         isCached = false;
      }
      //     printf ("%12.8f     %12.8f\n", vals[getCurrentThreadIdWrapper()].val, d);
      vals[getCurrentThreadIdWrapper()].val += d;
   }

   void subval(double d) {
      subvalUnsync(d);
   }
   // create replicated version if necessary...

   void subvalUnsync(double d) {
      double val;
      if (!isCached) __updateState();
      cachedVal -= d;
   }

   void mulval(double d) {
      mulvalUnsync(d);
   }
   // create replicated if necessary...

   void mulvalUnsync(double d) {
      double val;
      if (!isCached) __updateState();
      cachedVal *= d;
   }

};

// ==============================================================================

// ==============================================================================


void* PAD_REG_vector = 0;
volatile int PAD_REG_IDX_vector = 0;
pthread_mutex_t PAD_REG_MUTEX_vector;
volatile bool PAD_REG_INIT_vector = false;

class vector_par {

   struct state_val_pad {
      char pad0[PAD_SIZE];
      volatile double val[NDIM];
      char pad1[PAD_SIZE];
   };


   state_val_pad * vals;    //replicated values

   //char pad0[PAD_SIZE];
   volatile bool isConsistent; // is state consistent (isCached)
   volatile double seqVal[NDIM]; // this is cached value (cachedVal)
   int vals_id;
   volatile bool isReplInitialized;
   //char pad1[PAD_SIZE];

   //mutex
   pthread_mutex_t acc_mutex, acc_repl_mutex;

public:

   vector_par() {
      isConsistent = true;
      isReplInitialized = false;

      pthread_mutex_init(&acc_mutex, 0);
      pthread_mutex_init(&acc_repl_mutex, 0);

      initPadRegister();

      vals = 0;
      for (int i = 0; i < NDIM; i++) seqVal[i] = 0.0;

   }

   ~vector_par() {
      pthread_mutex_destroy(&acc_repl_mutex);
      pthread_mutex_destroy(&acc_mutex);
      if (isReplInitialized)
         reclaimRegisterItem(vals_id);
   }

   static void initPadRegister() {
      if (PAD_REG_INIT_vector)
         return;
      assert(!isParallelSectionWrapper());
      pthread_mutex_init(&PAD_REG_MUTEX_vector, 0);
      PAD_REG_vector = (void*) new state_val_pad*[PAD_REG_SIZE];
      state_val_pad** PAD_REGx = (state_val_pad**) PAD_REG_vector;
      PAD_REG_IDX_vector = 0;
      for (int i = 0; i < PAD_REG_SIZE; i++) {
         PAD_REGx[i] = new state_val_pad[MAX_PROC_SIZE];
      }
      PAD_REG_INIT_vector = true;
   }

   static void getNextRegisterItem(state_val_pad ** ret_ptr, int & ret_id) {
      // better implementation may include linked list for assigned
      // free elements, with additional elements produced if required
      //assert (PAD_REG_INIT_acc_double);
      if (!PAD_REG_INIT_vector)
         initPadRegister();

      //printf("%d   %d\n", PAD_REG_INIT_vector, PAD_REG_IDX_vector);
      //     assert (true == PAD_REG_INIT_vector);
      //     assert (PAD_REG_IDX_vector < PAD_REG_SIZE);
      pthread_mutex_lock(&PAD_REG_MUTEX_vector);
      int id = PAD_REG_IDX_vector++;
      pthread_mutex_unlock(&PAD_REG_MUTEX_vector);
      ret_id = id;
      *ret_ptr = ((state_val_pad**) PAD_REG_vector)[id];

   }

   static void reclaimRegisterItem(int id) {
      // todo
   }


   void initReplication() {
      if (isReplInitialized)
         return;
      pthread_mutex_lock(&acc_repl_mutex);
      if (isReplInitialized) {
         pthread_mutex_unlock(&acc_repl_mutex);
         return;
      }
      getNextRegisterItem(&vals, vals_id);

      for (int i = 0; i < getThreadNumWrapper(); i++)
         for (int k = 0; k < NDIM; ++k)
            vals[i].val[k] = 0.0;

      isReplInitialized = true;
      pthread_mutex_unlock(&acc_repl_mutex);
      //printf("VectorReplicationInit... %p\n", this);
   }

   //equivalent to __readval
   void __updateState() {
      //printf("State updated..\n");
      assert (!isParallelSectionWrapper());
      for (int k = 0; k < NDIM; ++k) {
         double val = 0.0;
         for (int i = 0; i < getThreadNumWrapper(); i++) {
            val += vals[i].val[k];
            vals[i].val[k] = 0.0;
         }
         seqVal[k] += val;
      }
      isConsistent = true;
   }

   void vecPrint() {
      if (!isConsistent) __updateState();
      printf("(%6.4f %6.4f %6.4f) ", seqVal[0], seqVal[1], seqVal[2]);
   }
   inline double vecField(int idx) {
      if (!isConsistent) __updateState();
      return seqVal[idx];
   }
   inline void vecFieldAdd(int idx, double v) {
      if (!isConsistent) __updateState();
      seqVal[idx] += v;
   }
   inline void vecFieldClr(int idx) {
      if (!isConsistent) __updateState();
      seqVal[idx] = 0.0;
   }
   inline void vecFieldSet(int idx, double v) {
      if (!isConsistent) __updateState();
      seqVal[idx] = v;
   }
   inline void vecLoad(double *v) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         seqVal[i] = v[i];
   }
   inline void vecStore(double *v) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         v[i] = seqVal[i];
   }

   void vecMin(double *v) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         if (seqVal[i] > (v[i]))
            seqVal[i] = (v[i]);
   }

   double vecDotProd(double *v) {
      int i;
      double d;
      if (!isConsistent) __updateState();
      d = 0.0;
      for (i = 0; i < NDIM; i++)
         d += (seqVal[i] * v[i]);
      return d;
   }

   double vecNorm1() {
      int i;
      double d = 0.0;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         d += seqVal[i];
      return d;
   }

   void vecClr() {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         seqVal[i] = 0.0;
   }

// also replicable...
   void vecSub(double *v) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         seqVal[i] -= v[i];
   }

   void vecSubSync(double *v) {
      pthread_mutex_lock(&acc_mutex);
      assert (isConsistent);
      vecSub(v);
      pthread_mutex_unlock(&acc_mutex);
   }


// also replicable...
   void vecProd(double *v) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         seqVal[i] *= v[i];
   }

   void vecProdSync(double *v) {
      pthread_mutex_lock(&acc_mutex);
      assert (isConsistent);
      vecProd(v);
      pthread_mutex_unlock(&acc_mutex);
   }


   void vecScale(double s) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         seqVal[i] *= s;
   }

   void vecAdd(double *v) {
      vecAddUnsync(v);
   }

   void vecAddUnsync(double *v) {
      int i;
      if (!isConsistent) __updateState();
      for (i = 0; i < NDIM; i++)
         seqVal[i] += v[i];
   }

   void vecAddSync(double *v) {
      pthread_mutex_lock(&acc_mutex);
      assert (isConsistent);
      vecAddUnsync(v);
      pthread_mutex_unlock(&acc_mutex);
   }

   void vecAddRepl(double *v) {
      if (isConsistent) {
         if (!isReplInitialized) initReplication();
         isConsistent = false;
      }
      for (int i = 0; i < NDIM; i++)
         vals[getCurrentThreadIdWrapper()].val[i] += v[i];
   }


};

