/*
 * =====================================================================================
 *
 *       Filename:  genericData.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 05:06:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericdata_cc_INC
#define  _genericdata_cc_INC

#include	"genericData.h"

LinkArrivalData::LinkArrivalData()
{
    valid = false;
}

LinkArrivalData::~LinkArrivalData()
{
}

VirtualChannelDescription::VirtualChannelDescription()
{
}

string
InputBufferState::toString() const
{
    stringstream str;
    str << "input_buffer_state"
        << " inport: " << input_port
        << " inch: " << input_channel
        << " outport: " << output_port
        << " outch: " << output_channel
        << " destination: " << destination 
        << " address: " << address;
    switch( pipe_stage)
    {
        case INVALID:
            str << " INVALID";
            break;
        case IB:
            str << " IB";
            break;
        case EMPTY:
            str << " EMPTY";
            break;
        case FULL:
            str << " FULL";
            break;
        case ROUTED:
            str << " ROUTED";
            break;
        case SWA_REQUESTED:
            str << " SWA_REQUESTED";
            break;
        case SW_ALLOCATED:
            str << " SW_ALLOCATED";
            break;
        case SW_TRAVERSAL:
            str << " SW_TRAVERSAL";
            break;
        case REQ_OUTVC_ARB:
            str << " REQ_OUTVC_ARB";
            break;
    }

    return str.str();
}

InputBufferState::InputBufferState()
{
    pipe_stage = INVALID;
}


#endif   /* ----- #ifndef _genericdata_cc_INC  ----- */

