/*
 * =====================================================================================
 *
 *       Filename:  processor.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 01:51:32 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _processor_cc_INC
#define  _processor_cc_INC
#include	"processor.h"

Processor::Processor ()
{
    type = NetworkComponent::processor;
}		/* -----  end of method Processor::Processor  ----- */

Processor::~Processor ()
{
}		/* -----  end of function Processor::~Processor  ----- */

string Processor::toString() const
{
    stringstream str;
    str << " Processor" << endl;
    return str.str();
}

void
Processor::init()
{

}
#endif   /* ----- #ifndef _processor_cc_INC  ----- */

