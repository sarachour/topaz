#include "barnes.h"

extern nbody_t NBODY;
extern parms_t PARMS;
// ----------------------------------------------------------------------------
int main(int argc, char **argv){
  nbody_init(&NBODY);
  parms_cons(&PARMS);
  nbody_startrun(&NBODY,&PARMS);
  nbody_initOutput(&NBODY,&PARMS);
  nbody_setbound(&NBODY);
  nbody_clearTiming(&NBODY);
  nbody_clearStats(&NBODY);

  nbody_stepsystem(&NBODY,&PARMS);

  //  bodyset * bset = Nbody.getbodies();
  //for (int i = 0; i < bset->size(); i++)
  //  bset->

  nbody_outputTiming(&NBODY);
  //Nbody.outputStats();
  nbody_output(&NBODY, &PARMS);
  nbody_outputEnergy(&NBODY);
  //Nbody.dump();
}
