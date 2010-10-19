/*
 * =====================================================================================
 *
 *       Filename:  temp.cc
 *
 *    Description:  Generate address traces
 *
 *        Version:  1.0
 *        Created:  05/10/2010 04:39:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <iostream>
#include "../source/tests/MersenneTwister.h"

int
main ( int argc, char *argv[] )
{
    MTRand mtrand1;
    /*  Addr space is 32 bits long 4294967296*/
    unsigned long long int time = 30;
    unsigned int time_incr = 10;
    unsigned long long int max_time = 1000000;
    while (time<max_time)
    {
//        unsigned long long int address = mtrand1.randInt(268435456*2);
        unsigned long long int address = 0x00f0ffff;
        unsigned int burst_length = mtrand1.randInt(1000);
        time += mtrand1.randInt(1000);
        for ( unsigned int i=0; i < burst_length && time < max_time; i++)
        {
            std::cout <<"0x" <<std::hex <<address << std::dec <<" 0 "<< time << " 1" << std::endl;
//            address = address+64;
            time += time_incr;
        }
    }
    return 0;
}				/* ----------  end of function main  ---------- */
