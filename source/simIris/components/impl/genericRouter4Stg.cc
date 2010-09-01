/*!
 * =====================================================================================
 *
 *       Filename: \file  genericRouter4Stg.cc
 *       Description: \brief Implements the GenericRouter4Stg class.
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

#ifndef  _genericRouter4Stg_cc_INC
#define  _genericRouter4Stg_cc_INC

#include	"genericRouter4Stg.h"
using namespace std;

GenericRouter4Stg::GenericRouter4Stg ()
{
    name = "RouterVcs" ;
    ticking = false;
}  /* -----  end of method GenericRouter4Stg::GenericRouter4Stg  (constructor)  ----- */

GenericRouter4Stg::~GenericRouter4Stg()
{
}

void
GenericRouter4Stg::init (uint p, uint v, uint cr, uint bs)
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
    vca.setup(ports,vcs);
    swa.resize(ports,vcs);
    xbar.setup(ports,vcs);
    downstream_credits.resize(ports);
    cr_time.resize(ports);

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
    vca.node_ip = node_ip;
    vca.address = address;

    /*  set_no_virtual_channels(vcs); */
    for(uint i=0; i<ports; i++)
    {
        downstream_credits[i].resize(vcs);
        cr_time[i].resize(vcs);
        in_buffers[i].resize( vcs, buffer_size );
        decoders[i].resize( vcs );
    }

    for(uint i=0; i<ports; i++)
        for(uint j=0; j<vcs; j++)
        {
            downstream_credits[i][j] = credits;
            cr_time[i][j] = -1;
        }

    /* init the countes */
    packets = 0;
    flits = 0;
    total_packet_latency = 0;

        IrisEvent* event = new IrisEvent();
        event->type = DETECT_DEADLOCK_EVENT;
        Simulator::Schedule( floor(Simulator::Now())+1, &NetworkComponent::process_event, this, event);

    return ;
}		/* -----  end of function GenericRouter4Stg::init  ----- */

/*! \brief These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
GenericRouter4Stg::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
GenericRouter4Stg::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
GenericRouter4Stg::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
GenericRouter4Stg::process_event ( IrisEvent* e )
{
    switch(e->type)
    {
        case LINK_ARRIVAL_EVENT:
            handle_link_arrival_event(e);
            break;
        case TICK_EVENT:
            handle_tick_event(e);
            break;
        case DETECT_DEADLOCK_EVENT:
            handle_detect_deadlock_event(e);
            break;
        default:
            _DBG("GenericRouter4Stg:: Unk event exception %d", e->type);
            break;
    }
    return ;
}		/* -----  end of function GenericRouter4Stg::process_event  ----- */

void 
GenericRouter4Stg::handle_detect_deadlock_event(IrisEvent* e )
{
    for ( uint i=0; i<ports; i++)
        for ( uint j=0; j<vcs; j++)
        {
            if( cr_time[i][j] != -1)
            {
                cr_time[i][j]++;
            if( cr_time[i][j] > 500 )
                {
                    _DBG_NOARG("************ KILLING SIMULATION *************");
                    cout << "\ndeadlock detected op:"<< i << " ovc: " << j <<  endl;
                    cout << "\ndeadlock mssg:"<< endl;
                    for ( uint ii=0; ii<ports*vcs; ii++)
                        if(input_buffer_state[ii].output_port == i && input_buffer_state[ii].output_channel == j)
                            cout << hex << input_buffer_state[ii].address << endl;
                    cout << "*************************" << endl;

//                    print_state_at_deadlock();
			exit(1);
                }
            }
        }

    e->type = DETECT_DEADLOCK_EVENT;
    Simulator::Schedule( floor(Simulator::Now())+50, &NetworkComponent::process_event, this, e);
}

void 
GenericRouter4Stg::dump_buffer_state()
{
    for ( uint i=0; i<ports*vcs; i++)
        if(input_buffer_state[i].pipe_stage != EMPTY && input_buffer_state[i].pipe_stage != INVALID)
        {
            cout << " Router[" << node_ip << "]->buff["<<i<<"] " << input_buffer_state[i].toString() << endl;
        }
    cout << endl;

    return;
}

