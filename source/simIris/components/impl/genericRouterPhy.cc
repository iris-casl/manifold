/*
 * =====================================================================================
 *
 *       Filename:  genericRouterPhy.cc
 *
 *       Description: Implementing a generic input buffered physical router model
 *
 *        Version:  1.0
 *        Created:  03/11/2010 09:20:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericRouterPhy_cc_INC
#define  _genericRouterPhy_cc_INC

#include	"genericRouterPhy.h"
using namespace std;

GenericRouterPhy::GenericRouterPhy ()
{
    name = "Router" ;
    ticking = false;
}  /* -----  end of method GenericRouterPhy::GenericRouterPhy  (constructor)  ----- */

GenericRouterPhy::~GenericRouterPhy()
{
}

void
GenericRouterPhy::init (uint p, uint v, uint cr, uint bs)
{
    ports =p;
    vcs =v;
    credits =cr;
    buffer_size = bs;

    address = myId();

    /*  set_input_ports(ports); */
    in_buffers.resize(ports);
    decoders.resize(ports);
    input_buffer_state.resize(ports*vcs);
    swa.resize(ports);
    xbar.setup(ports,vcs);
    downstream_credits.resize(ports);

    /*Resize per port stats */
    stat_packet_out.resize(ports);
    stat_flit_out.resize(ports);

    /* All decoders and vc arbiters need to know the node_ip for routing */
    for(uint i=0; i<ports; i++)
    {
        decoders[i].node_ip = node_ip;  // decoders need to know what node this router is located at
        decoders[i].address = address;  // this is for debugging
        stat_packet_out[i].resize(ports); // stat variables
        stat_flit_out[i].resize(ports);
    }
    swa.node_ip = node_ip;
    swa.address = address;

    /*  set_no_virtual_channels(vcs); */
    /* This picks the number of virtual channel mentioned in the config file and
     * configures the credits, buffers and decoders */
    for(uint i=0; i<ports; i++)
    {
        downstream_credits[i].resize(vcs);
        in_buffers[i].resize( vcs, buffer_size );
        decoders[i].resize( vcs );
    }

    /* Since this is the init phase you can safetly set the
     * downstream_credits to the number of credits. You know there are
     * no packets injected yet so all buffers are empty. Also the
     * input_buffer_state is set to empty.*/
    for(uint i=0; i<ports; i++)
        for(uint j=0; j<vcs; j++)
        {
            downstream_credits[i][j] = credits;
            input_buffer_state[i*vcs+j].pipe_stage = EMPTY;
        }

    /* init the countes */
    packets = 0;
    flits = 0;
    total_packet_latency = 0;

    return ;
}		/* -----  end of function GenericRouterPhy::init  ----- */

/*! \brief These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
GenericRouterPhy::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
GenericRouterPhy::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
GenericRouterPhy::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
GenericRouterPhy::process_event ( IrisEvent* e )
{
    switch(e->type)
    {
        case LINK_ARRIVAL_EVENT:
          /* This is the event that comes into the router when a flit
           * is delivered to it. It is an external event in the sense
           * it gets handed to it from the GenericLink component */
            handle_link_arrival_event(e);
            break;
        case TICK_EVENT:
            /* This is an event that updates router state based on
             * packets in the router. ( Basically walk the pipe stage
             * in backward order */
            handle_tick_event(e);
            break;
        default:
            /* There should be no other events that are delivered to
             * the component. If this happens then you either set the
             * wrong pointer when sending the event. Or check your
             * topology */
            _DBG("GenericRouterPhy:: Unk event exception %d", e->type);
            break;
    }
    return ;
}		/* -----  end of function GenericRouterPhy::process_event  ----- */

double GenericRouterPhy::get_average_packet_latency()
{
        return ((total_packet_latency+0.0)/packets);
}
        
double GenericRouterPhy::get_last_flit_out_cycle()
{
        return last_flit_out_cycle;
}

double GenericRouterPhy::get_flits_per_packet()
{
        return (flits+0.0/packets) ;
}

double GenericRouterPhy::get_buffer_occupancy()
{
        return 0.0;
}

