/*
 * =====================================================================================
 *
 *       Filename:  mshr.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2010 12:43:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include "mshr.h"
#include "../simIris/components/impl/genericTracePktGen.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  MSHR_H
 *      Method:  MSHR_H
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

MSHR_H::MSHR_H()
{
    unsink = 0;
    globalUnSink = 0;
    lastFinishTime = 0;	
    done = 0;
    lastScheduledIndex = 0;	
    mshr.clear();
    writeQueue.clear();
    waiting = false;
    lastFullTime = 0;	
    countOnce.resize(no_mcs);
    for(uint i=0; i<no_mcs; i++)
        countOnce[i].resize(NO_OF_BANKS);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  MSHR_H
 *      Method:  ~MSHR_H
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
MSHR_H::~MSHR_H()
{
     mshr.clear();
     writeQueue.clear();		
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  MSHR_H
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void MSHR_H::process_event(IrisEvent* e)
{
    Addr_t addr;
    UInt thread_id;
    Time time;
    UInt cmd;
    if (e->type == MSHR_DELETE)
    {
	Request * req = new Request();
	*req = *((Request*)e->event_data.at(0));
	DeleteInMSHR(req);	
//	cout << "I came here in MSHR DELETE with " << hex << req->address << "  " << id << endl; 

	if (waiting)    	
	{   
	    unsink += (ullint)(Simulator::Now()-lastFullTime);
	    *req = waitingForMSHR;
	    waiting = false;	
	
	    IrisEvent *event = new IrisEvent();	
    	    event->src = (Component*)this;
    	    event->dst = (Component*)this;
            event->type = CREDIT_EVENT; /* Note this is dummy for a bug fix. Event type means nothing */
    	    event->event_data.push_back(req);	
    	    Simulator::Schedule(Simulator::Now()+1, &MSHR_H::process_event, (MSHR_H*)event->dst, event);
/*
            if (!(((GenericTPG*)parent)->sending))
            {
	    IrisEvent *event2 = new IrisEvent();	
    	    event2->src = (Component*)this;
    	    event2->dst = (Component*)parent;
	    event2->type = OUT_PULL_EVENT;	
    	    Simulator::Schedule(Simulator::Now()+1, &GenericTPG::process_event, (GenericTPG*)event2->dst, event2);
            ((GenericTPG*)parent)->sending = true;
            }
*/	
	}
        else
            delete req;
    }		
    else if (mshr.size() >= MSHR_SIZE && !waiting)
    {
	lastFullTime = (ullint)Simulator::Now();
	waitingForMSHR = *((Request*)e->event_data.at(0));
	waiting = true;
/*	unsink++;
    	Request * req = new Request();	
    	IrisEvent *event = new IrisEvent();
    	*req = *((Request*)e->event_data.at(0));	//TODO needs to set this through manifold kernel's links
    	event->src = (Component*)this;
    	event->dst = (Component*)this;
    	event->event_data.push_back(req);	
*/
#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << waitingForMSHR.address << ": MSHR Full Now Waiting " << endl;	
#endif
//    	Simulator::Schedule(Simulator::Now()+1, &MSHR_H::process_event, (MSHR_H*)event->dst, event);  
    }
    else if (!waiting)
    {
	Request * req = new Request();
	*req = *((Request*)e->event_data.at(0));	//TODO needs to set this through manifold kernel's links
        assert( req->arrivalTime < max_sim_time);

	uint destination=map_addr(&req->address);
	req->mcNo = destination;
        req->mcNo= ((GenericTracePktGen*)parent)->mc_node_ip[req->mcNo];
	req->arrivalTime = (ullint)Simulator::Now();
	local_map_addr(req);
	req->startTime = (ullint)Simulator::Now();
	if (req->cmdType!=CACHE_WRITEBACK)
	    mshr.push_back(*req);
	else
	    writeQueue.push_back(*req);	 
	
	    	
#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req->address << ": " << id << " PUSHED IN MSHR of size " << mshr.size() << endl;
#endif
	delete req;
	if (!trace_filename.eof())
	{
	    trace_filename >> hex >> addr;
	    trace_filename >> thread_id;	
	    trace_filename >> dec >> time;
	    trace_filename >> dec >> cmd; 

#ifdef _64BIT
	    addr= addr & 0xffffffffffc0;	//masking out cache line index bits if they exist
#else
	    addr= addr & 0xffffffc0;	//masking out cache line index bits if they exist
#endif
	    time = time+lastFinishTime;
	    if (trace_filename.eof())
	    {
	    	if (done == 0)
	    	{
		    done = 1;			//TODO needs to set this
	//	    cout << "thread done " << id << endl;
	    	}	
	    	lastFinishTime = (ullint)Simulator::Now();
	    	globalUnSink += unsink;
	    	unsink = 0;

	    	trace_filename.close();	
	    	trace_filename.open(filename,ios::in);
    	    	if(!trace_filename.is_open())
    	    	{
	    	    cout<<"Err opening trace"<<endl;
	   	    exit(1);
    	    	}	
	   
	    	if (!trace_filename.eof())
	    	{ 
            	    trace_filename >> hex >> addr;
	    	    trace_filename >> thread_id;	
	    	    trace_filename >> dec >> time;
	    	    trace_filename >> dec >> cmd;
		
		    if (!trace_filename.eof())
		    	time = time+lastFinishTime;	
	    	}
	    }
	}
	
	

    	Request *req2 = new Request();
	req2->cmdType = (cache_command)cmd;
	req2->address = addr;
	req2->arrivalTime = time+unsink;
	req2->threadId = id;
	req2->address = GlobalAddrMap(req2->address,id);
	IrisEvent *event = new IrisEvent();
    	event->src = (Component*)this;
    	event->dst = (Component*)this;
    	event->event_data.push_back(req2);
        event->type = CREDIT_EVENT; /* Note this is dummy for a bug fix. Event type means nothing */
        Simulator::Schedule(time+unsink, &MSHR_H::process_event, (MSHR_H*)event->dst, event);
        nextReq = *req2; 
        if (!(((GenericTracePktGen*)parent)->sending))
        {
            IrisEvent *event3 = new IrisEvent();
            event3->src = (Component*)this;
            event3->dst = (Component*)parent;
            event3->type = OUT_PULL_EVENT;	
            Simulator::Schedule(Simulator::Now()+1, &GenericTracePktGen::process_event, (GenericTracePktGen*)event3->dst, event3);
            ((GenericTracePktGen*)parent)->sending = true;
        }
    }
    else
        cout << "I should never come in else of MSHR" << endl;	
    delete (Request*)e->event_data.at(0);	
    delete e;
}

