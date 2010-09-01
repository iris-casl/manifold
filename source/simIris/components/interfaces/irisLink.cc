/*
 * =====================================================================================
 *
 *       Filename:  link.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/22/2010 07:34:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _link_cc_INC
#define  _link_cc_INC

#include	"irisLink.h"
/*
 *--------------------------------------------------------------------------------------
 *       Class:  IrisLink
 *      Method:  IrisLink
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

string
IrisLink::toString () const
{
    stringstream str;
    str << NetworkComponent::toString();
    str << "Networklink: "
        << "\tinputConnection: " << input_connection
        << "\toutputConnection: " << output_connection
        << endl;
    return str.str();
}		/* -----  end of function IrisLink::toString  ----- */
#endif   /* ----- #ifndef _link_cc_INC  ----- */