double GenericRouterPhy::get_swa_fail_msg_ratio()
{
        return 0.0;
}

double GenericRouterPhy::get_swa_load()
{
        return 0.0;
}

double GenericRouterPhy::get_vca_fail_msg_ratio()
{
        return 0.0;
}

double GenericRouterPhy::get_vca_load()
{
        return 0.0;
}

double GenericRouterPhy::get_stat_packets()
{
        return packets;
}

double GenericRouterPhy::get_stat_flits()
{
        return flits;
}

double GenericRouterPhy::get_stat_ib_cycles()
{
        return 0.0;
}

double GenericRouterPhy::get_stat_rc_cycles()
{
        return 0.0;
}

double GenericRouterPhy::get_stat_vca_cycles()
{
        return 0.0;
}

double GenericRouterPhy::get_stat_swa_cycles()
{
        return 0.0;
}

double GenericRouterPhy::get_stat_st_cycles()
{
        return 0.0;
}

/*!\brief Prints out all the accumulated stats at the end of the
 * simulation. Routers have a print_stats component that gets called
 * on all routers at the end of the simulation. For information on
 * where this is called look at either the main front end file at
 * manifold_simiris.cc or the corresponding topology file that you are
 * running */
string
GenericRouterPhy::print_stats()
{
    stringstream str;
    str << "\n router[" << node_ip << "] packet latency: " << total_packet_latency
        << "\n router[" << node_ip << "] flits/packets: " << (flits+0.0)/(packets)
        << "\n router[" << node_ip << "] average packet latency: " << (total_packet_latency+0.0)/packets
        << "\n router[" << node_ip << "] last_flit_out_cycle: " << last_flit_out_cycle
        << " ";
    str << "\n router[" << node_ip << "] packets: " << packets
        << "\n router[" << node_ip << "] flits: " << flits;
    if( stat_print_level > 2 )
    {
        for( uint i=0; i<ports; i++)
            for ( uint j=0; j<ports; j++)
            {
                string in_port = "Inv";
                switch( i )
                {
                    case 0 : 
                        in_port = "Inj";
                        break;
                    case 1:
                        in_port = 'E';
                        break;
                    case 2:
                        in_port = 'W';
                        break;
                    case 3:
                        in_port = 'S';
                        break;
                    case 4:
                        in_port = 'N';
                        break;
                    default:
                        in_port = "Invalid";
                        break;
                }

                string out_port = "Inv";
                switch( j )
                {
                    case 0 : 
                        out_port = "Ejection";
                        break;
                    case 1:
                        out_port = 'W';
                        break;
                    case 2:
                        out_port = 'E';
                        break;
                    case 3:
                        out_port = 'N';
                        break;
                    case 4:
                        out_port = 'S';
                        break;
                    default:
                        out_port = "Invalid";
                        break;
                }
                if ( i!=j)
                {
                    str << "\n    router[" << node_ip << "] Packets out " << in_port 
                        << " going " << out_port << " : " << stat_packet_out[i][j];
                    str << "\n    router[" << node_ip << "] Flits out " << in_port 
                        << " going " << out_port << " : " << stat_flit_out[i][j];
                }

            }
    }


    return str.str();
}

