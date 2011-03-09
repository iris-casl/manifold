/*****************************************************************************
 *                                McPAT
 *                      SOFTWARE LICENSE AGREEMENT
 *            Copyright 2009 Hewlett-Packard Development Company, L.P.
 *                          All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Any User of the software ("User"), by accessing and using it, agrees to the
 * terms and conditions set forth herein, and hereby grants back to Hewlett-
 * Packard Development Company, L.P. and its affiliated companies ("HP") a
 * non-exclusive, unrestricted, royalty-free right and license to copy,
 * modify, distribute copies, create derivate works and publicly display and
 * use, any changes, modifications, enhancements or extensions made to the
 * software by User, including but not limited to those affording
 * compatibility with other hardware or software, but excluding pre-existing
 * software applications that may incorporate the software.  User further
 * agrees to use its best efforts to inform HP of any such changes,
 * modifications, enhancements or extensions.
 *
 * Correspondence should be provided to HP at:
 *
 * Director of Intellectual Property Licensing
 * Office of Strategy and Technology
 * Hewlett-Packard Company
 * 1501 Page Mill Road
 * Palo Alto, California  94304
 *
 * The software may be further distributed by User (but not offered for
 * sale or transferred for compensation) to third parties, under the
 * condition that such third parties agree to abide by the terms and
 * conditions of this license.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITH ANY AND ALL ERRORS AND DEFECTS
 * AND USER ACKNOWLEDGES THAT THE SOFTWARE MAY CONTAIN ERRORS AND DEFECTS.
 * HP DISCLAIMS ALL WARRANTIES WITH REGARD TO THE SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL
 * HP BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ***************************************************************************/

#include "io.h"
#include "parameter.h"
#include "const.h"
#include "basic_circuit.h"
#include <iostream>
#include <algorithm>
#include "XML_Parse.h"
#include <string>
#include <cmath>
#include <assert.h>
#include "noc.h"



NoC::NoC(ParseXML *XML_interface, int ithNoC_, InputParameter* interface_ip_, double M_traffic_pattern_, double link_len_)
:XML(XML_interface),
ithNoC(ithNoC_),
interface_ip(*interface_ip_),
router(0),
link_bus(0),
link_bus_exist(false),
router_exist(false),
M_traffic_pattern(M_traffic_pattern_)
{
	/*
	 * initialize, compute and optimize individual components.
	 */

	interface_ip.wire_is_mat_type = 2;
	interface_ip.wire_os_mat_type = 2;
	interface_ip.wt               =Global;
	set_noc_param();
	local_result=init_interface(&interface_ip);
	scktRatio = g_tp.sckt_co_eff;

	if (nocdynp.type)
	{/*
		 * if NOC compute router, router links must be computed separately
		 * and called from external
		 * since total chip area must be known first
		 */
		init_router();
	}
	else
	{
		init_link_bus(link_len_); //if bus compute bus
	}

	//  //clock power
	//  clockNetwork.init_wire_external(is_default, &interface_ip);
	//  clockNetwork.clk_area           =area*1.1;//10% of placement overhead. rule of thumb
	//  clockNetwork.end_wiring_level   =5;//toplevel metal
	//  clockNetwork.start_wiring_level =5;//toplevel metal
	//  clockNetwork.num_regs           = corepipe.tot_stage_vector;
	//  clockNetwork.optimize_wire();
}

void NoC::init_router()
{
	router  = new Router(nocdynp.flit_size,
			nocdynp.virtual_channel_per_port*nocdynp.input_buffer_entries_per_vc,
			nocdynp.virtual_channel_per_port, &(g_tp.peri_global),
			nocdynp.input_ports,nocdynp.output_ports, M_traffic_pattern);
	//router->print_router();
	area.set_area(area.get_area()+ router->area.get_area()*nocdynp.total_nodes);

	double long_channel_device_reduction = longer_channel_device_reduction(Uncore_device);
	router->power.readOp.longer_channel_leakage          = router->power.readOp.leakage * long_channel_device_reduction;
	router->buffer.power.readOp.longer_channel_leakage   = router->buffer.power.readOp.leakage * long_channel_device_reduction;
	router->crossbar.power.readOp.longer_channel_leakage = router->crossbar.power.readOp.leakage * long_channel_device_reduction;
	router->arbiter.power.readOp.longer_channel_leakage  = router->arbiter.power.readOp.leakage * long_channel_device_reduction;
	router_exist = true;
}

