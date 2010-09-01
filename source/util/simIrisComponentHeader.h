/*
 * =====================================================================================
 *
 *       Filename:  genericComponentHeader.h
 *
 *    Description:  This is the file for generic data type definitions that
 *    can be used across all components.
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:28:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericcomponentheader_h_INC
#define  _genericcomponentheader_h_INC

#define DEFAULT_ADDRESS 0
#define DEFAULT_CONVERT_PACKET_CYCLES 1
#define NO_DATA true
#define FLIT_ID 9800
#define CREDIT_ID 9801

#include        <cstdlib>
#include        <iostream>
#include        <stdio.h>
#include	<cassert>
#include	"../kernel/simulator.h"

/*  macro for general debug print statements. */
#define LOC std::cout << "\nTime:" << dec << Simulator::Now() <<" " << name << " " << address << " " << node_ip << " ";
#define _DBG(fmt,...) LOC printf(fmt,__VA_ARGS__);
#define _DBG_NOARG(fmt) LOC printf(fmt);


using namespace std;

typedef unsigned long int uniqueId;  /* assuming this will be atleast 4 bytes and hence ~4G addresses for node id's and transaction id's and so on */
typedef unsigned long long int simTime;
typedef unsigned long long int ullint;
typedef unsigned int uint;
enum message_class { INVALID_PKT, REQUEST_PKT, WRITE_REQ, RESPONSE_PKT, ONE_FLIT_REQ, CLUBBED_PKT, PRIORITY_REQ};

const unsigned int max_network_node_bits = 8;
const unsigned int max_transaction_id_bits = 8;
const unsigned int max_tail_length_bits = 8;
const unsigned int max_control_bits = 8;

//for dhruv
const unsigned int max_pkt_cnt_bits = 3;
const unsigned int head_and_tail_length = 80;

enum virtual_network { VN0, VN1, VN2};
//enum message_class { HOM /* Carries requests from a caching agent to the home agent, mainly used for  coherence. Mentioned her for completeness */,
//    SNP /* Snoop */,
//    NDR /* Non data response. This class is used to send short reponse messages */,
//    DRS /* Data response */,
//    NCB /* Non coherent bypass */,
//    NCS /* Non coherent standard */
//};

#endif   /* ----- #ifndef _genericcomponentheader_h_INC  ----- */
