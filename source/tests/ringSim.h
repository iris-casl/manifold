/*
 * =====================================================================================
 *
 *       Filename:  ringSim.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2010 01:36:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */


#ifndef  ringsim_h_INC
#define  ringsim_h_INC

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<cstdlib>
#include	<map>
#include	"../kernel/component.h"
#include	"../kernel/simulator.h"
#include	"../data_types/impl/irisEvent.h"
#include	"../components/interfaces/genericComponentHeader.h"
#include	"MersenneTwister.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  Token
 *  Description:  
 *
 * =====================================================================================
 */
class Token
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        Token ();                             /* constructor */
        ~Token ();
        int last_device;
        vector<unsigned int> captured_order;
        

    protected:

    private:

}; /* -----  end of class Token  ----- */
 
/*
 * =====================================================================================
 *        Class:  RingNode
 *  Description:  
 *
 * =====================================================================================
 */
class RingNode : public Component
{
    public:
        RingNode ();                             /* constructor */
        bool has_token;
        int name_id;
        int sent_counter;
        int dest_node_id;
        RingNode* next;
        void send_data();
        void process_event(IrisEvent* e);
        void* component_ptr;

    protected:

    private:

}; /* -----  end of class RingNode  ----- */

/*
 * =====================================================================================
 *        Class:  RingComponent
 *  Description:  
 *
 * =====================================================================================
 */
class RingComponent : public Component
{
    public:
        RingComponent ();                             /* constructor */
        Token* the_token;
        unsigned int busy_counter;
        unsigned int idle_counter;
        bool have_token;
        vector<RingNode*> nodes;
        void process_event (IrisEvent* e);
        std::string toString() const;

    protected:

    private:

}; /* -----  end of class RingComponent  ----- */


#endif   /* ----- #ifndef ringsim_INC  ----- */