extern unsigned int THREAD_BITS_POSITION;
Addr_t MSHR_H::GlobalAddrMap(Addr_t addr, UInt threadId)
{
    Addr_t newAddr;
    unsigned long int threadBits = (unsigned long int)ceil(log2(NO_OF_THREADS));
    unsigned long int threadBitsPos = THREAD_BITS_POSITION;
    Addr_t lowerMask = (Addr_t)pow(2.0,threadBitsPos*1.0)-1;
    unsigned long long int upperMask = 0xFFFFFFFF << threadBitsPos;
    Addr_t lowerAddr = addr & lowerMask;
    Addr_t upperAddr = addr & upperMask;
    Addr_t threadAddr = (threadId & ((Addr_t)pow(2.0,threadBits*1.0)-1));
    newAddr = lowerAddr | (threadAddr << threadBitsPos)| (upperAddr << threadBits);
    //    cout << hex << addr << ", " << threadId << ", " << newAddr << endl;	
    return newAddr;
}

void MSHR_H::DeleteInMSHR(Request* req)
{
    vector<Request>::iterator index = mshr.begin();
    for (unsigned int i=0; i<mshr.size(); i++)
    {
        if ( (mshr[i].address == req->address) && (mshr[i].mcNo == req->mcNo) )
        {	
            lastScheduledIndex--;		
            mshr.erase(i+index);
            //((GenericTPG*)parent)->roundTripLat += (Simulator::Now() - mshr[i].startTime);	 
            ((GenericTracePktGen*)parent)->roundTripLat += ((ullint)Simulator::Now() - req->startTime);	 
#ifdef DEBUG
            cout << endl << Simulator::Now() << hex << ": Deletion Time of Request " 
                << req->address << dec << ", of Thread " << id << ", " << req->mcNo << ", " << lastScheduledIndex 
                << " ready:" << ((GenericTracePktGen*)parent)->ready[0] << endl;
#endif	
            break;
        }
    }
    cout << dec;	
}

