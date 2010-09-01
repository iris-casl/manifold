/*
 * =====================================================================================
 *
 *       Filename:  interface.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 01:43:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _interface_cc_INC
#define  _interface_cc_INC

#include	"interface.h"
using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Interface
 *      Method:  Interface
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Interface::Interface ()
{
    address = 0;
    type = NetworkComponent::interface;
}  /* -----  end of method Interface::Interface  (constructor)  ----- */

Interface::~Interface ()
{
}		/* -----  end of function Interface::~Interface  ----- */

string
Interface::toString () const
{
    stringstream str;
    str << "Interface"
        << "\tAddr: " << address
        << "\t input_connection: " << input_connection
        << "\t output_connection: " << output_connection
        << "\t processor_connection: "<< processor_connection->toString()
        << endl;
    return str.str();
}		/* -----  end of method Interface::toString  ----- */

#endif   /* ----- #ifndef _interface_cc_INC  ----- */

