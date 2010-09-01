/*
 * =====================================================================================
 *
 *       Filename:  stats.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2010 06:53:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  stats_h_INC
#define  stats_h_INC

#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"

#include        "request.h"


class Statistic
{  	
    ///////////////// Some Counters ///////////////////
//    ullint Times;	// Ugly just to avoid overflow
    	
    public:   
	Component *mc;
        Component *pwr;

    	vector<double> avgLatPerBank;	
    	vector<ullint> latPerBank;
    	vector<ullint> reqPerBank;
    	vector<ullint> hitsPerBank;
	vector<ullint> throttlePerBank;
	vector<double> avgThrottlePerBank;
    	vector<double> hitRatePerBank;		

    	vector < vector<double> > avgLatPerThreadPerBank;	
    	vector<vector <ullint> >latPerThreadPerBank;	
    	vector<vector <ullint> >reqPerThreadPerBank;
    	vector<vector <ullint> >hitsPerThreadPerBank;
	vector<vector <ullint> >throttlePerThreadPerBank;
	vector<vector <double> >avgThrottlePerThreadPerBank;
    	vector<vector <double> >hitRatePerThreadPerBank;

    	vector<vector <ullint> >TotalFrontSideTimePerThreadPerBank;	
    	vector<vector <ullint> >TotalQueueDelayPerThreadPerBank;
    	vector<vector <ullint> >TotalCBufDelayPerThreadPerBank;
	vector<vector <ullint> > TotalBusMemDelayPerThreadPerBank;
    	

	vector<vector<vector <ullint> > >rbufferOccupancy;
	vector<vector<vector <ullint> > > emptyCycles;
	vector<vector<vector <float> > > rbufferOccupancyRatio;

	vector<ullint> TotalFrontSideTimePerBank;	
    	vector<ullint> TotalQueueDelayPerBank;
    	vector<ullint> TotalCBufDelayPerBank;
	vector<ullint> TotalBusMemDelayPerBank;
	
	vector<float> avgFrontSideTimePerBank;	
    	vector<float> avgQueueDelayPerBank;
    	vector<float> avgCBufDelayPerBank;
	vector<float> avgBusMemDelayPerBank;

	void InitStats();
	void CollectStatsPerRequest(Request *req);
	void CalculateAggregateStats();
	string PrintAggregateStats(uint n);
	void CollectStatsPerCycle();

	vector<bool*> doneOnce;
    	vector<double> avgLatPerThread;	
    	vector<ullint> latPerThread;
    	vector<ullint> reqPerThread;
    	vector<ullint> hitsPerThread;
	vector<ullint> throttlePerThread;
	vector<double> avgThrottlePerThread;
    	vector<double> hitRatePerThread;
    	vector<float> BLPPerThread;
    	
	vector<ullint> TotalFrontSideTimePerThread;	
    	vector<ullint> TotalQueueDelayPerThread;
    	vector<ullint> TotalCBufDelayPerThread;
	vector<ullint> TotalBusMemDelayPerThread;

	vector<float> avgFrontSideTimePerThread;	
    	vector<float> avgQueueDelayPerThread;
    	vector<float> avgCBufDelayPerThread;
	vector<float> avgBusMemDelayPerThread;
	
        vector< vector< vector<ullint> > > bufferFullCycles;

///////////////// DRAM Power Run Time Counters //////////////////////////
	float BNK_PRE;
	float CLK_LO_PRE;
	float CLK_LO_ACT;
	float PH;
	float RDsch;
	float WRsch;
	float termRDsch;
	float termWRsch;
	float RRDsch;

    ///////////////////////////////////////////////////  	
};/*  -----  end of class stats  ----- */

#define MaxVcc 1.58
#define MinVcc 1.43
#define DQS 2	// mostly remain same 
#define DM_MC 1	// mostly remain same 
#define IDD0 120 
#define IDD2PS 10
#define IDD2PF 25
#define IDD2N 60
#define IDD3P 40 
#define IDD3N 65
#define IDD4R 250
#define IDD4W 225
#define IDD5A 260
#define t_CK 1.25
#define SysVdd 1.5
#define SysClk 800
#define BL 8
#define PdqRD 5.3
#define PdqWR 13.2
#define PdqRDoth 0
#define PdqWRoth 0

/* ------------------------------------
#define MaxVcc 1.58
#define MinVcc 1.43
#define DQS 2	// mostly remain same 
#define DM 1	// mostly remain same 
#define IDD0 130 
#define IDD2PS 12
#define IDD2PF 40
#define IDD2N 80
#define IDD3P 65 
#define IDD3N 95
#define IDD4R 255
#define IDD4W 330
#define IDD5A 305
#define t_CK 1.5
#define SysVdd 1.5
#define SysClk 667
#define BL 8
#define PdqRD 5.3
#define PdqWR 13.2
#define PdqRDoth 0
#define PdqWRoth 0
------------------------*/

class PowerStats : public Statistic
{  	
    ///////////////// Some parameters for power ///////////////////
   public:

   	PowerStats();
   	~PowerStats(); 	
   	void CalcPower( float BNK_PRE, float CLK_LO_PRE, float CLK_LO_ACT, float RDsch, 
			float WRsch, float termRDsch, float termWRsch, float RRDsch );
	float Pow_PRE_PDN;
	float Pow_PRE_STBY;
	float Pow_ACT_PDN;
	float Pow_ACT_STBY;
	float Pow_REF;
	float Pow_ACT_PRE;
	float Pow_WR;
	float Pow_RD;
   	float Pow_DQ;
   	float Pow_termWR;
   	float Pow_termRDoth;
   	float Pow_termWRoth;
   	float Pow_Term;
   	float Pow_Bkg;
	float Pow_AP;
   	float Pow_RD_WR_Tr;
   	float Pow_Total;
   
   private:
}; ////* end of class PowerStats */// 

#endif   /*  ----- #ifndef stats_INC  ----- */
