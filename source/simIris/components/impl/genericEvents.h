/*
 * =====================================================================================
 *
 *       Filename:  genericEvents.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 04:54:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericevents_h_INC
#define  _genericevents_h_INC

#define OLD_PACKET_EVENT 999
#define NEW_PACKET_EVENT 1000
#define READY_EVENT 1101
#define CREDIT_EVENT 1102
#define TICK_EVENT 1103
#define LINK_ARRIVAL_EVENT 1104
#define SWAP_VC_EVENT 1105
#define IN_PULL_EVENT 1106
#define OUT_PULL_EVENT 1107
#define IN_PUSH_EVENT 1108
#define OUT_PUSH_EVENT 1109
#define VC_ARBITRATE_EVENT 1110

#define IN_ARBITRATE_EVENT 1111
#define OUT_ARBITRATE_EVENT 1112
#define PORT_ARBITRATE_EVENT 1113
#define ADDRESS_DECODE_EVENT 1114
#define TRAVERSE_CROSSBAR_EVENT 1115
#define CONFIGURE_CROSSBAR_EVENT 1116
#define FLIT_OUT_EVENT 1117

#define CHECK_IN_ARBITER_EVENT 1118
#define CHECK_OUT_ARBITER_EVENT 1119
#define START 1120
#define STOP 1121
#define CONTINUE 1122
#define START_SUBCOMPONENT 1123
#define STOP_SUBCOMPONENT 1124
#define START_READ 1125
#define START_WRITE 1126
#define PUSH_BUFFER 1127
#define REPLY 1128
#define STOP_CMD_QUEUE 1129
#define START_CMD_QUEUE 1130
#define IN_BUFFER_EVENT 1131
#define MSHR_DELETE 1132
#define SEND_TO_NI 1133
#define DETECT_DEADLOCK_EVENT 1134

#endif   /* ----- #ifndef _genericevents_h_INC  ----- */

