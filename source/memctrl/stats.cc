/*
 * =====================================================================================
 *
 *       Filename:  stats.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2010 06:53:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#include "MC.h"
#include "stats.h"
//#include "power.h" 

PowerStats::PowerStats()
{
   Pow_PRE_PDN = 0.0;
   Pow_PRE_STBY = 0.0;
   Pow_ACT_PDN = 0.0;
   Pow_ACT_STBY = 0.0;
   Pow_REF = 0.0;
   Pow_ACT_PRE = 0.0;
   Pow_WR = 0.0;
   Pow_RD = 0.0;
   Pow_DQ = 0.0;
   Pow_termWR = 0.0;
   Pow_termRDoth = 0.0;
   Pow_termWRoth = 0.0;
   Pow_Term = 0.0;
   Pow_Bkg = 0.0;
   Pow_AP = 0.0;	
   Pow_RD_WR_Tr = 0.0;
   Pow_Total = 0.0;	
}

PowerStats::~PowerStats()
{
}

void PowerStats::CalcPower(float BNK_PRE, float CLK_LO_PRE, float CLK_LO_ACT, float RDsch, 
			   float WRsch, float termRDsch, float termWRsch, float RRDsch )
{
    Pow_PRE_PDN  = (IDD2PF*MaxVcc) * BNK_PRE * CLK_LO_PRE * pow((SysVdd/MaxVcc),2) * (SysClk*1.0/1000)*t_CK;

    Pow_PRE_STBY  = (IDD2N*MaxVcc) * BNK_PRE * (1-CLK_LO_PRE) * pow((SysVdd/MaxVcc),2) * (SysClk*1.0/1000)*t_CK;

    Pow_ACT_PDN  = (IDD3P*MaxVcc) * (1-BNK_PRE) * CLK_LO_PRE * pow((SysVdd/MaxVcc),2) * (SysClk*1.0/1000)*t_CK;

    Pow_ACT_STBY  = (IDD3N*MaxVcc) * (1.0-BNK_PRE) * (1.0-CLK_LO_ACT) * pow((SysVdd/MaxVcc),2) * (SysClk*1.0/1000)*t_CK; 

    Pow_REF       = (IDD5A - IDD3N) * tRFC/(tREFI*1000.0) * MaxVcc * pow((SysVdd/MaxVcc),2);

    Pow_ACT_PRE   = ( IDD0 - (IDD3N*tRAS*1.0/tRC + IDD2N*(tRC-tRAS)*1.0/tRC) ) * MaxVcc * tRC*1.0/RRDsch * pow((SysVdd/MaxVcc),2);

    Pow_WR        = (IDD4W - IDD3N) * MaxVcc * (8/BL) * WRsch * pow((SysVdd/MaxVcc),2) * (SysClk*1.0/1000)*t_CK;

    Pow_RD        = (IDD4R - IDD3N) * MaxVcc * (8/BL) * RDsch * pow((SysVdd/MaxVcc),2) * (SysClk*1.0/1000)*t_CK;

    Pow_DQ        = (BL+DQS) * PdqRD * RDsch;

    Pow_termWR    = (BL+DQS+DM_MC) * PdqWR * WRsch;

    Pow_termRDoth = (BL+DQS) * PdqRD * termRDsch;   

    Pow_termWRoth = (BL+DQS+DM_MC) * PdqWR * termWRsch;   

    Pow_Term      = Pow_termWR + Pow_termRDoth + Pow_termWRoth;

    ///////////////// Until now is the device power ///////////////////////////////////// 
    ////// We will calculate the Total power by multiplying with 9 (9 devices) //////////	 	

    Pow_Bkg       = 9 * (Pow_PRE_PDN + Pow_PRE_STBY + Pow_ACT_PDN + Pow_ACT_STBY + Pow_REF);

    Pow_AP   = 9 * Pow_ACT_PRE;  

    Pow_RD_WR_Tr  = 9 * (Pow_RD + Pow_WR + Pow_DQ + Pow_Term); 

    Pow_Total     = Pow_Bkg + Pow_AP + Pow_RD_WR_Tr;

}

void Statistic::CollectStatsPerRequest(Request *req)
{
//    if (!(*doneOnce[req->threadId]))	
    {	
    ///////// calculating runing average ////////////////

    avgLatPerThreadPerBank[req->threadId][req->bankNo] 
	= ( avgLatPerThreadPerBank[req->threadId][req->bankNo] * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) )
	* ( reqPerThreadPerBank[req->threadId][req->bankNo] * 1.0 )	
	+ ( (req->retireTime - req->arrivalTime) * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) );
    
    avgThrottlePerThreadPerBank[req->threadId][req->bankNo] 
	= ( avgThrottlePerThreadPerBank[req->threadId][req->bankNo] * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) )
	* ( reqPerThreadPerBank[req->threadId][req->bankNo] * 1.0 )	
	+ ( req->throttleTime * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) );

    reqPerThreadPerBank[req->threadId][req->bankNo]++;	
    latPerThreadPerBank[req->threadId][req->bankNo] += (req->retireTime - req->arrivalTime);
    
    TotalFrontSideTimePerThreadPerBank[req->threadId][req->bankNo] += (req->rbufferInsertionTime - req->arrivalTime);
    TotalQueueDelayPerThreadPerBank[req->threadId][req->bankNo] += (req->cbufferInsertionTime - req->rbufferInsertionTime);
    TotalCBufDelayPerThreadPerBank[req->threadId][req->bankNo] += (req->busInsertionTime - req->cbufferInsertionTime);
    TotalBusMemDelayPerThreadPerBank[req->threadId][req->bankNo] += (req->retireTime - req->busInsertionTime - 1);

#ifdef DEBUG
    cout << "MC[" << ((MC*)mc)->id << "] " << req->address << ": " << Simulator::Now() 
	<<  ": Reached stats collection with arrival Time = "<< req->arrivalTime 
	<< ", req buffer insertion time = " << req->rbufferInsertionTime 
	<< " cmd queue insertion time = " << req->cbufferInsertionTime 
	<< ", bus insertion time = " << req->busInsertionTime 
	<< "and retire time = " << req->retireTime << endl;
#endif

    if (req->status == OPEN)    
	hitsPerThreadPerBank[req->threadId][req->bankNo]++;
    hitRatePerThreadPerBank[req->threadId][req->bankNo] =  hitsPerThreadPerBank[req->threadId][req->bankNo] * 100.0
							 / reqPerThreadPerBank[req->threadId][req->bankNo];  
    }
//      cout << req->status << endl;
//    cout << "Latency " << latPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Average Latency " << avgLatPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Requests " << reqPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Hits " << hitsPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Hit Rate " << hitRatePerThreadPerBank[req->threadId][req->bankNo] << endl;
	
}

void Statistic::CollectStatsPerCycle()
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
//		if ( ((MC*)mc)->reqH->chan[i].rank[j].rbuffer[k].empty() )
//		    emptyCycles[i][j][k]++;
//		rbufferOccupancy[i][j][k] += ((MC*)mc)->reqH->chan[i].rank[j].rbuffer[k].size();
	    }
	}
    }		
    
}

void Statistic::InitStats()
{
    doneOnce.resize(NO_OF_THREADS);

    avgLatPerThread.resize(NO_OF_THREADS);
    latPerThread.resize(NO_OF_THREADS);
    reqPerThread.resize(NO_OF_THREADS);
    hitsPerThread.resize(NO_OF_THREADS);
    throttlePerThread.resize(NO_OF_THREADS);
    avgThrottlePerThread.resize(NO_OF_THREADS);
    hitRatePerThread.resize(NO_OF_THREADS);
    BLPPerThread.resize(NO_OF_THREADS);

    avgLatPerBank.resize(NO_OF_BANKS);
    latPerBank.resize(NO_OF_BANKS);
    reqPerBank.resize(NO_OF_BANKS);
    hitsPerBank.resize(NO_OF_BANKS);
    throttlePerBank.resize(NO_OF_BANKS);
    avgThrottlePerBank.resize(NO_OF_BANKS);
    hitRatePerBank.resize(NO_OF_BANKS);

    avgLatPerThreadPerBank.resize(NO_OF_THREADS);
    latPerThreadPerBank.resize(NO_OF_THREADS);
    reqPerThreadPerBank.resize(NO_OF_THREADS);
    hitsPerThreadPerBank.resize(NO_OF_THREADS);
    throttlePerThreadPerBank.resize(NO_OF_THREADS);
    avgThrottlePerThreadPerBank.resize(NO_OF_THREADS);
    hitRatePerThreadPerBank.resize(NO_OF_THREADS);

    avgFrontSideTimePerThread.resize(NO_OF_THREADS);
    avgQueueDelayPerThread.resize(NO_OF_THREADS);
    avgCBufDelayPerThread.resize(NO_OF_THREADS);
    avgBusMemDelayPerThread.resize(NO_OF_THREADS);

    TotalFrontSideTimePerThread.resize(NO_OF_THREADS);
    TotalQueueDelayPerThread.resize(NO_OF_THREADS);
    TotalCBufDelayPerThread.resize(NO_OF_THREADS);
    TotalBusMemDelayPerThread.resize(NO_OF_THREADS);

    TotalFrontSideTimePerThreadPerBank.resize(NO_OF_THREADS);
    TotalQueueDelayPerThreadPerBank.resize(NO_OF_THREADS);
    TotalCBufDelayPerThreadPerBank.resize(NO_OF_THREADS);
    TotalBusMemDelayPerThreadPerBank.resize(NO_OF_THREADS);

    TotalFrontSideTimePerBank.resize(NO_OF_BANKS);
    TotalQueueDelayPerBank.resize(NO_OF_BANKS);
    TotalCBufDelayPerBank.resize(NO_OF_BANKS);
    TotalBusMemDelayPerBank.resize(NO_OF_BANKS);

    avgFrontSideTimePerBank.resize(NO_OF_BANKS);
    avgQueueDelayPerBank.resize(NO_OF_BANKS);
    avgCBufDelayPerBank.resize(NO_OF_BANKS);
    avgBusMemDelayPerBank.resize(NO_OF_BANKS);

    rbufferOccupancy.resize(NO_OF_CHANNELS);
    emptyCycles.resize(NO_OF_CHANNELS);
    rbufferOccupancyRatio.resize(NO_OF_CHANNELS);
    bufferFullCycles.resize(NO_OF_CHANNELS);

    for ( uint i=0; i<NO_OF_CHANNELS; i++)
    {
        rbufferOccupancy[i].resize(NO_OF_RANKS);
        emptyCycles[i].resize(NO_OF_RANKS);
        rbufferOccupancyRatio[i].resize(NO_OF_RANKS);
        bufferFullCycles[i].resize(NO_OF_RANKS);
    }

    for ( uint i=0; i<NO_OF_CHANNELS; i++)
        for ( uint j=0; j<NO_OF_RANKS; j++)
        {
            rbufferOccupancy[i][j].resize(NO_OF_BANKS);
            emptyCycles[i][j].resize(NO_OF_BANKS);
            rbufferOccupancyRatio[i][j].resize(NO_OF_BANKS);
            bufferFullCycles[i][j].resize(NO_OF_BANKS);
        }

    for ( uint i=0; i<NO_OF_THREADS; i++)
    {
        avgLatPerThreadPerBank[i].resize(NO_OF_BANKS);
        latPerThreadPerBank[i].resize(NO_OF_BANKS);
        reqPerThreadPerBank[i].resize(NO_OF_BANKS);
        hitsPerThreadPerBank[i].resize(NO_OF_BANKS);
        throttlePerThreadPerBank[i].resize(NO_OF_BANKS);
        avgThrottlePerThreadPerBank[i].resize(NO_OF_BANKS);
        hitRatePerThreadPerBank[i].resize(NO_OF_BANKS);
        TotalFrontSideTimePerThreadPerBank[i].resize(NO_OF_BANKS);
        TotalQueueDelayPerThreadPerBank[i].resize(NO_OF_BANKS);
        TotalCBufDelayPerThreadPerBank[i].resize(NO_OF_BANKS);
        TotalBusMemDelayPerThreadPerBank[i].resize(NO_OF_BANKS);
    }

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		emptyCycles[i][j][k] = 0;
		rbufferOccupancy[i][j][k] = 0;
	    }	
	}
    }		
/*    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		rbufferOccupancy[i][j][k] += ((MC*)mc)->reqH->chan[i].rank[j].rbuffer[k].size();
	    }
	}
    }		
*/	
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {
	avgLatPerThread[i] = 0.0;
	latPerThread[i] = 0;
	reqPerThread[i] = 0;
    	hitsPerThread[i] = 0;
	hitRatePerThread[i] = 0.0;
    	BLPPerThread[i] = 0.0;
	avgThrottlePerThread[i] = 0.0;
	throttlePerThread[i] = 0;
	avgFrontSideTimePerThread[i] = 0;
	avgQueueDelayPerThread[i] = 0;
	avgCBufDelayPerThread[i] = 0;
	avgBusMemDelayPerThread[i] = 0;	
	TotalFrontSideTimePerThread[i] = 0;
	TotalQueueDelayPerThread[i] = 0;
	TotalCBufDelayPerThread[i] = 0;
	TotalBusMemDelayPerThread[i] = 0;	
    }

    for (unsigned int i=0; i<NO_OF_BANKS; i++)
    {
	avgLatPerBank[i] = 0.0;
	latPerBank[i] = 0;
    	reqPerBank[i] = 0;
    	hitsPerBank[i] = 0;
    	hitRatePerBank[i] = 0.0;
	avgThrottlePerBank[i] = 0.0;
	throttlePerBank[i] = 0;
	avgFrontSideTimePerBank[i] = 0;
	avgQueueDelayPerBank[i] = 0;
	avgCBufDelayPerBank[i] = 0;
	avgBusMemDelayPerBank[i] = 0;	
	TotalFrontSideTimePerBank[i] = 0;
	TotalQueueDelayPerBank[i] = 0;
	TotalCBufDelayPerBank[i] = 0;
	TotalBusMemDelayPerBank[i] = 0;	
    }	

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
        {	 
	    avgLatPerThreadPerBank[i][j] = 0.0;
	    latPerThreadPerBank[i][j] = 0;   
	    reqPerThreadPerBank[i][j] = 0;
	    hitsPerThreadPerBank[i][j] = 0;
	    hitRatePerThreadPerBank[i][j] = 0.0;
	    avgThrottlePerThreadPerBank[i][j] = 0.0;
	    throttlePerThreadPerBank[i][j] = 0;
	   
	    TotalFrontSideTimePerThreadPerBank[i][j] = 0;
	    TotalQueueDelayPerThreadPerBank[i][j] = 0;
	    TotalCBufDelayPerThreadPerBank[i][j] = 0;
	    TotalBusMemDelayPerThreadPerBank[i][j] = 0;	
	}
    }