void NoC ::init_link_bus(double link_len_)
{


	if (nocdynp.min_ports==1 )
	if (nocdynp.type)
		link_name = "Links";
	else
		link_name = "Bus";

	link_len=link_len_;
	assert(link_len>0);

	interface_ip.throughput = nocdynp.link_throughput/nocdynp.clockRate;
	interface_ip.latency = nocdynp.link_latency/nocdynp.clockRate;

	link_len /= (nocdynp.horizontal_nodes + nocdynp.vertical_nodes)/2;

	if (nocdynp.total_nodes >1) link_len /=2; //All links are shared by neighbors
	link_bus = new interconnect(name, Uncore_device, 1, 1, nocdynp.flit_size,
				  link_len, &interface_ip, 3, true/*pipelinable*/, nocdynp.route_over_perc);

	link_bus_tot_per_Router.area.set_area(link_bus_tot_per_Router.area.get_area()+ link_bus->area.get_area()
			* nocdynp.global_linked_ports);

	area.set_area(area.get_area()+ link_bus_tot_per_Router.area.get_area()* nocdynp.total_nodes);
	link_bus_exist = true;
}
void NoC::computeEnergy(bool is_tdp)
{
	//power_point_product_masks
	double pppm_t[4]    = {1,1,1,1};
	double M=nocdynp.duty_cycle;
	if (is_tdp)
	    {
	    	//init stats for TDP
	    	stats_t.readAc.access  = M;
            tdp_stats = stats_t;
            if (router_exist)
            {
            	set_pppm(pppm_t, 1*M, 1, 1, 1);//reset traffic pattern
                router->power = router->power*pppm_t;
                set_pppm(pppm_t, nocdynp.total_nodes, nocdynp.total_nodes, nocdynp.total_nodes, nocdynp.total_nodes);
          	    power     = power + router->power*pppm_t;
            }
            if (link_bus_exist)
            {
            	if (nocdynp.type)
            		set_pppm(pppm_t, 1*M_traffic_pattern*M*(nocdynp.min_ports -1), nocdynp.global_linked_ports,
            			nocdynp.global_linked_ports, nocdynp.global_linked_ports);
            	    //reset traffic pattern; local port do not have router links
            	else
            		set_pppm(pppm_t, 1*M_traffic_pattern*M*(nocdynp.min_ports), nocdynp.global_linked_ports,
            		            			nocdynp.global_linked_ports, nocdynp.global_linked_ports);//reset traffic pattern

            	link_bus_tot_per_Router.power = link_bus->power*pppm_t;

                set_pppm(pppm_t, nocdynp.total_nodes,
                		         nocdynp.total_nodes,
                		         nocdynp.total_nodes,
                		         nocdynp.total_nodes);
            	power     = power + link_bus_tot_per_Router.power*pppm_t;

            }
	    }
	    else
	    {
	    	//init stats for runtime power (RTP)
	    	stats_t.readAc.access  = XML->sys.NoC[ithNoC].total_accesses;
            rtp_stats = stats_t;
        	set_pppm(pppm_t, 1, 0 , 0, 0);
        	if (router_exist)
        	{
            	router->buffer.rt_power.readOp.dynamic = (router->buffer.power.readOp.dynamic + router->buffer.power.writeOp.dynamic)*rtp_stats.readAc.access ;
            	router->crossbar.rt_power.readOp.dynamic = router->crossbar.power.readOp.dynamic*rtp_stats.readAc.access ;
            	router->arbiter.rt_power.readOp.dynamic = router->arbiter.power.readOp.dynamic*rtp_stats.readAc.access ;

        		router->rt_power = router->rt_power + (router->buffer.rt_power + router->crossbar.rt_power + router->arbiter.rt_power)*pppm_t +
					router->power*pppm_lkg;//TDP power must be calculated first!
        		rt_power     = rt_power + router->rt_power;
        	}
        	if (link_bus_exist)
        	{
        		set_pppm(pppm_t, rtp_stats.readAc.access, 1 , 1, rtp_stats.readAc.access);
        		link_bus->rt_power = link_bus->power * pppm_t;
        		rt_power = rt_power + link_bus->rt_power;
        	}

	    }
}


