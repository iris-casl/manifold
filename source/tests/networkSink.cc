/*
 * =====================================================================================
 *
 *       Filename:  networkSource.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 10:34:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _networksource_cc_INC
#define  _networksource_cc_INC
#define FLIT_ID 9800

#include	"networkSource.h"

unsigned long int
GenericNetworkSource::max_time ()
{
    unsigned long int time = 0;
//    MTRand mtrand1;
    if(packets.size() > 0)
        time = Simulator::Now() + 5; //mtrand1.randInt(100);

    return time;
}		/* -----  end of function NetworkSource::max_time  ----- */

string
GenericNetworkSource::toString () const
{
    stringstream stream;
    stream << "Network Source" << endl;
    stream << "\tpackets = " << packets.size() << endl;

    return stream.str();
}		/* -----  end of function NetworkSource::toString  ----- */

GenericNetworkSource::GenericNetworkSource()
{
}		/* -----  end of function NetworkSource::NetworkSource  ----- */


void
GenericNetworkSource::setup ()
{
    address = myId();
    in_filename = "trace_in.tr";
    in_file.open(in_filename.c_str());
    out_buffer.buffer_size = 8;
    out_buffer.set_no_virtual_channels(8);
    out_buffer.set_no_credits(8);
    ticking = true;
    sending_vc = -1;
    done = true;
    flits.resize(DEFAULT_VIRTUAL_CHANNELS);

//    generate_random_packets_for_interface();
    IrisEvent* event = new IrisEvent();
    event->type = TICK_EVENT;
    cout << "GenericNetworkSource:: generating TICK_EVENT " << Simulator::Now() << endl;
    Simulator::Schedule(Simulator::Now()+1, &GenericNetworkSource::process_event, this, event );
    cout << "Init on network source done " << Simulator::Now() << endl;

    return;
}		/* -----  end of function NetworkSource::setup  ----- */

void
GenericNetworkSource::process_event ( IrisEvent* e )
{
    switch( e->type)
    {
        case TICK_EVENT:
            handle_tick_event(e);
            break;
        case READY_EVENT:
            handle_ready_event(e);
        case LINK_ARRIVAL_EVENT:
            handle_link_arrival_event(e);
            break;
        default:
            cout << "GenericLink: Unk Event" << endl;
            break;
    }

    return;
}		/* -----  end of function NetworkSource::process_event  ----- */

void
GenericNetworkSource::handle_ready_event (IrisEvent* e)
{
    cout << "GenericNetworkSource:: handle_ready_event " << Simulator::Now() << endl;
    return ;
}		/* -----  end of function NetworkSource::handle_ready_event  ----- */

void
GenericNetworkSource::handle_tick_event(IrisEvent* e)
{
    cout << "GenericNetworkSource:: handle_tick_event " << Simulator::Now() << endl;
        delete e;

}

void
GenericNetworkSource::check_tick()
{
    cout << "GenericNetworkSource:: check_tick " << Simulator::Now() << endl;
    if ( !ticking )
    {

        // check the output buffers 
        if(!out_buffer.is_empty(0) && out_buffer.get_no_credits(0) > 0 )
            ticking = true;

        // maybe schedule a tick next cycle 
        if ( ticking )
        {
            IrisEvent* event = new IrisEvent();
            event->type = TICK_EVENT;
            Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        }
    }
         return;

}

void
GenericNetworkSource::handle_link_arrival_event( IrisEvent* e)
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.back());
    if( data->type == FLIT_ID)
        cout << " Error this is what generates flits it cant get a flit " << endl;
    else if ( data->type == CREDIT_ID)
    {
        cout << "GenericNetworkSource got a credit " << Simulator::Now() << endl;
        out_buffer.got_credit( data->vc);
    }
    else
        cout << "GenericNetworkSource:: Unk event exception in handle_link_arrival_event " << endl;

    ticking = true;
            IrisEvent* event = new IrisEvent();
            event->type = TICK_EVENT;
            Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    delete e;

}
void
GenericNetworkSource::generate_random_packets_for_interface ( uint vc)
{
    
    MTRand mtrand1;
    HighLevelPacket* hlp = new HighLevelPacket();        /*  Some low level packet */
    hlp->source = myId();
    hlp->destination = 2;
    hlp->transaction_id = mtrand1.randInt(100);
    hlp->data_payload_length = max_phy_link_bits;
    hlp->sent_time = Simulator::Now();
    hlp->virtual_channel = vc;

    for ( uint i=0 ; i < hlp->data_payload_length ; i++ )
        hlp->data.push_back(true);

    cout << " GenericNetworkSource: \nGenerated HLP: " << hlp->toString() << Simulator::Now() << endl;
    LowLevelPacket* llp = hlp->to_low_level_packet();
    
    cout << " GenericNetworkSource: \nConverted HLP to LLP in network Souce: " << llp->toString() << " at time " << Simulator::Now() << endl;
    for ( uint i=0 ; i<llp->head_flits.size() ; i++ )
        flits[vc].push_back(llp->head_flits[i]);

    for ( uint i=0 ; i<llp->body_flits.size() ; i++ )
        flits[vc].push_back(llp->body_flits[i]);

    for ( uint i=0 ; i<llp->tail_flits.size() ; i++ )
        flits[vc].push_back(llp->tail_flits[i]);

//    sending_vc = hlp->virtual_channel;
    in_file << hlp->toString() << endl;
    delete hlp;
//    done = false;
    return;
}		/* -----  end of function NetworkSource::generate_random_packets_for_interface  ----- */

#endif   /* ----- #ifndef _networksource_cc_INC  ----- */
