/*
 * =====================================================================================
 *
 *       Filename:  packetSource.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/2010 05:12:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _packetsource_cc_INC
#define  _packetsource_cc_INC

#include	"MersenneTwister.h"
#include	"packetSource.h"
//#define SEND_ONE_PACKET 1
#define MAX_NO_PACKETS 2
#define DEFAULT_VIRTUAL_CHANNELS 2
using namespace std;

RandomPacketGenerator::RandomPacketGenerator()
{
    name = "RandomPacketGenerator";
    interface_connections.resize(1);
}

RandomPacketGenerator::~RandomPacketGenerator()
{
}

void
RandomPacketGenerator::init()
{
    ready.resize( DEFAULT_VIRTUAL_CHANNELS );
    ready.insert( ready.begin(), ready.size(), false );
    setup();
    return;
}
void 
RandomPacketGenerator::setup()
{
    address = myId();
    no_of_packets = 0;
    out_filename = "rpg_trace_out.tr";
    in_filename = "rpg_trace_in.tr";
    out_file.open(out_filename.c_str());
    in_file.open(in_filename.c_str());
    last_vc = 0;
    ready.resize(DEFAULT_VIRTUAL_CHANNELS);

    /* These are the values for the init of the distribution */
    seed = 12345;
    max_address = 1024;
    min_delay = 5;
    max_delay = 100;
    min_length = 3;
    max_length = 10;
    hotspots = 7; 
    max_time = DEFAULT_RAN_MAX_TIME;

    stringstream str;
    str << " Running setup on RandomPacketGenerator " << Simulator::Now() << endl;
    timed_cout( str.str());

    /* Init the generator here */

    for ( uint i=0 ; i<ready.size() ; i++ )
    {
        IrisEvent* event = new IrisEvent();
        VirtualChannelDescription* vc = new VirtualChannelDescription();
        vc->vc = i;
        event->event_data.push_back(vc);
        event->type = READY_EVENT;
        Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
    }

    for ( uint i=0 ; i<ready.size() ; i++ )
        ready[i] = false;

}

void 
RandomPacketGenerator::finish()
{
    out_file.close();
}

void 
RandomPacketGenerator::process_event(IrisEvent* e)
{
    switch(e->type)
    {
        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;

        case OUT_PULL_EVENT:
            handle_out_pull_event(e);
            break;

        case READY_EVENT:
            handle_ready_event(e);
            break;

        default:
            cout << "RandomPacketGenerator:: Unk event exception" << endl;
            break;
    }
}

void 
RandomPacketGenerator::handle_out_pull_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "RandomPacketGenerator " << address << " " << node_ip << " handle_out_pull_event " << e->vc ;
    timed_cout( str.str());
#endif

    if(no_of_packets < MAX_NO_PACKETS)
    {
    uint no_vcs = ready.size();
    bool found = false;

    
    //  Make sure you start from last_vc 
    for ( uint i=last_vc ; i<last_vc+1 ; i++ )
        if ( ready[i] )
        {
            found = true;
            last_vc = i;
            break;
        }

    if ( !found )
        for ( uint i=0 ; i<last_vc+1 ; i++ )
            if ( ready[i] )
            {
                found = true;
                last_vc = i;
                break;
            }
    //  End of finding vc 

    MTRand mtrand1;
    if ( found )
    {
        /*  Send a high level packet now */
        IrisEvent* event = new IrisEvent();
        HighLevelPacket* hlp = new HighLevelPacket();
        event->type = NEW_PACKET_EVENT;
        hlp->virtual_channel = last_vc;
        hlp->source = myId();

        /* Random packet generator should change this to match the
         * distribution */
        if( node_ip == 1 || node_ip == 2 || node_ip == 3 ) 
            hlp->destination = 0;
        else
            hlp->destination = 1;
        hlp->transaction_id = mtrand1.randInt(100);
        hlp->data_payload_length = max_phy_link_bits;
        hlp->sent_time = Simulator::Now();
        for ( uint i=0 ; i < hlp->data_payload_length ; i++ )
            if(address == 5 )
                hlp->data.push_back(true);
            else
                hlp->data.push_back(false);

        in_file << hlp->toString() << endl;
        event->event_data.push_back(hlp);
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,interface_connections[0], event);

        stringstream str;
        str << "RandomPacketGenerator " << address << " " << node_ip << " Generated HLP: Time: " 
//            << hlp->toString() 
            << Simulator::Now() << " no_of_packets: " << ++no_of_packets << endl;
        timed_cout( str.str() );

        seed++;
        ready[hlp->virtual_channel] = false;
        found = false;

        for ( uint i=0 ; i<no_vcs ; i++ )
            if ( ready[i] ) //&& Simulator::Now() < max_time)
            {
                found = true;
                break;
            }
        if ( found )
        {
            IrisEvent* event = new IrisEvent();
            event->type = OUT_PULL_EVENT;
            Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);
            sending = true;
        }
        else
            sending = false;
    }
    else
        sending = false;
    }
    delete e;
}

void 
RandomPacketGenerator::handle_ready_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "RandomPacketGenerator " << address << " " << node_ip << " handle_ready_event " << e->vc ;
    timed_cout( str.str());
#endif

    if( no_of_packets < MAX_NO_PACKETS)
    {
    /* Send the next packet if it is time */
    VirtualChannelDescription* vc = static_cast<VirtualChannelDescription*>(e->event_data.at(0));
    ready[vc->vc] = true;

    if( !sending && Simulator::Now() < max_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        sending = true;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }
    }
    delete e;
}

void 
RandomPacketGenerator::handle_new_packet_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "RandomPacketGenerator " << address << " " << node_ip << " handle_new_packet_event " << e->vc ;
    timed_cout( str.str());
#endif

    HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
    cout << "\n\nRPG*********** GOT NEW PACKET *******************" << endl;
    cout << " Recv time: " << Simulator::Now() 
        << " pkt_latency: " << Simulator::Now() - hlp->sent_time << endl;
    out_file << hlp->toString() << " Recv time: " << Simulator::Now() 
        << " pkt_latency: " << Simulator::Now() - hlp->sent_time << endl;

    /* Send ready event back to NI */
    IrisEvent* event = new IrisEvent();
    event->type = READY_EVENT;
    VirtualChannelDescription* vc = new VirtualChannelDescription();
    vc->vc = hlp->virtual_channel;
    event->event_data.push_back(vc);
    Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
    delete hlp;
    delete e;
}

string 
RandomPacketGenerator::toString() const
{
    stringstream str;
    str << "RandomPacketGenerator"
        << "\t addr: " << address
        << " node_ip: " << node_ip
        << "\tInput File= " << in_filename
        << "\tOutput File= " << out_filename;
    return str.str();
}

#endif   /* ----- #ifndef _packetsource_cc_INC  ----- */
