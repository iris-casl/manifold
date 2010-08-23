/*
 * =====================================================================================
 *
 *       Filename:  genericRouterNoVcs.cc
 *
 *! \brief    Description: Implementing a generic 4 stage physical router model
 *    BW->RC->SA->ST->LT
 *    Buffer write(BW)
 *    Route Computation (RC)
 *    Configure Crossbar (CC)
 *    Switch Traversal (ST)
 *    Link Traversal (LT)
 *
 *    Model Description in cycles:
 *    ---------------------------
 *    BW and RC stages happen in the same cycle ( BW pushes the flits into the
 *    input buffer and the RC unit. )
 *    SA: Pick one output port from n requesting input ports (0<n<p) for the pxp crossbar 
 *    ST: Move the flits across the crossbar and push it out on the link
 *    LT: This is not modelled within the router and is part of the link component.
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

#ifndef  _genericRouterNoVcs_cc_INC
#define  _genericRouterNoVcs_cc_INC

#include	"genericRouterNoVcs.h"
using namespace std;

GenericRouterNoVcs::GenericRouterNoVcs ()
{
    name = "Router" ;
    ticking = false;
}  /* -----  end of method GenericRouterNoVcs::GenericRouterNoVcs  (constructor)  ----- */

GenericRouterNoVcs::~GenericRouterNoVcs()
{
}

void
GenericRouterNoVcs::init (uint p, uint v, uint cr, uint bs)
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
    downstream_credits.resize(ports);

    /* set_output_ports(ports); */
    xbar.set_input_ports(ports);
    xbar.set_output_ports(ports);
    xbar.set_no_virtual_channels(vcs);

    /* All decoders and vc arbiters need to know the node_ip for routing */
    for(uint i=0; i<ports; i++)
    {
        decoders[i].node_ip = node_ip;
        decoders[i].address = address;
    }

    /*  set_no_virtual_channels(vcs); */
    for(uint i=0; i<ports; i++)
    {
        downstream_credits[i].resize(vcs);
        in_buffers[i].resize( vcs, buffer_size );
        decoders[i].resize( vcs );
    }

    for(uint i=0; i<ports; i++)
        for(uint j=0; j<vcs; j++)
            downstream_credits[i][j] = credits;

    /* init the countes */
    packets = 0;
    flits = 0;
    total_packet_latency = 0;

    return ;
}		/* -----  end of function GenericRouterNoVcs::init  ----- */

/*! \brief These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
GenericRouterNoVcs::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
GenericRouterNoVcs::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
GenericRouterNoVcs::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
GenericRouterNoVcs::process_event ( IrisEvent* e )
{
    switch(e->type)
    {
        case LINK_ARRIVAL_EVENT:
            handle_link_arrival_event(e);
            break;
        case TICK_EVENT:
            handle_tick_event(e);
            break;
        default:
            _DBG("GenericRouterNoVcs:: Unk event exception %d", e->type);
            break;
    }
    return ;
}		/* -----  end of function GenericRouterNoVcs::process_event  ----- */

string
GenericRouterNoVcs::print_stats()
{
    stringstream str;
    str << "\n router[" << node_ip << "] packets: " << packets
        << "\n router[" << node_ip << "] flits: " << flits
        << "\n router[" << node_ip << "] packet latency: " << total_packet_latency
        << "\n router[" << node_ip << "] flits/packets: " << (flits+0.0)/(packets)
        << "\n router[" << node_ip << "] average packet latency: " << (total_packet_latency+0.0)/packets
        << "\n router[" << node_ip << "] last_flit_out_cycle: " << last_flit_out_cycle
        << " ";

    return str.str();
}

/*! \brief Event handle for the LINK_ARRIVAL_EVENT event. Entry from DES kernel */
void
GenericRouterNoVcs::handle_link_arrival_event ( IrisEvent* e )
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
    if(data->type == FLIT_ID)
    {
        /*  Update stats */
        flits++;
        if( data->ptr->type == TAIL || data->ptr->is_single_flit_pkt )
        {
            packets++;
        }

        /*Find the port the flit came in on */
        bool found = false;
        uint port = -1;
        for ( uint i=0 ; i< ports ; i++ )
            if(static_cast<GenericLink*>(input_connections[i])->input_connection) /*  This is here because we may assign null links to corner routers */
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
        if( data->ptr->type == HEAD )
        {
            HeadFlit* hf = static_cast<HeadFlit*>(data->ptr);
            input_buffer_state[port*vcs+data->vc].input_port = port;
            input_buffer_state[port*vcs+data->vc].input_channel = data->vc;
            input_buffer_state[port*vcs+data->vc].pipe_stage = FULL;
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
#ifdef _DEBUG_ROUTER
            _DBG(" BW inport:%d invc:%d oport:%d ovc:%d ft:%d", port, data->vc,
                 input_buffer_state[port*vcs+data->vc].output_port, input_buffer_state[port*vcs+data->vc].output_channel, data->ptr->type);
#endif

        }
        
    }
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
        _DBG(" Got a credit port:%d vc:%d in_buffers[port][vc]:%d ", port, data->vc, downstream_credits[port][data->vc]);
