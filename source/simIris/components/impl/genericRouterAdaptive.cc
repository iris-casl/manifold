/*
 * =====================================================================================
 *
 *       Filename:  genericRouterAdaptive.cc
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

#ifndef  _genericRouterAdaptive_cc_INC
#define  _genericRouterAdaptive_cc_INC

#include        "genericRouterAdaptive.h"
using namespace std;

GenericRouterAdaptive::GenericRouterAdaptive ()
{
    name = "Router" ;
    ticking = false;
}  /* -----  end of method GenericRouterAdaptive::GenericRouterAdaptive  (constructor)  ----- */

GenericRouterAdaptive::~GenericRouterAdaptive()
{
}

void
GenericRouterAdaptive::init (uint p, uint v, uint cr, uint bs)
{
    ports =p;
    vcs =v;
    credits =cr;
    buffer_size = bs;

    address = myId();

    /*  set_input_ports(ports); */
    in_buffers.resize(ports);
    if( multiple_flit_in_buf)
        new_pkt_buffer.resize(ports);
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
        decoders[i].node_ip = node_ip;
        decoders[i].address = address;
        stat_packet_out[i].resize(ports);
        stat_flit_out[i].resize(ports);
    }
    swa.node_ip = node_ip;
    swa.address = address;

    /*  set_no_virtual_channels(vcs); */
    for(uint i=0; i<ports; i++)
    {
        downstream_credits[i].resize(vcs);
        in_buffers[i].resize( vcs, buffer_size );
        if( multiple_flit_in_buf)
            new_pkt_buffer[i].resize( vcs, buffer_size );
        decoders[i].resize( vcs );
    }

    for(uint i=0; i<ports; i++)
        for(uint j=0; j<vcs; j++)
        {
            downstream_credits[i][j] = credits;
            input_buffer_state[i*vcs+j].pipe_stage = EMPTY;
            input_buffer_state[i*vcs+j].pkt_in_progress = false;
        }

    /* init the countes */
    packets = 0;
    flits = 0;
    total_packet_latency = 0;

    return ;
}               /* -----  end of function GenericRouterAdaptive::init  ----- */

/*! \brief These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
GenericRouterAdaptive::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
GenericRouterAdaptive::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
GenericRouterAdaptive::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
GenericRouterAdaptive::process_event ( IrisEvent* e )
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
            _DBG("GenericRouterAdaptive:: Unk event exception %d", e->type);
            break;
    }
    return ;
}               /* -----  end of function GenericRouterAdaptive::process_event  ----- */

string
GenericRouterAdaptive::print_stats()
{
    stringstream str;
    str << "\n router[" << node_ip << "] packet latency: " << total_packet_latency
        << "\n router[" << node_ip << "] flits/packets: " << (flits+0.0)/(packets)
        << "\n router[" << node_ip << "] average packet latency: " << (total_packet_latency+0.0)/packets
        << "\n router[" << node_ip << "] last_flit_out_cycle: " << last_flit_out_cycle
        << " ";
    str << "\n router[" << node_ip << "] packets: " << packets
        << "\n router[" << node_ip << "] flits: " << flits;
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
            if ( i != j) 
            {
                str << "\n    router[" << node_ip << "] Packets out " << in_port 
                    << " going " << out_port << " : " << stat_packet_out[i][j];
                str << "\n    router[" << node_ip << "] Flits out " << in_port 
                    << " going " << out_port << " : " << stat_flit_out[i][j];
            }

        }


    return str.str();
}

