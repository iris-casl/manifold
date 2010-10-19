/*!
 * =====================================================================================
 *
 *       Filename:  genericEvents.h
 *
 *    Description:  Has the enum of all event types used by the simulator 
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

enum IrisEventType {
    OLD_PACKET_EVENT ,
    NEW_PACKET_EVENT ,  /* Component gets a new packet */
    READY_EVENT ,       /* Component gets a packet level credit */ 
    CREDIT_EVENT ,
    TICK_EVENT ,        /* Update internal state of the component */
    LINK_ARRIVAL_EVENT ,        /* Flit or credit arrives on the link(incoming) */
    SWAP_VC_EVENT , 
    IN_PULL_EVENT , 
    OUT_PULL_EVENT ,    /* Push a packet out onto the link/network */ 
    IN_PUSH_EVENT , 
    OUT_PUSH_EVENT , 
    VC_ARBITRATE_EVENT , 
    IN_ARBITRATE_EVENT , 
    OUT_ARBITRATE_EVENT , 
    PORT_ARBITRATE_EVENT , 
    ADDRESS_DECODE_EVENT , 
    TRAVERSE_CROSSBAR_EVENT , 
    CONFIGURE_CROSSBAR_EVENT , 
    FLIT_OUT_EVENT , 
    CHECK_IN_ARBITER_EVENT , 
    CHECK_OUT_ARBITER_EVENT , 
    START ,
    STOP ,
    CONTINUE ,
    START_SUBCOMPONENT ,
    STOP_SUBCOMPONENT ,
    START_READ , 
    START_WRITE ,
    PUSH_BUFFER ,
    REPLY ,
    STOP_CMD_QUEUE ,
    START_CMD_QUEUE ,
    IN_BUFFER_EVENT , 
    MSHR_DELETE ,
    SEND_TO_NI , 
    DETECT_DEADLOCK_EVENT 
};

#endif   /* ----- #ifndef _genericevents_h_INC  ----- */