/*! \brief Event handle for the LINK_ARRIVAL_EVENT event. Entry from DES kernel */
void
GenericRouterPhy::handle_link_arrival_event ( IrisEvent* e )
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
    /* this data pointer has a type field which indicates if it is a
     * flit or credit. If you have other types that can be passed on
     * the link add it as a type in
     * source/util/simIrisComponentHeader.h.
     * When the configuration is as follows
     *  ROUTER --------> ROUTER 
     *          Link
     *  The previous router would have set the type to FLIT_ID in the
     *  switch traversal stage in this example or watever is the last
     *  stage before the flit leaves the packet. When coming from an
     *  interface. The interface sets it.
     */
    if(data->type == FLIT_ID)
    {
        /*  Update stats */
        flits++;
        if( data->ptr->type == TAIL || data->ptr->is_single_flit_pkt )
            packets++;

        /*Find the port the flit came in on */
        bool found = false;
        uint port = -1;
        /*  Need to check for null here as there may be null links to corner routers */
        for ( uint i=0 ; i< ports ; i++ )
            if(static_cast<GenericLink*>(input_connections[i])->input_connection) 
                if( e->src_id == static_cast<GenericLink*>(input_connections[i])->input_connection->address)
                {
                    found = true;
                    port = i;
                    break;
                }

        /* Throw and exception if it was not found */
        if( !found )
        {
            _DBG(" Input port not found src_addr: %d", e->src_id);
        }

        /* Push the flit into the buffer */
        in_buffers[port].change_push_channel(data->vc);
        in_buffers[port].push(data->ptr);
        decoders[port].push(data->ptr,data->vc);
        /* If this flit type is HEAD init the input_buffer_state as a
         * new message is come in. input_buffer_state is a good
         * structure to use for debugging the router as you can update
         * it to hold state from different pipeline stages in the
         * router.
         */
        if( data->ptr->type == HEAD )
        {
            HeadFlit* hf = static_cast<HeadFlit*>(data->ptr);
            input_buffer_state[port*vcs+data->vc].input_port = port;
            input_buffer_state[port*vcs+data->vc].input_channel = data->vc;
            input_buffer_state[port*vcs+data->vc].pipe_stage = FULL;
            /* The possible output ports vector is used by the RC
             * stage because RC does no determine the output port in
             * all cases. RC returns a set of output ports that the
             * packet can take and the arbiters decide which ones to
             * request for. Eg: Say you have north-last routing and
             * the destination is west. You can have more than one
             * output ports that will take you to the destination.
             * Hence RC will return all possible output ports and you
             * may at the point where you request switch allocation
             * based on buffer occupancy or some other metric request
             * only a few. However is strict DOR (xy) routing there is
             * only one output port that the packet can take.
             * Similarly for vcs. In cases where packets are allowed
             * only some virtual channels to take. RC will fill out
             * the possible_ovcs with the correct set of vcs. 
             * With stric DOR routing possible_vcs should have vc0 to vc(n-1)
             * where n is the number of vcs in the system. This is to
             * indicate that the vcs are just use to improve system
             * throughput/ better link utilization
             */
            input_buffer_state[port*vcs+data->vc].possible_oports.clear(); 
            input_buffer_state[port*vcs+data->vc].possible_ovcs.clear(); 
            input_buffer_state[port*vcs+data->vc].possible_oports.resize(1); 
            input_buffer_state[port*vcs+data->vc].possible_ovcs.resize(1); 
            input_buffer_state[port*vcs+data->vc].possible_oports[0] = decoders[port].get_output_port(data->vc);
            input_buffer_state[port*vcs+data->vc].possible_ovcs[0] = 0;
            input_buffer_state[port*vcs+data->vc].length= hf->length;
            input_buffer_state[port*vcs+data->vc].credits_sent= hf->length;
            input_buffer_state[port*vcs+data->vc].arrival_time= ceil(Simulator::Now());
            input_buffer_state[port*vcs+data->vc].clear_message= false;
            input_buffer_state[port*vcs+data->vc].flits_in_ib = 0;

        }
        else
        {
            istat->stat_router[node_ip]->ib_cycles++;
#ifdef _DEBUG_ROUTER
            _DBG(" BW inport:%d invc:%d oport:%d ovc:%d ft:%d", port, data->vc,
                 input_buffer_state[port*vcs+data->vc].output_port, input_buffer_state[port*vcs+data->vc].output_channel, data->ptr->type);
#endif

        }

    }
    /* If a credit comes in update the downstream_credit vector. This
     * means a buffer was emptied on the router you sent a flit to. 
     * Notice credit signal get propagated backwards */
    else if ( data->type == CREDIT_ID)
    {
        /* Find the corresponding output port */
        bool found = false;
        uint port = -1;
        for ( uint i=0 ; ports ; i++ )
            if(static_cast<GenericLink*>(output_connections[i])->output_connection) /* Some links of corner routers may be null */
                if( static_cast<GenericLink*>(output_connections[i])->output_connection->address == e->src_id)
                {
                    port = i;
                    found = true;
                    break;
                }
        if(!found)
        {
            _DBG(" Output port not found src_addr: %d", e->src_id);
        }

        downstream_credits[port][data->vc]++;
#ifdef _DEBUG_ROUTER
        /* This is a good macro to use if you are debugging it prints
         * out the routers component address node_id and the msg
         * How do component address work??
         * a node is combination of a processor+interface+router so
         * these 3 components share a node_id
         * But since they are different components they also have
         * unique addresses.
         */
        _DBG(" Got a credit port:%d vc:%d in_buffers[port][vc]:%d ", port, data->vc, downstream_credits[port][data->vc]);
#endif

    }
    else
    {
        _DBG( "handle_link_arrival_event Unk data type %d ", data->type);
    }

    /* Tick since you update a credit or flit. This router
     * component is not running time stepped and hence it stops
     * ticking when it has no packets. This is to wake it up */
    if(!ticking)
    {
        ticking = true;
        IrisEvent* event = new IrisEvent();
        event->type = TICK_EVENT;
        event->vc = e->vc;
        Simulator::Schedule( floor(Simulator::Now())+1, &NetworkComponent::process_event, this, event);
    }

    delete data;
    delete e;
    return ;
}		/* -----  end of function GenericRouterPhy::handle_link_arrival_event  ----- */

