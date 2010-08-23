#ifndef ZESTO_QSIM_INCLUDED
#define ZESTO_QSIM_INCLUDED

//#include <iostream>
//#include <string>
//#include <typeinfo>
#define USE_PIN_TRACES
//const uint64_t RUN_CYCLES = 1000000000;
#ifndef USE_PIN_TRACES
#include "../../../qsim-0.1-pre5/qsim.h"
//#include "../qsim-0.1-pre5/mgzd.h"
//#include "../qsim-0.1-pre5/distorm/distorm.h"
using std::string; using std::cout;

using Qsim::CDomain; using Qsim::Queue; using Qsim::QueueItem;

extern Queue *q[16];
#endif
#endif
