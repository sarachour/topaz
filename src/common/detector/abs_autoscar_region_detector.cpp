#include "scar_detector.h"
#include "util.h"
#include "stdio.h"
#include "logger.h"
#include "topaz.h"
#include "ctrl_system.h"


void AbsAutoScarRegionDetector::adjust_control(){
	
	float reexec = this->stats->get_reexec();
	float tp = this->stats->get_frac_rej_tp();
	float cost = tp;
	this->ctrl->update(cost);
	printf("r:%f	t:%f	c:%f	o:%f\n", reexec,tp,cost,this->ctrl->get());
}
void AbsAutoScarRegionDetector::set_control_target(){
	this->ctrl->goal(-1);
}

AbsAutoScarRegionDetector::AbsAutoScarRegionDetector(int n): AbsScarRegionDetector(n){
	
}
AbsAutoScarRegionDetector::~AbsAutoScarRegionDetector(){
	
}
