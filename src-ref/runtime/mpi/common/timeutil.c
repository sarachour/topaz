#include "timeutil.h"


#include <time.h>

#include <cstdio>
#include <cstdlib>



typedef struct timespec stime_t;


#ifndef INLINE
#define INLINE static inline
#endif

#define CONST const

INLINE void gettime(stime_t * pTimeDest) {
   // POSSIBLE CLOCKS:
   //   CLOCK_REALTIME
   //          System-wide  realtime clock.  Setting this clock requires appro-
   //          priate privileges.
   //   CLOCK_MONOTONIC
   //          Clock that cannot be set and  represents  monotonic  time  since
   //          some unspecified starting point.
   //   CLOCK_PROCESS_CPUTIME_ID
   //          High-resolution per-process timer from the CPU.
   //   CLOCK_THREAD_CPUTIME_ID
   //          Thread-specific CPU-time clock.
   clock_gettime(CLOCK_REALTIME, pTimeDest);
   //gettimeofday(pTimeDest,0);

}

INLINE void gettime2(stime_t * pTimeDest, int timerMode) {
   clock_gettime(timerMode, pTimeDest);

}

/// Assigns the value of one time variable to the other
INLINE void settime(stime_t * pTimeDest, CONST stime_t * pTimeSrc) {
   pTimeDest->tv_sec = pTimeSrc->tv_sec;
   pTimeDest->tv_nsec = pTimeSrc->tv_nsec;

}

/// Resets the time to 0
INLINE void resettime(stime_t * pTimeDest) {
   pTimeDest->tv_sec = 0;
   pTimeDest->tv_nsec = 0;

}

// Checks if time is reset
INLINE int isresettime(stime_t * pTimeDest) {
   return pTimeDest->tv_sec == 0 && pTimeDest->tv_nsec == 0;


}

#define NSEC_SCALE 1000000000
/// Returns the interval between two moments
INLINE void delta_t(stime_t *interval, CONST stime_t *begin, CONST stime_t *now) {

   interval->tv_nsec = now->tv_nsec - begin->tv_nsec;
   if(interval->tv_nsec < 0 ){
      interval->tv_nsec += NSEC_SCALE;
      interval->tv_sec = now->tv_sec - begin->tv_sec - 1;
   }
   else{
      interval->tv_sec = now->tv_sec - begin->tv_sec;
   }

}
//subtract from initial time
INLINE void sub_t(stime_t *dest, CONST stime_t *begin, CONST stime_t *add) {
   dest->tv_nsec = begin->tv_nsec - add->tv_nsec ;
   if(dest->tv_nsec < 0 ){
      dest->tv_nsec += NSEC_SCALE;
      dest->tv_sec =  begin->tv_sec -add->tv_sec - 1;
   }
   else{
	      dest->tv_sec =  begin->tv_sec - add->tv_sec;
   }
   if(dest->tv_sec < 0){
		dest->tv_sec = 0;
		dest->tv_nsec = 0;
	 }


}
/// Adds interval to the initial time
INLINE void add_t(stime_t *dest, CONST stime_t *begin, CONST stime_t *add) {
   dest->tv_nsec = add->tv_nsec + begin->tv_nsec;
   if(dest->tv_nsec >= NSEC_SCALE ){
      dest->tv_nsec -= NSEC_SCALE;
      dest->tv_sec = add->tv_sec + begin->tv_sec + 1;
   }
   else{
	      dest->tv_sec = add->tv_sec + begin->tv_sec;
   }

}

INLINE void add_dest_t(stime_t *dest, CONST stime_t *add) {
   add_t(dest, dest, add);
}

// factor is a power of 10
INLINE void scale_t(stime_t *dest, int factor) {
   int d = dest->tv_sec % factor;
   dest->tv_sec /= factor;
   dest->tv_nsec = (NSEC_SCALE/factor)*d + dest->tv_nsec / factor;
}


