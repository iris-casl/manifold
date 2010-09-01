#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <vector>
#include "simulator.h"
#include "link.h"


//class Simulator;

class Component
{
 private:
  int componentId;
 public:
  std::vector<Link*> inLinks;
  std::vector<Link*> outLinks;
  Component();
  Component(int lpId);
  ~Component();
  void setComponentId(int id);
  void addInputLink(Link* l);
  void addOutputLink(Link* l);
  int myId(){return componentId;}
};


#endif
