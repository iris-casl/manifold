/*
 * =====================================================================================
 *
 *       Filename:  router.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 12:17:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _router_h_INC
#define  _router_h_INC

#include	"networkComponent.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  Router
 *  Description:  
 * =====================================================================================
 */
class Router : public NetworkComponent
{
    public:
        Router (){
            type = NetworkComponent::router;
        }                             /* constructor */
        ~Router () {}
        vector <NetworkComponent* > input_connections;
        vector <NetworkComponent* > output_connections;
        virtual void set_no_nodes( unsigned int nodes) = 0;
        virtual void init(uint p, uint v, uint c, uint b) = 0;
        virtual string toString () const;
        virtual string print_stats() = 0;
        virtual void set_grid_x_location ( uint a, uint b, uint c) = 0;
        virtual void set_grid_y_location ( uint a, uint b, uint c) = 0;

        uint ports;
        uint vcs;
        uint credits;
        uint buffer_size;
    protected:

    private:

}; /* -----  end of class Router  ----- */

#endif   /* ----- #ifndef _router_h_INC  ----- */

