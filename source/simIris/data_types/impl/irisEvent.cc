/*
 * =====================================================================================
 *
 *       Filename:  event.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 12:47:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  event_INC
#define  event_INC

#include	"irisEvent.h"

string IrisEvent::toString()
{
    stringstream str;
    str << "Time: " << Simulator::Now() 
        << " src: " << this->src_id
//->myId() 
//        << " dst: " << this->dst->myId() 
        << " vc: " << this->vc;
    return str.str();
}

IrisEvent::IrisEvent()
{
    src_id = -1;
    vc = 0;
}

IrisEvent::~IrisEvent()
{
//    for( uint i=0; i < event_data.size(); i++)
//        free(event_data.at(i));
}

#endif   /* ----- #ifndef event_INC  ----- */