void
GenericRouterAdaptive::init_buffer_state(uint port,uint vc,HeadFlit* hf)
{
    input_buffer_state[port*vcs+vc].input_port = port;
    input_buffer_state[port*vcs+vc].input_channel = vc;
    input_buffer_state[port*vcs+vc].pipe_stage = FULL;
    input_buffer_state[port*vcs+vc].msg_class = hf->msg_class;
    input_buffer_state[port*vcs+vc].possible_oports.clear(); 
    input_buffer_state[port*vcs+vc].possible_ovcs.clear(); 
    input_buffer_state[port*vcs+vc].possible_ovcs.resize(1); 
    uint no_adaptive_ports = decoders[port].no_adaptive_ports(vc);
    for ( uint ii=0; ii<no_adaptive_ports; ii++)
    {
        uint rc_port = decoders[port].get_output_port(vc);
        vector<uint>::iterator i;
        i = find(available_ports.begin(), available_ports.end(), rc_port);
        if( static_cast<GenericLink*>(output_connections[rc_port])->output_connection != NULL )
            input_buffer_state[port*vcs+vc].possible_oports.push_back(rc_port);
    } 
    assert ( input_buffer_state[port*vcs+vc].possible_oports.size() > 0);
    input_buffer_state[port*vcs+vc].possible_ovcs[0] = 0;
    input_buffer_state[port*vcs+vc].length= hf->length;
    input_buffer_state[port*vcs+vc].bodies_sent = 0;
    input_buffer_state[port*vcs+vc].credits_sent= hf->length;
    input_buffer_state[port*vcs+vc].sent_head = false;
    input_buffer_state[port*vcs+vc].arrival_time = ceil(Simulator::Now());
    input_buffer_state[port*vcs+vc].clear_message = false;
    input_buffer_state[port*vcs+vc].flits_in_ib = 0;
    input_buffer_state[port*vcs+vc].has_pending_pkts = false;

    return;
}

/*! \brief Event handle for the LINK_ARRIVAL_EVENT event. Entry from DES kernel */
void
GenericRouterAdaptive::handle_link_arrival_event ( IrisEvent* e )
{
    if ( multiple_flit_in_buf)
        handle_link_arrival_event_multiple_flit_in_buffer(e);
    else
        handle_link_arrival_event_one_msg_per_buffer(e);

    return;
}

void
GenericRouterAdaptive::handle_link_arrival_event_one_msg_per_buffer( IrisEvent* e )
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
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
        if( data->ptr->type == HEAD )
        {
            HeadFlit* hf = static_cast<HeadFlit*>(data->ptr);
            hf->inport = port;
            decoders[port].push(data->ptr,data->vc);
            init_buffer_state(port,data->vc,hf);
            _DBG(" push HEAD into inbuf inp: %d",port);
        }
        else
        {
            decoders[port].push(data->ptr,data->vc);
            if ( data->ptr->type == BODY )
            {
                _DBG(" push BODY into inbuf inp: %d",port);
            }
            else
            {
                _DBG(" push TAIL into inbuf inp: %d",port);
            }
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
}		/* -----  end of function GenericRouterAdaptive::handle_link_arrival_event  ----- */

void
GenericRouterAdaptive::handle_link_arrival_event_multiple_flit_in_buffer( IrisEvent* e )
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
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
        if( data->ptr->type == HEAD )
        {
            if( !data->ptr->is_single_flit_pkt )
                input_buffer_state[port*vcs+data->vc].pkt_in_progress = true;
            else
                input_buffer_state[port*vcs+data->vc].pkt_in_progress = false;
            if( input_buffer_state[port*vcs+data->vc].pipe_stage  == EMPTY )
            {
                in_buffers[port].change_push_channel(data->vc);
                in_buffers[port].push(data->ptr);

                HeadFlit* hf = static_cast<HeadFlit*>(data->ptr);
                hf->inport = port;
                decoders[port].push(data->ptr,data->vc);
                init_buffer_state(port,data->vc,hf);
                _DBG(" push HEAD into inbuf inp: %d",port);

            }
            else
            {
                input_buffer_state[port*vcs+data->vc].has_pending_pkts = true;
                new_pkt_buffer[port].change_push_channel(data->vc);
                new_pkt_buffer[port].push(data->ptr);
                _DBG(" push HEAD into newpktbuf inp: %d",port);
            }
        }
        else
        {
            if ( !input_buffer_state[port*vcs+data->vc].has_pending_pkts )
            {
                in_buffers[port].change_push_channel(data->vc);
                in_buffers[port].push(data->ptr);
                decoders[port].push(data->ptr,data->vc);
                if( data->ptr->type == BODY)
                {
                    _DBG(" push BODY into inbuf inp: %d",port);
                }
                else
                {
                    _DBG(" push TAIL into inbuf inp: %d",port);
                    input_buffer_state[port*vcs+data->vc].pkt_in_progress = false;
                }

            }
            else
            {
                new_pkt_buffer[port].change_push_channel(data->vc);
                new_pkt_buffer[port].push(data->ptr);
                if( data->ptr->type == BODY)
                {
                    _DBG(" push BODY into nepktbuf inp: %d",port);
                }
                else
                {
                    _DBG(" push TAIL into newpktbuf inp: %d",port);
//                    input_buffer_state[port*vcs+data->vc].pkt_in_progress = false;
                }

            }

        }

    }
    else if ( data->type == CREDIT_ID)
    {
        /* Find the corresponding output port */
        bool found = false;
        uint port = -1;
        /* Some links of corner routers may be null. Check for tht condition
         * here. */
        for ( uint i=0 ; ports ; i++ )
            if(static_cast<GenericLink*>(output_connections[i])->output_connection) 
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
}               /* -----  end of function GenericRouterAdaptive::handle_link_arrival_event  ----- */