string
GenericRouter4Stg::print_stats()
{
    stringstream str;
    str << "\n RouterVcs" 
        << "\n router[" << node_ip << "] packet latency: " << total_packet_latency
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

/*! \brief Event handle for the LINK_ARRIVAL_EVENT event. Entry from DES kernel */
void
GenericRouter4Stg::handle_link_arrival_event ( IrisEvent* e )
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
#ifdef _DEBUG_ROUTER
            if( is_mc_router)
            {
                _DBG("RouterM Msg in in:%d addr:%llx dest:%d src:%d ty:%d",port*vcs+e->vc, hf->addr, hf->dst_address
                     ,hf->src_address,hf->msg_class);
            }
            else
            {
                _DBG("Router Msg in in:%d addr:%llx dest:%d src:%d ty:%d",port*vcs+e->vc, hf->addr, hf->dst_address
                     ,hf->src_address,hf->msg_class);
            }
#endif

            input_buffer_state[port*vcs+data->vc].input_port = port;
            hf->inport = port;
            decoders[port].push(data->ptr,data->vc);
            input_buffer_state[port*vcs+data->vc].input_channel = data->vc;
            input_buffer_state[port*vcs+data->vc].address= hf->addr;
            input_buffer_state[port*vcs+data->vc].destination= hf->dst_address;
            input_buffer_state[port*vcs+data->vc].pipe_stage = FULL;
            input_buffer_state[port*vcs+data->vc].sa_head_done = false;
            input_buffer_state[port*vcs+data->vc].msg_class = hf->msg_class;
            input_buffer_state[port*vcs+data->vc].possible_oports.clear(); 
            input_buffer_state[port*vcs+data->vc].possible_ovcs.clear(); 
            //            input_buffer_state[port*vcs+data->vc].possible_oports.resize(1); 
            //            input_buffer_state[port*vcs+data->vc].possible_ovcs.resize(1); 
            uint no_adaptive_ports = decoders[port].no_adaptive_ports(data->vc);
            uint no_adaptive_vcs = decoders[port].no_adaptive_vcs(data->vc);

            for ( uint i=0; i<no_adaptive_ports; i++ )
            {
                uint rc_port = decoders[port].get_output_port(data->vc);
                if( static_cast<GenericLink*>(output_connections[rc_port])->output_connection != NULL )
                    input_buffer_state[port*vcs+data->vc].possible_oports.push_back(rc_port);
            } 

            for ( uint i=0; i<no_adaptive_vcs; i++ )
            {
                uint rc_vc = decoders[port].get_virtual_channel(data->vc);
                input_buffer_state[port*vcs+data->vc].possible_ovcs.push_back(rc_vc);
            } 

            assert ( input_buffer_state[port*vcs+data->vc].possible_oports.size() > 0);
            assert ( input_buffer_state[port*vcs+data->vc].possible_ovcs.size() > 0);

            input_buffer_state[port*vcs+data->vc].length= hf->length;
            input_buffer_state[port*vcs+data->vc].credits_sent= hf->length;
            input_buffer_state[port*vcs+data->vc].arrival_time= ceil(Simulator::Now());
            input_buffer_state[port*vcs+data->vc].clear_message= false;
            //            input_buffer_state[port*vcs+data->vc].flits_in_ib = 0;

//            _DBG(" New Msg ip:%d ivc:%d pop:%d povc:%d ", input_buffer_state[port*vcs+data->vc].input_port,
//                 input_buffer_state[port*vcs+data->vc].input_channel, input_buffer_state[port*vcs+data->vc].possible_oports.size(),
//                 input_buffer_state[port*vcs+data->vc].possible_ovcs.size());
        }
        else
        {
            decoders[port].push(data->ptr,data->vc);

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
        cr_time[port][data->vc] = -1;

#ifdef _DEBUG_ROUTER
        _DBG(" Got a credit port:%d vc:%d  ", port, data->vc, downstream_credits[port][data->vc]);
        vector < uint > pending_ops ;
        vector < uint > pending_ips ;
        for( uint i=0; i<ports*vcs; i++)
            if(input_buffer_state[i].output_port == port && input_buffer_state[i].output_channel == data->vc)
            {
                pending_ops.push_back(port*vcs+data->vc);
                pending_ips.push_back(i);
            }

        cout << " Can send msgs (i-o): " ;
        for( uint i=0; i<pending_ops.size(); i++)
            cout << pending_ips[i] <<"-" << pending_ops[i] << " occ/" << in_buffers[(int)(pending_ops[i]/vcs)].get_occupancy(pending_ops[i]%vcs)
                <<" bst:"<<input_buffer_state[i].pipe_stage << " ";
        pending_ops.clear();
        pending_ips.clear();
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
}		/* -----  end of function GenericRouter4Stg::handle_link_arrival_event  ----- */