void
GenericRouterPhy::do_switch_traversal()
{
    /* Switch traversal:
     * Main steps 
     * 1. Check the crossbar (empty or not)
     * 2. Pull from it ad send the flit out
     * 3. If tail free locked resources
     * 4. For every flit update state and send credits back*/
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == SW_TRAVERSAL)
        {
            uint oport = input_buffer_state[i].output_port;
            uint iport = input_buffer_state[i].input_port;
            ticking = true;
            if( !xbar.is_empty(oport,0) 
                && input_buffer_state[i].flits_in_ib > 0
                && downstream_credits[oport][0]>0 )
            {
                istat->stat_router[node_ip]->st_cycles++;
                in_buffers[iport].change_pull_channel(0);
                Flit* f = in_buffers[iport].pull();
                input_buffer_state[i].flits_in_ib--;

                last_flit_out_cycle = Simulator::Now();

                LinkArrivalData* data = new LinkArrivalData();
                data->type = FLIT_ID;
                data->vc = 0;
                data->ptr = f;

                stat_flit_out[iport][oport]++;
#ifdef _DEEP_DEBUG
                _DBG(" FLIT OUT ftype: %d ", f->type);
                switch( f->type)
                {
                    case HEAD:
                        cout << static_cast<HeadFlit*>(f)->toString();
                        break;
                    case BODY:
                        cout << static_cast<BodyFlit*>(f)->toString();
                        break;
                    case TAIL:
                        cout << static_cast<TailFlit*>(f)->toString();
                        break;
                }
#endif
                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(data);
                event->src_id = address;
                event->dst_id = output_connections[oport]->address;
                event->vc = data->vc;
                Simulator::Schedule( Simulator::Now()+0.75,
                                     &NetworkComponent::process_event,
                                     static_cast<GenericLink*>(output_connections[oport])->output_connection,event);
                static_cast<GenericLink*>(output_connections[oport])->flits_passed++;
                downstream_credits[oport][0]--;
                if( f->type == TAIL || f->is_single_flit_pkt )
                {
                    /* Update packet stats */
                    double lat;
                    if(do_two_stage_router)
                        lat = Simulator::Now() - input_buffer_state[i].arrival_time;
                    else
                        lat = (Simulator::Now() - input_buffer_state[i].arrival_time)+1;

                    total_packet_latency+= lat;
                    if( f->type == HEAD)
                    {
                        HeadFlit* hf = static_cast<HeadFlit*>(f);
                        hf->avg_network_latency += lat;
                        hf->hop_count++;
                    }
                    else
                    {
                        TailFlit* tf = static_cast<TailFlit*>(f);
                        tf->avg_network_latency += lat;
                        tf->hop_count++;
                    }
                    stat_packet_out[iport][oport]++;

                    /* Clear locked resources. This is at a
                     * message level */
                    input_buffer_state[i].clear_message = true;
                    input_buffer_state[i].pipe_stage = EMPTY;
                    swa.clear_winner(input_buffer_state[i].output_port, input_buffer_state[i].input_port);
                    xbar.pull(input_buffer_state[i].output_port,input_buffer_state[i].output_channel);
#ifdef _DEBUG_ROUTER
                    _DBG(" Tail FO clear pkt for inport %d oport %d ", iport, oport);
#endif
                }
                else
                {
#ifdef _DEBUG_ROUTER
                    _DBG(" Flit out for inport %d oport %d fty: %d ", iport, oport, f->type);
#endif
                }

                /* Safe to send credits here as the flit is sure to empty
                 * the buffer. */
                //                    if(!send_early_credit)
                send_credit_back(i);

            }
        }

}