void NoC::displayEnergy(uint32_t indent,int plevel,bool is_tdp)
{
	string indent_str(indent, ' ');
	string indent_str_next(indent+2, ' ');
	bool long_channel = XML->sys.longer_channel_device;

	double M =M_traffic_pattern*nocdynp.duty_cycle;
	/*only router as a whole has been applied the M_traffic_pattern(0.6 by default) factor in router.cc;
	 * 	When power of crossbars, arbiters, etc need to be displayed, the M_traffic_pattern factor need to
	 * be applied together with McPAT's extra traffic pattern.
	 * */
	if (is_tdp)
	{
		cerr << name << endl;
		cerr << indent_str << "Area = " << area.get_area()*1e-6<< " mm^2" << endl;
		cerr << indent_str<< "Peak Dynamic = " << power.readOp.dynamic*nocdynp.clockRate << " W" << endl;
		cerr << indent_str << "Subthreshold Leakage = "
			<< (long_channel? power.readOp.longer_channel_leakage:power.readOp.leakage) <<" W" << endl;
		cerr << indent_str << "Gate Leakage = " << power.readOp.gate_leakage << " W" << endl;
		cerr << indent_str<< "Runtime Dynamic = " << rt_power.readOp.dynamic/nocdynp.executionTime << " W" << endl;
		cerr<<endl;

		if (router_exist)
		{
			cerr << indent_str << "Router: " << endl;
			cerr << indent_str_next << "Area = " << router->area.get_area()*1e-6<< " mm^2" << endl;
			cerr << indent_str_next<< "Peak Dynamic = " << router->power.readOp.dynamic*nocdynp.clockRate << " W" << endl;
			cerr << indent_str_next << "Subthreshold Leakage = "
			<< (long_channel? router->power.readOp.longer_channel_leakage:router->power.readOp.leakage)  <<" W" << endl;
			cerr << indent_str_next << "Gate Leakage = " << router->power.readOp.gate_leakage << " W" << endl;
			cerr << indent_str_next<< "Runtime Dynamic = " << router->rt_power.readOp.dynamic/nocdynp.executionTime << " W" << endl;
			cerr<<endl;
			if (plevel >2){
				cerr << indent_str<< indent_str << "Virtual Channel Buffer:" << endl;
				cerr << indent_str<< indent_str_next << "Area = " << router->buffer.area.get_area()*1e-6*nocdynp.input_ports<< " mm^2" << endl;
				cerr << indent_str<< indent_str_next << "Peak Dynamic = " <<(router->buffer.power.readOp.dynamic + router->buffer.power.writeOp.dynamic)
				*nocdynp.min_ports*M*nocdynp.clockRate << " W" << endl;
				cerr << indent_str<< indent_str_next << "Subthreshold Leakage = "
				<< (long_channel? router->buffer.power.readOp.longer_channel_leakage*nocdynp.input_ports:router->buffer.power.readOp.leakage*nocdynp.input_ports)  <<" W" << endl;
				cerr << indent_str<< indent_str_next << "Gate Leakage = " << router->buffer.power.readOp.gate_leakage*nocdynp.input_ports << " W" << endl;
				cerr << indent_str<< indent_str_next << "Runtime Dynamic = " << router->buffer.rt_power.readOp.dynamic/nocdynp.executionTime << " W" << endl;
				cerr <<endl;
				cerr << indent_str<< indent_str<< "Crossbar:" << endl;
				cerr << indent_str<< indent_str_next << "Area = " << router->crossbar.area.get_area()*1e-6  << " mm^2" << endl;
				cerr << indent_str<< indent_str_next << "Peak Dynamic = " << router->crossbar.power.readOp.dynamic*nocdynp.clockRate*nocdynp.min_ports*M << " W" << endl;
				cerr << indent_str<< indent_str_next << "Subthreshold Leakage = "
				<< (long_channel? router->crossbar.power.readOp.longer_channel_leakage:router->crossbar.power.readOp.leakage)  << " W" << endl;
				cerr << indent_str<< indent_str_next << "Gate Leakage = " << router->crossbar.power.readOp.gate_leakage  << " W" << endl;
				cerr << indent_str<< indent_str_next << "Runtime Dynamic = " << router->crossbar.rt_power.readOp.dynamic/nocdynp.executionTime << " W" << endl;
				cerr <<endl;
				cerr << indent_str<< indent_str<< "Arbiter:" << endl;
				cerr << indent_str<< indent_str_next << "Peak Dynamic = " << router->arbiter.power.readOp.dynamic*nocdynp.clockRate*nocdynp.min_ports*M  << " W" << endl;
				cerr << indent_str<< indent_str_next << "Subthreshold Leakage = "
				<< (long_channel? router->arbiter.power.readOp.longer_channel_leakage:router->arbiter.power.readOp.leakage)  << " W" << endl;
				cerr << indent_str<< indent_str_next << "Gate Leakage = " << router->arbiter.power.readOp.gate_leakage  << " W" << endl;
				cerr << indent_str<< indent_str_next << "Runtime Dynamic = " << router->arbiter.rt_power.readOp.dynamic/nocdynp.executionTime << " W" << endl;
				cerr <<endl;
			}
		}
		if (link_bus_exist)
		{
			cerr << indent_str << (nocdynp.type? "Per Router ":"") << link_name<<": " << endl;
			cerr << indent_str_next << "Area = " << link_bus_tot_per_Router.area.get_area()*1e-6<< " mm^2" << endl;
			cerr << indent_str_next<< "Peak Dynamic = " << link_bus_tot_per_Router.power.readOp.dynamic*
				nocdynp.clockRate << " W" << endl;
			cerr << indent_str_next << "Subthreshold Leakage = "
			<< (long_channel? link_bus_tot_per_Router.power.readOp.longer_channel_leakage:link_bus_tot_per_Router.power.readOp.leakage)
			     <<" W" << endl;
			cerr << indent_str_next << "Gate Leakage = " << link_bus_tot_per_Router.power.readOp.gate_leakage
				<< " W" << endl;
			cerr << indent_str_next<< "Runtime Dynamic = " << link_bus->rt_power.readOp.dynamic/nocdynp.executionTime << " W" << endl;
			cerr<<endl;

		}
	}
	else
	{
//		cerr << indent_str_next << "Instruction Fetch Unit    Peak Dynamic = " << ifu->rt_power.readOp.dynamic*clockRate << " W" << endl;
//		cerr << indent_str_next << "Instruction Fetch Unit    Subthreshold Leakage = " << ifu->rt_power.readOp.leakage <<" W" << endl;
//		cerr << indent_str_next << "Instruction Fetch Unit    Gate Leakage = " << ifu->rt_power.readOp.gate_leakage << " W" << endl;
//		cerr << indent_str_next << "Load Store Unit   Peak Dynamic = " << lsu->rt_power.readOp.dynamic*clockRate  << " W" << endl;
//		cerr << indent_str_next << "Load Store Unit   Subthreshold Leakage = " << lsu->rt_power.readOp.leakage  << " W" << endl;
//		cerr << indent_str_next << "Load Store Unit   Gate Leakage = " << lsu->rt_power.readOp.gate_leakage  << " W" << endl;
//		cerr << indent_str_next << "Memory Management Unit   Peak Dynamic = " << mmu->rt_power.readOp.dynamic*clockRate  << " W" << endl;
//		cerr << indent_str_next << "Memory Management Unit   Subthreshold Leakage = " << mmu->rt_power.readOp.leakage  << " W" << endl;
//		cerr << indent_str_next << "Memory Management Unit   Gate Leakage = " << mmu->rt_power.readOp.gate_leakage  << " W" << endl;
//		cerr << indent_str_next << "Execution Unit   Peak Dynamic = " << exu->rt_power.readOp.dynamic*clockRate  << " W" << endl;
//		cerr << indent_str_next << "Execution Unit   Subthreshold Leakage = " << exu->rt_power.readOp.leakage  << " W" << endl;
//		cerr << indent_str_next << "Execution Unit   Gate Leakage = " << exu->rt_power.readOp.gate_leakage  << " W" << endl;
	}
}

