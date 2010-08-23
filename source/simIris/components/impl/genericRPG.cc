
#ifndef  _genericrpg_cc_INC
#define  _genericrpg_cc_INC
#define MAX_SIM_TIME 2000000000
#include "genericRPG.h"

GenericRPG::GenericRPG ()
{
    name = "GenericRPG";
    sending = false;
    /* Do this in setup with the address or node_ip appended */
//    out_filename = "traceOut.tr";
    last_vc = 0;
    interface_connections.resize(1);
    seed = 10;
}		/* -----  end of function GenericRPG::GenericRPG  ----- */

GenericRPG::~GenericRPG ()
{
}		/* -----  end of function GenericRPG::~GenericRPG  ----- */

void
GenericRPG::init_generator ()
{
    generator.seed(seed); 

    generator.addressRange( 0, MAX_ADDRESS + 1 );
    generator.delayRange( MIN_DELAY, MAX_DELAY );
    generator.lengthRange( MIN_LENGTH, MAX_LENGTH);

    generator.addressHotSpotRange( 0, MAX_ADDRESS + 1, HOT_SPOTS);
    generator.delayHotSpotRange( MIN_DELAY, MAX_DELAY, HOT_SPOTS );
    generator.lengthHotSpotRange( MIN_LENGTH, MAX_LENGTH, HOT_SPOTS );
	
    generator.addressDistribution( destination_type );
    generator.lengthDistribution( length_type );
    generator.delayDistribution( delay_type );
    return ;
}		/* -----  end of function GenericRPG::init_generator  ----- */

void
GenericRPG::set_no_vcs( uint v )
{
    vcs = v;
}

void
GenericRPG::setup (uint n, uint v, uint time)
{
    vcs = v;
    max_sim_time = time;
    /* Need to init these variables. Using default for now */
    /*  lambda, out_filename, distribution,  */
    if( run_destination_type.compare("poisson")==0 )
    {
        destination_type = libRandom::randomNumberGenerator::poisson;
	length_type = libRandom::randomNumberGenerator::poisson;
	delay_type = libRandom::randomNumberGenerator::poisson;
    }
    else if( run_destination_type.compare("gaussian") == 0)
    {
        destination_type = libRandom::randomNumberGenerator::gaussian;
	length_type = libRandom::randomNumberGenerator::gaussian;
	delay_type = libRandom::randomNumberGenerator::gaussian;
    }
    else if( run_destination_type.compare("hotSpot") == 0 )
    {
        destination_type = libRandom::randomNumberGenerator::hotSpot;
	length_type = libRandom::randomNumberGenerator::hotSpot;
	delay_type = libRandom::randomNumberGenerator::hotSpot;
    }
    else
    {
        destination_type = libRandom::randomNumberGenerator::uniform;
	length_type = libRandom::randomNumberGenerator::uniform;
	delay_type = libRandom::randomNumberGenerator::uniform;
    }
		
    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    init_generator();
//    setup();
    packets = 0;
    address = myId();
    only_sink = false;
    
    // send ready events for each virtual channel
    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        VirtualChannelDescription* vc = new VirtualChannelDescription();
        event->type = READY_EVENT;
        event->vc = i;
        vc->vc = i;
        event->event_data.push_back(vc);
//        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }

    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = true;
	
    // open the output trace file
    /*
    stringstream str;
    str << "rpg_" << node_ip << "_trace_out.tr";
    out_filename = str.str();
    out_file.open(out_filename.c_str());
    if( !out_file.is_open() )
    {
        stringstream stream;
        stream << "Could not open output trace file " << out_filename << ".\n";
        timed_cout(stream.str());
    }	
     */
    return ;
}		/* -----  end of function GenericRPG::setup  ----- */

void
GenericRPG::finish ()
{
//    out_file.close();
    return ;
}		/* -----  end of function GenericRPG::finish  ----- */

void
GenericRPG::process_event (IrisEvent* e)
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
            cout << "\nRPG: " << address << "process_event: UNK event type" << endl;
            break;
    }
    return ;
}		/* -----  end of function GenericRPG::process_event  ----- */