#endif

    }
    else
    {
        _DBG( "handle_link_arrival_event Unk data type %d ", data->type);
    }

        /* Tick since you update a credit or flit */
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
}		/* -----  end of function GenericRouterNoVcs::handle_link_arrival_event  ----- */

void
GenericRouterNoVcs::do_switch_traversal()
{
    /* Switch traversal */
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == ST)
        {
                uint oport = input_buffer_state[i].output_port;
                uint iport = input_buffer_state[i].input_port;
                uint ich = input_buffer_state[i].input_channel;
                if( !xbar.is_empty(oport,0) 
                    && input_buffer_state[i].flits_in_ib > 0
                    && downstream_credits[oport][0]>0 )
                {
                    in_buffers[iport].change_pull_channel(ich);
                    Flit* f = in_buffers[iport].pull();
                    input_buffer_state[i].flits_in_ib--;
                    
                    last_flit_out_cycle = Simulator::Now();

                    LinkArrivalData* data = new LinkArrivalData();
                    data->type = FLIT_ID;
                    data->vc = 0;
                    data->ptr = f;
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
                    Simulator::Schedule( Simulator::Now()+1,
                                     &NetworkComponent::process_event,
                                     output_connections[oport],event);
                    downstream_credits[oport][0]--;
                    if( f->type == TAIL || f->is_single_flit_pkt )
                    {
                        input_buffer_state[i].clear_message = true;
                        input_buffer_state[i].pipe_stage = EMPTY;
                        swa.clear_winner(input_buffer_state[i].output_port, input_buffer_state[i].input_port);
                        xbar.pull(input_buffer_state[i].output_port,input_buffer_state[i].output_channel);
#ifdef _DEBUG_ROUTER
    _DBG(" Tail FO clear pkt for inport %d inch %d oport %d ", iport, ich, oport);
#endif
                        /* Update packet stats */
                        double lat = Simulator::Now() - input_buffer_state[i].arrival_time;
                        total_packet_latency+= lat;
                    }
                    else
                    {
#ifdef _DEBUG_ROUTER
    _DBG(" Flit out for inport %d inch %d oport %d fty: %d ", iport, ich, oport, f->type);
#endif
                    }

                    /* Safe to send credits here as the flit is sure to empty
                     * the buffer. */
                        send_credit_back(i);

                }
                else
                {
                    if(in_buffers[iport].get_occupancy(ich)==0)
                    {
#ifdef _DEBUG_ROUTER
                        _DBG_NOARG(" Waiting for flits on inch");
#endif
//                        ticking = true;
                    }
                    else if (xbar.is_empty(oport,0))
                    {
                        _DBG_NOARG(" switch was allocated but crossbar is empty ");
                    }
                    else if (downstream_credits[oport][0] == 0)
                    {
#ifdef _DEBUG_ROUTER
                        _DBG(" Waiting for downstream credits on oport %d ", oport);
#endif
//                       ticking = true;
                    }
                }

        }

}

void
GenericRouterNoVcs::do_switch_allocation()
{
    /* Switch Allocation */
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == SWA_REQUESTED)
        {
            if ( !swa.is_empty())
            {
                uint oport = -1;
                SA_unit vca_winner;
                uint iport = input_buffer_state[i].input_port;
                uint ich = input_buffer_state[i].input_channel;

                for ( uint j=0; j<input_buffer_state[i].possible_oports.size(); j++)
                {
                    oport = input_buffer_state[i].possible_oports[j];
                    vca_winner = swa.pick_winner(oport);
                }

                if( vca_winner.port == iport )
                {
                        input_buffer_state[i].pipe_stage = ST;
                        input_buffer_state[i].output_port = oport;
                        input_buffer_state[i].output_channel = 0;
                        xbar.configure_crossbar(iport,oport,0);
                        xbar.push(oport,0); /* The crossbar is generic and needs to know the vc being used as well */

#ifdef _DEBUG_ROUTER
                _DBG(" SWA won for inport %d oport %d ", iport, oport);
#endif
                    /* After allocating the downstream path for this message
                     * send a credit on this inport for the HEAD. */
//                    send_credit_back(i);
                }
#ifdef _DEBUG_ROUTER
                else
                {
                    _DBG(" Dint win for inport %d SWA winner was inport %d for oport %d "
                         ,iport, vca_winner.port, oport);
                }
#endif
                ticking = true;
            }
            else
            {
                cout << "ERROR SWA was requested but the switch allocator has no requests" << endl;
            }
        }

}