void
GenericRouterPhy::do_switch_allocation()
{
    /* Switch Allocation */
    bool sa_active = false;
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == SWA_REQUESTED)
        {
            if ( !swa.is_empty())
            {
                uint oport = -1;
                SA_unit vca_winner;
                uint iport = input_buffer_state[i].input_port;
                for ( uint j=0; j<input_buffer_state[i].possible_oports.size(); j++)
                {
                    oport = input_buffer_state[i].possible_oports[j];
                    vca_winner = swa.pick_winner(oport);
                    sa_active = true;
                }

                if( vca_winner.port == iport )
                {
                    input_buffer_state[i].pipe_stage = SW_TRAVERSAL;
                    input_buffer_state[i].output_port = oport;
                    input_buffer_state[i].output_channel = 0;
                    xbar.configure_crossbar(iport,oport,0);
                    /* The crossbar is a physical crossbar but
                     * needs to know the virtual channel as well. So that it pulls
                     * from the correct place. This need not always be
                     * and other crossbar designs may not require this.
                     * */
                    xbar.push(oport,0);

#ifdef _DEBUG_ROUTER
                    _DBG(" SWA won for inport %d oport %d ", iport, oport);
#endif
                }
                else
                {
#ifdef _DEBUG_ROUTER
                    _DBG(" Dint win for inport %d SWA winner was inport %d for oport %d "
                         ,iport, vca_winner.port, oport);
#endif
                    swa.clear_requested(oport, iport);
                    input_buffer_state[i].pipe_stage = FULL;
                    input_buffer_state[i].flits_in_ib--;

                }
                ticking = true;
            }
            else
            {
                cout << "ERROR SWA was requested but the switch allocator has no requests" << endl;
            }
        }

        if(sa_active)
            istat->stat_router[node_ip]->sa_cycles++;
}

void
GenericRouterPhy::do_input_buffering()
{
    /*  Input buffering 
     *  Flits are pushed into the input buffer in the link arrival handler
     *  itself. To ensure the pipeline stages are executed in reverse pipe
     *  order IB is done here and all link_traversals have higher priority and get done before
     *  tick. Head/Body and Tail flits go thru the IB stage.*/
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == FULL )
        {
#ifdef _DEBUG_ROUTER
            _DBG(" IB + RC inport:%d oport:%d length:%d arrival_time: %f", input_buffer_state[i].input_port,
                 input_buffer_state[i].output_port, input_buffer_state[i].length, Simulator::Now());
#endif
            input_buffer_state[i].pipe_stage = IB;
            input_buffer_state[i].flits_in_ib++;
            ticking = true;
            istat->stat_router[node_ip]->ib_cycles++;
        }

    /*! \brief Body and tail flits get written in link arrival and since the message
     * state may already been pushed to ST because of the header we want to
     * ensure that all flits go thru an IB and ST stage. Hence ST is done on
     * the flits_in_ib information and not buffer occupancy. */
    for( uint i=0; i<(ports*vcs); i++)
    {
        uint iport = input_buffer_state[i].input_port;
        uint oport = input_buffer_state[i].output_port ;

        /* The head could have advanced to switch traversal or
         * still be in ib when the body comes in. Notice buffer
         * state is maintained per packet and hence its the head
         * that is controlling it
         * */
        if ((input_buffer_state[i].pipe_stage == SW_TRAVERSAL || input_buffer_state[i].pipe_stage == IB )
            || input_buffer_state[i].pipe_stage == SWA_REQUESTED 
            && (input_buffer_state[i].flits_in_ib < in_buffers[iport].get_occupancy(0)))
        { 
            input_buffer_state[i].flits_in_ib++;
            ticking = true;
#ifdef _DEBUG_ROUTER
            _DBG(" IB for BODY/TAIL for inport %d oport %d ", iport, oport);
#endif
        }
    }

    return;
}

