/*!
 * =====================================================================================
 *
 *       Filename:  vcaMsgAware.h
 *
 *    Description:  This is a pseudo-component that does vca in a noc with
 *    multiple msg_classes.
 *    Every msg_class is said to heave a msg_id which indicates the priority.
 *    Vca is done for input packets based on this priority.
 *
 *    Warning: Make sure the no_msg_classes is set before calling init
 *
 *        Version:  1.0
 *        Created:  10/18/2010 01:06:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include	<vector>
#include	<map>
#include	<deque>
#include	<algorithm>
#include	"../interfaces/networkComponent.h"
#include        "../../../util/genericData.h"

using namespace std;

class VcaMsgAware
{
    public:
        VcaMsgAware ();
        ~VcaMsgAware ();
        vector< vector< map<uint, uint> > > winners;
        void init( uint ports, uint vcs);
        void set_no_msg_classes ( uint no_msg_classes );
        void request( uint output_port, uint msg_class_id, uint requestor_id /* Usually in_port*vcs+invc */, ullint req_arrival_time );
        uint pick_winners( void );
        uint get_no_requestors( void );
        void clear_winner( uint winner, uint oc);

    private:
        vector< vector< vector<uint> > > arbiter;
        vector< vector< vector<ullint> > > arbiter_arrival_time;
        vector< vector< deque<uint> > > available_vcs;
        uint vcs;
        uint ports;
        uint no_mclasses;
};
