/*
 * =====================================================================================
 *
 *       Filename:  ringSim.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2010 01:45:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */


#ifndef  ringsim_cc_INC
#define  ringsim_cc_INC
#define NO_OF_RING_NODES 5
#define DEBUG 1

#include	"ringSim.h"

using namespace std;

typedef unsigned long int uniqueId;


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Token
 *      Method:  Token
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Token::Token ()
{
    last_device = 0;
}  /* -----  end of method Token::Token  (constructor)  ----- */

Token::~Token()
{
    captured_order.clear();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RingNode
 *      Method:  RingNode
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
RingNode::RingNode ()
{
    has_token = false;
    sent_counter = 0;
    dest_node_id = -1;

}  /* -----  end of method RingNode::RingNode  (constructor)  ----- */

void RingNode::process_event(IrisEvent* e)
{

#if DEBUG
    cout<< " Called process event of RingNode " << e->toString() << endl;
#endif

    has_token = true;
    MTRand mtrand1;
    Token* tok = (Token*) e->event_data.at(0);
    RingComponent* ring  = (RingComponent*) this->component_ptr;
    e->src = (Component*)this;
    e->dst = (Component*)this->component_ptr;
    tok->last_device = this->name_id;

    /* Randomly decide to send a packet and pick a random destination */
        if( mtrand1.randInt( 1 ))
        {
#if DEBUG
            cout << Simulator::Now() << " Going to pass the token around.. entering idle" << endl;
#endif
            ring->idle_counter++;
            Simulator::Schedule(Simulator::Now()+ 1, &RingComponent::process_event,(RingComponent*)this->component_ptr , e);
        }
        else
        {
            int random_dest = mtrand1.randInt( NO_OF_RING_NODES );
            /* If random dest is urself adding one to make it ur neighbor */
            if (random_dest == this->name_id)
                random_dest++;

            unsigned int random_pkt_send_time = mtrand1.randInt(100);
            tok->captured_order.push_back(this->name_id);
            ring->have_token = false;
            ring->busy_counter += random_pkt_send_time;
#if DEBUG
            cout << Simulator::Now() << " Going to send packet to dest: " << random_dest << " busy for the next " << random_pkt_send_time << " cycles." << endl;
#endif
            Simulator::Schedule(Simulator::Now()+random_pkt_send_time, &RingComponent::process_event,(RingComponent*)this->component_ptr , e);
        }
}

/*  Ring Component Impl */
RingComponent::RingComponent()
{
    have_token = true;
    idle_counter = 0;
    busy_counter = 0;

}

string RingComponent:: toString() const
{
    stringstream str;
    str << Simulator::Now() << "have_token: " << have_token << " idleCounter " << idle_counter << ". ";
    return str.str();
}

void RingComponent::process_event(IrisEvent* e)
{
#if DEBUG
    cout<< " Called process event of RingComponent " << e->toString() << endl;
#endif

    RingNode* next_node = (RingNode*)e->src;
    e->dst = (Component*)next_node->next;
    e->src = this;
    Token* tok = (Token*)e->event_data.at(0);
    if(tok->last_device == NO_OF_RING_NODES)
        tok->last_device = 0;
    else
        tok->last_device++;

    Simulator::Schedule(Simulator::Now()+1, &RingNode::process_event, (RingNode*)e->dst, e);

}

int main(int argc, char* argv[])
{
    unsigned long int sim_cycles;
    if( argc == 2)
        sim_cycles =  atoi(argv[1]);
    else
        sim_cycles = 100;

    /*  Create simulator components */
    RingComponent* ring = new RingComponent();
    RingNode* rings[NO_OF_RING_NODES];
    Token * tok = new Token();
    ring->the_token = tok;

    for( unsigned int i=0; i<NO_OF_RING_NODES; i++)
    {
        rings[i] = new RingNode();
        rings[i]->name_id = i;
        rings[i]->component_ptr = (void*) ring;
    }


    /*  Connections for the ring */
    for( unsigned int i=0; i<NO_OF_RING_NODES; i++)
    {
        if(i == (NO_OF_RING_NODES-1))
            rings[i]->next = rings[0];
        else
            rings[i]->next = rings[i+1];
    }

#if DEBUG
    cout << "Ring: " << ring << endl;
    cout <<"\t Node_ptr Next_node Node_id"  << endl;
    for( unsigned int i=0; i<NO_OF_RING_NODES; i++)
        cout	<< "Test: "<< rings[i] << " " << rings[i]->next << " " << rings[i]->name_id << endl;
#endif

    IrisEvent* e =  new IrisEvent();
    e->src = (Component*)ring;
    e->dst = (Component*)rings[0];
    e->time = Simulator::Now()+1;
    e->event_data.push_back((void*)ring->the_token);
    ring->have_token = false;

    Simulator::Schedule(Simulator::Now()+1, &RingNode::process_event, rings[0], e);

    /* Schedule the event here */
    Simulator::StopAt(sim_cycles);
    Simulator::Run();

    /* Print some stats */
    cout << "Ring simulation for " << sim_cycles << "cycles. " << endl;
    cout	<< "No of idle cycles: " << ring->idle_counter << endl;
    cout	<< "No of busy cycles: " << ring->busy_counter << endl;
    cout	<< "Data sent order: " << endl;
    for( unsigned int i=0; i<tok->captured_order.size(); i++)
        cout << tok->captured_order.at(i) << " ";
    cout<<endl;
    
    delete tok;
    delete e;
    delete ring;
    for( unsigned int i=0; i<NO_OF_RING_NODES; i++)
        delete rings[i];

}

#endif   /* ----- #ifndef ringsim_cc_INC  ----- */
