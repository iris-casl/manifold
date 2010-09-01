#ifndef __LINK_H__
#define __LINK_H__

#include "simulator.h"
#include <list>
#include <iostream>
#include <stdint.h>

using namespace std;

class OutputBase
{
 public:
  OutputBase(int ID, double l) : componentId(ID), latency(l){}
    virtual void CallHandler(uint64_t, int) = 0;
    int componentId;
    double latency;
};

template<typename OBJ>
  class Output0 : public OutputBase
{
 public:
  Output0(int componentId, double l, void (OBJ::*f)(uint64_t, int), OBJ* obj0)
    : OutputBase(componentId, l), handler(f), obj(obj0){}
    void (OBJ::*handler)(uint64_t, int);
    OBJ* obj;
    void CallHandler(uint64_t data, int src)
    {
      //cout<<"In Output0 handler"<<endl;
      (obj->*handler)(data, src);
    }
};

class Link
{
 public:

  int src;
  int width;

  list<OutputBase*> outputs;
  Link(int srcComponentId, int linkWidth);

  void Send(uint64_t data, int srcComponentId);  
};

 template<typename OBJ>
 void addOutput(Link* l, int outComponent, double latency, 
		   void (OBJ::*f)(uint64_t, int), OBJ* obj0)
  {
    OutputBase* temp = new Output0<OBJ>(outComponent, latency, f, obj0);
    l->outputs.push_back(temp);
  }
#endif
