/*
 * =====================================================================================
 *
 *       Filename:  HighLevelPacket.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:22:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _highlevelpacket_cc_INC
#define  _highlevelpacket_cc_INC

#include	"highLevelPacket.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  HighLevelPacket
 *      Method:  HighLevelPacket
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
HighLevelPacket::HighLevelPacket ()
{
    avg_network_latency = 0;
    hop_count = 0;
    source = -1;
    destination = -1;
    pkt_cnt=0;
    reqs_left=-1;
    startIndex=0;
    vn = VN0;
    msg_class = INVALID_PKT;
    transaction_id = 0;
    virtual_channel = 0;
    data_payload_length = 0;
    sent_time = (ullint)Simulator::Now();
    recv_time = 0;
    req_start_time = 0;
    waiting_in_ni = 0;
    stat_memory_serviced_time = 0;
}  /* -----  end of method HighLevelPacket::HighLevelPacket  (constructor)  ----- */

HighLevelPacket::~HighLevelPacket()
{
    data.clear();
}

string HighLevelPacket::toString() const
{
    stringstream str;
    str << " Creation time: " << sent_time 
        << " Source: " << source 
        << " Dest: " << destination 
        << " transaction_id: " << transaction_id
        << " VC: " << virtual_channel
        << " msg_class: " << msg_class
        << " hop_count: " << hop_count
        << " avg_network_latency: " << avg_network_latency
        << " data_size: " << data.size()
        << " data_payload_length (bits): " << data_payload_length
        << " addr: " << hex << addr << dec
        << "\t";
    return str.str();
}  /* -----  end of method HighLevelPacket::toString::HighLevelPacket::toString  (constructor)  ----- */

simTime HighLevelPacket::get_transit_time()
{
    return recv_time - sent_time;
}

bool HighLevelPacket::operator==( const HighLevelPacket* p )
{
    if( transaction_id == p->transaction_id &&
        source == p->source)
        return true;
    else
        return false;
}

/* This function instantiates a low level packet. One needs to explicitly
 * destroy it if used */
void
HighLevelPacket::to_low_level_packet(LowLevelPacket* pkt)
{
    pkt->source = source;
    pkt->destination = destination;
    pkt->transaction_id = transaction_id;
    pkt->sent_time = sent_time;
    pkt->msg_class = msg_class;
    pkt->pkt_cnt = pkt_cnt;
    pkt->addr = addr;
    pkt->hop_count = hop_count;
    pkt->avg_network_latency = avg_network_latency;
    pkt->stat_memory_serviced_time = stat_memory_serviced_time;
    pkt->req_start_time = req_start_time;
    HeadFlit *hf = new HeadFlit();
    if( msg_class == REQUEST_PKT)
    {
        pkt->length = 2;
    }
    else if( msg_class == ONE_FLIT_REQ || msg_class == PRIORITY_REQ)
    {
        pkt->length = 1;
        hf->is_single_flit_pkt = true;
    }
    else
    {
        pkt->length = (uint)(((this->data_payload_length+0.0)/max_phy_link_bits) + 2); // data_payload_length is the number of bits of data;
        hf->is_single_flit_pkt = false;
    }
    pkt->virtual_channel = virtual_channel;

    hf->control_bits.resize(CONTROL_VECTOR_SIZE);
    pkt->control_bits.resize(CONTROL_VECTOR_SIZE);

    /*  Create the mask based on message class and vc and virtual network */
    uint vc_mask = virtual_channel & 0x0f;
    uint vn_mask = (vn & 0x0f) << 4;
    uint mc_mask = (msg_class & 0x0f) << 8;
    uint mask = vc_mask | vn_mask | mc_mask;
    for ( uint i=0 ; i< pkt->control_bits.size() ;i++ )
    {
        pkt->control_bits[i] = ( mask >> i ) & 0x01;
        hf->control_bits[i] = ( mask >> i ) & 0x01;
    }

    if( msg_class == PRIORITY_REQ || msg_class == ONE_FLIT_REQ || msg_class == REQUEST_PKT)
        for( uint i=0; i<this->data.size(); i++)
        {
            hf->payload.push_back(this->data[i]);
        }

    hf->packet_originated_time = pkt->sent_time;
    hf->addr = addr;
    hf->src_address = source;
    hf->dst_address = destination;
    hf->transaction_id = transaction_id;
    hf->pkt_cnt = pkt->pkt_cnt;
    hf->length = pkt->length;
    hf->vc = virtual_channel;
    hf->msg_class = msg_class;
    hf->populate_head_flit();
    hf->avg_network_latency = avg_network_latency;
    hf->hop_count = hop_count;
    hf->stat_memory_serviced_time = stat_memory_serviced_time;
    hf->req_start_time = req_start_time;
    hf->waiting_in_ni = waiting_in_ni; 
    pkt->flits.push_back(hf);

    if( msg_class == RESPONSE_PKT || msg_class == WRITE_REQ)
    {
    vector< bool> temp;
    uint no_of_body_flits = (uint)( ceil(this->data.size()*1.0/max_phy_link_bits));
    for ( uint i=0 ; i<no_of_body_flits ; i++ )
    {
        BodyFlit* bf = new BodyFlit();
        for ( uint j=0 ; j<max_phy_link_bits && (i*max_phy_link_bits + j)< data.size() ; j++ )
            bf->bf_data.push_back(this->data[i*max_phy_link_bits + j]);

        bf->populate_body_flit();
        pkt->flits.push_back(bf);
        temp.clear();
    }
    }

    if( msg_class == REQUEST_PKT || msg_class == RESPONSE_PKT || msg_class == WRITE_REQ)
    {
        TailFlit* tf = new TailFlit();
        tf->packet_originated_time = pkt->sent_time;
        tf->populate_tail_flit();
        pkt->flits.push_back(tf);
    }

    /* Compute the number of head flits required based on source addr dest
     * addr, transaction id and control bits and phit size */

    return ;
}

void
HighLevelPacket::from_low_level_packet ( LowLevelPacket* llp )
{
    source = llp->source;
    destination = llp->destination;
    transaction_id = llp->transaction_id;
    pkt_cnt = llp->pkt_cnt ;
    data_payload_length = llp->length;
    sent_time = llp->sent_time;
    virtual_channel = llp->virtual_channel;
    msg_class = llp->msg_class;
    addr = llp->addr;
    avg_network_latency = llp->avg_network_latency;
    hop_count = llp->hop_count;
    stat_memory_serviced_time = llp->stat_memory_serviced_time;
    req_start_time = llp->req_start_time;
    waiting_in_ni = llp->waiting_in_ni;

    for( uint i=0; i<llp->payload.size(); i++)
        data.push_back( llp->payload[i]);

    uint mask = 0;
    for (uint i=0; i<llp->control_bits.size(); i++)
        mask = (mask <<i)|(llp->control_bits[i] & 0x01);

    /*  Need to check this mask propagation: The VC aint right */
    //virtual_channel = (mask & 0x00c0)>>6;
    switch ((mask & 0x00f0) >> 4)
    {
        case 0:
            vn = VN0;
            break;
        case 1:
            vn = VN1;
            break;
        default:
            vn = VN2;
            break;
    }

    llp->flits.clear();
    llp->control_bits.clear();
    llp->payload.clear();
    return ;
}		/* -----  end of function HighLevelPacket::from_low_level_packet  ----- */

#endif   /* ----- #ifndef _highlevelpacket_cc_INC  ----- */