void
GenericRouterAdaptive::do_switch_traversal()
{
    /* Switch traversal */
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == ST)
        {
            uint oport = input_buffer_state[i].output_port;
            uint iport = input_buffer_state[i].input_port;
            bool sent_head = input_buffer_state[i].sent_head;
            bool downstream_pkt_progress = false;
            if (!sent_head && oport!=0)
            {
                GenericRouterAdaptive* next_router = static_cast<GenericRouterAdaptive*>(
                                                                                         static_cast<GenericLink*>(output_connections[oport])->output_connection);
                downstream_pkt_progress = next_router->is_pkt_in_progress(static_cast<GenericLink*>(output_connections[oport]),0);
                _DBG("Sending HEAD but next_buf_free for %d buffst: ",oport); 
                cout << downstream_pkt_progress;

            }
            /* 
               else if ( f->type == HEAD )
               {
               GenericInterface* next_int = static_cast<GenericInterface*>(
               static_cast<GenericLink*>(output_connections[oport])->output_connection);
               next_buffer_free = next_int->is_pkt_in_progress(static_cast<GenericLink*>(output_connections[oport]),0);

               }
             * */
            if( !xbar.is_empty(oport,0) 
                && input_buffer_state[i].flits_in_ib > 0
                && downstream_credits[oport][0]>0 && !downstream_pkt_progress)
            {
                if( oport == 0)
                    cout << "\n *********OP0 SENDING a pkt from ROUTER" << endl;
                else
                    cout << "\n ********* SENDING a pkt from ROUTER" << endl;
                in_buffers[iport].change_pull_channel(0);
                Flit* f = in_buffers[iport].pull();
                input_buffer_state[i].flits_in_ib--;


                LinkArrivalData* data = new LinkArrivalData();
                data->type = FLIT_ID;
                data->vc = 0;
                data->ptr = f;

                last_flit_out_cycle = Simulator::Now();
                stat_flit_out[iport][oport]++;
                static_cast<GenericLink*>(output_connections[oport])->flits_passed++;

                if (f->type == HEAD && oport!=0)
                {
                    GenericRouterAdaptive* next_router = static_cast<GenericRouterAdaptive*>(
                                                                                             static_cast<GenericLink*>(output_connections[oport])->output_connection);
                    assert( next_router->is_pkt_in_progress(static_cast<GenericLink*>
                                                            (output_connections[oport]),0) == false );
                }
                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(data);
                event->src_id = address;
                event->dst_id = output_connections[oport]->address;
                event->vc = data->vc;
                if(do_two_stage_router)
                    Simulator::Schedule( Simulator::Now()+0.75,
                                         &NetworkComponent::process_event,
                                         static_cast<GenericLink*>(output_connections[oport])->output_connection,event);
                else
                    Simulator::Schedule( Simulator::Now()+1.75,
                                         &NetworkComponent::process_event,
                                         static_cast<GenericLink*>(output_connections[oport])->output_connection,event);
                downstream_credits[oport][0]--;

                if( f->type == BODY )
                    input_buffer_state[i].bodies_sent++;

                double lat = 0;
                /* Update packet stats */
                if(do_two_stage_router)
                    lat = Simulator::Now() - input_buffer_state[i].arrival_time;
                else
                    lat = (Simulator::Now() - input_buffer_state[i].arrival_time)+1;
                if( f->type == HEAD)
                {
                    input_buffer_state[i].sent_head = true;
                    HeadFlit* hf = static_cast<HeadFlit*>(f);
                    hf->avg_network_latency += lat;
                    hf->hop_count++;
                }
                if( f->type == TAIL || f->is_single_flit_pkt )
                {

                    total_packet_latency+= lat;
                    stat_packet_out[iport][oport]++;

                    if ( f->type == TAIL) 
                    {
                        TailFlit* tf = static_cast<TailFlit*>(f);
                        tf->avg_network_latency += lat;
                        tf->hop_count++;
                    }

                    if ( input_buffer_state[i].msg_class == ONE_FLIT_REQ)
                        assert( input_buffer_state[i].bodies_sent+1 == input_buffer_state[i].length);
                    else if (input_buffer_state[i].msg_class == RESPONSE_PKT)
                        assert( input_buffer_state[i].bodies_sent+2 == input_buffer_state[i].length);

                    input_buffer_state[i].clear_message = true;
                    input_buffer_state[i].pipe_stage = EMPTY;
                    swa.clear_winner(input_buffer_state[i].output_port, input_buffer_state[i].input_port);
                    xbar.pull(input_buffer_state[i].output_port,input_buffer_state[i].output_channel);
                    input_buffer_state[i].possible_oports.clear();
                }


                /* Safe to send credits here as the flit is sure to empty
                 * the buffer. */
                //                    if(!send_early_credit)
                send_credit_back(i);

            }
            else
            {
                if (downstream_credits[oport][0] == 0 && downstream_pkt_progress)
                {
                    ticking = true;
                }
            }

        }

}

