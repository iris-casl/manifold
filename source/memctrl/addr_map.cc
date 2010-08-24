/*
 * =====================================================================================
 *
 *       Filename:  addr_map.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 03:01:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "addr_map.h"
#include "request_handler.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AddrMap
 *      Method:  AddrMap
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

extern uint BANK_BITS;
AddrMap::AddrMap()
{
    
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AddrMap
 *      Method:  ~AddrMap
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AddrMap::~AddrMap()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AddrMap
 *      Method:  process_event‘RankHandler
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AddrMap::process_event(IrisEvent* e)
{	
   /*  Request *req = new Request();
    IrisEvent *event = new IrisEvent();
    *req = ((RequestHandler*)parent)->pipeline;
    map_addr(req);	
    int bufferId = ((RequestHandler*)parent)->chan[req->channelNo].rank[req->rankNo].bank[req->bankNo].bufferId;
    req->rbufferInsertionTime = Simulator::Now()+1;
    ((RequestHandler*)parent)->chan[req->channelNo].rank[req->rankNo].rbuffer[bufferId].push_back(*req);
    event->src = (Component*)this;
    event->dst = (Component*)(&((RequestHandler*)parent)->chan[req->channelNo].rank[req->rankNo].bank[req->bankNo]);
    event->type = START;
#ifdef DEEP_DEBUG
cout << dec << Simulator::Now() << ": " << hex << req->address << ": Now I am in addrMap with channel " << dec << req->channelNo << ", rank " << req->rankNo << ", bank " << req->bankNo << ", row " << req->rowNo << ", column " << req->columnNo << endl;
#endif

    Simulator::Schedule(Simulator::Now()+1, &BankHandler::process_event, (BankHandler*)event->dst, event);    	
    
 //   cout << ((RequestHandler*)parent)->chan[req->channelNo].rank[req->rankNo].rbuffer[req->bankNo].size() << endl;
    if (((RequestHandler*)parent)->chan[req->channelNo].rank[req->rankNo].bank[req->bankNo].IsBufferFull())
    {
	((RequestHandler*)parent)->oneBufferFull = true;
#ifdef DEEP_DEBUG
	cout << "Now the buffer is full" << endl;
#endif	
    } 	
    delete req;	
    */
    Request req;
    IrisEvent *event = new IrisEvent();
    req = ((RequestHandler*)parent)->pipeline;
//#ifndef CLUBBED
    map_addr(&req);
//#endif	
    int bufferId = ((RequestHandler*)parent)->chan[req.channelNo].rank[req.rankNo].bank[req.bankNo].bufferId;
    req.rbufferInsertionTime = (ullint)Simulator::Now()+1;
    ((RequestHandler*)parent)->chan[req.channelNo].rank[req.rankNo].rbuffer[bufferId].push_back(req);
    event->src = (Component*)this;
    event->dst = (Component*)(&((RequestHandler*)parent)->chan[req.channelNo].rank[req.rankNo].bank[req.bankNo]);
    event->type = START;
#ifdef DEEP_DEBUG
cout << dec << Simulator::Now() << ": 0x" << hex << req.address << ": Now I am in addrMap with channel " << dec << req.channelNo << ", rank " << req.rankNo << ", bank " << req.bankNo << ", row " << req.rowNo << ", column " << req.columnNo << "BufferId " << bufferId << endl;
#endif

    Simulator::Schedule(Simulator::Now()+1, &BankHandler::process_event, (BankHandler*)event->dst, event);    	
    
 //   cout << ((RequestHandler*)parent)->chan[req->channelNo].rank[req->rankNo].rbuffer[req->bankNo].size() << endl;
    if (((RequestHandler*)parent)->chan[req.channelNo].rank[req.rankNo].bank[req.bankNo].IsBufferFull())
    {
	((RequestHandler*)parent)->oneBufferFull = true;
#ifdef DEEP_DEBUG
	cout << "Now the buffer is full" << endl;
#endif	
    } 	
    delete e;
}

std::string AddrMap::toString()
{
	return "AddrMap";
}

