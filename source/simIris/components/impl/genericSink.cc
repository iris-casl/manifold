/*
 * =====================================================================================
 *! \brief
 *       Filename:  genericSink.cc
 *
 *    Description: This class models a sink that drains out network packets
 *    and sends a response after a fixed 60 cycle latency. ( 60 from the flat
 *    mc model )
 *
 *        Version:  1.0
 *        Created:  02/21/2010 08:48:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericsink_cc_INC
#define  _genericsink_cc_INC

#include	"genericSink.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  GenericSink
 *      Method:  GenericSink
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
GenericSink::GenericSink ()
{
    out_filename = "sink_trace_.tr";
    name = "GenericSink";
}  /* -----  end of method GenericSink::GenericSink  (constructor)  ----- */

void
GenericSink::setup (uint v, uint time, uint n)
{
    address = myId();
    vcs =v;
    max_sim_time = time;
    no_nodes = n;
    ready.resize(vcs);
//    interface_connections.resize(1);

    stringstream str;
    str << out_filename << address ;
    out_file.open(str.str().c_str());

    /* Send ready events for each virtual channel */
    for ( uint i = 0; i < vcs; i++ )
    {
        ready[i] = false;
        IrisEvent* e = new IrisEvent();
        VirtualChannelDescription* vc = new VirtualChannelDescription();
        vc->vc = i;
        e->event_data.push_back(vc);
        e->type = READY_EVENT;
        Simulator::Schedule(Simulator::Now()+1, this, interface_connections[0], e);
        
        cout	<< "GenericSink:: Sending ready event to interface" << endl;
    }
        return;
}		/* -----  end of function GenericSink  ----- */

void
GenericSink::process_event (IrisEvent* e)
{
    switch(e->type)
    {
        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;
        case OUT_PULL_EVENT:
            handle_outpull_event(e);
            break;
        case READY_EVENT:
            handle_ready_event(e);
            break;
        default:
            cout << "Unk event type" <<endl;
            break;
    }

    return ;
}		/* -----  end of function GenericSink::process_event  ----- */

void
GenericSink::handle_new_packet_event ( IrisEvent* e )
{
    HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
    cout << " GenericSink:: ********** GOT NEW PACKET ************" << endl;
    cout << hlp->toString()<< " Latency: " << Simulator::Now() - hlp->sent_time << endl;
    out_file << hlp->toString()<< " Latency: " << Simulator::Now() - hlp->sent_time << endl;
    /* Event handled now delete it */

    //send back a ready event
    IrisEvent* event = new IrisEvent();
    event->type = READY_EVENT;
    VirtualChannelDescription* vc = new VirtualChannelDescription();
    vc->vc = hlp->virtual_channel;
    event->event_data.push_back(vc);
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);

    delete hlp;
    delete e;

    return ;
}		/* -----  end of function GenericSink::handle_new_packet_event  ----- */

void
GenericSink::handle_outpull_event ( IrisEvent* e )
{
    bool found = false;
    for( unsigned int i = last_vc ; i < ready.size(); i++ )
        if( ready[i] )
        {
            found = true;
            last_vc= i;
            break;
        }
	
    if(!found )
        for( unsigned int i = 0; i < last_vc ; i++ )
            if( ready[i] )
            {
                found = true;
                last_vc = i;
                break;
            }
		
    MTRand mtrand1;
    if( found )
    {
        packets++;
        unsigned int current_packet_time = 0;
        HighLevelPacket* hlp = new HighLevelPacket();
        hlp->virtual_channel = last_vc;
        last_vc++;
        hlp->source = address;
        hlp->destination = mtrand1.randInt(no_nodes); 
        hlp->transaction_id = mtrand1.randInt(1000);
        if( hlp->destination == node_ip )
            hlp->destination = (hlp->destination + 1) % (no_nodes + 1);
/* 
        if ( node_ip == 0)
            hlp->destination = 1;
        else
            hlp->destination = 0;
 * */

        hlp->sent_time = Simulator::Now()+60 ; //MAX(generator.delay() , 1);

        _DBG( " Sending pkt: no of packets %d ", packets );

        out_file << "HLP: " << hlp->toString() << endl;

        ready[ hlp->virtual_channel ] = false;
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->event_data.push_back(hlp);
        current_packet_time = hlp->sent_time; 
        Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );
                
    }
    else
        sending = false;

    delete e;
    return ;
}		/* -----  end of function GenericSink::handle_outpull_event  ----- */

void
GenericSink::handle_ready_event ( IrisEvent* e )
{
    VirtualChannelDescription* vc = static_cast<VirtualChannelDescription*>(e->event_data.at(0));
    ready[vc->vc] = true;

    if( !sending  )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = e->vc;
        /* Need to be careful with the same cycles scheduling. Can result in
         * events of the past. Need to test this. */
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event); 
        sending = true;
    }

    delete e;
    cout << " GenericSink:: got ready event at time " << Simulator::Now() << endl;
    return ;
}		/* -----  end of function GenericSink::handle_ready_event  ----- */

string
GenericSink::toString () const
{
    stringstream str;
    str << "GenericSink" 
        << "\t addr: " << address
        << "\toutput file = " << out_filename;
    return str.str();
}		/* -----  end of function GenericSink::toString  ----- */

#endif   /* ----- #ifndef _genericsink_cc_INC  ----- */

