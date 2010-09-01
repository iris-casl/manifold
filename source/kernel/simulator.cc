// Implementation of the IAASim Simulator
// George F. Riley, Georgia Tech, Summer 2009

#include "simulator.h"

int        EventBase::nextUID = 0;
EventSet_t Simulator::events;
bool       Simulator::halted = false;
double     Simulator::simTime;
int        Simulator::rank;
int        Simulator::nextComponentID;
ComponentMap_t        Simulator::components;

// Event0Stat is not a template, so we implement it here
void Event0Stat::CallHandler()
{
  handler();
}


void Simulator::Run()
{
  rank=0;
  while(!events.empty() && !halted)
    {
      EventSet_t::iterator i = events.begin();
      EventBase* ev = *i;  // Get the event
      // Set the simulation time
      simTime = ev->time;
      // Call the event handler
      cout.flush();
      ev->CallHandler();
      // Remove the event from the pending list
      events.erase(i);
      // And delete the event
      delete ev;
    }
  
}

void Simulator::Stop()
{
  halted = true;
}

void Simulator::StopAt(double stopTime)
{ // code later
  Schedule(stopTime, &Simulator::Stop);
}

double Simulator::Now()
{
  return simTime;
}

bool Simulator::Cancel(EventId& evid)
{
  EventSet_t::iterator it = events.find(&evid);
  if (it == events.end()) return false; // Not found
  events.erase(it);                     // Otherwise erase it
  return true;
}

EventId Simulator::Peek()
{ // Return eventid for earliest event, but do not remove it
  // Event list must not be empty
  EventSet_t::iterator it = events.begin();
  return EventId((*it)->time, (*it)->uid);
}

EventBase* Simulator::GetEarliestEvent()
{
  EventSet_t::iterator it = events.begin();
  if (it == events.end()) return 0;
  return *it;
}

int Simulator::MyRank()
{
  return rank;

}

  
void Simulator::registerComponent(Component* obj, int lp)
{
  ComponentDescription* compDesc;
  if(lp==rank)
    {
      compDesc = new ComponentDescription(lp, obj);
    }
  else
    {
      compDesc = new ComponentDescription(lp, NULL);
    }
  components[nextComponentID]=compDesc;
  obj->setComponentId(nextComponentID);
  nextComponentID++;
}

ComponentDescription* Simulator::getComponentDesc(int compId)
{
  return components[compId];
}
