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
    out_buffer.buffer_size = DEFAULT_OUTPUT_BUFFER_SIZE;
    out_buffer.set_no_virtual_channels(DEFAULT_VIRTUAL_CHANNELS);
    ticking = true;
    sending_vc = 0;
    done = true;
#ifdef SEND_SINGLE_PACKET
    done_sending = false;
#endif
    flits.resize(DEFAULT_VIRTUAL_CHANNELS);

//    generate_random_packets_for_interface();
    IrisEvent* event = new IrisEvent();
    event->type = TICK_EVENT;
    stringstream str;
    str << "GenericNetworkSource:: generating TICK_EVENT " << Simulator::Now() << endl;
    Simulator::Schedule(Simulator::Now()+1, &GenericNetworkSource::process_event, this, event );
    str << "Init on network source done " << Simulator::Now() << endl;
    timed_cout(str.str());

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
            cout << "\nGenericLink: Unk Event" << endl;
            break;
    }

    return;
}		/* -----  end of function NetworkSource::process_event  ----- */

void
GenericNetworkSource::handle_ready_event (IrisEvent* e)
{
    stringstream str;
    str << "GenericNetworkSource:: handle_ready_event " << Simulator::Now() << endl;
    timed_cout(str.str());
    check_tick();
    return ;
}		/* -----  end of function NetworkSource::handle_ready_event  ----- */

void
GenericNetworkSource::handle_tick_event(IrisEvent* e)
{
        if( done && out_buffer.is_empty(sending_vc))
        {
            bool send = true;
            /* 
            for(uint i=0; i<DEFAULT_VIRTUAL_CHANNELS; i++)
                if(!out_buffer.is_empty(i))
                {
                    send = false;
                    break;
                }
             * */
            if(send)
            {
                MTRand mtrand1;
                sending_vc = mtrand1.randInt(DEFAULT_VIRTUAL_CHANNELS-1);
                generate_random_packets_for_interface(sending_vc);
                done = false;
            }
        }

            if(!out_buffer.is_channel_full(sending_vc) && flits[sending_vc].size()>0)
            {
                out_buffer.change_push_channel(sending_vc);
                Flit* temp = flits[sending_vc].front();
                switch(temp->type)
                {
                    case HEAD: 
                        sending_vc = static_cast<HeadFlit*>(temp)->vc;
                        break;
                    case BODY:
                        break;
                    case TAIL:
                        done = true;
                        break;
                }
                out_buffer.push(temp);
                flits[sending_vc].pop_front();
                stringstream str;
                str << "NetSrc:: pushed flit into buffer. Type: " << temp->type
                    << " buffersize: " << out_buffer.get_occupancy(sending_vc);
                timed_cout(str.str());
            }

        /*  check the output buffer */
            if(!out_buffer.is_empty(sending_vc) && out_buffer.get_no_credits(sending_vc) > 0
               && last_sent_cycle < Simulator::Now())        //&& out_arbiter.ready(i))
            {
                last_sent_cycle = Simulator::Now();
                LinkArrivalData* data = new LinkArrivalData();
                IrisEvent* event = new IrisEvent();
                data->type = FLIT_ID;
                data->valid = true;
                data->vc = sending_vc;
                out_buffer.change_pull_channel(sending_vc);
                data->ptr = out_buffer.pull();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(data);
                event->src = this;
                event->vc = sending_vc;
                stringstream str;
                str << "NetSrc::Flit out on "<< sending_vc << " type: " << data->ptr->type;
                if( data->ptr->type == HEAD )
                    str << static_cast<HeadFlit*>(data->ptr)->toString();
                timed_cout(str.str());
                Simulator::Schedule( Simulator::Now()+1, 
                                     &NetworkComponent::process_event, output_connection, event);
            }

        /* tick again next cycle */
        check_tick();
        delete e;

}

void
GenericNetworkSource::init()
{
    return;
}

void
GenericNetworkSource::check_tick()
{
/* Keep ticking till the end for now */
            IrisEvent* event = new IrisEvent();
            event->type = TICK_EVENT;
            Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
         return;

}

void
GenericNetworkSource::handle_link_arrival_event( IrisEvent* e)
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.back());
    if( data->type == FLIT_ID)
    {
        stringstream str;
        str << "NetSrc got a flit. Is this also a sink? " << endl;
        timed_cout(str.str());
        /* send a credit back */
                LinkArrivalData* cr = new LinkArrivalData();
                cr->type = CREDIT_ID;
                cr->valid = false;
                cr->vc = data->vc;

                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(cr);
                event->src = this;
                event->vc = data->vc;

                Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, input_connection, event);
    }
    else if ( data->type == CREDIT_ID)
    {
        out_buffer.got_credit( data->vc);
        stringstream str;
        str << "NetSrc got a credit " << out_buffer.get_no_credits(data->vc);
        timed_cout(str.str());
    }
    else
        cout << "\nGenericNetworkSource:: Unk event exception in handle_link_arrival_event " << endl;

    check_tick();
    delete e;

}
void
GenericNetworkSource::generate_random_packets_for_interface ( uint vc)
{
    
    MTRand mtrand1;
    HighLevelPacket* hlp = new HighLevelPacket();        /*  Some low level packet */
    hlp->source = myId();
    hlp->destination = 0;
    hlp->transaction_id = mtrand1.randInt(100);
    hlp->data_payload_length = max_phy_link_bits;
    hlp->sent_time = Simulator::Now()+1;
    hlp->virtual_channel = vc;

    for ( uint i=0 ; i < hlp->data_payload_length ; i++ )
        hlp->data.push_back(true);

    stringstream str;
    str << "NetSrc:Generated HLP: vc: " << hlp->virtual_channel ;
    LowLevelPacket* llp = hlp->to_low_level_packet();
    
//    str << " Converted HLP to LLP in network Souce: " << llp->toString() << " at time " << Simulator::Now() << endl;
    timed_cout(str.str());

    for ( uint i=0 ; i<llp->head_flits.size() ; i++ )
        flits[vc].push_back(llp->head_flits[i]);

    for ( uint i=0 ; i<llp->body_flits.size() ; i++ )
        flits[vc].push_back(llp->body_flits[i]);

    for ( uint i=0 ; i<llp->tail_flits.size() ; i++ )
        flits[vc].push_back(llp->tail_flits[i]);

    in_file << hlp->toString() << endl;
    delete hlp;

    return;
}		/* -----  end of function NetworkSource::generate_random_packets_for_interface  ----- */

#endif   /* ----- #ifndef _networksource_cc_INC  ----- */
