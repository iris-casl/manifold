#include "simulator.h"
#include "link.h"

using namespace std;

Link::Link(int srcComponentId, int linkWidth)
{
  src=srcComponentId;
  width=linkWidth;
  //lookup src component
  ComponentDescription* compDesc=Simulator::getComponentDesc(srcComponentId);
  if(compDesc->lpId==Simulator::MyRank())
    {
      //add link to src component
      compDesc->ptr->addOutputLink(this);
    }
}

void Link::Send(uint64_t data, int srcComponentId)
{
  list<OutputBase*>::iterator iter;
  for(iter=outputs.begin(); iter!=outputs.end(); iter++)
    {
      ComponentDescription* compDesc=
	Simulator::getComponentDesc((*iter)->componentId);
      if(compDesc->lpId==Simulator::MyRank())
	{
	  Simulator::Schedule(Simulator::Now()+((*iter)->latency), 
			      &OutputBase::CallHandler, (*iter), data, 
			      srcComponentId);
	}
      else
	{
	  //Send with MPI
	}
    }
}