void
GenericRouter4Stg::do_switch_traversal()
{
    /* Switch traversal */
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == ST)
        {
            uint op = input_buffer_state[i].output_port;
            uint oc = input_buffer_state[i].output_channel;
            uint ip = input_buffer_state[i].input_port;
            uint ic= input_buffer_state[i].input_channel;
            if( !xbar.is_empty(op,oc) 
                && in_buffers[ip].get_occupancy(ic)> 0
                && downstream_credits[op][oc]>0 )
            {
                in_buffers[ip].change_pull_channel(ic);
                Flit* f = in_buffers[ip].pull();
                f->vc = oc;
                //                    input_buffer_state[i].flits_in_ib--;
                //                    swa.clear_winner(op, ip);
                xbar.pull(op,oc);
                //                    if(!(f->type == TAIL || f->is_single_flit_pkt)) 
                {
                    swa.request(op, oc, ip, ic);
                    input_buffer_state[i].pipe_stage = SWA_REQUESTED;
                }

                last_flit_out_cycle = Simulator::Now();
                stat_flit_out[ip][op]++;
                static_cast<GenericLink*>(output_connections[op])->flits_passed++;

                LinkArrivalData* data = new LinkArrivalData();
                data->type = FLIT_ID;
                data->vc = oc;
                data->ptr = f;

                if( f->type == HEAD)
                {
#ifdef _DEBUG_ROUTER
                    if( is_mc_router)
                    {
                        _DBG("RouterM Msg out addr %llx %d-%d",static_cast<HeadFlit*>(f)->addr,ip*vcs+ic, op*vcs+oc);
                    }
                    else
                    {
                        _DBG("Router Msg out addr %llx %d-%d",static_cast<HeadFlit*>(f)->addr,ip*vcs+ic, op*vcs+oc);
                    }
#endif
                }

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
                    stat_packet_out[ip][op]++;

                    input_buffer_state[i].clear_message = true;
                    input_buffer_state[i].pipe_stage = EMPTY;
                    vca.clear_winner(op, oc, ip, ic);
                    swa.clear_requestor(op, ip,oc);
                    input_buffer_state[i].possible_oports.clear();
                    input_buffer_state[i].possible_ovcs.clear();
                }

                cr_time[op][oc] = 0;
                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(data);
                event->src_id = address;
                event->dst_id = output_connections[op]->address;
                event->vc = data->vc;

                /*! \brief The do_two_stage_router flag is used here to
                 * model variable router pipeline delay depending on
                 * system clock and router complexity. */
                if(do_two_stage_router)
                    Simulator::Schedule( Simulator::Now()+0.75,
                                         &NetworkComponent::process_event,
                                         static_cast<GenericLink*>(output_connections[op])->output_connection,event);
                else
                    Simulator::Schedule( Simulator::Now()+1.75,
                                         &NetworkComponent::process_event,
                                         static_cast<GenericLink*>(output_connections[op])->output_connection,event);

                /*! \brief Send a credit back and update buffer state for the
                 * downstream router buffer */
                send_credit_back(i);
                downstream_credits[op][oc]--;

            }
            else
            {
                xbar.pull(op,oc);
                input_buffer_state[i].pipe_stage = VCA_COMPLETE;
                swa.clear_requestor(op, ip,oc);
            }
        }

}

