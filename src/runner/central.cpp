#include "topaz.h"
#include "runner.h"

#include "pin_util.h"

int central_routine(int argc, char ** argv){
	int ret = 0;
	topaz_init(argc, argv);
	Topaz::topaz->getTimers()->start(MAIN_TIMER); // start main computation timer
	topaz_main(argc, argv);
	Topaz::topaz->getTimers()->stop(MAIN_TIMER); // start computation timer
	topaz_finish(argc, argv);
	Topaz::topaz->finalize();
	printf("+++ Main Exiting...\n");
	return ret;
}
