/*!
 * =====================================================================================
 *
 *       Filename: \file  routerReqReply.cc
 *       Description: Implements a router that can use req and reply virtual
 *       networks.
 *       1. Flit level flow control
 *       2. virtual channels handle the msg classes
 *       3. DOR 
 *
 *        Version:  1.0
 *        Created:  03/11/2010 09:20:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _routerReqReply_cc_INC
#define  _routerReqReply_cc_INC

#include	"routerReqReply.h"
using namespace std;

RouterReqReply::RouterReqReply ()
{
    name = "RouterReqReply" ;
    ticking = false;
}  /* -----  end of method RouterReqReply::RouterReqReply  (constructor)  ----- */

RouterReqReply::~RouterReqReply()
{
}

void
RouterReqReply::init (uint p, uint v, uint cr, uint bs)
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
    sw_alloc.resize(ports);
    downstream_credits.resize(ports);
    cr_time.resize(ports);
    request_op.resize(ports);
    vca.set_no_msg_classes(no_msg_classes);
    vca.init(ports, vcs );

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
            input_buffer_state[i*vcs+j].pipe_stage = EMPTY;
        }

    /* init the countes */
    stat_packets = 0;
    stat_flits = 0;
    stat_total_packet_latency = 0;
    stat_buffer_occupancy = 0;
    stat_swa_fail_msg_ratio = 0;
    stat_swa_load = 0;
    stat_vca_fail_msg_ratio = 0;
    stat_vca_load = 0;
    stat_ib_cycles = 0;
    stat_rc_cycles = 0;
    stat_vca_cycles = 0;
    stat_swa_cycles = 0;
    stat_st_cycles = 0;

    /* 
    std::ostringstream buffer;
    buffer << "debug/router_";
    buffer << node_ip;
    string debug_log_string= buffer.str();
    debug_log.open(debug_log_string.c_str());

     * */
    IrisEvent* event = new IrisEvent();
    event->type = DETECT_DEADLOCK_EVENT;
    Simulator::Schedule( floor(Simulator::Now())+1, &NetworkComponent::process_event, this, event);

    return ;
}		/* -----  end of function RouterReqReply::init  ----- */

/*! \brief These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
RouterReqReply::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
RouterReqReply::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
RouterReqReply::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
RouterReqReply::process_event ( IrisEvent* e )
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
            _DBG("RouterReqReply:: Unk event exception %d", e->type);
            break;
    }
    return ;
}		/* -----  end of function RouterReqReply::process_event  ----- */

