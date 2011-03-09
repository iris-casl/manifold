// Define the event scheduler for IAASim
// George F. Riley, Georgia Tech, Summer 2009

// This scheduler is loosely based on the event scheduler found in ns-3.
// The design is such that any function, either a global static function
// or a member function on an object can be the event handler. Further
// the handler function can have arbitrary parameter types.  This implementation
// defines functions with up to four arguments.  Should the need arise
// it is easy to add longer argument lists.

#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <set>
#include <map>
#include "component.h"

class Component;

// First define the base class Event, then the events with up
// to four templated parameters.

// Define the base event class
class EventBase 
{
 public:
  EventBase(double t) : time(t), uid(nextUID++) {}
    EventBase(double t, int u) : time(t), uid(u) {}
      virtual void CallHandler() = 0;  // All subclasses must implement CallHandler
 public:
      double time;   // Timestamp for the event
      int    uid;    // Each event has a unique identifier to break timestamp ties
      static int nextUID;
 private:
};

// Define an EventID class that is the return type from all schedule
// functions.  This is used to cancel the event
class EventId : public EventBase
{
public:
  EventId(double t, int u) : EventBase(t, u) {}
  void CallHandler() {}
};

// Define the variable parameter list Event objects  
template<typename T, typename OBJ>
class Event0 : public EventBase
{
public:
  Event0(double t, void (T::*f)(void), OBJ* obj0)
    : EventBase(t), handler(f), obj(obj0){}
  void (T::*handler)(void);
  OBJ*      obj;
public:
  void CallHandler();
};

template <typename T, typename OBJ>
void Event0<T, OBJ>::CallHandler()
{
  (obj->*handler)();
}

template<typename T, typename OBJ, typename U1, typename T1>
class Event1 : public EventBase
{
public:
  Event1(double t, void (T::*f)(U1), OBJ* obj0, T1 t1_0)
    : EventBase(t), handler(f), obj(obj0), t1(t1_0){}
  void (T::*handler)(U1);
  OBJ*      obj;
  T1        t1;
public:
  void CallHandler();
};

template <typename T, typename OBJ, typename U1, typename T1>
void Event1<T, OBJ, U1, T1>::CallHandler()
{
  (obj->*handler)(t1);
}

template<typename T, typename OBJ,
         typename U1, typename T1, 
         typename U2, typename T2>
class Event2 : public EventBase
{
public:
  Event2(double t, void (T::*f)(U1, U2), OBJ* obj0, T1 t1_0, T2 t2_0)
    : EventBase(t), handler(f), obj(obj0), t1(t1_0), t2(t2_0) {}
  void (T::*handler)(U1, U2);
  OBJ*      obj;
  T1        t1;
  T2        t2;
public:
  void CallHandler();
};

template <typename T, typename OBJ, 
          typename U1, typename T1,
          typename U2, typename T2>
void Event2<T, OBJ, U1, T1, U2, T2>::CallHandler()
{
  (obj->*handler)(t1, t2);
}

template <typename T,  typename OBJ,
          typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3>
class Event3 : public EventBase {
public:
   Event3(double t, void (T::*f)(U1, U2, U3), OBJ *obj0, T1 t1_0, T2 t2_0, T3 t3_0)  
     : EventBase(t), handler(f), obj(obj0), t1(t1_0), t2(t2_0), t3(t3_0) {}
   void (T::*handler)(U1, U2, U3);
   OBJ* obj;
   T1 t1;
   T2 t2;
   T3 t3;
   
public:
   void CallHandler();
};

template <typename T,  typename OBJ,
          typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3> 
void Event3<T,OBJ,U1,T1,U2,T2,U3,T3>::CallHandler() {
     (obj->*handler)(t1,t2,t3);
}

template <typename T,  typename OBJ,
          typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3,
          typename U4, typename T4>
