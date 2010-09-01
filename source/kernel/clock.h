#include <list>

class tickObjBase 
{
  public:
  virtual void CallTick() = 0;
};

template <typename OBJ>
class tickObj : public tickObjBase {
 public:
  tickObj(OBJ* o, void (OBJ::*f)(void)) : obj(o), funct(f) {}
  void CallTick() 
  {
    (obj->*funct)();
  }
  OBJ* obj;
  void (OBJ::*funct)(void);
};


using namespace std;
class Clock
{
 public:
  Clock(double);
 public:
  double frequency;    //Frequency of the clock
  double period;  //Period of the clock
  void risingEdge();
  void fallingEdge();
  //List of handlers registered for rising edge
  list<tickObjBase*> risingObjs;
  //List of handlers registered for falling edge
  list<tickObjBase*> fallingObjs;
};

template <typename OBJ> void registerClock(Clock* c, OBJ* obj, void(OBJ::*rising)(void), void(OBJ::*falling)(void))
{
  if(rising) 
    {
      tickObjBase* riseObj=new tickObj<OBJ>(obj, rising);
      c->risingObjs.push_back(riseObj);
    }
  if(falling)
    {
      tickObjBase* fallObj=new tickObj<OBJ>(obj, falling);
      c->fallingObjs.push_back(fallObj);
    }
}