void NoC::set_noc_param()
{

	nocdynp.type            = XML->sys.NoC[ithNoC].type;
	nocdynp.clockRate       =XML->sys.NoC[ithNoC].clockrate;
	nocdynp.clockRate       *= 1e6;
	nocdynp.executionTime   = XML->sys.total_cycles/(XML->sys.target_core_clockrate*1e6);

	nocdynp.flit_size     = XML->sys.NoC[ithNoC].flit_bits;
	if (nocdynp.type)
	{
		nocdynp.input_ports   = XML->sys.NoC[ithNoC].input_ports;
		nocdynp.output_ports  = XML->sys.NoC[ithNoC].output_ports;//later minus 1
		nocdynp.min_ports     = min(nocdynp.input_ports,nocdynp.output_ports);
		nocdynp.global_linked_ports = (nocdynp.input_ports-1) + (nocdynp.output_ports-1);
		/*
		 * 	Except local i/o ports, all ports needs links( global_linked_ports);
		 *  However only min_ports can be fully active simultaneously
		 *  since the fewer number of ports (input or output ) is the bottleneck.
		 */
	}
	else
	{
		nocdynp.input_ports   = 1;
		nocdynp.output_ports  = 1;
		nocdynp.min_ports     = min(nocdynp.input_ports,nocdynp.output_ports);
		nocdynp.global_linked_ports = 1;
	}

	nocdynp.virtual_channel_per_port     = XML->sys.NoC[ithNoC].virtual_channel_per_port;
	nocdynp.input_buffer_entries_per_vc  = XML->sys.NoC[ithNoC].input_buffer_entries_per_vc;

	nocdynp.horizontal_nodes  = XML->sys.NoC[ithNoC].horizontal_nodes;
	nocdynp.vertical_nodes    = XML->sys.NoC[ithNoC].vertical_nodes;
	nocdynp.total_nodes       = nocdynp.horizontal_nodes*nocdynp.vertical_nodes;
	nocdynp.duty_cycle        = XML->sys.NoC[ithNoC].duty_cycle;
	nocdynp.has_global_link   = XML->sys.NoC[ithNoC].has_global_link;
	nocdynp.link_throughput   = XML->sys.NoC[ithNoC].link_throughput;
	nocdynp.link_latency      = XML->sys.NoC[ithNoC].link_latency;
	nocdynp.chip_coverage     = XML->sys.NoC[ithNoC].chip_coverage;
	nocdynp.route_over_perc   = XML->sys.NoC[ithNoC].route_over_perc;

	assert (nocdynp.chip_coverage <=1);
	assert (nocdynp.route_over_perc <=1);

	if (nocdynp.type)
		name = "NOC";
	else
		name = "BUSES";

}

/* This is runtime leakage feedback control. It updates the leakage power for the given temperature.
   Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
void NoC::leakage_feedback(double temperature)
{
  interface_ip.temp = (unsigned int)temperature;
  uca_org_t init_result = init_interface(&interface_ip); // init_result is dummy

  if (router_exist)
  {
    router->leakage_feedback(temperature);

    double long_channel_device_reduction = longer_channel_device_reduction(Uncore_device);
    router->power.readOp.longer_channel_leakage = router->power.readOp.leakage * long_channel_device_reduction;
    router->buffer.power.readOp.longer_channel_leakage = router->buffer.power.readOp.leakage * long_channel_device_reduction;
    router->crossbar.power.readOp.longer_channel_leakage = router->crossbar.power.readOp.leakage * long_channel_device_reduction;
    router->arbiter.power.readOp.longer_channel_leakage = router->arbiter.power.readOp.leakage * long_channel_device_reduction;
  }

  if(link_bus_exist)
    link_bus->leakage_feedback(temperature);
}

NoC ::~NoC(){

	if(router) 	               {delete router; router = 0;}
	if(link_bus) 	           {delete link_bus; link_bus = 0;}
}