class Event4 : public EventBase {
public:
   Event4(double t, void (T::*f)(U1, U2, U3, U4), OBJ *obj0, T1 t1_0, T2 t2_0, T3 t3_0, T4 t4_0)  
     : EventBase(t), handler(f), obj(obj0), t1(t1_0), t2(t2_0), t3(t3_0), t4(t4_0){}
   void (T::*handler)(U1, U2, U3, U4);
   OBJ* obj;
   T1 t1;
   T2 t2;
   T3 t3;
   T4 t4;
   
public:
   void CallHandler();
};

template <typename T,  typename OBJ,
          typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3,
          typename U4, typename T4> 
void Event4<T,OBJ,U1,T1,U2,T2,U3,T3,U4,T4>::CallHandler() {
     (obj->*handler)(t1,t2,t3,t4);
}


// Create events that call static functions rather then
// rather than object member functions
// Also need a variant of the Event0 that calls a static function,
// not a member function.
class Event0Stat : public EventBase
{
public:
  Event0Stat(double t, void (*f)(void))
    : EventBase(t), handler(f){}
  void (*handler)(void);
public:
  void CallHandler();
};

template<typename U1, typename T1>
class Event1Stat : public EventBase
{
public:
  Event1Stat(double t, void (*f)(U1), T1 t1_0)
    : EventBase(t), handler(f), t1(t1_0){}
  void (*handler)(U1);
  T1        t1;
public:
  void CallHandler();
};

template <typename U1, typename T1>
void Event1Stat<U1, T1>::CallHandler()
{
  handler(t1);
}


template<typename U1, typename T1, 
         typename U2, typename T2>
class Event2Stat : public EventBase
{
public:
  Event2Stat(double t, void (*f)(U1, U2), T1 t1_0, T2 t2_0)
    : EventBase(t), handler(f), t1(t1_0), t2(t2_0) {}
  void (*handler)(U1, U2);
  T1        t1;
  T2        t2;
public:
  void CallHandler();
};

template <typename U1, typename T1,
          typename U2, typename T2>
void Event2Stat<U1, T1, U2, T2>::CallHandler()
{
  handler(t1, t2);
}

template <typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3>
class Event3Stat : public EventBase {
public:
   Event3Stat(double t, void (*f)(U1, U2, U3), T1 t1_0, T2 t2_0, T3 t3_0)  
     : EventBase(t), handler(f), t1(t1_0), t2(t2_0), t3(t3_0) {}
   void (*handler)(U1, U2, U3);
   T1 t1;
   T2 t2;
   T3 t3;
   
public:
   void CallHandler();
};

template <typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3> 
void Event3Stat<U1,T1,U2,T2,U3,T3>::CallHandler()
{
  handler(t1,t2,t3);
}

template <typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3,
          typename U4, typename T4>
class Event4Stat : public EventBase {
public:
   Event4Stat(double t, void (*f)(U1, U2, U3, U4), T1 t1_0, T2 t2_0, T3 t3_0, T4 t4_0)  
     : EventBase(t), handler(f), t1(t1_0), t2(t2_0), t3(t3_0), t4(t4_0){}
   void (*handler)(U1, U2, U3, U4);
   T1 t1;
   T2 t2;
   T3 t3;
   T4 t4;
   
public:
   void CallHandler();
};

template <typename U1, typename T1,
          typename U2, typename T2,
          typename U3, typename T3,
          typename U4, typename T4> 
void Event4Stat<U1,T1,U2,T2,U3,T3,U4,T4>::CallHandler() {
     handler(t1,t2,t3,t4);
}

// Now define the sorted set of events and the event comparator
class event_less
{
public:
  event_less() { }
  inline bool operator()(EventBase* const & l, const EventBase* const & r) const {
    if(l->time < r->time) return true;
    if (l->time == r->time) return l->uid < r->uid;
    return false;
  }
};

// Define the type for the sorted event list
typedef std::set<EventBase*, event_less> EventSet_t;


class ComponentDescription
{
 public:
  ComponentDescription(int lp, Component* obj):lpId(lp), ptr(obj){}
  int lpId;
  Component* ptr;
};

//Define the type for the map of component IDs to LPs/pointers
typedef std::map<int, ComponentDescription*> ComponentMap_t;