void 
RouterReqReply::handle_detect_deadlock_event(IrisEvent* e )
{
    for ( uint i=0; i<ports; i++)
        for ( uint j=0; j<vcs; j++)
        {
            if( cr_time[i][j] != -1)
            {
                cr_time[i][j]++;
                if( cr_time[i][j] > 10000 )
                {
                    _DBG_NOARG("************ KILLING SIMULATION *************");
                    cout << "\ndeadlock detected op:"<< i << " ovc: " << j <<  endl;
                    cout << "\ndeadlock mssg:"<< endl;
                    cout << "ip\tic\top\toc\taddr:"<< endl;
                    for ( uint ii=0; ii<ports*vcs; ii++)
                        if(input_buffer_state[ii].output_port == i && input_buffer_state[ii].output_channel == j)
                        {
                            cout << dec << input_buffer_state[ii].input_port;
                            cout << dec << "\t" << input_buffer_state[ii].input_channel;
                            cout << dec << "\t" << input_buffer_state[ii].output_port;
                            cout << dec << "\t" << input_buffer_state[ii].output_channel;
                            cout << hex << "\t" << input_buffer_state[ii].address << endl;
                        }
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
RouterReqReply::dump_buffer_state()
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
RouterReqReply::print_stats()
{
    stringstream str;
    str << name ;
    str << "\n router[" << node_ip << "] packet latency: " << stat_total_packet_latency
        << "\n router[" << node_ip << "] flits/packet: " << (stat_flits+0.0)/(stat_packets)
        << "\n router[" << node_ip << "] average packet latency: " << (stat_total_packet_latency+0.0)/stat_packets
        << "\n router[" << node_ip << "] last_flit_out_cycle: " << last_flit_out_cycle
        << "\n router[" << node_ip << "] stat_buffer_occupancy: " << stat_buffer_occupancy 
        << "\n router[" << node_ip << "] stat_swa_fail_msg_ratio: " << stat_swa_fail_msg_ratio
        << "\n router[" << node_ip << "] stat_swa_load: " << stat_swa_load
        << "\n router[" << node_ip << "] stat_vca_fail_msg_ratio: " << stat_vca_fail_msg_ratio
        << "\n router[" << node_ip << "] stat_vca_load: " << stat_vca_load
        << "\n router[" << node_ip << "] packets: " << stat_packets
        << "\n router[" << node_ip << "] flits: " << stat_flits
        << "\n router[" << node_ip << "] stat_ib_cycles: " << stat_ib_cycles
        << "\n router[" << node_ip << "] stat_rc_cycles: " << stat_rc_cycles
        << "\n router[" << node_ip << "] stat_vca_cycles: " << stat_vca_cycles
        << "\n router[" << node_ip << "] stat_swa_cycles: " << stat_swa_cycles
        << "\n router[" << node_ip << "] stat_st_cycles: " << stat_st_cycles
        << " ";

    str << "\n router[" << node_ip << "] out_port_links_utilization: ";
    for( uint i=0; i<ports; i++)
        str << static_cast<GenericLink*>(output_connections[i])->flits_passed*1.0/max_sim_time << " ";
    str << endl;

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
                if ( i != j) 
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
RouterReqReply::handle_link_arrival_event ( IrisEvent* e )
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
    if(data->type == FLIT_ID)
    {

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

        /* counting ib for body/teail can be complicated as the msg state
         * could have progressed to some other stage. Hence counting it at
         * link arrival makes it easy. */
        if(data->ptr->type !=HEAD)
        {
            istat->stat_router[node_ip]->ib_cycles++;
            stat_ib_cycles++;
        }

       // _DBG(" handle_link_arrival_event: port %d vc %d ft %d ps:%d ", port, data->vc, data->ptr->type, input_buffer_state[port].pipe_stage);
        if(data->ptr->type == HEAD)
        {
            cout	<< static_cast<HeadFlit*>(data->ptr)->addr<< endl;
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
}		/* -----  end of function RouterReqReply::handle_link_arrival_event  ----- */

void
RouterReqReply::do_input_buffering()
{
    for( uint i=0; i<ports*vcs; i++)
    {
        uint inport = (uint)(i/vcs);
        uint invc = (uint)(i%vcs);
        if( input_buffer_state[i].pipe_stage == FULL || input_buffer_state[i].pipe_stage == EMPTY )
        {
            if( in_buffers[inport].get_occupancy(invc) > 0)
            {

                in_buffers[inport].change_pull_channel(invc);
                Flit* f = in_buffers[inport].peek();
                //                _DBG(" do_input_buffering %d ft%d",inport*vcs+invc, f->type);

                if( f->type == HEAD )
                {
                    HeadFlit* hf = static_cast<HeadFlit*>(f);
                    /*  Update stats for head */
                    istat->stat_router[node_ip]->ib_cycles++;
                    stat_ib_cycles++;
                    //_DBG(" HEAD IN %d %lld",f->type, hf->addr);

                    input_buffer_state[inport*vcs+f->vc].input_port = inport;
                    hf->inport = inport;
                    decoders[inport].push(f,f->vc);
                    input_buffer_state[inport*vcs+f->vc].input_channel = f->vc;
                    input_buffer_state[inport*vcs+f->vc].address= hf->addr;
                    input_buffer_state[inport*vcs+f->vc].destination= hf->dst_address;
                    input_buffer_state[inport*vcs+f->vc].pipe_stage = IB;
                    input_buffer_state[inport*vcs+f->vc].msg_class = hf->msg_class;
                    input_buffer_state[inport*vcs+f->vc].possible_oports.clear(); 
                    input_buffer_state[inport*vcs+f->vc].possible_ovcs.clear(); 
                    uint no_adaptive_ports = decoders[inport].no_adaptive_ports(f->vc);
                    uint no_adaptive_vcs = decoders[inport].no_adaptive_vcs(f->vc);
                    istat->stat_router[node_ip]->rc_cycles++;
                    stat_rc_cycles++;

                    for ( uint i=0; i<no_adaptive_ports; i++ )
                    {
                        uint rc_port = decoders[inport].get_output_port(f->vc);
                        if( static_cast<GenericLink*>(output_connections[rc_port])->output_connection != NULL )
                            input_buffer_state[inport*vcs+f->vc].possible_oports.push_back(rc_port);
                    } 

                    for ( uint i=0; i<no_adaptive_vcs; i++ )
                    {
                        uint rc_vc = decoders[inport].get_virtual_channel(f->vc);
                        input_buffer_state[inport*vcs+f->vc].possible_ovcs.push_back(rc_vc);
                    } 

                    assert ( input_buffer_state[inport*vcs+f->vc].possible_oports.size() > 0);
                    assert ( input_buffer_state[inport*vcs+f->vc].possible_ovcs.size() > 0);

                    input_buffer_state[inport*vcs+f->vc].length= hf->length;
                    input_buffer_state[inport*vcs+f->vc].credits_sent= hf->length;
                    input_buffer_state[inport*vcs+f->vc].arrival_time= Simulator::Now();
                    input_buffer_state[inport*vcs+f->vc].clear_message= false;
                }
                /* Dont need this only heads go thru RC
                   else
                   {
                // Body flits are pushed thru the decoder but
                // they dont do anything. Tail flits clear the state
                decoders[inport].push(f,f->vc);
                input_buffer_state[inport*vcs+f->vc].flits_in_ib++;
                cout  << "came here " << endl;

                }
                 * */
                ticking =true;
            }
        }
    }

    /* Body and tail flits get written in link arrival and since the message
     * state may already been pushed to ST because of the header we want to
     * ensure that all flits go thru an IB and ST stage. Hence ST is done on
     * the flits_in_ib information and not buffer occupancy. 
     for( uint i=0; i<(ports*vcs); i++)
     {
     uint ip = input_buffer_state[i].input_port;
     uint ic = input_buffer_state[i].input_channel;

     if (input_buffer_state[i].pipe_stage == VCA_COMPLETE || input_buffer_state[i].pipe_stage == SWA_REQUESTED 
     || input_buffer_state[i].pipe_stage == SW_TRAVERSAL)
     {
     in_buffers[ip].change_pull_channel(ic);
     Flit* f = in_buffers[ip].peek();
     if( f->type != HEAD && (input_buffer_state[i].flits_in_ib < in_buffers[ip].get_occupancy(ic)))
     {
     input_buffer_state[i].flits_in_ib++;
     ticking=true;
     }
     }

     }
     * */
}

void
RouterReqReply::do_switch_traversal()
{
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == SW_TRAVERSAL)
        {
            uint op = input_buffer_state[i].output_port;
            uint oc = input_buffer_state[i].output_channel;
            uint ip = input_buffer_state[i].input_port;
            uint ic= input_buffer_state[i].input_channel;
            if( in_buffers[ip].get_occupancy(ic)> 0
                && downstream_credits[op][oc]>0 )
            {
                in_buffers[ip].change_pull_channel(ic);
                Flit* f = in_buffers[ip].pull();
                f->vc = oc;

                if(f->type != TAIL && !f->is_single_flit_pkt)
                {
                    cout << " resetting ";
                    input_buffer_state[i].pipe_stage = VCA_COMPLETE;
                    request_switch_allocation();
                    ticking = true;
                }

                /* stats */
                last_flit_out_cycle = Simulator::Now();
                stat_flit_out[ip][op]++;
                stat_flits++;
                static_cast<GenericLink*>(output_connections[op])->flits_passed++;
                istat->stat_link[static_cast<GenericLink*>(output_connections[op])->link_id]->flits_transferred++;

                /* For energy computation: the per access energy reported is
                 * for one flit traversing the crossbar. Unlike arbiter access
                 * energy which is for multiple digraph matching in one
                 * access. This count may also be used as the local link
                 * access count if the ST and LT are considered as the same
                 * stage for very short wire lengths. */
                istat->stat_router[node_ip]->st_cycles++;
                stat_st_cycles++;


                LinkArrivalData* data = new LinkArrivalData();
                data->type = FLIT_ID;
                data->vc = oc;
                data->ptr = f;

               // _DBG(" Fout %d=%d %d",ip*vcs+ic,op*vcs+oc, f->type);

                /*  Updating all pkt stats in the head irrespective of msg
                 *  class. Assuming all pkts do have a head. However latency
                 *  for determining switch throughput is to be determined when
                 *  the tail exits.
                 *  */
                if( f->type == HEAD)
                {
                    HeadFlit* hf = static_cast<HeadFlit*>(f);
                    hf->hop_count++;
                   // _DBG(" Fout %d %d %d %d %d %d addr:%lld ",ip,ic,op,oc,hf->src_address, hf->dst_address, hf->addr);
                    cout << f->is_single_flit_pkt;
                    //                    cout << "  " << hf->addr;
                }
                if( f->type == TAIL || f->is_single_flit_pkt )
                {
                    /* Update packet level stats */
                    uint lat = Simulator::Now() - input_buffer_state[i].arrival_time + 1;
                    stat_total_packet_latency += lat;
                    stat_packets++;

                    if( f->type == HEAD)
                    {
                        HeadFlit* hf = static_cast<HeadFlit*>(f);
                        hf->avg_network_latency += lat;
                    }
                    else
                    {
                        TailFlit* tf = static_cast<TailFlit*>(f);
                        tf->avg_network_latency += lat;
                    }

                    stat_packet_out[ip][op]++;

                    input_buffer_state[i].clear_message = true;
                    if(in_buffers[ip].get_occupancy(ic) > 0)
                        input_buffer_state[i].pipe_stage = FULL;
                    else
                        input_buffer_state[i].pipe_stage = EMPTY;

                    vca.clear_winner(ip*vcs+ic, oc );
                    input_buffer_state[i].possible_oports.clear();
                    input_buffer_state[i].possible_ovcs.clear();
                    input_buffer_state[i].output_port = -1;
                    input_buffer_state[i].output_channel = -1;
                }

                cr_time[op][oc] = 0;
                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(data);
                event->src_id = address;
                event->dst_id = output_connections[op]->address;
                event->vc = data->vc;

                Simulator::Schedule( Simulator::Now()+0.75,
                                     &NetworkComponent::process_event,
                                     static_cast<GenericLink*>(output_connections[op])->output_connection,event);

                /*! \brief Send a credit back and update buffer state for the
                 * downstream router buffer */
                send_credit_back(i);
                downstream_credits[op][oc]--;

            }
        }
    return;
}

void
RouterReqReply::do_switch_allocation()
{
    bool was_swa_active = false; /* flag to make sure swa accesses are not double counted */
    uint tot_swa_req = 0;
    uint swa_won=0;

    /*  Take the first response as the winner. If no response take the head */
    /* 
       for( uint i=0; i<ports; i++)
       for( uint j=0; j< sw_alloc[i].size();j++ )
       if ( input_buffer_state[sw_alloc[i][j]].msg_class == RESPONSE_PKT )
       {
       winner_op = j;
       break;
       }
     * */

    for( uint i=0; i<ports; i++)
    {
        if(  sw_alloc[i].size() )
        {
            uint winner_op = 0;

            /* Since only one msg can win. counting failure cycles here as a
             * measure of congestion. */
            swa_won++;
            tot_swa_req += sw_alloc[i].size();

            uint msg_id = sw_alloc[i][winner_op];
            uint ip = input_buffer_state[msg_id].input_port;
            uint ic = input_buffer_state[msg_id].input_channel;
            uint op = input_buffer_state[msg_id].output_port;
            uint oc = input_buffer_state[msg_id].output_channel;
            assert(op == i);
            vector<uint>::iterator it = find (sw_alloc[i].begin(), sw_alloc[i].end(), msg_id);;
            if( input_buffer_state[msg_id].pipe_stage == SWA_REQUESTED /*  && downstream_credits[op][oc]*/)
            {
                sw_alloc[i].erase(it);
                input_buffer_state[msg_id].pipe_stage = SW_TRAVERSAL;
                ticking = true;
                was_swa_active = true;
            }
        }
    }
    if( tot_swa_req )
    {
        stat_swa_fail_msg_ratio += ((tot_swa_req - swa_won+0.0)/tot_swa_req);
        stat_swa_fail_msg_ratio = stat_swa_fail_msg_ratio/2;
        stat_swa_load += ((tot_swa_req+0.0)/(ports*vcs));
        stat_swa_load = stat_swa_load/2;
    }

    if( was_swa_active )
    {
        istat->stat_router[node_ip]->sa_cycles++;
        stat_swa_cycles++;
    }

    return;
}

void
RouterReqReply::do_virtual_channel_allocation()
{
    /* flag to count accesses. Multiple winners
     * can be chosen for one access of the arbiter */
    bool was_vca_active = false; 
    uint tot_vca_req = vca.get_no_requestors();

    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == VCA_REQUESTED)
        {
            uint op=input_buffer_state[i].output_port; 
            uint ip=input_buffer_state[i].input_port; 
            uint ic=input_buffer_state[i].input_channel; 
            uint mc=input_buffer_state[i].msg_class; 
            uint at=input_buffer_state[i].arrival_time; 
            uint mid=0;

            if( do_request_reply_network)
                if( mc == ONE_FLIT_REQ || mc == REQUEST_PKT )
                    mid = 1;

            vca.request(op, mid, ip*vcs+ic, at);
//            _DBG(" VCA REQ %d=%d",ip*vcs+ic,op);
        }
    uint vca_won = 0;
    vca_won = vca.pick_winners();

    for( uint i=0; i<no_msg_classes; i++)
        for( uint j=0; j<ports; j++)
        {
            map<uint ,uint>::iterator iter;
            for( iter = vca.winners[i][j].begin(); iter != vca.winners[i][j].end(); ++iter ) 
                {
                    uint msg_index = iter->first;
                    uint msg_oc = iter->second;
                    if( input_buffer_state[msg_index].pipe_stage == VCA_REQUESTED /* && downstream_credits[op][oc] > 0 */) 
                    {
                        uint op = input_buffer_state[msg_index].output_port; 
                        uint ip = input_buffer_state[msg_index].input_port;
                        uint ic = input_buffer_state[msg_index].input_channel;
                        assert( op == j);
                        /* can request only if there are credits improving 
                         * chances of winning for other pkts that can actually 
                         * exit the router */
                        input_buffer_state[msg_index].output_channel = msg_oc;
                        input_buffer_state[msg_index].pipe_stage = VCA_COMPLETE;
                        ticking = true;
                    }
                }
        }

    if( vca_won && tot_vca_req )
    {
        istat->stat_router[node_ip]->vca_cycles++;
        stat_vca_cycles++;
        stat_vca_fail_msg_ratio += ((tot_vca_req - vca_won+0.0)/tot_vca_req);
        stat_vca_fail_msg_ratio = stat_vca_fail_msg_ratio/2;
        stat_vca_load += ((tot_vca_req+0.0)/(ports*vcs));
        stat_vca_load = stat_vca_load/2;
    } 

    request_switch_allocation();
}

/* This is called from two places. First when head completes vca and requests
 * for swa. Second when flit goes out and msg requests for swa again as long
 * as there the tail goes out*/
void
RouterReqReply::request_switch_allocation()
{
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == VCA_COMPLETE)
        {
            uint ip = input_buffer_state[i].input_port;
            uint ic = input_buffer_state[i].input_channel;
            uint op = input_buffer_state[i].output_port;
            uint oc = input_buffer_state[i].output_channel;

            /* Can skip this and speculatively request sa. May end up wasting
             * an st cycle during wrong speculation. */
            if(in_buffers[ip].get_occupancy(ic) )
            {
                //                _DBG(" SWA_REQ %d=%d",ip*vcs+ic,op*vcs+oc);
                sw_alloc[op].push_back(i);
                input_buffer_state[i].pipe_stage = SWA_REQUESTED;
                ticking = true;
            }
        }
    return;
}

/*! \brief Event handle for the TICK_EVENT. Entry from DES kernel */
void
RouterReqReply::handle_tick_event ( IrisEvent* e )
{
    ticking = false;
    do_switch_traversal();
    do_switch_allocation();
    do_virtual_channel_allocation();

    /*!  Input buffering 
     *  Flits are pushed into the input buffer in the link arrival handler
     *  itself. To ensure the pipeline stages are executed in reverse pipe
     *  order IB is done here and all link_traversals have higher priority and get done before
     *  tick. Head/Body and Tail flits go thru the IB stage.*/
    do_input_buffering();

    /*  In case of adaptive routing need some selection function to pick from
     *  available output ports before requesting
     *  */

    /*! Request VCA at the end of IB*/
    for( uint i=0; i<(ports*vcs); i++)
        if( input_buffer_state[i].pipe_stage == IB )
        {
            uint ip = input_buffer_state[i].input_port;
            uint ic = input_buffer_state[i].input_channel;
            uint op = input_buffer_state[i].possible_oports[0];
            //            if( downstream_credits[op][oc]>0)
            {
                input_buffer_state[i].pipe_stage = VCA_REQUESTED;
                input_buffer_state[i].output_port = op;
                //                _DBG(" VCA REQ1 %d=%d(op)",ip*vcs+ic,op);
                ticking = true;
            }
        }

    /*Per cycle router stats */
    for( uint i=0; i<ports; i++)
        for( uint j=0; j<vcs; j++)
            if(in_buffers[i].get_occupancy(j) )
            {
                double buff_occ =(in_buffers[i].get_occupancy(j)+0.0)/credits;
                stat_buffer_occupancy += buff_occ;
                stat_buffer_occupancy = stat_buffer_occupancy/2;
            }


    /*! The router can be set to not tick and therefore speed up simulation if
     * there are no messages to route. To be replaced by the start and stop
     * clock functionality in the next release of the kernel. Under high load
     * you also register the component to a clock so that it will be called
     * every cycle and hence an event need not be explicitly generated.
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

}		/* -----  end of function RouterReqReply::handle_input_arbitration_event  ----- */

string
RouterReqReply::toString() const
{
    stringstream str;
    str << "RouterReqReply"
        << "\t addr: " << address
        << " node_ip: " << node_ip
        << "\n Input buffers: " << in_buffers.size() << " ";
    if( in_buffers.size() > 0)
        str << in_buffers[0].toString();

    str << "\n decoders: " << decoders.size() << " ";
    if( decoders.size() > 0)
        str << decoders[0].toString();

    return str.str();
}

void
RouterReqReply::send_credit_back(uint i)
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

    static_cast<GenericLink*>(input_connections[port])->credits_passed++;
    istat->stat_link[static_cast<GenericLink*>(input_connections[port])->link_id]->credits_transferred++;
}

#endif   /* ----- #ifndef _routerReqReply_cc_INC  ----- */