void
GenericRouter4Stg::do_switch_allocation()
{
    /* Switch Allocation */
    for( uint i=0; i<ports*vcs; i++)
        if( input_buffer_state[i].pipe_stage == SWA_REQUESTED)
            if ( !swa.is_empty())
            {
                uint op = -1, oc = -1;
                SA_unit sa_winner;
                uint ip = input_buffer_state[i].input_port;
                uint ic = input_buffer_state[i].input_channel;
                op = input_buffer_state[i].output_port;
                oc= input_buffer_state[i].output_channel;
                sa_winner = swa.pick_winner(op);

                if(input_buffer_state[i].sa_head_done)
                {
                    if( sa_winner.port == ip && sa_winner.ch == ic
                        && in_buffers[ip].get_occupancy(ic) > 0
                        && downstream_credits[op][oc]>0 )
                    {
                        input_buffer_state[i].pipe_stage = ST;
                        xbar.configure_crossbar(ip,op,oc);
                        xbar.push(op,oc); 
                        /* TODO: Remove oc from the crossbar. phy crossbar (pxp) */
                        ticking = true;
                    }
                    else
                    {
                        input_buffer_state[i].pipe_stage = VCA_COMPLETE;
                        swa.clear_requestor(op, ip,oc);
                    }
                }
                else
                {
                    if( sa_winner.port == ip && sa_winner.ch == ic
                        //                        && in_buffers[ip].get_occupancy(ic) > 0
                        && downstream_credits[op][oc]==credits )
                    {
                        input_buffer_state[i].sa_head_done = true;
                        input_buffer_state[i].pipe_stage = ST;
                        xbar.configure_crossbar(ip,op,oc);
                        xbar.push(op,oc); 
                        /* TODO: Remove oc from the crossbar. phy crossbar (pxp) */
                        ticking = true;
                    }
                    else
                    {
                        input_buffer_state[i].pipe_stage = VCA_COMPLETE;
                        swa.clear_requestor(op, ip,oc);
                    }

                }

            }
}