unsigned int MSHR_H::countBLP(Request req)
{
    unsigned int count = 0;		
    for (unsigned int i=0; i<no_mcs; i++)
    {
        //	BoolVector *temp = countOnce[i]; 
        for (unsigned int j=0; j<NO_OF_BANKS; j++)
        {
            countOnce[i][j] = false;
        }
    }			
    for (unsigned int i=0; i<lastScheduledIndex; i++)
    {
        if (!countOnce[mshr[i].mcNo][mshr[i].bankNo])
        {
            countOnce[mshr[i].mcNo][mshr[i].bankNo] = true;	
            count++;
        }
    }
    //    cout << lastScheduledIndex << "saji " << count << endl;			
    return count;	
}

void MSHR_H::demap_addr(Addr_t oldAddress, Addr_t newAddress)
{
    vector<Request>::iterator index = mshr.begin();
    for (unsigned int i=0; i<mshr.size(); i++)
    {
        if (mshr[i].address == oldAddress)
        {
            mshr[i].address = newAddress;
#ifdef DEEP_DEBUG
            cout << Simulator::Now() << hex << ": Replace Address " << oldAddress << " in MSHR of Thread " << id << " to " << mshr[i].address << endl;
#endif	
            //cout << hex <<  oldAddress << " hihi " << newAddress << endl;
            break;
        }
    }

}

short int 
MSHR_H::map_addr(unsigned long long int *addr)
{
    unsigned long long int temp = MC_ADDR_BITS;   
    unsigned long long int temp2 = temp-(int)log2(no_mcs);   
    unsigned long long int lower_mask = (ullint)pow(2.0,temp2*1.0)-1;
#ifdef _64BIT
    unsigned long long int upper_mask = (ullint)((0xFFFFFFFFFFFF)-(pow(2.0,temp*1.0)-1));
#else
    unsigned long long int upper_mask = (ullint)((0xFFFFFFFF)-(pow(2.0,temp*1.0)-1));
#endif
    unsigned long long int lower_addr = (*addr) & lower_mask;
    unsigned long long int upper_addr = ((*addr) & upper_mask) >> (int)log2(no_mcs);

#ifdef GLOBAL_XOR
    short int tempBits = (int)log2(NO_OF_THREADS) - (int)log2(no_mcs);
    short int tempFactor = ((id >> tempBits) & (no_mcs-1));
    short int mc_addr = (((*addr) >> temp2) & (no_mcs-1)) ^ tempFactor;
#else    
    short int mc_addr = ((*addr) >> temp2) & (no_mcs-1);
#endif

    //    *addr = upper_addr | lower_addr;
    return mc_addr;
}

void MSHR_H::local_map_addr(Request* req)
{
    req->lowerBits = req->address & COLUMN_SIZE;
    //TODO needs to set this
    req->channelNo = ( req->address >> ((Addr_t)log2(DRAM_SIZE)-(Addr_t)log2(NO_OF_CHANNELS)) ) & (NO_OF_CHANNELS-1); 	
    // Masking higher bits to find channel No;

#ifdef PAGE_INTERLEAVING		// equivalent to l:r:b:c:v	As defined by Bruce Jacob's book.   c:v = n:z

    req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);// Masking lower bits to find Column No.
    req->bankNo = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_BANKS-1); 	// Masking the next b bits after page bits
    req->rowNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (NO_OF_ROWS-1);// Masking the next r bits	 
    req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
    // Masking next l bits	
#endif

#ifdef PERMUTATION			// equivalent to l:r:(b xor tlower):c:v

    Addr_t temp;
    Addr_t temp2;	
    req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);// Masking lower bits to find Column No.
    temp = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_BANKS-1); 		// Masking the next b bits after page bits
    temp2 = (req->address >> ((Addr_t)log2(DRAM_SIZE)-TAG_BITS)) & (NO_OF_BANKS-1); // Masking lower b bits of the tag
    req->bankNo = temp ^ temp2;	
    req->rowNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (NO_OF_ROWS-1);// Masking the next r bits	 
    req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
    // Masking next l bits	
#endif