///////////////// DRAM Power Run Time Counters //////////////////////////
    BNK_PRE = 0.0;
    CLK_LO_PRE = 0.0;
    CLK_LO_ACT = 0.0;
    PH = 0.0;
    RDsch = 0.0;
    WRsch = 0.0;
    termRDsch = 0.0;
    termWRsch = 0.0;

    pwr = (Component*)(new PowerStats());
}

void Statistic::CalculateAggregateStats()
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BANKS; k++)
    	    {
                if (((MC*)mc)->reqH->chan[i].rank[j].bank[k].bufferOccupancy != 0)
		    rbufferOccupancyRatio[i][j][k] = ((MC*)mc)->reqH->chan[i].rank[j].bank[k].bufferSize*1.0 / ((MC*)mc)->reqH->chan[i].rank[j].bank[k].bufferOccupancy;
		
		bufferFullCycles[i][j][k] = ((MC*)mc)->reqH->chan[i].rank[j].bank[k].bufferFullCounter;
	    }
	}
    }		
    for (unsigned int j=0; j<NO_OF_BANKS; j++)
    {    	
	for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    	{
	    
	    latPerBank[j] += latPerThreadPerBank[i][j];
	    throttlePerBank[j] += throttlePerThreadPerBank[i][j];	
	    reqPerBank[j] += reqPerThreadPerBank[i][j];
	    hitsPerBank[j] += hitsPerThreadPerBank[i][j];
	    avgLatPerBank[j] += (avgLatPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	    avgThrottlePerBank[j] += (avgThrottlePerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);		
	//    avgFrontSideTimePerBank[j] += (avgFrontSideTimePerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	//    avgQueueDelayPerBank[j] += (avgQueueDelayPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	//    avgCBufDelayPerBank[j] += (avgCBufDelayPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	//    avgBusMemDelayPerBank[j] += (avgBusMemDelayPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);	
	    TotalFrontSideTimePerBank[j] += TotalFrontSideTimePerThreadPerBank[i][j];
	    TotalQueueDelayPerBank[j] += TotalQueueDelayPerThreadPerBank[i][j];
	    TotalCBufDelayPerBank[j] += TotalCBufDelayPerThreadPerBank[i][j];
	    TotalBusMemDelayPerBank[j] += TotalBusMemDelayPerThreadPerBank[i][j];	
    	}
	if (reqPerBank[j] !=0 ) 
	{
    	avgLatPerBank[j] = avgLatPerBank[j] / reqPerBank[j];
	avgThrottlePerBank[j] = avgThrottlePerBank[j] / reqPerBank[j];
        hitRatePerBank[j] += hitsPerBank[j] * 100.0 / reqPerBank[j];

	avgFrontSideTimePerBank[j] = TotalFrontSideTimePerBank[j]*1.0 / reqPerBank[j];
	avgQueueDelayPerBank[j] = TotalQueueDelayPerBank[j]*1.0 / reqPerBank[j];
	avgCBufDelayPerBank[j] = TotalCBufDelayPerBank[j]*1.0 / reqPerBank[j];
	avgBusMemDelayPerBank[j] = TotalBusMemDelayPerBank[j]*1.0 / reqPerBank[j];	
	}
    }
    
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    latPerThread[i] += latPerThreadPerBank[i][j];
	    throttlePerThread[i] += throttlePerThreadPerBank[i][j];
	    reqPerThread[i] += reqPerThreadPerBank[i][j];
	    hitsPerThread[i] += hitsPerThreadPerBank[i][j];
	    avgLatPerThread[i] += (avgLatPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	    avgThrottlePerThread[i] += (avgThrottlePerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);		
	//    avgFrontSideTimePerThread[i] += (avgFrontSideTimePerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	//    avgQueueDelayPerThread[i] += (avgQueueDelayPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	//    avgCBufDelayPerThread[i] += (avgCBufDelayPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	//    avgBusMemDelayPerThread[i] += (avgBusMemDelayPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);	
	    TotalFrontSideTimePerThread[i] += TotalFrontSideTimePerThreadPerBank[i][j];
	    TotalQueueDelayPerThread[i] += TotalQueueDelayPerThreadPerBank[i][j];
	    TotalCBufDelayPerThread[i] += TotalCBufDelayPerThreadPerBank[i][j];
	    TotalBusMemDelayPerThread[i] += TotalBusMemDelayPerThreadPerBank[i][j];	
    	}
	if (reqPerThread[i])
	{
	avgLatPerThread[i] = avgLatPerThread[i] / reqPerThread[i];
	avgThrottlePerThread[i] = avgThrottlePerThread[i] / reqPerThread[i];
        hitRatePerThread[i] += hitsPerThread[i] * 100.0 / reqPerThread[i];
	
	avgFrontSideTimePerThread[i] = TotalFrontSideTimePerThread[i]*1.0 / reqPerThread[i];
	avgQueueDelayPerThread[i] = TotalQueueDelayPerThread[i]*1.0 / reqPerThread[i];
	avgCBufDelayPerThread[i] = TotalCBufDelayPerThread[i]*1.0 / reqPerThread[i];
	avgBusMemDelayPerThread[i] = TotalBusMemDelayPerThread[i]*1.0 / reqPerThread[i];	
	}
    }

