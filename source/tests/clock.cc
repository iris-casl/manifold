/*
 * =====================================================================================
 *
 *       Filename:  clock.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2010 03:46:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include	"clock.h"

using namespace std;

Clock::Clock()
{
   this->sim_timestamp = 0;
   Simulator::Schedule(Simulator::Now()+1, &Clock::updateTimestamp, this);
}

void Clock::updateTimestamp()
{
    this->sim_timestamp++;
    cout	<< "Clock: Updating clock time to "<< this->sim_timestamp << endl;
    Simulator::Schedule(Simulator::Now()+1, &Clock::updateTimestamp, this);
}
