#include "component.h"

class Simulator;

Component::~Component()
{

}

Component::Component()
{
  Simulator::registerComponent(this, 0);
}

Component::Component(int lpId)
{
  Simulator::registerComponent(this, lpId);
}

void Component::setComponentId(int id)
{
  componentId=id;
}

void Component::addInputLink(Link* l)
{
  inLinks.push_back(l);
}

void Component::addOutputLink(Link* l)
{
  outLinks.push_back(l);
}