#ifdef CACHELINE_INTERLEAVING		// equivalent to l:r:n:b:z	

    Addr_t temp;
    Addr_t temp2;	
    temp = (req->address & (CACHE_BLOCK_SIZE-1)) >> (Addr_t)log2(COLUMN_SIZE);// Masking upper z bits only and leaving bits of the col
    req->bankNo = (req->address >> (Addr_t)log2(CACHE_BLOCK_SIZE)) & (NO_OF_BANKS-1); // Masking the next b bits after cache block bits
    temp2 = ( req->address >> ((Addr_t)log2(CACHE_BLOCK_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (BLOCKS_PER_ROW-1);// Masking next n bits 
    //TODO I think this line is incorrect should be temp2 << ((int)log2(CACHE_BLOCK_SIZE-1)-log2(COLUMN_SIZE)) | temp;
    req->columnNo = (temp2 << ((Addr_t)log2(CACHE_BLOCK_SIZE)-(Addr_t)log2(COLUMN_SIZE))) | temp;
    //	req->columnNo = (temp2 >> (CACHE_BLOCK_SIZE-1) ) | temp;
    req->rowNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (NO_OF_ROWS-1);// Masking the next r bits	 
    req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
    // Masking next l bits
#endif

#ifdef SWAPPING 				// equivalent to l:(rupper:n:rlower):b:(rmiddle):z //(v+clower) n is put in lower tag bits

    unsigned short n = log2(BLOCKS_PER_ROW); 		// n is a design parameter, should be changed to explore 
    Addr_t nMask = BLOCKS_PER_ROW-1;
    Addr_t colMask;
    Addr_t rowMask;
    Addr_t temp;
    Addr_t temp2;
    req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);	// Masking lower bits to find req->columnNo temporarily
    req->bankNo = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_BANKS-1); 	// Masking the next b bits after page bits
    req->rowNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (NO_OF_ROWS-1);  
    // Masking the next r bits to find req->rowNo temporarily
    //// Very ugly logic but it works ///////////
    temp = ((req->address >> ((Addr_t)log2(ROW_SIZE)-n)) & nMask) << ((Addr_t)log2(DRAM_SIZE)-TAG_BITS-(Addr_t)log2(ROW_SIZE)-(Addr_t)log2(NO_OF_BANKS));
    temp2 = ((req->address >> ((Addr_t)log2(DRAM_SIZE)-TAG_BITS)) & nMask) << ((Addr_t)log2(ROW_SIZE)-n); 
    // Rip the required bits out from req->address
    cout << hex << temp << "        " << temp2 << "    " << req->rowNo << "   " << req->columnNo << endl;
    colMask = nMask << ((Addr_t)log2(ROW_SIZE)-n); 		// Shift the mask to required bit position
    rowMask = nMask << ((Addr_t)log2(DRAM_SIZE)-TAG_BITS-(Addr_t)log2(ROW_SIZE)-(Addr_t)log2(NO_OF_BANKS));	
    cout << colMask << "  " <<  rowMask << endl;
    req->columnNo = (req->columnNo & (~colMask)) | temp2;	// Make the required bit position of column zero & or with temp2
    req->columnNo = req->columnNo >> (Addr_t)log2(COLUMN_SIZE);	// Remove the lower z bits from column no	
    req->rowNo = (req->rowNo & (~rowMask)) | temp;
    req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
    // Masking next l bits
#endif	

#ifdef GENERIC 		// l:r:b:c:v    User should swap these around to find a good result

    ///////////////////////////////// This is just to demonstrate how a mask should be created ////////////////////////////////////////
    Addr_t zbits = (req->address & (CACHE_BLOCK_SIZE-1)); //>> (int)log2(COLUMN_SIZE);	
    Addr_t nbits = ( (req->address >> (Addr_t)log2(CACHE_BLOCK_SIZE)) & (BLOCKS_PER_ROW-1) ) << (Addr_t)log2(CACHE_BLOCK_SIZE);
    Addr_t bbits = ( (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_BANKS-1) ) << (Addr_t)log2(ROW_SIZE);
    Addr_t rbits = ( (req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS))) & (NO_OF_ROWS-1) ) 
        << ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS));
    Addr_t lbits = ( (req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1) )
        <<  ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS));// Masking next l bits
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
    //	cout << zbits << " " << nbits << endl; 
    req->columnNo = (zbits | nbits) >> (Addr_t)log2(COLUMN_SIZE);	// Oring lower and removing column bits to find Column No
    req->bankNo = bbits >> (Addr_t)log2(ROW_SIZE);		// Taking bbits shifting to LSBs
    req->rowNo = rbits >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)); // Taking rbits and shifting to LSBs
    req->rankNo = lbits >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)); // Taking lbits and shifting to LSBs
#endif

#ifdef NO_SCHEME				// no scheme   equivalent to l:b:r:c:v

    req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);// Masking lower bits to find Column No.
    req->rowNo = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_ROWS-1);		// Masking the next r bits	 
    req->bankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_BANKS-1); 	// Masking the next b bits
    req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
    // Masking next l bits 
#endif
}