void
GenericRouterAdaptive::do_switch_allocation()
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
                //for ( uint j=0; j<input_buffer_state[i].possible_oports.size(); j++)
                // {
                oport = input_buffer_state[i].output_port;
                vca_winner = swa.pick_winner(oport);
                // }

                if( vca_winner.port == iport )
                {
                    input_buffer_state[i].pipe_stage = ST;
                    input_buffer_state[i].output_port = oport;
                    input_buffer_state[i].output_channel = 0;
                    xbar.configure_crossbar(iport,oport,0);
                    xbar.push(oport,0); /* The crossbar is generic and needs to know the vc being used as well */

                    /* After allocating the downstream path for this message
                     * send a credit on this inport for the HEAD. 
                     if(send_early_credit)
                     send_credit_back(i);
                     * */
                }
                else
                {
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

}

/*! \brief Event handle for the TICK_EVENT. Entry from DES kernel */
void
GenericRouterAdaptive::handle_tick_event ( IrisEvent* e )
{

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
    do_switch_allocation();
    do_switch_traversal();


    /*  Input buffering 
     *  Flits are pushed into the input buffer in the link arrival handler
     *  itself. To ensure the pipeline stages are executed in reverse pipe
     *  order IB is done here and all link_traversals have higher priority and get done before
     *  tick. Head/Body and Tail flits go thru the IB stage.*/
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == FULL )
        {
            input_buffer_state[i].pipe_stage = IB;
            input_buffer_state[i].flits_in_ib++;
            ticking = true;
        }

    /*! \brief Body and tail flits get written in link arrival and since the message
     * state may already been pushed to ST because of the header we want to
     * ensure that all flits go thru an IB and ST stage. Hence ST is done on
     * the flits_in_ib information and not buffer occupancy. */
    for( uint i=0; i<(ports*vcs); i++)
    {
        uint iport = input_buffer_state[i].input_port;
        uint oport = input_buffer_state[i].output_port ;

        if ((input_buffer_state[i].pipe_stage == ST || input_buffer_state[i].pipe_stage == IB )
            && (input_buffer_state[i].flits_in_ib < in_buffers[iport].get_occupancy(0)))
        { 
            input_buffer_state[i].flits_in_ib++;
            ticking = true;
            /* Sending credits back for body+tail: Condition being
             * HEAD in ST and having downstream credits 
             if(send_early_credit)
             if( input_buffer_state[i].pipe_stage == ST && downstream_credits[oport][0] > 0)
             send_credit_back(i);
             */
        }
    }

    /* Request Switch Allocation */
    for( uint i=0; i<(ports*vcs); i++)
    {
        if( input_buffer_state[i].pipe_stage == IB )
        {
            uint iport = input_buffer_state[i].input_port;
            uint oport = -1;
            for ( uint aa=0; aa<input_buffer_state[i].possible_oports.size();aa++)
            {
                if( multiple_flit_in_buf)
                {
                    if( downstream_credits[input_buffer_state[i].possible_oports[aa]][0] >0 )
                        oport = input_buffer_state[i].possible_oports[aa];
                }
                else
                {
                    if ( downstream_credits[input_buffer_state[i].possible_oports[aa]][0] == credits)
                        oport = input_buffer_state[i].possible_oports[aa];
                }
            }

            if ( oport == -1)
                oport = input_buffer_state[i].possible_oports[0];

            bool request = false;
            if ( multiple_flit_in_buf && downstream_credits[oport][0]>0)
                request = true;
            else if ( downstream_credits[oport][0] == credits )
                request = true;

            if( !swa.is_requested(oport,iport) && request)
            {
                swa.request(oport, iport, input_buffer_state[i].msg_class);
                input_buffer_state[i].output_port = oport;
                input_buffer_state[i].pipe_stage = SWA_REQUESTED;
                ticking = true;
            }
        }
    }
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == ST )
            ticking = true;

    /*This stage is included only for cases when multiple msgs can occupy the
     * same input buffer. (See usage for flag multiple_flit_in_buf) */
    if( multiple_flit_in_buf)
    {
        for( uint i=0; i<(ports*vcs); i++)
            if( input_buffer_state[i].pipe_stage == EMPTY 
                && input_buffer_state[i].has_pending_pkts )
            {
                input_buffer_state[i].has_pending_pkts = true;
                uint inp = (uint)floor(i/vcs);
                new_pkt_buffer[inp].change_pull_channel(0);
                while( new_pkt_buffer[inp].get_occupancy(0) > 0)
                {
                    Flit* f = new_pkt_buffer[inp].pull();
                    in_buffers[inp].push(f);

                    if ( new_pkt_buffer[inp].get_occupancy(0)==0 )
                        input_buffer_state[i].has_pending_pkts = false;

                    ticking = true;

                    if ( f->type == HEAD )
                    {
                        HeadFlit* hf = static_cast<HeadFlit*>(f);
                        hf->inport = inp;
                        decoders[inp].push(hf,0);
                        init_buffer_state(inp,0,hf);
                        _DBG(" push HEAD into inbuf inp: %d",inp);
                    }
                    else if ( f->type == BODY)
                    {
                        decoders[inp].push(f,0);
                        _DBG(" push HEAD into inbuf inp: %d",inp);
                    }
                    else if ( f->type == TAIL)
                    {
                        decoders[inp].push(f,0);
                        _DBG(" push HEAD into inbuf inp: %d",inp);
                        break;
                    }


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

}               /* -----  end of function GenericRouterAdaptive::handle_input_arbitration_event  ----- */

string
GenericRouterAdaptive::toString() const
{
    stringstream str;
    str << "GenericRouterAdaptive"
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
GenericRouterAdaptive::send_credit_back(uint i)
{
    //    if( input_buffer_state[i].credits_sent)
    //    {
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
    if(do_two_stage_router)
        Simulator::Schedule(Simulator::Now()+0.75, &NetworkComponent::process_event,
                            static_cast<GenericLink*>(input_connections[port])->input_connection, event);
    else
        Simulator::Schedule(Simulator::Now()+1.75, &NetworkComponent::process_event,
                            static_cast<GenericLink*>(input_connections[port])->input_connection, event);

#ifdef _DEBUG_ROUTER
    _DBG(" Credit back for inport %d inch %d ", port, data->vc);
#endif
    /* 
       }
       else
       {
       _DBG(" Requested to send extra Credit back for inport %d ",
       input_buffer_state[i].input_port);
       }
     * */
}

bool
GenericRouterAdaptive::is_pkt_in_progress(GenericLink* link, uint vc)
{
    bool found = false;
    uint port = -1;
    /*  Need to check for null here as there may be null links to corner routers */
    for ( uint i=0 ; i< ports ; i++ )
        if( link->address == static_cast<GenericLink*>(input_connections[i])->address)
        {
            found = true;
            port = i;
            break;
        }
    if ( !found )
    {
        cout << "ERROR cant find input connection " << endl;
        exit(1);
    }
//    _DBG("was queried for inport %d status:",port);
//    cout << input_buffer_state[port*vcs+vc].pkt_in_progress;
    return input_buffer_state[port*vcs+vc].pkt_in_progress;
}

#endif   /* ----- #ifndef _genericRouterAdaptive_cc_INC  ----- */