// Define the simulator class.  It is implemented with only static functions
class Simulator 
{
public:
  static void    Run();                 // Run the simulation
  static void    Stop();                // Stop the simulation
  static void    StopAt(double);        // Stop at the specified time
  static double  Now();                 // Return the current simulation time
  static bool    Cancel(EventId&);      // Cancel previously scheduled event
  static EventId Peek();                // Peek (don't remove) earliest event
  static EventBase* GetEarliestEvent(); // Remove and return earliest event
  static int     MyRank();              // Return rank number (0 if serial)
  // Define the templated schedule functions
  template <typename T, typename OBJ>
    static EventId Schedule(double t, void(T::*handler)(void), OBJ* obj)
  {
    EventBase* ev = new Event0<T, OBJ>(t, handler, obj);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename T, typename OBJ,
    typename U1, typename T1>
    static EventId Schedule(double t, void(T::*handler)(U1), OBJ* obj, T1 t1)
  {
    EventBase* ev = new Event1<T, OBJ, U1, T1>(t, handler, obj, t1);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename T, typename OBJ,
    typename U1, typename T1,
    typename U2, typename T2>
    static EventId Schedule(double t, void(T::*handler)(U1, U2), OBJ* obj, T1 t1, T2 t2)
  {
    EventBase* ev = new Event2<T, OBJ, U1, T1, U2, T2>(t, handler, obj, t1, t2);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename T, typename OBJ,
    typename U1, typename T1,
    typename U2, typename T2,
    typename U3, typename T3>
    static EventId Schedule(double t, void(T::*handler)(U1, U2, U3), OBJ* obj, T1 t1, T2 t2, T3 t3)
  {
    EventBase* ev = new Event3<T, OBJ, U1, T1, U2, T2, U3, T3>(t, handler, obj, t1, t2, t3);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename T, typename OBJ,
    typename U1, typename T1,
    typename U2, typename T2,
    typename U3, typename T3,
    typename U4, typename T4>
    static EventId Schedule(double t, void(T::*handler)(U1, U2, U3, U4), OBJ* obj, T1 t1, T2 t2, T3 t3, T4 t4)
  {
    EventBase* ev = new Event4<T, OBJ, U1, T1, U2, T2, U3, T3, U4, T4>(t, handler, obj, t1, t2, t3, t4);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  // Schedulers for static callback functions
  static EventId Schedule(double t, void(*handler)(void))
  {
    EventBase* ev = new Event0Stat(t, handler);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename U1, typename T1>
    static EventId Schedule(double t, void(*handler)(U1), T1 t1)
  {
    EventBase* ev = new Event1Stat<U1, T1>(t, handler, t1);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename U1, typename T1,
            typename U2, typename T2>
    static EventId Schedule(double t, void(*handler)(U1, U2), T1 t1, T2 t2)
  {
    EventBase* ev = new Event2Stat<U1, T1, U2, T2>(t, handler, t1, t2);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename U1, typename T1,
            typename U2, typename T2,
            typename U3, typename T3>
    static EventId Schedule(double t, void(*handler)(U1, U2, U3), T1 t1, T2 t2, T3 t3)
  {
    EventBase* ev = new Event3Stat<U1, T1, U2, T2, U3, T3>(t, handler, t1, t2, t3);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  template <typename U1, typename T1,
            typename U2, typename T2,
            typename U3, typename T3,
            typename U4, typename T4>
    static EventId Schedule(double t, void(*handler)(U1, U2, U3, U4), T1 t1, T2 t2, T3 t3, T4 t4)
  {
    EventBase* ev = new Event4Stat<U1, T1, U2, T2, U3, T3, U4, T4>(t, handler, t1, t2, t3, t4);
    events.insert(ev);
    return EventId(t, ev->uid);
  }

  static void registerComponent(Component* obj, int lp);
  static ComponentDescription* getComponentDesc(int);

private:
  static EventSet_t events;
  static double     simTime;
  static ComponentMap_t components;
  static int nextComponentID;
  static int rank;
  static bool       halted;
};

#endif