////////////////////// DRAM Power Stats calc //////////////////////

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
        RDsch = ((MC*)mc)->dram->dc[i].dramReadCycles / Simulator::Now();
        WRsch = ((MC*)mc)->dram->dc[i].dramWriteCycles/ Simulator::Now();
  
	unsigned long long int TotalHits = 0;
    	unsigned long long int TotalReqs = 0; 	
    	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{    
	    TotalHits += hitsPerBank[j];
	    TotalReqs += reqPerBank[j];
    	}
    	if (TotalReqs > 0)			
    	    PH = TotalHits*1.0/TotalReqs;
	RRDsch = round(BL/(2*(RDsch+WRsch) * (1-PH)) * 10000 / SysClk) / 10;	
    }
   ((PowerStats*)pwr)->CalcPower(BNK_PRE, CLK_LO_PRE, CLK_LO_ACT, RDsch, WRsch, termRDsch, termWRsch, RRDsch);  
}

string Statistic::PrintAggregateStats( uint n)
{
    stringstream str;
    str << dec;	
/*    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Total Request  for Thread " << i << ", bank " << j << " = " << reqPerThreadPerBank[i][j] << endl;
	}
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
		str << "MC [" << n << "] " << "Avg Latency for Thread " << i << ", bank " << j << " = " << avgLatPerThreadPerBank[i][j] << endl;
	}
    }

    str << endl;			
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
		str << "Avg Throttling time for Thread " << i << ", bank " << j << " = " << avgThrottlePerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }
    str << endl;			
    
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Hits for Thread " << i << ", bank " << j << " = " << hitsPerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }	


    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Hit Rate for Thread " << i << ", bank " << j << " = " << hitRatePerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }	

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Total FrontSide Time for Thread " << i << ", bank " << j << " = " << TotalFrontSideTimePerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }	

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Total Queuing Delay for Thread " << i << ", bank " << j << " = " << TotalQueueDelayPerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }	
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Total Cmd Buffer Time for Thread " << i << ", bank " << j << " = " << TotalCBufDelayPerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }	
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "MC [" << n << "] " << "Total Bus & Memory Time for Thread " << i << ", bank " << j << " = " << TotalBusMemDelayPerThreadPerBank[i][j] << endl;
	}
    str << endl;			
    }	

*/
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Total Request  for Thread " << i << " = " << reqPerThread[i] << endl;
    }
    str << endl;			
   
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Hits  for Thread " << i << " = " << hitsPerThread[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Hit Rate  for Thread " << i << " = " << hitRatePerThread[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Latency  for Thread " << i << " = " << avgLatPerThread[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average FrontSide Time for Thread " << i << " = " << avgFrontSideTimePerThread[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Queueing Delay for Thread " << i << " = " << avgQueueDelayPerThread[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Cmd Buffer Time for Thread " << i << " = " << avgCBufDelayPerThread[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Bus & Memory Delay for Thread " << i << " = " << avgBusMemDelayPerThread[i] << endl;
    }
    str << endl;			

/*    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Throttling time for Thread " << i << " = " << avgThrottlePerThread[i] << endl;
    }
    str << endl;			
*/

 				
    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Total Request  for Bank " << i << " = " << reqPerBank[i] << endl;
    }
    str << endl;			
   
    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Hits  for Bank " << i << " = " << hitsPerBank[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Hit Rate  for Bank " << i << " = " << hitRatePerBank[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Latency  for Bank " << i << " = " << avgLatPerBank[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average FrontSide Time for Bank " << i << " = " << avgFrontSideTimePerBank[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Queueing Delay for Bank " << i << " = " << avgQueueDelayPerBank[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Cmd Buffer Time for Bank " << i << " = " << avgCBufDelayPerBank[i] << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Bus & Memory Delay for Bank " << i << " = " << avgBusMemDelayPerBank[i] << endl;
    }
    str << endl;			

/*    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "MC [" << n << "] " << "Average Throttling time for Bank " << i << " = " << avgThrottlePerBank[i] << endl;
    }
    str << endl;
*/
     for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BANKS; k++)
    	    {
		str << "MC [" << n << "] " << "Average Occupancy for Channel " << i << ", " << " Rank " << j << ", " << " Bank " << ", " << k << " = " << rbufferOccupancyRatio[i][j][k] << endl;
	    }
	}
  }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BANKS; k++)
    	    {
		str << "MC [" << n << "] " << "Total Occupancy Cycles for Channel " << i << ", " 
                    << " Rank " << j << ", " << " Bank " << ", " << k << " = " << ((MC*)mc)->reqH->chan[i].rank[j].bank[k].bufferOccupancy << endl;
	    }
	}
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BANKS; k++)
    	    {
		str << "MC [" << n << "] " << "Buffer Full Cycles for Channel " << i << ", " 
                    << " Rank " << j << ", " << " Bank " << ", " << k << " = " << bufferFullCycles[i][j][k] << endl;
	    }
	}
    }
    str << endl;	
