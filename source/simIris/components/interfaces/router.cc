/*
 * =====================================================================================
 *
 *       Filename:  router.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 01:56:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _router_cc_INC
#define  _router_cc_INC

#include	"router.h"
using namespace std;


string
Router::toString () const 
{
    stringstream str;
    str << "Router" << endl;
    return str.str() ;
}		/* -----  end of method Router::toString  ----- */

#endif   /* ----- #ifndef _router_cc_INC  ----- */
