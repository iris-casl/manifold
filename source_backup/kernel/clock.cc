#include "clock.h"
#include "simulator.h"
#include <list>

using namespace std;

Clock::Clock(double freq)
{
  frequency=freq;
  period=1/frequency;
  //schedule first rising edge event
  Simulator::Schedule(Simulator::Now()+(period/2), 
                      &Clock::risingEdge, this);
  //schedule first falling edge event
  Simulator::Schedule(Simulator::Now()+period, &Clock::fallingEdge, this);
}

void Clock::risingEdge()
{
  //schedule next risingEdge
  Simulator::Schedule(Simulator::Now()+period, &Clock::risingEdge, this);
  //call handler for each component registered for rising
  list<tickObjBase*>::iterator iter;
  for(iter=risingObjs.begin(); iter!=risingObjs.end(); iter++)
    {
      (*iter)->CallTick();
    } 
}

void Clock::fallingEdge()
{
  //schedule next fallingEdge
  Simulator::Schedule(Simulator::Now()+period, &Clock::fallingEdge, this);
  //call handler for each component registered for falling
  list<tickObjBase*>::iterator iter;
  for(iter=fallingObjs.begin(); iter!=fallingObjs.end(); iter++)
    {
      (*iter)->CallTick();
    } 
}