/*
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		str << "MC [" << n << "] " << "Empty Cycles for Channel " << i << ", " << " Rank " << j << ", " << " Bank " << ", " << k << " = " << emptyCycles[i][j][k] << endl;
	    }
	}
    }
    str << endl;			
*/
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		str << "MC [" << n << "] " << "DRAM Busy Time for Channel " << i << ", " << " Rank " << j << ", " << " Bank " << ", " << k << " = " << ((MC*)mc)->dram->dc[i].dramBankBusyCycles[j][k] << endl;
	    }
	}
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
        str  <<  "MC [" << n << "] " << "bus_busy_cycles:\t" << ((MC*)mc)->bus->dataBus[i].busBusyTime << endl;
        str  <<  "MC [" << n << "] " << "per_bus_occupancy:\t" << (((MC*)mc)->bus->dataBus[i].busBusyTime+0.0)/Simulator::Now()*100 << endl;
        str  <<  "MC [" << n << "] " << "dram_busy_cycles:\t" << ((MC*)mc)->dram->dc[i].dramBusyCycles << endl;
        str  <<  "MC [" << n << "] " << "dram_read_cycles:\t" << ((MC*)mc)->dram->dc[i].dramReadCycles << endl;
        str  <<  "MC [" << n << "] " << "dram_write_cycles:\t" << ((MC*)mc)->dram->dc[i].dramWriteCycles << endl;
        str  <<  "MC [" << n << "] " << "percentage_dram_read_cycles:\t" << RDsch * 100.0 << endl;
        str  <<  "MC [" << n << "] " << "percentage_dram_write_cycles:\t" << WRsch * 100.0 << endl;
	str  <<  "MC [" << n << "] " << "overall_page_hit_rate:\t" << PH * 100.0 << endl;
    	str  <<  "MC [" << n << "] " << "overall_background_power(mW):\t" << ((PowerStats*)pwr)->Pow_Bkg << endl;
	str  <<  "MC [" << n << "] " << "overall_activate_precharge_power(mW):\t" << ((PowerStats*)pwr)->Pow_AP << endl;
    	str  <<  "MC [" << n << "] " << "overall_read_write_termination_power(mW):\t" << ((PowerStats*)pwr)->Pow_RD_WR_Tr << endl;
    	str  <<  "MC [" << n << "] " << "total_dram_power(mW):\t" << ((PowerStats*)pwr)->Pow_Total << endl;
    }
    str << endl;
    str <<  "MC [" << n << "] " << "total_cycles:\t" << Simulator::Now() << endl;

    return str.str();
}
