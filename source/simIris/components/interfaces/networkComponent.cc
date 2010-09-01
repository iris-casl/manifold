/*
 * =====================================================================================
 *
 *       Filename:  networkComponent.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 01:48:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _networkcomponent_cc_INC
#define  _networkcomponent_cc_INC

#include	"networkComponent.h"
/*
 *--------------------------------------------------------------------------------------
 *       Class:  NetworkComponent
 *      Method:  NetworkComponent
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
NetworkComponent::NetworkComponent ()
{
    /*  For now component writers are meant to set the id of the base
     *  component class and i use that for the address. May want to check this
     *  after discussions on what the component id is meant for. */
    address = myId();

}  /* -----  end of method NetworkComponent::NetworkComponent  (constructor)  ----- */

NetworkComponent::~NetworkComponent ()
{
}		/* -----  end of function NetworkComponent::~NetworkComponent  ----- */

string
NetworkComponent::toString () const
{
    stringstream str;
    str << "NetworkComponent: " 
        << "\t address: " << address
        << "\t type: " << type
        << endl;
    return str.str();
}		/* -----  end of method NetworkComponent::toString  ----- */

#endif   /* ----- #ifndef _networkcomponent_cc_INC  ----- */