void AddrMap::map_addr(Request* req)
{
    req->lowerBits = req->address & COLUMN_SIZE;
        //TODO needs to set this
    req->channelNo = ( req->address >> ((Addr_t)log2(DRAM_SIZE)-(Addr_t)log2(NO_OF_CHANNELS)) ) & (NO_OF_CHANNELS-1); 	
											// Masking higher bits to find channel No;

    switch (addr_map_scheme)
    {
      case PAGE_INTERLEAVING:
      { 	// equivalent to l:r:b:c:v	As defined by Bruce Jacob's book.   c:v = n:z

	req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);// Masking lower bits to find Column No.
	req->bankNo = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_BANKS-1); 	// Masking the next b bits after page bits
	req->rowNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (NO_OF_ROWS-1);// Masking the next r bits	 
	req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
													// Masking next l bits
        break;
      }

      case PERMUTATION:
      {		// equivalent to l:r:(b xor tlower):c:v

	Addr_t temp;
	Addr_t temp2;	
	req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);// Masking lower bits to find Column No.
	temp = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_BANKS-1); 		// Masking the next b bits after page bits
	temp2 = (req->address >> ((Addr_t)log2(DRAM_SIZE)-TAG_BITS)) & (NO_OF_BANKS-1); // Masking lower b bits of the tag
	req->bankNo = temp ^ temp2;	
	req->rowNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)) ) & (NO_OF_ROWS-1);// Masking the next r bits	 
	req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
													// Masking next l bits	
        break;
      }

      case CACHELINE_INTERLEAVING:
      {		// equivalent to l:r:n:b:z	
	
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
        break;
      }
      
      case SWAPPING:
      {		// equivalent to l:(rupper:n:rlower):b:(rmiddle):z //(v+clower) n is put in lower tag bits
	
	unsigned short n = log2(BLOCKS_PER_ROW); 		// n is a design parameter, should be changed to explore 
	Addr_t nMask = BLOCKS_PER_ROW-1;
	Addr_t colMask;
	Addr_t rowMask;
	Addr_t temp;
	Addr_t temp2;
	req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1); // Masking lower bits to find req->columnNo temporarily
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
        break;
      }
      case GENERIC:
      {		// l:r:b:c:v    User should swap these around to find a good result

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
	req->rankNo = lbits >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS));//Taking lbits, shifting to LSBs
        break;
      }
      case NO_SCHEME:
      {		// no scheme   equivalent to l:b:r:c:v

	req->columnNo = (req->address >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1);// Masking lower bits to find Column No.
	req->rowNo = (req->address >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_ROWS-1);		// Masking the next r bits	 
	req->bankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_BANKS-1); 	// Masking the next b bits
	req->rankNo = ( req->address >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_BANKS)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);  
													// Masking next l bits 
        break;
      }
      case LOCAL_ADDR_MAP:
      {

        Addr_t tempAddr = req->address;
        unsigned int lowerBit = BANK_BITS;   
        unsigned int upperBit = lowerBit+(int)log2(NO_OF_BANKS);
        unsigned int lower_mask = pow(2.0,lowerBit*1.0)-1;
        ullint upper_mask = (0xFFFFFFFF)-(pow(2.0,upperBit*1.0)-1);
        unsigned int lower_addr = (tempAddr) & lower_mask;
        ullint upper_addr = ((tempAddr) & upper_mask) >> (int)log2(NO_OF_BANKS);
        req->bankNo = ((tempAddr) >> lowerBit) & (NO_OF_BANKS-1);
//      cout << hex << req->bankNo << " " << tempAddr << " " << upper_addr << " " << lower_addr << " " << endl;
        tempAddr = upper_addr | lower_addr;
        req->columnNo = (tempAddr >> (Addr_t)log2(COLUMN_SIZE)) & (NO_OF_COLUMNS-1); //    Masking lower bits to find Column No.
        req->rowNo = (tempAddr >> (Addr_t)log2(ROW_SIZE)) & (NO_OF_ROWS-1);          // Masking the next r bits       
        req->rankNo = ( tempAddr >> ((Addr_t)log2(ROW_SIZE)+(Addr_t)log2(NO_OF_ROWS)) ) & (NO_OF_RANKS-1);
        break;
      }
      default:
      {
          cout << " ERROR : addr_map.cc line 234" << endl;
          exit(1);
      }
    }
}


