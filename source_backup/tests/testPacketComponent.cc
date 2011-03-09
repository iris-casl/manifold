/*
 * =====================================================================================
 *
 *       Filename:  testHLP.cc
 *
 *    Description:  Test case for packet format and conversion code unit
 *    testing
 *
 *        Version:  1.0
 *        Created:  02/11/2010 07:06:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */


#include	"../data_types/impl/highLevelPacket.h"
#include	"../components/interfaces/genericComponentHeader.h"
#include	"MersenneTwister.h"

MTRand mtrand1;

void test_to_low_level_packet()
{
    HighLevelPacket* hlp = new HighLevelPacket();
    hlp->source = 5;
    hlp->destination = 10;
    hlp->transaction_id = 100;
    hlp->data_payload_length = 10; /*  data is specified in terms of the number of flits */
    hlp->virtual_channel = mtrand1.randInt(4);

    for ( uint i=0 ; i< (hlp->data_payload_length*max_phy_link_bits) ; i++ )
        hlp->data.push_back(mtrand1.randInt(1));

    cout << " Generated HLP: " << hlp->toString() << endl;
    LowLevelPacket* llp = hlp->to_low_level_packet();
    cout << " Generated LLP: " << llp->toString() << endl;

    delete llp;
    delete hlp;

    return;
}			

void test_from_low_level_packet()
{
    LowLevelPacket* llp = new LowLevelPacket();
    llp->source = 5;
    llp->destination = 10;
    llp->transaction_id = 8;
    llp->length = mtrand1.randInt(20);
    llp->virtual_channel = mtrand1.randInt(4);

    HeadFlit* hf = new HeadFlit();
    hf->src_address = llp->source;
    hf->dst_address = llp->destination;
    hf->transaction_id = llp->transaction_id;
    virtual_network vn = VN1;
    message_class mc = HOM;

    /*  Create the mask based on message class and vc and virtual network */
    uint mask = (llp->virtual_channel & 0x3) | ( (vn& 0x03)<<2) | ((mc & 0x0f)<<4);
    for ( uint i=0 ; i< max_control_bits ;i++ )
    {
        llp->control_bits.push_back(( mask >> i ) & 0x01);
        hf->control_bits.push_back(( mask >> i ) & 0x01);
    }

    hf->populate_head_flit();
    llp->head_flits.push_back(hf);

    vector<bool> temp;
    for ( uint i = 0; i< llp->length - 2 ; i++ )
    {
        BodyFlit* bf = new BodyFlit();
        for ( uint j=0; j<max_phy_link_bits; j++)
            temp.push_back(mtrand1.randInt(1));
        bf->populate_body_flit(temp);
        temp.clear();
        llp->body_flits.push_back(bf);
    }

    TailFlit* tf = new TailFlit();
    tf->populate_tail_flit();
    llp->tail_flits.push_back(tf);

    HighLevelPacket* hlp = new HighLevelPacket();
    hlp->from_low_level_packet( llp );

    cout << " Generated LLP: " << llp->toString() << endl;
    cout << " Generated HLP: " << hlp->toString() << endl;
    delete llp;
    delete hlp;
    return;

}


int
main ( int argc, char *argv[] )
{
//        test_from_low_level_packet();
    test_to_low_level_packet();        

    return 0;
}				/* ----------  end of function main  ---------- */
