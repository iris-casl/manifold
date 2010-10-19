/*
 * =====================================================================================
 *
 *       Filename:  flit.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/22/2010 03:08:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _flit_cc_INC
#define  _flit_cc_INC

#include	"flit.h"
/*
 *--------------------------------------------------------------------------------------
 *       Class:  Phit
 *      Method:  Phit
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Phit::Phit ()
{
}  /* -----  end of method Phit::Phit  (constructor)  ----- */

Phit::~Phit()
{
    data.clear();
}

string
Phit::toString () const
{
    stringstream str;
    str << "Phit: ";
    for( int i=data.size() ; i>0 ;i--)
        str << data[i-1];

    str << endl;
    return str.str();
}		/* -----  end of function Phit::toString  ----- */

/* Flit Class Impl */
Flit::Flit()
{
    is_single_flit_pkt = false;

}

void Flit::populate_phit_data(vector<bool>* data)
{
    uint no_of_phits = (uint)(ceil((data->size() + 0.0)/max_phy_link_bits));
//    phits.resize(no_of_phits);
    for ( uint i=0; i<no_of_phits; i++)
        phits.push_back( new Phit());

    for( uint i=0; i< no_of_phits; i++)
    {
        for ( uint j=0;j<max_phy_link_bits && (i*max_phy_link_bits + j)<data->size(); j++ )
            phits[i]->data.push_back(data->at(i*max_phy_link_bits + j));
    }
}

Flit::~Flit()
{
    /* 
    for( uint i=0; i<phits.size(); i++)
        delete(phits.at(i));
     * */
}

string Flit::toString() const
{
    stringstream str;
    str << "Flit" 
        << "\tFlit type: " << type
        << endl;

    for ( uint i=0 ; i<phits.size() ; i++ )
        str << phits[i]->toString();

    return str.str();
}

/* HeadFlit Class Impl */
HeadFlit::HeadFlit()
{
    type = HEAD;
    src_address = -1;
    dst_address = -1;
    transaction_id = -1;
    pkt_cnt = 0;
    is_dateline_traversed = false;
}

HeadFlit::~HeadFlit()
{
//    phits.clear();
    for( uint i=0; i<phits.size(); i++)
        delete(phits.at(i));

    control_bits.clear();
    payload.clear();
}

string 
HeadFlit::toString() const
{
    stringstream str;
    str << "HeadFlit"
        << "\tSrc: " << src_address/3
        << "\tDst: " << dst_address
        << "\tTran_id: " << transaction_id
        << "\t VC: " << vc
        << "\t length: " << length
        << "\t control_bits_size: " << control_bits.size()
        << "\t payload_size: " << payload.size()
        << "\thop_count : " << hop_count 
        << "\tavg_net_latency : " << avg_network_latency
        << "\t msg_class: " << msg_class
	<< "\t packetcount: " << pkt_cnt
	<< "\t addr: " << hex << addr << dec
        << endl;

    str << "Control bits: " ;
    for ( uint i=0 ; i<control_bits.size() ; i++ )
        str << control_bits[i] << " ";
    str << endl;

    for ( uint i=0 ; i<phits.size() ; i++ )
        str << phits[i]->toString();

    return str.str();
}

void
HeadFlit::populate_head_flit()
{
    vector<bool> data;
    for ( uint i=0; i<max_network_node_bits ; i++ )
        data.push_back((src_address >> i)&0x01);

    for ( uint i=0; i<max_network_node_bits ; i++ )
        data.push_back((dst_address >> i)&0x01);

    for ( uint i=0; i<max_transaction_id_bits ; i++ )
        data.push_back((transaction_id >> i)&0x01);

    for ( uint i=0; i<max_pkt_cnt_bits ; i++ )
        data.push_back((pkt_cnt >> i)&0x01);

    for ( uint i=0; i<control_bits.size() ; i++ )
        data.push_back(control_bits[i]);

    for ( uint i=0; i<payload.size() ; i++ )
        data.push_back(payload[i]);

    populate_phit_data(&data);
    return;
}

/* BodyFlit Class Impl */
BodyFlit::BodyFlit()
{
    type = BODY;
}

BodyFlit::~BodyFlit()
{
//    phits.clear();
    for( uint i=0; i<phits.size(); i++)
        delete(phits.at(i));
}

TailFlit::~TailFlit()
{
//    phits.clear();
    for( uint i=0; i<phits.size(); i++)
        delete(phits.at(i));
}

string BodyFlit::toString() const
{
    stringstream str;
    str << "BodyFlit" 
        << "\tFlit type: " << type
        << endl;

    for ( uint i=0 ; i<phits.size() ; i++ )
        str << phits[i]->toString();

    return str.str();
}

void
BodyFlit::populate_body_flit()
{
    /*  Assumption: 1 body flit = 1phit */
        populate_phit_data(&bf_data);

    return ;
}

/* TailFlit Class Impl */
TailFlit::TailFlit()
{
    type = TAIL;
    packet_originated_time = (ullint)Simulator::Now();
}

string TailFlit::toString() const
{
    stringstream str;
    str << "TailFlit"
        << "\tFlit type: " << type
        << "\tpacket_originated_time: " << packet_originated_time
        << "\thop_count : " << hop_count 
        << "\tavg_net_latency : " << avg_network_latency
        << endl;

    for ( uint i=0 ; i<phits.size() ; i++ )
        str << phits[i]->toString();

    return str.str();
}

void
TailFlit::populate_tail_flit ()
{
    vector<bool> data;
    for ( uint i=0; i<max_tail_length_bits; i++ )
        data.push_back((packet_originated_time >> i)&0x01);

    populate_phit_data(&data);
    return;
}		/* -----  end of function TailFlit::populate_tail_flit  ----- */

#endif   /* ----- #ifndef _flit_cc_INC  ----- */