/*! \brief Event handle for the TICK_EVENT. Entry from DES kernel */
void
GenericRouterNoVcs::handle_tick_event ( IrisEvent* e )
{

#ifdef _DEEP_DEBUG
    _DBG_NOARG("input_buffer_state matrix\n");
    for( uint i=0; i<ports*vcs; i++)
        cout <<i<<" "<< input_buffer_state[i].toString()<< " buff_occ:" <<in_buffers[input_buffer_state[i].
            input_port].get_occupancy(input_buffer_state[i].input_channel) << endl;
#endif

    ticking = false;

    /* We need to estimate the actual time at which the input buffer empties
     * and send the credit back. After SA it is known that the buffer for the
     * HEAD will empty in the next cycle and hence a credit can be sent to the
     * link for the HEAD. Post this a check for if the message is in ST and
     * there are flits doing IB. One can send a credit back for BODY and TAIL
     * flits such that the credit is at LT when the flit is in ST.
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == ST || input_buffer_state[i].pipe_stage == SW_ALLOCATED)
        {
            if(in_buffers[input_buffer_state[i].input_port].get_occupancy(input_buffer_state[i].input_channel) >0 
               && input_buffer_state[i].credits_sent>0 )
                send_credit_back(i);
        }
     * */

    do_switch_traversal();
    do_switch_allocation();

   
    /*  Input buffering 
     *  Flits are pushed into the input buffer in the link arrival handler
     *  itself. Just ensuring the state to IB is done last in reverse pipe
     *  order has all link_traversals have higher priority and get done before
     *  tick. */
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == FULL )
    {
#ifdef _DEBUG_ROUTER
        _DBG(" IB + RC inport:%d oport:%d length:%d arrival_time: %f", input_buffer_state[i].input_port,
             input_buffer_state[i].output_port, input_buffer_state[i].length, Simulator::Now());
#endif
            input_buffer_state[i].pipe_stage = IB;
            ticking = true;
    }

    /*! \brief Body and tail flits get written in link arrival and since the message
     * state may already been pushed to ST because of the header we want to
     * ensure that all flits go thru an IB and ST stage. Hence ST is done on
     * the flits_in_ib information and not buffer occupancy. */
    for( uint i=0; i<(ports*vcs); i++)
    {
            uint iport = input_buffer_state[i].input_port;
            uint ich = input_buffer_state[i].input_channel;
            uint oport = input_buffer_state[i].output_port ;

            if ((input_buffer_state[i].pipe_stage == ST || input_buffer_state[i].pipe_stage == IB )
//			|| input_buffer_state[i].pipe_stage == SWA_REQUESTED 
                && (input_buffer_state[i].flits_in_ib < in_buffers[iport].get_occupancy(ich)))
                { 
                    input_buffer_state[i].flits_in_ib++;
                    ticking = true;
#ifdef _DEBUG_ROUTER
                _DBG(" IB for BODY/TAIL for inport %d oport %d ", iport, oport);
#endif
                    /*! \brief Sending credits back for body+tail: Condition being
                     * HEAD in ST and having downstream credits 
                        */
//                    if( input_buffer_state[i].pipe_stage == ST && downstream_credits[oport][0] > 0)
//                        send_credit_back(i);
                }
    }

    /* Route and Request Switch Allocation */
    for( uint i=0; i<(ports*vcs); i++)
    {
        if( input_buffer_state[i].pipe_stage == IB )
        {
            uint iport = input_buffer_state[i].input_port;
            uint ich = input_buffer_state[i].input_channel;
            uint oport = input_buffer_state[i].possible_oports[0];
            //input_buffer_state[i].pipe_stage = ROUTED;
            if( !swa.is_requested(oport,iport) 
                && downstream_credits[oport][0] > 0)
            {
                swa.request(oport, iport);
                input_buffer_state[i].pipe_stage = SWA_REQUESTED;
                ticking = true;
#ifdef _DEBUG_ROUTER
                _DBG(" Req SWA for inport %d inch %d oport %d ", iport, ich,oport);
#endif
            }
#ifdef _DEBUG_ROUTER
            else
            {
                if (swa.is_requested(oport, iport))
                {
                    _DBG("Cant req SWA. SWA full inport:%d inch:%d oport:%d ",iport,ich,oport);
                }
                else if(downstream_credits[input_buffer_state[i].possible_oports[0]][input_buffer_state[i].possible_ovcs[0]] == 0)
                {
                    _DBG("Not requesting SWA. No downstream credits for oport:%d ",oport);
                }
            }
#endif
//            ticking = true;
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

}		/* -----  end of function GenericRouterNoVcs::handle_input_arbitration_event  ----- */

string
GenericRouterNoVcs::toString() const
{
    stringstream str;
    str << "GenericRouterNoVcs"
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

void
GenericRouterNoVcs::send_credit_back(uint i)
{
    if( input_buffer_state[i].credits_sent)
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
    Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,input_connections[port], event);
#ifdef _DEBUG_ROUTER
                    _DBG(" Credit back for inport %d inch %d ", port, data->vc);
#endif
    }
}


#endif   /* ----- #ifndef _genericRouterNoVcs_cc_INC  ----- */

