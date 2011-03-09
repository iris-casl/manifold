/*
 * =====================================================================================
 *
 *       Filename:  networkComponent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 11:53:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _networkcomponent_h_INC
#define  _networkcomponent_h_INC

#include	"../../../kernel/component.h"
#include	"../../../util/simIrisComponentHeader.h"
#include	"../../data_types/impl/irisEvent.h"
#include	<sstream>
#include	<fstream>

using namespace std;
/*
 * =====================================================================================
 *        Class:  NetworkComponent
 *  Description:  
 * =====================================================================================
 */
class NetworkComponent : public Component
{
    public:
        enum types { processor, interface, link, router };
        types type;
        string name;
        uniqueId address;
        uint node_ip;

        NetworkComponent ();                             /* constructor */
        virtual ~NetworkComponent();
        virtual string toString () const;
        virtual void process_event(IrisEvent* e) = 0;

    protected:

    private:

}; /* -----  end of class NetworkComponent  ----- */

#endif   /* ----- #ifndef _networkcomponent_h_INC  ----- */