void
GenericRPG::handle_new_packet_event ( IrisEvent* e)
{
    // get the packet data
    HighLevelPacket* hlp = static_cast< HighLevelPacket* >( e->event_data.at(0));
    _DBG( "-------------- GOT NEW PACKET ---------------\n pkt_latency: %d", (int)( Simulator::Now() - hlp->sent_time));

    // write out the packet data to the output trace file
    /*
    if( !out_file.is_open() )	
        out_file.open( out_filename.c_str(), std::ios_base::app );		

    if( !out_file.is_open() )
    {
        cout << "Could not open output trace file " << out_filename << ".\n";
    }
        
        out_file << hlp->toString();
        out_file << "\tPkt latency: " << Simulator::Now() - hlp->sent_time << endl;
     */

//        delete(hlp);
    // send back a ready event
    IrisEvent* event = new IrisEvent();
    event->type = READY_EVENT;
    VirtualChannelDescription* vc = new VirtualChannelDescription();
    vc->vc = hlp->virtual_channel;
    event->event_data.push_back(vc);
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);

//    delete e;
    return ;
}		/* -----  end of function GenericRPG::handle_new_packet_event  ----- */

void
GenericRPG::handle_out_pull_event ( IrisEvent* e )
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
		
    if( found && !only_sink)
    {
        packets++;
        unsigned int current_packet_time = 0;
        HighLevelPacket* hlp = new HighLevelPacket();
        hlp->virtual_channel = last_vc;
        last_vc++;
        hlp->source = address;
        /* 
        hlp->destination = MIN( generator.address(), MAX_ADDRESS);
        if( hlp->destination == node_ip )
            hlp->destination = (hlp->destination + 1) % (MAX_ADDRESS + 1);
         * */
        hlp->destination = 0;
        if( node_ip == 0 )
            hlp->destination = 1;

        hlp->data_payload_length= 1*max_phy_link_bits; //generator.length();
//        hlp->length = MIN( hlp->length, MAX_LENGTH);
//        hlp->length = MAX( hlp->length, MIN_LENGTH);
        hlp->sent_time = Simulator::Now() ; //MAX(generator.delay() , 1);
        for ( uint i=0 ; i < hlp->data_payload_length ; i++ )
            hlp->data.push_back(true);

        /* Need to generate the transaction id */
        hlp->transaction_id = generator.length();
        seed++;

        stringstream str;
        str << "GenericRPG " << address << " Sending packet : " << hlp->toString();
        str << "\n No of packets: " << packets;
        timed_cout(str.str());

        ready[ hlp->virtual_channel ] = false;
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->event_data.push_back(hlp);
        current_packet_time = hlp->sent_time; /* Making a copy so we dont depend on hlp deletionMaking a copy so we dont depend on hlp deletion. May be needed later if the functions are broken. */
        Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );
                
        /*  Check for an empty vc and call yourself as long as it is not the
         *  MAX_SIM_TIME or last packet. If you were to do Run() till all
         *  events in the system are complete. This will probably determine
         *  the end of the simulation */
        found = false;
        for( unsigned int i = 0; i < ready.size(); i++ )
            if( ready[i] && current_packet_time < MAX_SIM_TIME ) 
            {
                found = true;
                break;
            }
    
            if( found )
            {
                IrisEvent* event = new IrisEvent();
                event->type = OUT_PULL_EVENT;
                event->vc = e->vc;
                Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);
                sending = true;
            }
            else
                sending = false;
            
        }
        else
            sending = false;

//    delete e;
    return ;
}		/* -----  end of function GenericRPG::handle_out_pull_event  ----- */

void
GenericRPG::handle_ready_event ( IrisEvent* e)
{
#ifdef _DEBUG
    _DBG_NOARG("handle_ready_event");
#endif

    // send the next packet if it is less than the current time
    VirtualChannelDescription* vc = static_cast<VirtualChannelDescription*>(e->event_data.at(0));
    ready[vc->vc] = true;

    if( !sending && Simulator::Now() < MAX_SIM_TIME )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = e->vc;
        /* Need to be careful with the same cycles scheduling. Can result in
         * events of the past. Need to test this. */
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event); 
        sending = true;
    }

//    delete e;
    return ;
}		/* -----  end of function GenericRPG::handle_ready_event  ----- */

void
GenericRPG::init ()
{
		
    return ;
}		/* -----  end of function GenericRPG::init  ----- */

string
GenericRPG::toString () const
{
    stringstream str;
    str << "GenericRPG";
    return str.str();
}		/* -----  end of function GenericRPG::toString  ----- */

#endif   /* ----- #ifndef _genericrpg_cc_INC  ----- */

