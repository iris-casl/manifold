/*!
 * =====================================================================================
 *
 *       Filename:  genericLink.h
 *
 *    Description:  Simple link btw router->router interface->router
 *    router->interface.
 *    Unidirectional Link. for bidirectional use two of these
 *    Future uses may be minimal when moving to the parallel kernal and links
 *    cross LP boundaries.
 *
 *        Version:  1.0
 *        Created:  02/22/2010 07:27:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericlink_h_INC
#define  _genericlink_h_INC

#include	"../interfaces/irisLink.h"
#include	"../../../util/genericData.h"
#include	"../../data_types/impl/irisEvent.h"

using namespace std;
extern ullint max_sim_time;

class GenericLink : public IrisLink
{
    public:
        GenericLink() {}                             /* constructor */
        ~GenericLink () {}
        uint cycles;
        uint stages;
        void setup();
        void process_event(IrisEvent* e);
        string toString() const;
        string print_stats() const;
        ullint get_flits_utilization();
        ullint get_credits_utilization();
        ullint flits_passed;
        ullint credits_passed;
        uint link_id;
        bool is_dateline;

    protected:

    private:
        uint node_ip;
        void handle_link_arrival_event(IrisEvent* e);

}; /* -----  end of class GenericLink  ----- */


#endif   /* ----- #ifndef _genericlink_h_INC  ----- */