/*! \brief Event handle for the TICK_EVENT. Entry from DES kernel 
 * This implements the pipe stages for the router. Easy way to follow
 * it. There are as many messages in the system as ports*vcs. 
 * Loop over the input_buffer_state ( ports*vcs ) times. Check for a
 * message in a certain pipe stage and do that pipe stage.
 * */
void
GenericRouterPhy::handle_tick_event ( IrisEvent* e )
{

#ifdef _DEEP_DEBUG
    _DBG_NOARG("input_buffer_state matrix\n");
    for( uint i=0; i<ports*vcs; i++)
        cout <<i<<" "<< input_buffer_state[i].toString()<< " buff_occ:" <<in_buffers[input_buffer_state[i].
            input_port].get_occupancy(input_buffer_state[i].input_channel) << endl;
#endif

    ticking = false;

    do_switch_traversal();
    do_switch_allocation();
    /* Fos this router it can also be 
     * do_switch_allocation();
     * do_switch_traversal()
     * which then makes both take one cycle as oposed to each
     * taking one cycle.
     * Need to examine dependencies carefully for things like this.
     * */

    do_input_buffering();

    /* Request Switch Allocation. At the end of IB all packets can ask
     * for SA. In the SA stage you pick a winner and move forward. 
     * */
    for( uint i=0; i<(ports*vcs); i++)
    {
        if( input_buffer_state[i].pipe_stage == IB )
        {
            uint iport = input_buffer_state[i].input_port;
            uint oport = input_buffer_state[i].possible_oports[0];
            //input_buffer_state[i].pipe_stage = ROUTED;
            if( !swa.is_requested(oport,iport) 
                && downstream_credits[oport][0] == credits)
            {
                swa.request(oport, iport);
                input_buffer_state[i].pipe_stage = SWA_REQUESTED;
                ticking = true;
            }
        }
    }

    if(ticking)
    {
        ticking = true;
        IrisEvent* event = new IrisEvent();
        event->type = TICK_EVENT;
        event->vc = e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }

    delete e;
    return;

}		/* -----  end of function GenericRouterPhy::handle_input_arbitration_event  ----- */

string
GenericRouterPhy::toString() const
{
    stringstream str;
    str << "GenericRouterPhy"
        << "\t addr: " << address
        << " node_ip: " << node_ip
        << "\n Input buffers: " << in_buffers.size() << " ";
    if( in_buffers.size() > 0)
        str << in_buffers[0].toString();

    str << "\n SWA: " << swa.toString();

    str << "\n decoders: " << decoders.size() << " ";
    if( decoders.size() > 0)
        str << decoders[0].toString();

    str << "\n Xbar ";
    str << xbar.toString();

    return str.str();
}

/* Credits in this router are sent at a per flit basis
 * FLIT_LEVEL_FLOW_CONTROL
 * */
void
GenericRouterPhy::send_credit_back(uint i)
{
    input_buffer_state[i].credits_sent--;
    LinkArrivalData* data = new LinkArrivalData();
    uint port = input_buffer_state[i].input_port;
    data->type = CREDIT_ID;
    data->vc = input_buffer_state[i].input_channel;
    IrisEvent* event = new IrisEvent();
    event->type = LINK_ARRIVAL_EVENT;
    event->event_data.push_back(data);
    event->src_id = address;
    event->vc = data->vc; 
    Simulator::Schedule(Simulator::Now()+0.75, &NetworkComponent::process_event,
                        static_cast<GenericLink*>(input_connections[port])->input_connection, event);

#ifdef _DEBUG_ROUTER
    _DBG(" Credit back for inport %d inch %d ", port, data->vc);
#endif
}


#endif   /* ----- #ifndef _genericRouterPhy_cc_INC  ----- */

