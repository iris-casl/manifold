/*
 * =====================================================================================
 *
 *       Filename:  packetSource.h
 *
 *    Description:  This is a test packet to send packets to a interface on
 *    VC0 at some random time and random packet length. 
 *
 *        Version:  1.0
 *        Created:  03/11/2010 05:00:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _packetsource_h_INC
#define  _packetsource_h_INC

#include        "MersenneTwister.h"
#include        "../components/interfaces/processor.h"
#include        "../components/impl/genericEvents.h"
#include        "../data_types/impl/highLevelPacket.h"
#include	"../data_types/impl/irisEvent.h"
#include	"../components/impl/genericData.h"
#include	<fstream>

#define DEFAULT_RAN_MAX_TIME 100
//#define PURE_SINK 1

using namespace std;

/*
 * =====================================================================================
 *        Class:  RandomPacketGenerator
 *  Description:  
 * =====================================================================================
 */
class RandomPacketGenerator : public Processor
{
    public:
        RandomPacketGenerator ();                             /* constructor */
        ~RandomPacketGenerator ();
        uint address;
        uint min_address;
        uint max_address;
        uint min_length;
        uint max_length;
        uint min_delay;
        uint max_delay;
        uint hotspots;
        uint max_time;
        uint seed;
        void setup();
        void finish();
        void process_event(IrisEvent* e);
        string toString() const;
        uint node_ip;
        void init();

    protected:

    private:
        string in_filename;
        string out_filename;
        ofstream out_file;
        ofstream in_file;
        vector<bool> ready;
        uint last_vc;
        uint no_of_packets;
        bool sending;
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);

}; /* -----  end of class RandomPacketGenerator  ----- */


#endif   /* ----- #ifndef _packetsource_h_INC  ----- */
