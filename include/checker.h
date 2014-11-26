#ifndef DETECTOR_H
#define DETECTOR_H

class Checker{
	private:
		bool __internal;
	public:
		Checker();
		bool accept();
		bool reject();
		void assert(bool expr);
};
#endif
