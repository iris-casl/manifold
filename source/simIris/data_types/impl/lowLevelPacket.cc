/*
 * =====================================================================================
 *
 *       Filename:  LowLevelPacket.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:56:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _lowlevelpacket_cc_INC
#define  _lowlevelpacket_cc_INC

#include	"lowLevelPacket.h"

using namespace std;

/* LowLevelPacket Class Impl */
LowLevelPacket::LowLevelPacket()
{
    source = -1;
    destination = -1;
    transaction_id = -1;
    virtual_channel = 0;
    sent_time = 0;
    length = 0;

}

LowLevelPacket::~LowLevelPacket()
{
    flits.clear();
    control_bits.clear();
    payload.clear();
}		/* -----  end of function LowLevelPacket::~LowLevelPacket()  ----- */

void LowLevelPacket::operator=( const LowLevelPacket* p )
{
    source = p->source;
    destination = p->destination;
    transaction_id = p->transaction_id;
    pkt_cnt = p->pkt_cnt;
    virtual_channel = p->virtual_channel;
    sent_time = p->sent_time;
    msg_class = p->msg_class;
    length = p->length;
    for ( uint i=0; i<control_bits.size(); i++)
        this->control_bits.push_back(p->control_bits[i]); 

    for ( uint i=0; i<payload.size(); i++)
        this->payload.push_back(p->payload[i]); 
}

string
LowLevelPacket::toString () const
{
    stringstream str;
    str << "LowLevelPacket: " << endl;
    str << "\t Source: " << source 
        << "\t Destination: " << destination
        << "\t virtual_channel: " << virtual_channel 
        << "\t sent time: " << sent_time
        << "\t No of flits: " << flits.size()
        << "\t length: " << length
        << "\t msg_class: " << msg_class
        << "\t addr: " << hex << addr << dec
        << "\t hop_count: " << hop_count
        << "\t network_latency: " << avg_network_latency
		<< "\t PacketCount: " << dec << pkt_cnt
	<< "\t size: " << static_cast<HeadFlit*>(flits[0])->payload.size() 
        << endl;
    for ( uint i=0 ; i<flits.size() ; i++ )
        if( flits[i]->type == HEAD)
            str << static_cast<HeadFlit*>(flits[i])->toString();
        else if( flits[i]->type == BODY)
            str << static_cast<BodyFlit*>(flits[i])->toString();
        else if( flits[i]->type == TAIL)
            str << static_cast<TailFlit*>(flits[i])->toString();
        else
            str << " Error unk flit type" ;
/* 
 * */
    return str.str();
}		/* -----  end of function LoeLevelPacket::toString  ----- */

bool
LowLevelPacket::valid_packet ()
{
    if( flits.size())
        return (flits.size() == static_cast<HeadFlit*>(flits[0])->length);
    else
        return false;

}		/* -----  end of function LowLevelPacket::valid_packet  ----- */

void
LowLevelPacket::add ( Flit* ptr )
{
    switch (ptr->type)
    {
        case HEAD:
            {
                HeadFlit* hf = static_cast < HeadFlit*> (ptr);
//                flits.push_back(hf);
                this->source = hf->src_address;
                this->destination = hf->dst_address;
                this->transaction_id = hf->transaction_id;
                this->virtual_channel = hf->vc;
                this->length = hf->length;
                this->msg_class = hf->msg_class;
                this->sent_time = hf->packet_originated_time;
                this->addr = hf->addr;
                this->avg_network_latency = hf->avg_network_latency;
                this->hop_count = hf->hop_count;
                this->stat_memory_serviced_time = hf->stat_memory_serviced_time;
                this->req_start_time = hf->req_start_time;
                this->waiting_in_ni = hf->waiting_in_ni;
		this->pkt_cnt = hf->pkt_cnt;

    for ( uint i=0; i<hf->control_bits.size(); i++)
        this->control_bits.push_back(hf->control_bits[i]); 

    for ( uint i=0; i<hf->payload.size(); i++)
        this->payload.push_back(hf->payload[i]); 

                delete hf;
                break;
            }
        case BODY:
            {
                BodyFlit* bf = static_cast<BodyFlit*> (ptr);
    for ( uint i=0; i<bf->bf_data.size(); i++)
        this->payload.push_back(bf->bf_data[i]); 
//                flits.push_back(bf);
                delete bf;
                break;
            }
        case TAIL:
            {
                TailFlit* tf = static_cast<TailFlit*> (ptr);
//                flits.push_back(tf);
                this->sent_time = tf->packet_originated_time;
                this->avg_network_latency = tf->avg_network_latency - this->avg_network_latency;
                delete tf;
                break;
            }
        default:
            fprintf(stdout,"ERROR Unk flit type");
            break;
    }
    return ;
}		/* -----  end of function LowLevelPacket::add  ----- */

Flit* LowLevelPacket::at(unsigned int index)
{
    if(index > flits.size())
        fprintf(stdout," Error Index out of bounds in lowlevelpacket");

    return flits.at(index);
}

Flit* LowLevelPacket::get_next_flit()
{
    Flit *ptr;
    ptr = flits.front();
    flits.pop_front();
    return ptr;
}

void
LowLevelPacket::clear ()
{
    flits.clear();
    return;
}		/* -----  end of function LowLevelPacket::clear  ----- */

uint
LowLevelPacket::size ()
{
    return flits.size();
}		/* -----  end of function LowLevelPacket::size  ----- */

#endif   /* ----- #ifndef _lowlevelpacket_cc_INC  ----- */