INLINE float fraction_t(CONST stime_t *numer, CONST stime_t *denom) {
  unsigned long n = numer->tv_sec*NSEC_SCALE + numer->tv_nsec;
  unsigned long d = denom->tv_sec*NSEC_SCALE + denom->tv_nsec;
  return n/(float)d;
}

/// Compares two times
//     Returns:
//              -1:  Time1 is smaller than Time2
//               0:  Time1 and Time2 are equal
//              +1:  Time1 is greater than Time2
INLINE int compare_t(CONST stime_t * time1, CONST stime_t * time2) {
   stime_t tmp;
   delta_t(&tmp, time2, time1);
   return (tmp.tv_sec < 0)? -1 : (tmp.tv_sec >0 || tmp.tv_nsec >0)? 1: 0; // hack
}

/// Prints time to the string constant (whole numbers are seconds, fractions up to nanoseconds)
INLINE char * printTime(CONST stime_t * t, char * printbuffer) {
   sprintf(printbuffer, "%ld.%09ld", t->tv_sec, t->tv_nsec);
   return printbuffer;
}


#define TIMER_START_C(timer) \
   do { \
      stime_t tstart##timer, tend##timer; \
      char tbuffer##timer[20]; \
      gettime(&tstart##timer);

#define TIMER_END_C(timer, msg) \
      gettime(&tend##timer); \
      delta_t(&tend##timer, &tstart##timer, &tend##timer); \
      if (msg[0] != '\0') { \
         printTime(&tend##timer, tbuffer##timer); \
         printf(msg " %s\n", tbuffer##timer); \
      } \
   } while (0);

#define TIMER_END_CSCALE(timer, msg, scale)		\
      gettime(&tend##timer); \
      delta_t(&tend##timer, &tstart##timer, &tend##timer); \
      scale_t(&tend##timer, scale); \
      if (msg[0] != '\0') { \
         printTime(&tend##timer, tbuffer##timer); \
         printf(msg " %s\n", tbuffer##timer); \
      } \
   } while (0);

#define TIMER_PAUSE_C(timer) \
      gettime(&tend##timer); \
      delta_t(&tend##timer, &tstart##timer, &tend##timer); \
      add_t(&ttotal##timer, &ttotal##timer, &tend##timer); \
   } while (0);


#define STOPWATCH_INITIALIZE(timer) \
   stime_t ttotal##timer; \
   char tbuffer##timer[20]; \
   resettime(&ttotal##timer);

#define STOPWATCH_FINALIZE(timer, msg) \
   if (msg[0] != '\0') { \
      printTime(&ttotal##timer, tbuffer##timer); \
      printf(msg " %s\n", tbuffer##timer); \
   }

#define STOPWATCH_FINALIZE_SCALE(timer, scale, msg) \
   scale_t(&ttotal##timer, scale); \
   if (msg[0] != '\0') { \
      printTime(&ttotal##timer, tbuffer##timer); \
      printf(msg " %s\n", tbuffer##timer); \
   }





#include <iostream>
#include <string>

class Interval;
class Time;

class TimerBase {

public:

   TimerBase(bool now=false, bool printatend=false, const char * name = 0) {
      if (now) setCurrentTime();
      else resetTime();

      fPrintInDestructor = printatend;
      fTimerName = name!=0? name : "#T";
      if (fPrintInDestructor) {
         setOutputStream(std::cerr);
      }
   }

   TimerBase(const TimerBase& src) {
      settime(getImpl(), src.getImpl());
   }

   virtual ~TimerBase() {
     if (fPrintInDestructor)
       printTimerBase();
   }
   
   void printTimerBase() {
     getOutputStream() << fTimerName << " = " << this->toString() << "\n";
   }

   void setCurrentTime() {
     gettime(getImpl());
   }

   void setTime(TimerBase& other) {
      settime(getImpl(), other.getImpl());
   }

   void resetTime() {
      resettime(getImpl());
   }

   bool isResetTime() {
      return isresettime(getImpl());
   }

   std::string toString() const {
      char buffer[200];
      return std::string(printTime(getImpl(), buffer));
   }

   double toDouble() const {
      char buffer[200];
      printTime(getImpl(), buffer);
      return atof(buffer);
   }


protected:
   stime_t * getImpl() { return &fTimeImpl; }
   const stime_t * getImpl() const { return &fTimeImpl; }

   stime_t fTimeImpl;

   bool fPrintInDestructor;
   std::ostream * fOutputStream;
   const char * fTimerName;
   std::ostream& getOutputStream () const { return fOutputStream? *fOutputStream : std::cerr; }
   void setOutputStream (std::ostream& str) { fOutputStream = &str; }

   friend Interval operator-(const Time& t1, const TimerBase& t2);

};


class Time : public TimerBase {
public:
   Time (bool now = false, bool printAtTermination = false, const char * timerName = 0) : TimerBase(now, printAtTermination, timerName) {}
   Time (const Time& src) : TimerBase(src) {}

   friend Interval operator-(const Time& t1, const TimerBase& t2);

   bool operator<(const Time& t2) const {
     return compare_t(this->getImpl(), t2.getImpl()) == -1;
   }

   bool operator>(const Time& t2) const {
     return compare_t(this->getImpl(), t2.getImpl()) == 1;
   }

   bool operator==(const Time& t2) const {
     return compare_t(this->getImpl(), t2.getImpl()) == 0;
   }
};

class Interval : public TimerBase {
public:
   Interval (bool now = false, bool printAtTermination = false, const char * timerName = 0) : TimerBase(now, printAtTermination, timerName) {}
   Interval (const Interval& src) : TimerBase(src) {}

   Interval& operator+=(const Interval& t2) {
      Interval& t1 = *this;
      add_t(t1.getImpl(), t1.getImpl(), t2.getImpl());
      return t1;
   }

   Interval operator+(const Interval& v2) {
      Interval ret;
      add_t(ret.getImpl(), this->getImpl(), v2.getImpl());
      return ret;
   }

   Interval operator+(const double val) {
     Interval ret;
     stime_t& tmp = fTimeImpl;
     tmp.tv_sec = (int) val;
     tmp.tv_nsec = (int) (1000000000 * (val - tmp.tv_sec));
     add_dest_t(&tmp, this->getImpl());
     return ret;
   }
   

   Interval sub(const Interval& v2) {
     Interval ret;
     sub_t(ret.getImpl(), this->getImpl(), v2.getImpl());
     return ret;
   }

   float operator/(const Interval& v2) {
     return fraction_t(this->getImpl(), v2.getImpl());
   }
    Interval div(const double& v2) {
			Interval ret = *this;
      scale_t(ret.getImpl(), v2);
      return ret;
   }

};


inline Interval operator-(const Time& t1, const TimerBase& t2) {
   Interval ret;
   delta_t(ret.getImpl(), t2.getImpl(), t1.getImpl());
   return ret;
}

class Timer {
public:
	Timer(const char * timerName = 0) : elapsed(false, false, timerName) {
		started = false;
		timer_rt=0;
		reset();
	}

	void calibrate(){
		int i=0;
		int NRUNS=100;
		timer_rt=0;
		stop();
		reset();
		for(i=0; i < NRUNS; i++){
			start();
			pause();
		}
		return;
	
		timer_rt = elapsed.div(NRUNS);
		reset();
	}
	void start() {
		if (!started) {
			tstart.setCurrentTime();
			started = true;
		}
	}

	void pause() {
		if (started) {
			Time tnow(true);
			started = false;
			Interval diff = tnow - tstart;
			//printf("diff: %f, calib: %f,", diff.toDouble(), timer_rt.toDouble());
			diff = diff.sub(timer_rt);
			elapsed += diff;
			//printf("new: %f -> elapsed %f\n", diff.toDouble(), elapsed.toDouble());
		}
	}

	void reset() {
		started = false;
		elapsed.resetTime();
	}

	void stop() {
		pause();
	}

	void print () {
		std::cerr << elapsed.toString() << "\n";
	}

	double toDouble() const {
		if (started) return -1.0;
		else return	elapsed.toDouble();
	}

	std::string toString() const {
		if (started) return "NA";
		else return elapsed.toString();
	}

private:
	Interval elapsed;

	Time tstart;

	bool started;
  Interval timer_rt;
};

/*
 * Returns the percantage of time of one of the two complementary timers
 */
static inline double TimerPercentC(const Timer& t1, const Timer& t2) {
	double t1t = t1.toDouble();
	double t2t = t2.toDouble();
	if (t1t < 0 || t2t < 0) { return -1.0; } // if there was an error in the timer calls
	return t1t/(t1t + t2t);
}





Timer MOtherTimer("Main");
Timer MTotalTimer("Main-Total");
Timer MCommTimer("M-Comm");
Timer WTotalTimer("W-Comm");
Timer WKernelTimer("W-Comm");
Timer WOtherTimer("W-Other");
Timer WCommTimer("W-Comm");
Timer MCheckTimer("W-Checker");


void MainResetAll(){
	MOtherTimer.reset();
	MCommTimer.reset();
	MOtherTimer.start();
}
void MainPauseAll(){
	MOtherTimer.pause();
	MCommTimer.pause();
}
void MainStart(){
	MOtherTimer.calibrate();
	MCommTimer.calibrate();
	MTotalTimer.calibrate();
	MOtherTimer.start();
	MTotalTimer.start();
}
void MCommStart(){
	MTotalTimer.pause();
	MOtherTimer.pause();
	MTotalTimer.start();
	MCommTimer.start();
}
void MCommStop(){
	MCommTimer.pause();
	MTotalTimer.pause();
	MOtherTimer.start();
	MTotalTimer.start();
}

void MainStop(){
	MOtherTimer.pause();
	MTotalTimer.stop();
	float other = MOtherTimer.toDouble();
	float comm = MCommTimer.toDouble();
	float total = other+comm;
	float total_nocomm = other;
	float total_real = MTotalTimer.toDouble();
	printf("label	type	abs-time	pct-time	pct-time-no-comm\n");
	printf("M	Comm	%f	%f%%	0%%\n", comm, comm/total*100.0);
	printf("M	Other	%f	%f%%	100%%\n", other, other/total*100.0);
	printf("M	Total	%f %f\n", total, total_real);
}

void WKernelStart(){
	WTotalTimer.pause();
	WOtherTimer.pause();
	WTotalTimer.start();
	WKernelTimer.start();
}

void WKernelStop(){
	WTotalTimer.pause();
	WKernelTimer.pause();
	WTotalTimer.start();
	WOtherTimer.start();
}

void WCommStart(){
	WTotalTimer.pause();
	WOtherTimer.pause();
	WTotalTimer.start();
	WCommTimer.start();
}
void WCommStop(){
	WTotalTimer.pause();
	WCommTimer.pause();
	WTotalTimer.start();
	WOtherTimer.start();
}
void WorkerStart(){
	WCommTimer.calibrate();
	WOtherTimer.calibrate();
	WKernelTimer.calibrate();
	WTotalTimer.calibrate();
	WOtherTimer.start();
	WTotalTimer.start();
}

void WorkerStop(int rank){
	WOtherTimer.stop();
	WTotalTimer.stop();
	float other = WOtherTimer.toDouble();
	float kernel = WKernelTimer.toDouble();
	float comm = WCommTimer.toDouble();
	float total = other+kernel+comm;
	float total_nocomm = other+kernel;
	float total_rel = WTotalTimer.toDouble();
	printf("label	type	abs-time	pct-time	pct-time-no-comm\n");
	printf("W[%d]	Comm	%f	%f%%	0%%\n", rank, comm, comm/total*100.0);
	printf("W[%d]	Kernel	%f	%f%%	%f%%\n", rank, kernel, kernel/total*100.0, kernel/total_nocomm*100.0);
	printf("W[%d]	Other	%f	%f%%	%f%%\n", rank, other, other/total*100.0, other/total_nocomm*100.0);
	printf("W[%d]	Total	%f %f\n",rank, total, total_rel);
	
}