/*! \brief Event handle for the TICK_EVENT. Entry from DES kernel */
void
GenericRouter4Stg::handle_tick_event ( IrisEvent* e )
{
    ticking = false;

    do_switch_allocation();
    do_switch_traversal();

    if(!vca.is_empty())
    {
        vca.pick_winner();
        /* 
           _DBG_NOARG(" VCA winners: " );
           for ( uint ai=0; ai<vca.current_winners.size(); ai++)
           if ( vca.current_winners[ai] != -1)
           cout << vca.current_winners[ai] << "-" << ai<< "\t";
         * */
    }

    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == VCA_REQUESTED)
        {
            uint ic = input_buffer_state[i].input_channel;
            uint ip = input_buffer_state[i].input_port;
            vector<uint> pos_win;
            /* 
               vector<uint> winners;
               for ( uint ai=0; ai<vca.current_winners.size(); ai++)
               winners.push_back(vca.current_winners[ai]);

               uint winnerin = -1;
               vector<uint>::iterator itr;
               itr = find(winners.begin(), winners.end(),ip*vcs + ivc);
               while (itr != winners.end())
               {
               winnerin = *itr;
               pos_win.push_back(itr - winners.begin());
               winners.erase(itr);
               itr = find(winners.begin(), winners.end(),ip*vcs + ivc);
               }
               winners.clear();        
               cout <<"\n winners for this inport: " << pos_win.size();
               for ( uint ai=0; ai<pos_win.size(); ai++)
               cout << "\t" << pos_win[ai];
             * */

            uint max_cr = 0;
            uint pref_op = -1;
            uint pref_oc = -1;
            for ( uint ai=0; ai<vca.current_winners.size(); ai++)
                if ( vca.current_winners[ai] == ip*vcs+ic)
                {
                    pos_win.push_back(ai);
                    uint a = (int)(ai/vcs);
                    uint b = ai%vcs;
                    if ( max_cr < downstream_credits[a][b])
                    {
                        pref_op = a;
                        pref_oc = b;
                        max_cr = downstream_credits[a][b];
                    }
                    //Can break here and use the first one found
                }

            if( pos_win.size() > 0 )
            {
                if(pref_op == -1)
                {
                    uint pos = pos_win[0]; 
                    pref_op = (int)(pos/vcs);
                    pref_oc = pos%vcs;
                }

                input_buffer_state[i].output_port = pref_op;
                input_buffer_state[i].output_channel= pref_oc;

                vca.squash_requests(pref_op,pref_oc,ip,ic);

                input_buffer_state[i].pipe_stage = VCA_COMPLETE;
            }
        }

    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == VCA_COMPLETE)
        {
            uint ip = input_buffer_state[i].input_port;
            uint ic = input_buffer_state[i].input_channel;
            uint op = input_buffer_state[i].output_port;
            uint oc = input_buffer_state[i].output_channel;

            //            if(!swa.is_requested(input_buffer_state[i].output_port,
            //                                 input_buffer_state[i].input_port,
            //                                 input_buffer_state[i].output_channel)
            if(in_buffers[ip].get_occupancy(ic))
            {
                swa.request(op, oc, ip, ic);
                input_buffer_state[i].pipe_stage = SWA_REQUESTED;
                ticking = true;
            }
        }

    /*  Input buffering 
     *  Flits are pushed into the input buffer in the link arrival handler
     *  itself. To ensure the pipeline stages are executed in reverse pipe
     *  order IB is done here and all link_traversals have higher priority and get done before
     *  tick. Head/Body and Tail flits go thru the IB stage.*/
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == FULL )
        {
            input_buffer_state[i].pipe_stage = IB;
            //            input_buffer_state[i].flits_in_ib++;
            ticking = true;
        }

    /*! \brief Body and tail flits get written in link arrival and since the message
     * state may already been pushed to ST because of the header we want to
     * ensure that all flits go thru an IB and ST stage. Hence ST is done on
     * the flits_in_ib information and not buffer occupancy. 
     for( uint i=0; i<(ports*vcs); i++)
     {
     uint ip = input_buffer_state[i].input_port;
     uint ic = input_buffer_state[i].input_channel;

     if ((input_buffer_state[i].pipe_stage == ST || input_buffer_state[i].pipe_stage == IB )
     && (input_buffer_state[i].flits_in_ib < in_buffers[ip].get_occupancy(ivc)))
     { 
     input_buffer_state[i].flits_in_ib++;
     ticking = true;
     }
     }
     * */

    /* Request VCA */
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == IB )
        {
            uint ip = input_buffer_state[i].input_port;
            uint ic = input_buffer_state[i].input_channel;
            uint op = -1;
            uint oc = -1;
            for ( uint aa=0; aa<input_buffer_state[i].possible_oports.size();aa++)
                for ( uint aa=0; aa<input_buffer_state[i].possible_oports.size();aa++)
                    for ( uint ab=0; ab<input_buffer_state[i].possible_ovcs.size();ab++)
                    {
                        op = input_buffer_state[i].possible_oports[aa];
                        oc = input_buffer_state[i].possible_ovcs[ab];
                        if( downstream_credits[op][oc] == credits
                            && !vca.is_requested(op, oc, ip, ic) && vca.request(op, oc , ip, ic))
                        {
                            input_buffer_state[i].pipe_stage = VCA_REQUESTED;
                            ticking = true;
                        }
                    }
        }

    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == ST )
            ticking = true;
    /* 
     * */

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

}		/* -----  end of function GenericRouter4Stg::handle_input_arbitration_event  ----- */

string
GenericRouter4Stg::toString() const
{
    stringstream str;
    str << "GenericRouter4Stg"
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
GenericRouter4Stg::send_credit_back(uint i)
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

    //    if( input_buffer_state[i].pipe_stage == EMPTY)
    //        cout << " Sent cr back to i" << port*vcs+data->vc << " was tail" ;
    //    else
    //        cout << " Sent cr back to i" << port*vcs+data->vc ;
    /* 
       }
       else
       {
       _DBG(" Requested to send extra Credit back for inport %d ",
       input_buffer_state[i].input_port);
       }
     * */
}

#endif   /* ----- #ifndef _genericRouter4Stg_cc_INC  ----- */

