/*------------------------------------------------------------
 *                              CACTI 6.5
 *         Copyright 2008 Hewlett-Packard Development Corporation
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein, and
 * hereby grant back to Hewlett-Packard Company and its affiliated companies ("HP")
 * a non-exclusive, unrestricted, royalty-free right and license under any changes,
 * enhancements or extensions  made to the core functions of the software, including
 * but not limited to those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to HP any such changes,
 * enhancements or extensions that they make and inform HP of noteworthy uses of
 * this software.  Correspondence should be provided to HP at:
 *
 *                       Director of Intellectual Property Licensing
 *                       Office of Strategy and Technology
 *                       Hewlett-Packard Company
 *                       1501 Page Mill Road
 *                       Palo Alto, California  94304
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HP DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL HP
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/


#include "basic_circuit.h"

#include "parameter.h"

double wire_resistance(double resistivity, double wire_width, double wire_thickness,
    double barrier_thickness, double dishing_thickness, double alpha_scatter)
{
  double resistance;
  resistance = alpha_scatter * resistivity /((wire_thickness - barrier_thickness - dishing_thickness)*(wire_width - 2 * barrier_thickness));
  return(resistance);
}

double wire_capacitance(double wire_width, double wire_thickness, double wire_spacing,
    double ild_thickness, double miller_value, double horiz_dielectric_constant,
    double vert_dielectric_constant, double fringe_cap)
{
  double vertical_cap, sidewall_cap, total_cap;
  vertical_cap = 2 * PERMITTIVITY_FREE_SPACE * vert_dielectric_constant * wire_width / ild_thickness;
  sidewall_cap = 2 * PERMITTIVITY_FREE_SPACE * miller_value * horiz_dielectric_constant * wire_thickness / wire_spacing;
  total_cap = vertical_cap + sidewall_cap + fringe_cap;
  return(total_cap);
}


void init_tech_params(double technology, bool is_tag)
{
  int    iter, tech, tech_lo, tech_hi;
  double curr_alpha, curr_vpp;
  double wire_width, wire_thickness, wire_spacing,
         fringe_cap, pmos_to_nmos_sizing_r;
//  double aspect_ratio,ild_thickness, miller_value = 1.5, horiz_dielectric_constant, vert_dielectric_constant;
  double barrier_thickness, dishing_thickness, alpha_scatter;
  double curr_vdd_dram_cell, curr_v_th_dram_access_transistor, curr_I_on_dram_cell, curr_c_dram_cell;

  uint32_t ram_cell_tech_type    = (is_tag) ? g_ip->tag_arr_ram_cell_tech_type : g_ip->data_arr_ram_cell_tech_type;
  uint32_t peri_global_tech_type = (is_tag) ? g_ip->tag_arr_peri_global_tech_type : g_ip->data_arr_peri_global_tech_type;

  technology  = technology * 1000.0;  // in the unit of nm

  // initialize parameters
  g_tp.reset();
  double gmp_to_gmn_multiplier_periph_global = 0;

  double curr_Wmemcella_dram, curr_Wmemcellpmos_dram, curr_Wmemcellnmos_dram,
         curr_area_cell_dram, curr_asp_ratio_cell_dram, curr_Wmemcella_sram,
         curr_Wmemcellpmos_sram, curr_Wmemcellnmos_sram, curr_area_cell_sram,
         curr_asp_ratio_cell_sram, curr_I_off_dram_cell_worst_case_length_temp;
  double curr_Wmemcella_cam, curr_Wmemcellpmos_cam, curr_Wmemcellnmos_cam, curr_area_cell_cam,//Sheng: CAM data
         curr_asp_ratio_cell_cam;
  double SENSE_AMP_D, SENSE_AMP_P; // J
  double area_cell_dram = 0;
  double asp_ratio_cell_dram = 0;
  double area_cell_sram = 0;
  double asp_ratio_cell_sram = 0;
  double area_cell_cam = 0;
  double asp_ratio_cell_cam = 0;
  double mobility_eff_periph_global = 0;
  double Vdsat_periph_global = 0;
  double nmos_effective_resistance_multiplier;
  double width_dram_access_transistor;

  double curr_logic_scaling_co_eff = 0;//This is based on the reported numbers of Intel Merom 65nm, Penryn45nm and IBM cell 90/65/45 date
  double curr_core_tx_density = 0;//this is density per um^2; 90, ...22nm based on Intel Penryn
  double curr_chip_layout_overhead = 0;
  double curr_macro_layout_overhead = 0;
  double curr_sckt_co_eff = 0;

  if (technology < 181 && technology > 179)
      {
        tech_lo = 180;
        tech_hi = 180;
      }
  else if (technology < 91 && technology > 89)
  {
    tech_lo = 90;
    tech_hi = 90;
  }
  else if (technology < 66 && technology > 64)
  {
    tech_lo = 65;
    tech_hi = 65;
  }
  else if (technology < 46 && technology > 44)
  {
    tech_lo = 45;
    tech_hi = 45;
  }
  else if (technology < 33 && technology > 31)
  {
    tech_lo = 32;
    tech_hi = 32;
  }
  else if (technology < 23 && technology > 21)
  {
    tech_lo = 22;
    tech_hi = 22;
    if (ram_cell_tech_type == 3 )
    {
       cout<<"current version does not support eDRAM technologies at 22nm"<<endl;
       exit(0);
    }
  }
//  else if (technology < 17 && technology > 15)
//  {
//    tech_lo = 16;
//    tech_hi = 16;
//  }
  else if (technology < 180 && technology > 90)
    {
      tech_lo = 180;
      tech_hi = 90;
    }
  else if (technology < 90 && technology > 65)
  {
    tech_lo = 90;
    tech_hi = 65;
  }
  else if (technology < 65 && technology > 45)
  {
    tech_lo = 65;
    tech_hi = 45;
  }
  else if (technology < 45 && technology > 32)
  {
    tech_lo = 45;
    tech_hi = 32;
  }
  else if (technology < 32 && technology > 22)
    {
      tech_lo = 32;
      tech_hi = 22;
    }
//  else if (technology < 22 && technology > 16)
//    {
//      tech_lo = 22;
//      tech_hi = 16;
//    }
      else
    {
  	  cout<<"Invalid technology nodes"<<endl;
  	  exit(0);
    }

  double vdd[NUMBER_TECH_FLAVORS];
  double Lphy[NUMBER_TECH_FLAVORS];
  double Lelec[NUMBER_TECH_FLAVORS];
  double t_ox[NUMBER_TECH_FLAVORS];
  double v_th[NUMBER_TECH_FLAVORS];
  double c_ox[NUMBER_TECH_FLAVORS];
  double mobility_eff[NUMBER_TECH_FLAVORS];
  double Vdsat[NUMBER_TECH_FLAVORS];
  double c_g_ideal[NUMBER_TECH_FLAVORS];
  double c_fringe[NUMBER_TECH_FLAVORS];
  double c_junc[NUMBER_TECH_FLAVORS];
  double I_on_n[NUMBER_TECH_FLAVORS];
  double I_on_p[NUMBER_TECH_FLAVORS];
  double Rnchannelon[NUMBER_TECH_FLAVORS];
  double Rpchannelon[NUMBER_TECH_FLAVORS];
  double n_to_p_eff_curr_drv_ratio[NUMBER_TECH_FLAVORS];
  double I_off_n[NUMBER_TECH_FLAVORS][101];
  double I_g_on_n[NUMBER_TECH_FLAVORS][101];
  //double I_off_p[NUMBER_TECH_FLAVORS][101];
  double gmp_to_gmn_multiplier[NUMBER_TECH_FLAVORS];
  //double curr_sckt_co_eff[NUMBER_TECH_FLAVORS];
  double long_channel_leakage_reduction[NUMBER_TECH_FLAVORS];

  for (iter = 0; iter <= 1; ++iter)
  {
    // linear interpolation
    if (iter == 0)
    {
      tech = tech_lo;
      if (tech_lo == tech_hi)
      {
        curr_alpha = 1;
      }
      else
      {
        curr_alpha = (technology - tech_hi)/(tech_lo - tech_hi);
      }
    }
    else
    {
      tech = tech_hi;
      if (tech_lo == tech_hi)
      {
        break;
      }
      else
      {
        curr_alpha = (tech_lo - technology)/(tech_lo - tech_hi);
      }
    }

    if (tech == 180)
    {
      //180nm technology-node. Corresponds to year 1999 in ITRS
      //Only HP transistor was of interest that 180nm since leakage power was not a big issue. Performance was the king
      //MASTAR does not contain data for 0.18um process. The following parameters are projected based on ITRS 2000 update and IBM 0.18 Cu Spice input
      bool Aggre_proj = false;
      SENSE_AMP_D = .28e-9; // s
      SENSE_AMP_P = 14.7e-15; // J
      vdd[0]   = 1.5;
      Lphy[0]  = 0.12;//Lphy is the physical gate-length. micron
      Lelec[0] = 0.10;//Lelec is the electrical gate-length. micron
      t_ox[0]  = 1.2e-3*(Aggre_proj? 1.9/1.2:2);//micron
      v_th[0]  = Aggre_proj? 0.36 : 0.4407;//V
      c_ox[0]  = 1.79e-14*(Aggre_proj? 1.9/1.2:2);//F/micron2
      mobility_eff[0] = 302.16 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
      Vdsat[0] = 0.128*2; //V
      c_g_ideal[0] = (Aggre_proj? 1.9/1.2:2)*6.64e-16;//F/micron
      c_fringe[0]  = (Aggre_proj? 1.9/1.2:2)*0.08e-15;//F/micron
      c_junc[0] = (Aggre_proj? 1.9/1.2:2)*1e-15;//F/micron2
      I_on_n[0] = 750e-6;//A/micron
      I_on_p[0] = 350e-6;//A/micron
      //Note that nmos_effective_resistance_multiplier, n_to_p_eff_curr_drv_ratio and gmp_to_gmn_multiplier values are calculated offline
      nmos_effective_resistance_multiplier = 1.54;
      n_to_p_eff_curr_drv_ratio[0] = 2.45;
      gmp_to_gmn_multiplier[0] = 1.22;
      Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];//ohm-micron
      Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];//ohm-micron
      long_channel_leakage_reduction[0] = 1;
      /*
      I_off_n[0][0]  = 7e-10;//A/micron
      I_off_n[0][10] = 8.26e-10;
      I_off_n[0][20] = 9.74e-10;
      I_off_n[0][30] = 1.15e-9;
      I_off_n[0][40] = 1.35e-9;
      I_off_n[0][50] = 1.60e-9;
      I_off_n[0][60] = 1.88e-9;
      I_off_n[0][70] = 2.29e-9;
      I_off_n[0][80] = 2.70e-9;
      I_off_n[0][90] = 3.19e-9;
      I_off_n[0][100] = 3.76e-9;

      I_g_on_n[0][0]  = 1.65e-10;//A/micron
      I_g_on_n[0][10] = 1.65e-10;
      I_g_on_n[0][20] = 1.65e-10;
      I_g_on_n[0][30] = 1.65e-10;
      I_g_on_n[0][40] = 1.65e-10;
      I_g_on_n[0][50] = 1.65e-10;
      I_g_on_n[0][60] = 1.65e-10;
      I_g_on_n[0][70] = 1.65e-10;
      I_g_on_n[0][80] = 1.65e-10;
      I_g_on_n[0][90] = 1.65e-10;
      I_g_on_n[0][100] = 1.65e-10;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[0][0] = 1.65e-10; 
      I_g_on_n[0][1] = 1.65e-10; I_g_on_n[0][2] = 1.65e-10; I_g_on_n[0][3] = 1.65e-10; I_g_on_n[0][4] = 1.65e-10; I_g_on_n[0][5] = 1.65e-10; 
      I_g_on_n[0][6] = 1.65e-10; I_g_on_n[0][7] = 1.65e-10; I_g_on_n[0][8] = 1.65e-10; I_g_on_n[0][9] = 1.65e-10; I_g_on_n[0][10] = 1.65e-10;
      I_g_on_n[0][11] = 1.65e-10; I_g_on_n[0][12] = 1.65e-10; I_g_on_n[0][13] = 1.65e-10; I_g_on_n[0][14] = 1.65e-10; I_g_on_n[0][15] = 1.65e-10; 
      I_g_on_n[0][16] = 1.65e-10; I_g_on_n[0][17] = 1.65e-10; I_g_on_n[0][18] = 1.65e-10; I_g_on_n[0][19] = 1.65e-10; I_g_on_n[0][20] = 1.65e-10;
      I_g_on_n[0][21] = 1.65e-10; I_g_on_n[0][22] = 1.65e-10; I_g_on_n[0][23] = 1.65e-10; I_g_on_n[0][24] = 1.65e-10; I_g_on_n[0][25] = 1.65e-10; 
      I_g_on_n[0][26] = 1.65e-10; I_g_on_n[0][27] = 1.65e-10; I_g_on_n[0][28] = 1.65e-10; I_g_on_n[0][29] = 1.65e-10; I_g_on_n[0][30] = 1.65e-10;
      I_g_on_n[0][31] = 1.65e-10; I_g_on_n[0][32] = 1.65e-10; I_g_on_n[0][33] = 1.65e-10; I_g_on_n[0][34] = 1.65e-10; I_g_on_n[0][35] = 1.65e-10; 
      I_g_on_n[0][36] = 1.65e-10; I_g_on_n[0][37] = 1.65e-10; I_g_on_n[0][38] = 1.65e-10; I_g_on_n[0][39] = 1.65e-10; I_g_on_n[0][40] = 1.65e-10;
      I_g_on_n[0][41] = 1.65e-10; I_g_on_n[0][42] = 1.65e-10; I_g_on_n[0][43] = 1.65e-10; I_g_on_n[0][44] = 1.65e-10; I_g_on_n[0][45] = 1.65e-10; 
      I_g_on_n[0][46] = 1.65e-10; I_g_on_n[0][47] = 1.65e-10; I_g_on_n[0][48] = 1.65e-10; I_g_on_n[0][49] = 1.65e-10; I_g_on_n[0][50] = 1.65e-10;
      I_g_on_n[0][51] = 1.65e-10; I_g_on_n[0][52] = 1.65e-10; I_g_on_n[0][53] = 1.65e-10; I_g_on_n[0][54] = 1.65e-10; I_g_on_n[0][55] = 1.65e-10; 
      I_g_on_n[0][56] = 1.65e-10; I_g_on_n[0][57] = 1.65e-10; I_g_on_n[0][58] = 1.65e-10; I_g_on_n[0][59] = 1.65e-10; I_g_on_n[0][60] = 1.65e-10;
      I_g_on_n[0][61] = 1.65e-10; I_g_on_n[0][62] = 1.65e-10; I_g_on_n[0][63] = 1.65e-10; I_g_on_n[0][64] = 1.65e-10; I_g_on_n[0][65] = 1.65e-10; 
      I_g_on_n[0][66] = 1.65e-10; I_g_on_n[0][67] = 1.65e-10; I_g_on_n[0][68] = 1.65e-10; I_g_on_n[0][69] = 1.65e-10; I_g_on_n[0][70] = 1.65e-10;
      I_g_on_n[0][71] = 1.65e-10; I_g_on_n[0][72] = 1.65e-10; I_g_on_n[0][73] = 1.65e-10; I_g_on_n[0][74] = 1.65e-10; I_g_on_n[0][75] = 1.65e-10; 
      I_g_on_n[0][76] = 1.65e-10; I_g_on_n[0][77] = 1.65e-10; I_g_on_n[0][78] = 1.65e-10; I_g_on_n[0][79] = 1.65e-10; I_g_on_n[0][80] = 1.65e-10;
      I_g_on_n[0][81] = 1.65e-10; I_g_on_n[0][82] = 1.65e-10; I_g_on_n[0][83] = 1.65e-10; I_g_on_n[0][84] = 1.65e-10; I_g_on_n[0][85] = 1.65e-10; 
      I_g_on_n[0][86] = 1.65e-10; I_g_on_n[0][87] = 1.65e-10; I_g_on_n[0][88] = 1.65e-10; I_g_on_n[0][89] = 1.65e-10; I_g_on_n[0][90] = 1.65e-10;
      I_g_on_n[0][91] = 1.65e-10; I_g_on_n[0][92] = 1.65e-10; I_g_on_n[0][93] = 1.65e-10; I_g_on_n[0][94] = 1.65e-10; I_g_on_n[0][95] = 1.65e-10; 
      I_g_on_n[0][96] = 1.65e-10; I_g_on_n[0][97] = 1.65e-10; I_g_on_n[0][98] = 1.65e-10; I_g_on_n[0][99] = 1.65e-10; I_g_on_n[0][100] = 1.65e-10;
      I_off_n[0][0] = 7.00e-10; 
      I_off_n[0][1] = 7.12e-10; I_off_n[0][2] = 7.23e-10; I_off_n[0][3] = 7.36e-10; I_off_n[0][4] = 7.48e-10; I_off_n[0][5] = 7.60e-10; 
      I_off_n[0][6] = 7.73e-10; I_off_n[0][7] = 7.86e-10; I_off_n[0][8] = 7.99e-10; I_off_n[0][9] = 8.12e-10; I_off_n[0][10] = 8.26e-10;
      I_off_n[0][11] = 8.40e-10; I_off_n[0][12] = 8.54e-10; I_off_n[0][13] = 8.68e-10; I_off_n[0][14] = 8.82e-10; I_off_n[0][15] = 8.97e-10; 
      I_off_n[0][16] = 9.12e-10; I_off_n[0][17] = 9.27e-10; I_off_n[0][18] = 9.42e-10; I_off_n[0][19] = 9.58e-10; I_off_n[0][20] = 9.74e-10;
      I_off_n[0][21] = 9.90e-10; I_off_n[0][22] = 1.01e-9; I_off_n[0][23] = 1.02e-9; I_off_n[0][24] = 1.04e-9; I_off_n[0][25] = 1.06e-9; 
      I_off_n[0][26] = 1.08e-9; I_off_n[0][27] = 1.09e-9; I_off_n[0][28] = 1.11e-9; I_off_n[0][29] = 1.13e-9; I_off_n[0][30] = 1.15e-9;
      I_off_n[0][31] = 1.17e-9; I_off_n[0][32] = 1.19e-9; I_off_n[0][33] = 1.21e-9; I_off_n[0][34] = 1.23e-9; I_off_n[0][35] = 1.25e-9; 
      I_off_n[0][36] = 1.27e-9; I_off_n[0][37] = 1.29e-9; I_off_n[0][38] = 1.31e-9; I_off_n[0][39] = 1.33e-9; I_off_n[0][40] = 1.35e-9;
      I_off_n[0][41] = 1.37e-9; I_off_n[0][42] = 1.40e-9; I_off_n[0][43] = 1.42e-9; I_off_n[0][44] = 1.44e-9; I_off_n[0][45] = 1.47e-9; 
      I_off_n[0][46] = 1.50e-9; I_off_n[0][47] = 1.52e-9; I_off_n[0][48] = 1.55e-9; I_off_n[0][49] = 1.57e-9; I_off_n[0][50] = 1.60e-9;
      I_off_n[0][51] = 1.63e-9; I_off_n[0][52] = 1.65e-9; I_off_n[0][53] = 1.68e-9; I_off_n[0][54] = 1.70e-9; I_off_n[0][55] = 1.73e-9; 
      I_off_n[0][56] = 1.76e-9; I_off_n[0][57] = 1.79e-9; I_off_n[0][58] = 1.82e-9; I_off_n[0][59] = 1.85e-9; I_off_n[0][60] = 1.88e-9;
      I_off_n[0][61] = 1.91e-9; I_off_n[0][62] = 1.95e-9; I_off_n[0][63] = 1.99e-9; I_off_n[0][64] = 2.03e-9; I_off_n[0][65] = 2.08e-9; 
      I_off_n[0][66] = 2.12e-9; I_off_n[0][67] = 2.16e-9; I_off_n[0][68] = 2.21e-9; I_off_n[0][69] = 2.25e-9; I_off_n[0][70] = 2.29e-9;
      I_off_n[0][71] = 2.33e-9; I_off_n[0][72] = 2.37e-9; I_off_n[0][73] = 2.41e-9; I_off_n[0][74] = 2.45e-9; I_off_n[0][75] = 2.49e-9; 
      I_off_n[0][76] = 2.53e-9; I_off_n[0][77] = 2.57e-9; I_off_n[0][78] = 2.61e-9; I_off_n[0][79] = 2.66e-9; I_off_n[0][80] = 2.70e-9;
      I_off_n[0][81] = 2.75e-9; I_off_n[0][82] = 2.79e-9; I_off_n[0][83] = 2.84e-9; I_off_n[0][84] = 2.89e-9; I_off_n[0][85] = 2.93e-9; 
      I_off_n[0][86] = 2.98e-9; I_off_n[0][87] = 3.03e-9; I_off_n[0][88] = 3.09e-9; I_off_n[0][89] = 3.14e-9; I_off_n[0][90] = 3.19e-9;
      I_off_n[0][91] = 3.24e-9; I_off_n[0][92] = 3.30e-9; I_off_n[0][93] = 3.35e-9; I_off_n[0][94] = 3.41e-9; I_off_n[0][95] = 3.46e-9; 
      I_off_n[0][96] = 3.52e-9; I_off_n[0][97] = 3.58e-9; I_off_n[0][98] = 3.64e-9; I_off_n[0][99] = 3.70e-9; I_off_n[0][100] = 3.76e-9;

      //SRAM cell properties
      curr_Wmemcella_sram = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
      curr_area_cell_sram = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_sram = 1.46;
      //CAM cell properties //TODO: data need to be revisited
      curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
      curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;//360
      curr_asp_ratio_cell_cam = 2.92;//2.5
      //Empirical undifferetiated core/FU coefficient
      curr_logic_scaling_co_eff  = 1.5;//linear scaling from 90nm
      curr_core_tx_density       = 1.25*0.7*0.7*0.4;
      curr_sckt_co_eff           = 1.11;
      curr_chip_layout_overhead  = 1.0;//die measurement results based on Niagara 1 and 2
      curr_macro_layout_overhead = 1.0;//EDA placement and routing tool rule of thumb

    }

    if (tech == 90)
    {
      SENSE_AMP_D = .28e-9; // s
      SENSE_AMP_P = 14.7e-15; // J
      //90nm technology-node. Corresponds to year 2004 in ITRS
      //ITRS HP device type
      vdd[0]   = 1.2;
      Lphy[0]  = 0.037;//Lphy is the physical gate-length. micron
      Lelec[0] = 0.0266;//Lelec is the electrical gate-length. micron
      t_ox[0]  = 1.2e-3;//micron
      v_th[0]  = 0.23707;//V
      c_ox[0]  = 1.79e-14;//F/micron2
      mobility_eff[0] = 342.16 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
      Vdsat[0] = 0.128; //V
      c_g_ideal[0] = 6.64e-16;//F/micron
      c_fringe[0]  = 0.08e-15;//F/micron
      c_junc[0] = 1e-15;//F/micron2
      I_on_n[0] = 1076.9e-6;//A/micron
      I_on_p[0] = 712.6e-6;//A/micron
      //Note that nmos_effective_resistance_multiplier, n_to_p_eff_curr_drv_ratio and gmp_to_gmn_multiplier values are calculated offline
      nmos_effective_resistance_multiplier = 1.54;
      n_to_p_eff_curr_drv_ratio[0] = 2.45;
      gmp_to_gmn_multiplier[0] = 1.22;
      Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];//ohm-micron
      Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];//ohm-micron
      long_channel_leakage_reduction[0] = 1;
      /*
      I_off_n[0][0]  = 3.24e-8;//A/micron
      I_off_n[0][10] = 4.01e-8;
      I_off_n[0][20] = 4.90e-8;
      I_off_n[0][30] = 5.92e-8;
      I_off_n[0][40] = 7.08e-8;
      I_off_n[0][50] = 8.38e-8;
      I_off_n[0][60] = 9.82e-8;
      I_off_n[0][70] = 1.14e-7;
      I_off_n[0][80] = 1.29e-7;
      I_off_n[0][90] = 1.43e-7;
      I_off_n[0][100] = 1.54e-7;

      I_g_on_n[0][0]  = 1.65e-8;//A/micron
      I_g_on_n[0][10] = 1.65e-8;
      I_g_on_n[0][20] = 1.65e-8;
      I_g_on_n[0][30] = 1.65e-8;
      I_g_on_n[0][40] = 1.65e-8;
      I_g_on_n[0][50] = 1.65e-8;
      I_g_on_n[0][60] = 1.65e-8;
      I_g_on_n[0][70] = 1.65e-8;
      I_g_on_n[0][80] = 1.65e-8;
      I_g_on_n[0][90] = 1.65e-8;
      I_g_on_n[0][100] = 1.65e-8;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[0][0] = 1.65e-8; 
      I_g_on_n[0][1] = 1.65e-8; I_g_on_n[0][2] = 1.65e-8; I_g_on_n[0][3] = 1.65e-8; I_g_on_n[0][4] = 1.65e-8; I_g_on_n[0][5] = 1.65e-8; 
      I_g_on_n[0][6] = 1.65e-8; I_g_on_n[0][7] = 1.65e-8; I_g_on_n[0][8] = 1.65e-8; I_g_on_n[0][9] = 1.65e-8; I_g_on_n[0][10] = 1.65e-8;
      I_g_on_n[0][11] = 1.65e-8; I_g_on_n[0][12] = 1.65e-8; I_g_on_n[0][13] = 1.65e-8; I_g_on_n[0][14] = 1.65e-8; I_g_on_n[0][15] = 1.65e-8; 
      I_g_on_n[0][16] = 1.65e-8; I_g_on_n[0][17] = 1.65e-8; I_g_on_n[0][18] = 1.65e-8; I_g_on_n[0][19] = 1.65e-8; I_g_on_n[0][20] = 1.65e-8;
      I_g_on_n[0][21] = 1.65e-8; I_g_on_n[0][22] = 1.65e-8; I_g_on_n[0][23] = 1.65e-8; I_g_on_n[0][24] = 1.65e-8; I_g_on_n[0][25] = 1.65e-8; 
      I_g_on_n[0][26] = 1.65e-8; I_g_on_n[0][27] = 1.65e-8; I_g_on_n[0][28] = 1.65e-8; I_g_on_n[0][29] = 1.65e-8; I_g_on_n[0][30] = 1.65e-8;
      I_g_on_n[0][31] = 1.65e-8; I_g_on_n[0][32] = 1.65e-8; I_g_on_n[0][33] = 1.65e-8; I_g_on_n[0][34] = 1.65e-8; I_g_on_n[0][35] = 1.65e-8; 
      I_g_on_n[0][36] = 1.65e-8; I_g_on_n[0][37] = 1.65e-8; I_g_on_n[0][38] = 1.65e-8; I_g_on_n[0][39] = 1.65e-8; I_g_on_n[0][40] = 1.65e-8;
      I_g_on_n[0][41] = 1.65e-8; I_g_on_n[0][42] = 1.65e-8; I_g_on_n[0][43] = 1.65e-8; I_g_on_n[0][44] = 1.65e-8; I_g_on_n[0][45] = 1.65e-8; 
      I_g_on_n[0][46] = 1.65e-8; I_g_on_n[0][47] = 1.65e-8; I_g_on_n[0][48] = 1.65e-8; I_g_on_n[0][49] = 1.65e-8; I_g_on_n[0][50] = 1.65e-8;
      I_g_on_n[0][51] = 1.65e-8; I_g_on_n[0][52] = 1.65e-8; I_g_on_n[0][53] = 1.65e-8; I_g_on_n[0][54] = 1.65e-8; I_g_on_n[0][55] = 1.65e-8; 
      I_g_on_n[0][56] = 1.65e-8; I_g_on_n[0][57] = 1.65e-8; I_g_on_n[0][58] = 1.65e-8; I_g_on_n[0][59] = 1.65e-8; I_g_on_n[0][60] = 1.65e-8;
      I_g_on_n[0][61] = 1.65e-8; I_g_on_n[0][62] = 1.65e-8; I_g_on_n[0][63] = 1.65e-8; I_g_on_n[0][64] = 1.65e-8; I_g_on_n[0][65] = 1.65e-8; 
      I_g_on_n[0][66] = 1.65e-8; I_g_on_n[0][67] = 1.65e-8; I_g_on_n[0][68] = 1.65e-8; I_g_on_n[0][69] = 1.65e-8; I_g_on_n[0][70] = 1.65e-8;
      I_g_on_n[0][71] = 1.65e-8; I_g_on_n[0][72] = 1.65e-8; I_g_on_n[0][73] = 1.65e-8; I_g_on_n[0][74] = 1.65e-8; I_g_on_n[0][75] = 1.65e-8; 
      I_g_on_n[0][76] = 1.65e-8; I_g_on_n[0][77] = 1.65e-8; I_g_on_n[0][78] = 1.65e-8; I_g_on_n[0][79] = 1.65e-8; I_g_on_n[0][80] = 1.65e-8;
      I_g_on_n[0][81] = 1.65e-8; I_g_on_n[0][82] = 1.65e-8; I_g_on_n[0][83] = 1.65e-8; I_g_on_n[0][84] = 1.65e-8; I_g_on_n[0][85] = 1.65e-8; 
      I_g_on_n[0][86] = 1.65e-8; I_g_on_n[0][87] = 1.65e-8; I_g_on_n[0][88] = 1.65e-8; I_g_on_n[0][89] = 1.65e-8; I_g_on_n[0][90] = 1.65e-8;
      I_g_on_n[0][91] = 1.65e-8; I_g_on_n[0][92] = 1.65e-8; I_g_on_n[0][93] = 1.65e-8; I_g_on_n[0][94] = 1.65e-8; I_g_on_n[0][95] = 1.65e-8; 
      I_g_on_n[0][96] = 1.65e-8; I_g_on_n[0][97] = 1.65e-8; I_g_on_n[0][98] = 1.65e-8; I_g_on_n[0][99] = 1.65e-8; I_g_on_n[0][100] = 1.65e-8;
      I_off_n[0][0] = 3.24e-8; 
      I_off_n[0][1] = 3.31e-8; I_off_n[0][2] = 3.38e-8; I_off_n[0][3] = 3.46e-8; I_off_n[0][4] = 3.53e-8; I_off_n[0][5] = 3.61e-8; 
      I_off_n[0][6] = 3.69e-8; I_off_n[0][7] = 3.77e-8; I_off_n[0][8] = 3.85e-8; I_off_n[0][9] = 3.93e-8; I_off_n[0][10] = 4.01e-8;
      I_off_n[0][11] = 4.09e-8; I_off_n[0][12] = 4.18e-8; I_off_n[0][13] = 4.26e-8; I_off_n[0][14] = 4.35e-8; I_off_n[0][15] = 4.44e-8; 
      I_off_n[0][16] = 4.53e-8; I_off_n[0][17] = 4.62e-8; I_off_n[0][18] = 4.71e-8; I_off_n[0][19] = 4.81e-8; I_off_n[0][20] = 4.90e-8;
      I_off_n[0][21] = 5.00e-8; I_off_n[0][22] = 5.09e-8; I_off_n[0][23] = 5.19e-8; I_off_n[0][24] = 5.29e-8; I_off_n[0][25] = 5.39e-8; 
      I_off_n[0][26] = 5.50e-8; I_off_n[0][27] = 5.60e-8; I_off_n[0][28] = 5.71e-8; I_off_n[0][29] = 5.81e-8; I_off_n[0][30] = 5.92e-8;
      I_off_n[0][31] = 6.03e-8; I_off_n[0][32] = 6.14e-8; I_off_n[0][33] = 6.25e-8; I_off_n[0][34] = 6.37e-8; I_off_n[0][35] = 6.48e-8; 
      I_off_n[0][36] = 6.60e-8; I_off_n[0][37] = 6.72e-8; I_off_n[0][38] = 6.84e-8; I_off_n[0][39] = 6.96e-8; I_off_n[0][40] = 7.08e-8;
      I_off_n[0][41] = 7.20e-8; I_off_n[0][42] = 7.33e-8; I_off_n[0][43] = 7.45e-8; I_off_n[0][44] = 7.58e-8; I_off_n[0][45] = 7.71e-8; 
      I_off_n[0][46] = 7.84e-8; I_off_n[0][47] = 7.98e-8; I_off_n[0][48] = 8.11e-8; I_off_n[0][49] = 8.24e-8; I_off_n[0][50] = 8.38e-8;
      I_off_n[0][51] = 8.52e-8; I_off_n[0][52] = 8.66e-8; I_off_n[0][53] = 8.80e-8; I_off_n[0][54] = 8.94e-8; I_off_n[0][55] = 9.08e-8; 
      I_off_n[0][56] = 9.23e-8; I_off_n[0][57] = 9.37e-8; I_off_n[0][58] = 9.52e-8; I_off_n[0][59] = 9.67e-8; I_off_n[0][60] = 9.82e-8;
      I_off_n[0][61] = 9.97e-8; I_off_n[0][62] = 1.01e-7; I_off_n[0][63] = 1.03e-7; I_off_n[0][64] = 1.04e-7; I_off_n[0][65] = 1.06e-7; 
      I_off_n[0][66] = 1.08e-7; I_off_n[0][67] = 1.09e-7; I_off_n[0][68] = 1.11e-7; I_off_n[0][69] = 1.12e-7; I_off_n[0][70] = 1.14e-7;
      I_off_n[0][71] = 1.16e-7; I_off_n[0][72] = 1.17e-7; I_off_n[0][73] = 1.19e-7; I_off_n[0][74] = 1.20e-7; I_off_n[0][75] = 1.22e-7; 
      I_off_n[0][76] = 1.23e-7; I_off_n[0][77] = 1.25e-7; I_off_n[0][78] = 1.26e-7; I_off_n[0][79] = 1.28e-7; I_off_n[0][80] = 1.29e-7;
      I_off_n[0][81] = 1.30e-7; I_off_n[0][82] = 1.32e-7; I_off_n[0][83] = 1.33e-7; I_off_n[0][84] = 1.35e-7; I_off_n[0][85] = 1.36e-7; 
      I_off_n[0][86] = 1.38e-7; I_off_n[0][87] = 1.39e-7; I_off_n[0][88] = 1.40e-7; I_off_n[0][89] = 1.42e-7; I_off_n[0][90] = 1.43e-7;
      I_off_n[0][91] = 1.44e-7; I_off_n[0][92] = 1.45e-7; I_off_n[0][93] = 1.47e-7; I_off_n[0][94] = 1.48e-7; I_off_n[0][95] = 1.49e-7; 
      I_off_n[0][96] = 1.50e-7; I_off_n[0][97] = 1.51e-7; I_off_n[0][98] = 1.52e-7; I_off_n[0][99] = 1.53e-7; I_off_n[0][100] = 1.54e-7;

      //ITRS LSTP device type
      vdd[1]   = 1.3;
      Lphy[1]  = 0.075;
      Lelec[1] = 0.0486;
      t_ox[1]  = 2.2e-3;
      v_th[1]  = 0.48203;
      c_ox[1]  = 1.22e-14;
      mobility_eff[1] = 356.76 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[1] = 0.373;
      c_g_ideal[1] = 9.15e-16;
      c_fringe[1]  = 0.08e-15;
      c_junc[1] = 1e-15;
      I_on_n[1] = 503.6e-6;
      I_on_p[1] = 235.1e-6;
      nmos_effective_resistance_multiplier = 1.92;
      n_to_p_eff_curr_drv_ratio[1] = 2.44;
      gmp_to_gmn_multiplier[1] =0.88;
      Rnchannelon[1] = nmos_effective_resistance_multiplier * vdd[1] / I_on_n[1];
      Rpchannelon[1] = n_to_p_eff_curr_drv_ratio[1] * Rnchannelon[1];
      long_channel_leakage_reduction[1] = 1;
      /*
      I_off_n[1][0]  = 2.81e-12;
      I_off_n[1][10] = 4.76e-12;
      I_off_n[1][20] = 7.82e-12;
      I_off_n[1][30] = 1.25e-11;
      I_off_n[1][40] = 1.94e-11;
      I_off_n[1][50] = 2.94e-11;
      I_off_n[1][60] = 4.36e-11;
      I_off_n[1][70] = 6.32e-11;
      I_off_n[1][80] = 8.95e-11;
      I_off_n[1][90] = 1.25e-10;
      I_off_n[1][100] = 1.7e-10;

      I_g_on_n[1][0]  = 3.87e-11;//A/micron
      I_g_on_n[1][10] = 3.87e-11;
      I_g_on_n[1][20] = 3.87e-11;
      I_g_on_n[1][30] = 3.87e-11;
      I_g_on_n[1][40] = 3.87e-11;
      I_g_on_n[1][50] = 3.87e-11;
      I_g_on_n[1][60] = 3.87e-11;
      I_g_on_n[1][70] = 3.87e-11;
      I_g_on_n[1][80] = 3.87e-11;
      I_g_on_n[1][90] = 3.87e-11;
      I_g_on_n[1][100] = 3.87e-11;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[1][0] = 3.87e-11; 
      I_g_on_n[1][1] = 3.87e-11; I_g_on_n[1][2] = 3.87e-11; I_g_on_n[1][3] = 3.87e-11; I_g_on_n[1][4] = 3.87e-11; I_g_on_n[1][5] = 3.87e-11; 
      I_g_on_n[1][6] = 3.87e-11; I_g_on_n[1][7] = 3.87e-11; I_g_on_n[1][8] = 3.87e-11; I_g_on_n[1][9] = 3.87e-11; I_g_on_n[1][10] = 3.87e-11;
      I_g_on_n[1][11] = 3.87e-11; I_g_on_n[1][12] = 3.87e-11; I_g_on_n[1][13] = 3.87e-11; I_g_on_n[1][14] = 3.87e-11; I_g_on_n[1][15] = 3.87e-11; 
      I_g_on_n[1][16] = 3.87e-11; I_g_on_n[1][17] = 3.87e-11; I_g_on_n[1][18] = 3.87e-11; I_g_on_n[1][19] = 3.87e-11; I_g_on_n[1][20] = 3.87e-11;
      I_g_on_n[1][21] = 3.87e-11; I_g_on_n[1][22] = 3.87e-11; I_g_on_n[1][23] = 3.87e-11; I_g_on_n[1][24] = 3.87e-11; I_g_on_n[1][25] = 3.87e-11; 
      I_g_on_n[1][26] = 3.87e-11; I_g_on_n[1][27] = 3.87e-11; I_g_on_n[1][28] = 3.87e-11; I_g_on_n[1][29] = 3.87e-11; I_g_on_n[1][30] = 3.87e-11;
      I_g_on_n[1][31] = 3.87e-11; I_g_on_n[1][32] = 3.87e-11; I_g_on_n[1][33] = 3.87e-11; I_g_on_n[1][34] = 3.87e-11; I_g_on_n[1][35] = 3.87e-11; 
      I_g_on_n[1][36] = 3.87e-11; I_g_on_n[1][37] = 3.87e-11; I_g_on_n[1][38] = 3.87e-11; I_g_on_n[1][39] = 3.87e-11; I_g_on_n[1][40] = 3.87e-11;
      I_g_on_n[1][41] = 3.87e-11; I_g_on_n[1][42] = 3.87e-11; I_g_on_n[1][43] = 3.87e-11; I_g_on_n[1][44] = 3.87e-11; I_g_on_n[1][45] = 3.87e-11; 
      I_g_on_n[1][46] = 3.87e-11; I_g_on_n[1][47] = 3.87e-11; I_g_on_n[1][48] = 3.87e-11; I_g_on_n[1][49] = 3.87e-11; I_g_on_n[1][50] = 3.87e-11;
      I_g_on_n[1][51] = 3.87e-11; I_g_on_n[1][52] = 3.87e-11; I_g_on_n[1][53] = 3.87e-11; I_g_on_n[1][54] = 3.87e-11; I_g_on_n[1][55] = 3.87e-11; 
      I_g_on_n[1][56] = 3.87e-11; I_g_on_n[1][57] = 3.87e-11; I_g_on_n[1][58] = 3.87e-11; I_g_on_n[1][59] = 3.87e-11; I_g_on_n[1][60] = 3.87e-11;
      I_g_on_n[1][61] = 3.87e-11; I_g_on_n[1][62] = 3.87e-11; I_g_on_n[1][63] = 3.87e-11; I_g_on_n[1][64] = 3.87e-11; I_g_on_n[1][65] = 3.87e-11; 
      I_g_on_n[1][66] = 3.87e-11; I_g_on_n[1][67] = 3.87e-11; I_g_on_n[1][68] = 3.87e-11; I_g_on_n[1][69] = 3.87e-11; I_g_on_n[1][70] = 3.87e-11;
      I_g_on_n[1][71] = 3.87e-11; I_g_on_n[1][72] = 3.87e-11; I_g_on_n[1][73] = 3.87e-11; I_g_on_n[1][74] = 3.87e-11; I_g_on_n[1][75] = 3.87e-11; 
      I_g_on_n[1][76] = 3.87e-11; I_g_on_n[1][77] = 3.87e-11; I_g_on_n[1][78] = 3.87e-11; I_g_on_n[1][79] = 3.87e-11; I_g_on_n[1][80] = 3.87e-11;
      I_g_on_n[1][81] = 3.87e-11; I_g_on_n[1][82] = 3.87e-11; I_g_on_n[1][83] = 3.87e-11; I_g_on_n[1][84] = 3.87e-11; I_g_on_n[1][85] = 3.87e-11; 
      I_g_on_n[1][86] = 3.87e-11; I_g_on_n[1][87] = 3.87e-11; I_g_on_n[1][88] = 3.87e-11; I_g_on_n[1][89] = 3.87e-11; I_g_on_n[1][90] = 3.87e-11;
      I_g_on_n[1][91] = 3.87e-11; I_g_on_n[1][92] = 3.87e-11; I_g_on_n[1][93] = 3.87e-11; I_g_on_n[1][94] = 3.87e-11; I_g_on_n[1][95] = 3.87e-11; 
      I_g_on_n[1][96] = 3.87e-11; I_g_on_n[1][97] = 3.87e-11; I_g_on_n[1][98] = 3.87e-11; I_g_on_n[1][99] = 3.87e-11; I_g_on_n[1][100] = 3.87e-11;
      I_off_n[1][0] = 2.81e-12; 
      I_off_n[1][1] = 2.96e-12; I_off_n[1][2] = 3.12e-12; I_off_n[1][3] = 3.29e-12; I_off_n[1][4] = 3.47e-12; I_off_n[1][5] = 3.66e-12; 
      I_off_n[1][6] = 3.86e-12; I_off_n[1][7] = 4.08e-12; I_off_n[1][8] = 4.30e-12; I_off_n[1][9] = 4.53e-12; I_off_n[1][10] = 4.76e-12;
      I_off_n[1][11] = 5.01e-12; I_off_n[1][12] = 5.26e-12; I_off_n[1][13] = 5.54e-12; I_off_n[1][14] = 5.82e-12; I_off_n[1][15] = 6.13e-12; 
      I_off_n[1][16] = 6.44e-12; I_off_n[1][17] = 6.77e-12; I_off_n[1][18] = 7.10e-12; I_off_n[1][19] = 7.46e-12; I_off_n[1][20] = 7.82e-12;
      I_off_n[1][21] = 8.20e-12; I_off_n[1][22] = 8.60e-12; I_off_n[1][23] = 9.02e-12; I_off_n[1][24] = 9.46e-12; I_off_n[1][25] = 9.93e-12; 
      I_off_n[1][26] = 1.04e-11; I_off_n[1][27] = 1.09e-11; I_off_n[1][28] = 1.14e-11; I_off_n[1][29] = 1.20e-11; I_off_n[1][30] = 1.25e-11;
      I_off_n[1][31] = 1.31e-11; I_off_n[1][32] = 1.37e-11; I_off_n[1][33] = 1.43e-11; I_off_n[1][34] = 1.49e-11; I_off_n[1][35] = 1.56e-11; 
      I_off_n[1][36] = 1.63e-11; I_off_n[1][37] = 1.71e-11; I_off_n[1][38] = 1.78e-11; I_off_n[1][39] = 1.86e-11; I_off_n[1][40] = 1.94e-11;
      I_off_n[1][41] = 2.02e-11; I_off_n[1][42] = 2.11e-11; I_off_n[1][43] = 2.20e-11; I_off_n[1][44] = 2.30e-11; I_off_n[1][45] = 2.40e-11; 
      I_off_n[1][46] = 2.50e-11; I_off_n[1][47] = 2.60e-11; I_off_n[1][48] = 2.71e-11; I_off_n[1][49] = 2.82e-11; I_off_n[1][50] = 2.94e-11;
      I_off_n[1][51] = 3.06e-11; I_off_n[1][52] = 3.19e-11; I_off_n[1][53] = 3.32e-11; I_off_n[1][54] = 3.45e-11; I_off_n[1][55] = 3.59e-11; 
      I_off_n[1][56] = 3.74e-11; I_off_n[1][57] = 3.89e-11; I_off_n[1][58] = 4.04e-11; I_off_n[1][59] = 4.20e-11; I_off_n[1][60] = 4.36e-11;
      I_off_n[1][61] = 4.53e-11; I_off_n[1][62] = 4.70e-11; I_off_n[1][63] = 4.88e-11; I_off_n[1][64] = 5.07e-11; I_off_n[1][65] = 5.27e-11; 
      I_off_n[1][66] = 5.46e-11; I_off_n[1][67] = 5.67e-11; I_off_n[1][68] = 5.88e-11; I_off_n[1][69] = 6.10e-11; I_off_n[1][70] = 6.32e-11;
      I_off_n[1][71] = 6.55e-11; I_off_n[1][72] = 6.78e-11; I_off_n[1][73] = 7.03e-11; I_off_n[1][74] = 7.28e-11; I_off_n[1][75] = 7.54e-11; 
      I_off_n[1][76] = 7.80e-11; I_off_n[1][77] = 8.08e-11; I_off_n[1][78] = 8.36e-11; I_off_n[1][79] = 8.65e-11; I_off_n[1][80] = 8.95e-11;
      I_off_n[1][81] = 9.26e-11; I_off_n[1][82] = 9.58e-11; I_off_n[1][83] = 9.91e-11; I_off_n[1][84] = 1.03e-10; I_off_n[1][85] = 1.06e-10; 
      I_off_n[1][86] = 1.10e-10; I_off_n[1][87] = 1.13e-10; I_off_n[1][88] = 1.17e-10; I_off_n[1][89] = 1.21e-10; I_off_n[1][90] = 1.25e-10;
      I_off_n[1][91] = 1.29e-10; I_off_n[1][92] = 1.33e-10; I_off_n[1][93] = 1.37e-10; I_off_n[1][94] = 1.42e-10; I_off_n[1][95] = 1.46e-10; 
      I_off_n[1][96] = 1.51e-10; I_off_n[1][97] = 1.55e-10; I_off_n[1][98] = 1.60e-10; I_off_n[1][99] = 1.65e-10; I_off_n[1][100] = 1.70e-10;

      //ITRS LOP device type
      vdd[2] = 0.9;
      Lphy[2] = 0.053;
      Lelec[2] = 0.0354;
      t_ox[2] = 1.5e-3;
      v_th[2] = 0.30764;
      c_ox[2] = 1.59e-14;
      mobility_eff[2] = 460.39 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[2] = 0.113;
      c_g_ideal[2] = 8.45e-16;
      c_fringe[2] = 0.08e-15;
      c_junc[2] = 1e-15;
      I_on_n[2] = 386.6e-6;
      I_on_p[2] = 209.7e-6;
      nmos_effective_resistance_multiplier = 1.77;
      n_to_p_eff_curr_drv_ratio[2] = 2.54;
      gmp_to_gmn_multiplier[2] = 0.98;
      Rnchannelon[2] = nmos_effective_resistance_multiplier * vdd[2] / I_on_n[2];
      Rpchannelon[2] = n_to_p_eff_curr_drv_ratio[2] * Rnchannelon[2];
      long_channel_leakage_reduction[2] = 1;
      /*
      I_off_n[2][0] = 2.14e-9;
      I_off_n[2][10] = 2.9e-9;
      I_off_n[2][20] = 3.87e-9;
      I_off_n[2][30] = 5.07e-9;
      I_off_n[2][40] = 6.54e-9;
      I_off_n[2][50] = 8.27e-8;
      I_off_n[2][60] = 1.02e-7;
      I_off_n[2][70] = 1.20e-7;
      I_off_n[2][80] = 1.36e-8;
      I_off_n[2][90] = 1.52e-8;
      I_off_n[2][100] = 1.73e-8;

      I_g_on_n[2][0]  = 4.31e-8;//A/micron
      I_g_on_n[2][10] = 4.31e-8;
      I_g_on_n[2][20] = 4.31e-8;
      I_g_on_n[2][30] = 4.31e-8;
      I_g_on_n[2][40] = 4.31e-8;
      I_g_on_n[2][50] = 4.31e-8;
      I_g_on_n[2][60] = 4.31e-8;
      I_g_on_n[2][70] = 4.31e-8;
      I_g_on_n[2][80] = 4.31e-8;
      I_g_on_n[2][90] = 4.31e-8;
      I_g_on_n[2][100] = 4.31e-8;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[2][0] = 4.31e-8; 
      I_g_on_n[2][1] = 4.31e-8; I_g_on_n[2][2] = 4.31e-8; I_g_on_n[2][3] = 4.31e-8; I_g_on_n[2][4] = 4.31e-8; I_g_on_n[2][5] = 4.31e-8; 
      I_g_on_n[2][6] = 4.31e-8; I_g_on_n[2][7] = 4.31e-8; I_g_on_n[2][8] = 4.31e-8; I_g_on_n[2][9] = 4.31e-8; I_g_on_n[2][10] = 4.31e-8;
      I_g_on_n[2][11] = 4.31e-8; I_g_on_n[2][12] = 4.31e-8; I_g_on_n[2][13] = 4.31e-8; I_g_on_n[2][14] = 4.31e-8; I_g_on_n[2][15] = 4.31e-8; 
      I_g_on_n[2][16] = 4.31e-8; I_g_on_n[2][17] = 4.31e-8; I_g_on_n[2][18] = 4.31e-8; I_g_on_n[2][19] = 4.31e-8; I_g_on_n[2][20] = 4.31e-8;
      I_g_on_n[2][21] = 4.31e-8; I_g_on_n[2][22] = 4.31e-8; I_g_on_n[2][23] = 4.31e-8; I_g_on_n[2][24] = 4.31e-8; I_g_on_n[2][25] = 4.31e-8; 
      I_g_on_n[2][26] = 4.31e-8; I_g_on_n[2][27] = 4.31e-8; I_g_on_n[2][28] = 4.31e-8; I_g_on_n[2][29] = 4.31e-8; I_g_on_n[2][30] = 4.31e-8;
      I_g_on_n[2][31] = 4.31e-8; I_g_on_n[2][32] = 4.31e-8; I_g_on_n[2][33] = 4.31e-8; I_g_on_n[2][34] = 4.31e-8; I_g_on_n[2][35] = 4.31e-8; 
      I_g_on_n[2][36] = 4.31e-8; I_g_on_n[2][37] = 4.31e-8; I_g_on_n[2][38] = 4.31e-8; I_g_on_n[2][39] = 4.31e-8; I_g_on_n[2][40] = 4.31e-8;
      I_g_on_n[2][41] = 4.31e-8; I_g_on_n[2][42] = 4.31e-8; I_g_on_n[2][43] = 4.31e-8; I_g_on_n[2][44] = 4.31e-8; I_g_on_n[2][45] = 4.31e-8; 
      I_g_on_n[2][46] = 4.31e-8; I_g_on_n[2][47] = 4.31e-8; I_g_on_n[2][48] = 4.31e-8; I_g_on_n[2][49] = 4.31e-8; I_g_on_n[2][50] = 4.31e-8;
      I_g_on_n[2][51] = 4.31e-8; I_g_on_n[2][52] = 4.31e-8; I_g_on_n[2][53] = 4.31e-8; I_g_on_n[2][54] = 4.31e-8; I_g_on_n[2][55] = 4.31e-8; 
      I_g_on_n[2][56] = 4.31e-8; I_g_on_n[2][57] = 4.31e-8; I_g_on_n[2][58] = 4.31e-8; I_g_on_n[2][59] = 4.31e-8; I_g_on_n[2][60] = 4.31e-8;
      I_g_on_n[2][61] = 4.31e-8; I_g_on_n[2][62] = 4.31e-8; I_g_on_n[2][63] = 4.31e-8; I_g_on_n[2][64] = 4.31e-8; I_g_on_n[2][65] = 4.31e-8; 
      I_g_on_n[2][66] = 4.31e-8; I_g_on_n[2][67] = 4.31e-8; I_g_on_n[2][68] = 4.31e-8; I_g_on_n[2][69] = 4.31e-8; I_g_on_n[2][70] = 4.31e-8;
      I_g_on_n[2][71] = 4.31e-8; I_g_on_n[2][72] = 4.31e-8; I_g_on_n[2][73] = 4.31e-8; I_g_on_n[2][74] = 4.31e-8; I_g_on_n[2][75] = 4.31e-8; 
      I_g_on_n[2][76] = 4.31e-8; I_g_on_n[2][77] = 4.31e-8; I_g_on_n[2][78] = 4.31e-8; I_g_on_n[2][79] = 4.31e-8; I_g_on_n[2][80] = 4.31e-8;
      I_g_on_n[2][81] = 4.31e-8; I_g_on_n[2][82] = 4.31e-8; I_g_on_n[2][83] = 4.31e-8; I_g_on_n[2][84] = 4.31e-8; I_g_on_n[2][85] = 4.31e-8; 
      I_g_on_n[2][86] = 4.31e-8; I_g_on_n[2][87] = 4.31e-8; I_g_on_n[2][88] = 4.31e-8; I_g_on_n[2][89] = 4.31e-8; I_g_on_n[2][90] = 4.31e-8;
      I_g_on_n[2][91] = 4.31e-8; I_g_on_n[2][92] = 4.31e-8; I_g_on_n[2][93] = 4.31e-8; I_g_on_n[2][94] = 4.31e-8; I_g_on_n[2][95] = 4.31e-8; 
      I_g_on_n[2][96] = 4.31e-8; I_g_on_n[2][97] = 4.31e-8; I_g_on_n[2][98] = 4.31e-8; I_g_on_n[2][99] = 4.31e-8; I_g_on_n[2][100] = 4.31e-8;
      I_off_n[2][0] = 2.14e-9; 
      I_off_n[2][1] = 2.21e-9; I_off_n[2][2] = 2.28e-9; I_off_n[2][3] = 2.35e-9; I_off_n[2][4] = 2.42e-9; I_off_n[2][5] = 2.50e-9; 
      I_off_n[2][6] = 2.57e-9; I_off_n[2][7] = 2.65e-9; I_off_n[2][8] = 2.73e-9; I_off_n[2][9] = 2.82e-9; I_off_n[2][10] = 2.90e-9;
      I_off_n[2][11] = 2.99e-9; I_off_n[2][12] = 3.08e-9; I_off_n[2][13] = 3.17e-9; I_off_n[2][14] = 3.26e-9; I_off_n[2][15] = 3.36e-9; 
      I_off_n[2][16] = 3.46e-9; I_off_n[2][17] = 3.56e-9; I_off_n[2][18] = 3.66e-9; I_off_n[2][19] = 3.76e-9; I_off_n[2][20] = 3.87e-9;
      I_off_n[2][21] = 3.98e-9; I_off_n[2][22] = 4.09e-9; I_off_n[2][23] = 4.20e-9; I_off_n[2][24] = 4.32e-9; I_off_n[2][25] = 4.44e-9; 
      I_off_n[2][26] = 4.56e-9; I_off_n[2][27] = 4.68e-9; I_off_n[2][28] = 4.81e-9; I_off_n[2][29] = 4.94e-9; I_off_n[2][30] = 5.07e-9;
      I_off_n[2][31] = 5.20e-9; I_off_n[2][32] = 5.34e-9; I_off_n[2][33] = 5.48e-9; I_off_n[2][34] = 5.63e-9; I_off_n[2][35] = 5.77e-9; 
      I_off_n[2][36] = 5.92e-9; I_off_n[2][37] = 6.07e-9; I_off_n[2][38] = 6.23e-9; I_off_n[2][39] = 6.38e-9; I_off_n[2][40] = 6.54e-9;
      I_off_n[2][41] = 6.70e-9; I_off_n[2][42] = 6.86e-9; I_off_n[2][43] = 7.03e-9; I_off_n[2][44] = 7.20e-9; I_off_n[2][45] = 7.38e-9; 
      I_off_n[2][46] = 7.55e-9; I_off_n[2][47] = 7.73e-9; I_off_n[2][48] = 7.91e-9; I_off_n[2][49] = 8.09e-9; I_off_n[2][50] = 8.27e-9;
      I_off_n[2][51] = 8.46e-9; I_off_n[2][52] = 8.64e-9; I_off_n[2][53] = 8.84e-9; I_off_n[2][54] = 9.03e-9; I_off_n[2][55] = 9.23e-9; 
      I_off_n[2][56] = 9.43e-9; I_off_n[2][57] = 9.62e-9; I_off_n[2][58] = 9.82e-9; I_off_n[2][59] = 1.00e-8; I_off_n[2][60] = 1.02e-8;
      I_off_n[2][61] = 1.04e-8; I_off_n[2][62] = 1.06e-8; I_off_n[2][63] = 1.08e-8; I_off_n[2][64] = 1.09e-8; I_off_n[2][65] = 1.11e-8; 
      I_off_n[2][66] = 1.13e-8; I_off_n[2][67] = 1.15e-8; I_off_n[2][68] = 1.17e-8; I_off_n[2][69] = 1.18e-8; I_off_n[2][70] = 1.20e-8;
      I_off_n[2][71] = 1.22e-8; I_off_n[2][72] = 1.23e-8; I_off_n[2][73] = 1.25e-8; I_off_n[2][74] = 1.27e-8; I_off_n[2][75] = 1.28e-8; 
      I_off_n[2][76] = 1.30e-8; I_off_n[2][77] = 1.31e-8; I_off_n[2][78] = 1.33e-8; I_off_n[2][79] = 1.34e-8; I_off_n[2][80] = 1.36e-8;
      I_off_n[2][81] = 1.38e-8; I_off_n[2][82] = 1.39e-8; I_off_n[2][83] = 1.41e-8; I_off_n[2][84] = 1.42e-8; I_off_n[2][85] = 1.44e-8; 
      I_off_n[2][86] = 1.45e-8; I_off_n[2][87] = 1.47e-8; I_off_n[2][88] = 1.49e-8; I_off_n[2][89] = 1.50e-8; I_off_n[2][90] = 1.52e-8;
      I_off_n[2][91] = 1.54e-8; I_off_n[2][92] = 1.56e-8; I_off_n[2][93] = 1.58e-8; I_off_n[2][94] = 1.60e-8; I_off_n[2][95] = 1.62e-8; 
      I_off_n[2][96] = 1.64e-8; I_off_n[2][97] = 1.66e-8; I_off_n[2][98] = 1.68e-8; I_off_n[2][99] = 1.71e-8; I_off_n[2][100] = 1.73e-8;

      if (ram_cell_tech_type == lp_dram)
      {
        //LP-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.2;
        Lphy[3] = 0.12;
        Lelec[3] = 0.0756;
        curr_v_th_dram_access_transistor = 0.4545;
        width_dram_access_transistor = 0.14;
        curr_I_on_dram_cell = 45e-6;
        curr_I_off_dram_cell_worst_case_length_temp = 21.1e-12;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram = 0;
        curr_area_cell_dram = 0.168;
        curr_asp_ratio_cell_dram = 1.46;
        curr_c_dram_cell = 20e-15;

        //LP-DRAM wordline transistor parameters
        curr_vpp = 1.6;
        t_ox[3] = 2.2e-3;
        v_th[3] = 0.4545;
        c_ox[3] = 1.22e-14;
        mobility_eff[3] =  323.95 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.3;
        c_g_ideal[3] = 1.47e-15;
        c_fringe[3] = 0.08e-15;
        c_junc[3] = 1e-15;
        I_on_n[3] = 321.6e-6;
        I_on_p[3] = 203.3e-6;
        nmos_effective_resistance_multiplier = 1.65;
        n_to_p_eff_curr_drv_ratio[3] = 1.95;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0] = 1.42e-11;
        I_off_n[3][10] = 2.25e-11;
        I_off_n[3][20] = 3.46e-11;
        I_off_n[3][30] = 5.18e-11;
        I_off_n[3][40] = 7.58e-11;
        I_off_n[3][50] = 1.08e-10;
        I_off_n[3][60] = 1.51e-10;
        I_off_n[3][70] = 2.02e-10;
        I_off_n[3][80] = 2.57e-10;
        I_off_n[3][90] = 3.14e-10;
        I_off_n[3][100] = 3.85e-10;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 1.42e-11; 
        I_off_n[3][1] = 1.49e-11; I_off_n[3][2] = 1.56e-11; I_off_n[3][3] = 1.63e-11; I_off_n[3][4] = 1.71e-11; I_off_n[3][5] = 1.79e-11; 
        I_off_n[3][6] = 1.88e-11; I_off_n[3][7] = 1.97e-11; I_off_n[3][8] = 2.06e-11; I_off_n[3][9] = 2.15e-11; I_off_n[3][10] = 2.25e-11;
        I_off_n[3][11] = 2.35e-11; I_off_n[3][12] = 2.46e-11; I_off_n[3][13] = 2.57e-11; I_off_n[3][14] = 2.68e-11; I_off_n[3][15] = 2.80e-11; 
        I_off_n[3][16] = 2.92e-11; I_off_n[3][17] = 3.05e-11; I_off_n[3][18] = 3.18e-11; I_off_n[3][19] = 3.32e-11; I_off_n[3][20] = 3.46e-11;
        I_off_n[3][21] = 3.61e-11; I_off_n[3][22] = 3.76e-11; I_off_n[3][23] = 3.91e-11; I_off_n[3][24] = 4.08e-11; I_off_n[3][25] = 4.25e-11; 
        I_off_n[3][26] = 4.42e-11; I_off_n[3][27] = 4.60e-11; I_off_n[3][28] = 4.79e-11; I_off_n[3][29] = 4.98e-11; I_off_n[3][30] = 5.18e-11;
        I_off_n[3][31] = 5.38e-11; I_off_n[3][32] = 5.60e-11; I_off_n[3][33] = 5.82e-11; I_off_n[3][34] = 6.05e-11; I_off_n[3][35] = 6.29e-11; 
        I_off_n[3][36] = 6.53e-11; I_off_n[3][37] = 6.78e-11; I_off_n[3][38] = 7.04e-11; I_off_n[3][39] = 7.31e-11; I_off_n[3][40] = 7.58e-11;
        I_off_n[3][41] = 7.86e-11; I_off_n[3][42] = 8.15e-11; I_off_n[3][43] = 8.45e-11; I_off_n[3][44] = 8.76e-11; I_off_n[3][45] = 9.07e-11; 
        I_off_n[3][46] = 9.40e-11; I_off_n[3][47] = 9.74e-11; I_off_n[3][48] = 1.01e-10; I_off_n[3][49] = 1.04e-10; I_off_n[3][50] = 1.08e-10;
        I_off_n[3][51] = 1.12e-10; I_off_n[3][52] = 1.16e-10; I_off_n[3][53] = 1.20e-10; I_off_n[3][54] = 1.24e-10; I_off_n[3][55] = 1.28e-10; 
        I_off_n[3][56] = 1.33e-10; I_off_n[3][57] = 1.37e-10; I_off_n[3][58] = 1.42e-10; I_off_n[3][59] = 1.46e-10; I_off_n[3][60] = 1.51e-10;
        I_off_n[3][61] = 1.56e-10; I_off_n[3][62] = 1.61e-10; I_off_n[3][63] = 1.66e-10; I_off_n[3][64] = 1.71e-10; I_off_n[3][65] = 1.76e-10; 
        I_off_n[3][66] = 1.81e-10; I_off_n[3][67] = 1.86e-10; I_off_n[3][68] = 1.91e-10; I_off_n[3][69] = 1.97e-10; I_off_n[3][70] = 2.02e-10;
        I_off_n[3][71] = 2.07e-10; I_off_n[3][72] = 2.13e-10; I_off_n[3][73] = 2.18e-10; I_off_n[3][74] = 2.24e-10; I_off_n[3][75] = 2.29e-10; 
        I_off_n[3][76] = 2.35e-10; I_off_n[3][77] = 2.40e-10; I_off_n[3][78] = 2.46e-10; I_off_n[3][79] = 2.51e-10; I_off_n[3][80] = 2.57e-10;
        I_off_n[3][81] = 2.63e-10; I_off_n[3][82] = 2.68e-10; I_off_n[3][83] = 2.74e-10; I_off_n[3][84] = 2.79e-10; I_off_n[3][85] = 2.85e-10; 
        I_off_n[3][86] = 2.90e-10; I_off_n[3][87] = 2.96e-10; I_off_n[3][88] = 3.02e-10; I_off_n[3][89] = 3.08e-10; I_off_n[3][90] = 3.14e-10;
        I_off_n[3][91] = 3.20e-10; I_off_n[3][92] = 3.27e-10; I_off_n[3][93] = 3.34e-10; I_off_n[3][94] = 3.41e-10; I_off_n[3][95] = 3.48e-10; 
        I_off_n[3][96] = 3.55e-10; I_off_n[3][97] = 3.62e-10; I_off_n[3][98] = 3.70e-10; I_off_n[3][99] = 3.77e-10; I_off_n[3][100] = 3.85e-10;
      }
      else if (ram_cell_tech_type == comm_dram)
      {
        //COMM-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.6;
        Lphy[3] = 0.09;
        Lelec[3] = 0.0576;
        curr_v_th_dram_access_transistor = 1;
        width_dram_access_transistor = 0.09;
        curr_I_on_dram_cell = 20e-6;
        curr_I_off_dram_cell_worst_case_length_temp = 1e-15;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram = 0;
        curr_area_cell_dram = 6*0.09*0.09;
        curr_asp_ratio_cell_dram = 1.5;
        curr_c_dram_cell = 30e-15;

        //COMM-DRAM wordline transistor parameters
        curr_vpp = 3.7;
        t_ox[3] = 5.5e-3;
        v_th[3] = 1.0;
        c_ox[3] = 5.65e-15;
        mobility_eff[3] =  302.2 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.32;
        c_g_ideal[3] = 5.08e-16;
        c_fringe[3] = 0.08e-15;
        c_junc[3] = 1e-15;
        I_on_n[3] = 1094.3e-6;
        I_on_p[3] = I_on_n[3] / 2;
        nmos_effective_resistance_multiplier = 1.62;
        n_to_p_eff_curr_drv_ratio[3] = 2.05;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0] = 5.80e-15;
        I_off_n[3][10] = 1.21e-14;
        I_off_n[3][20] = 2.42e-14;
        I_off_n[3][30] = 4.65e-14;
        I_off_n[3][40] = 8.60e-14;
        I_off_n[3][50] = 1.54e-13;
        I_off_n[3][60] = 2.66e-13;
        I_off_n[3][70] = 4.45e-13;
        I_off_n[3][80] = 7.17e-13;
        I_off_n[3][90] = 1.11e-12;
        I_off_n[3][100] = 1.67e-12;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 5.80e-15; 
        I_off_n[3][1] = 6.18e-15; I_off_n[3][2] = 6.63e-15; I_off_n[3][3] = 7.14e-15; I_off_n[3][4] = 7.71e-15; I_off_n[3][5] = 8.34e-15; 
        I_off_n[3][6] = 9.02e-15; I_off_n[3][7] = 9.74e-15; I_off_n[3][8] = 1.05e-14; I_off_n[3][9] = 1.13e-14; I_off_n[3][10] = 1.21e-14;
        I_off_n[3][11] = 1.30e-14; I_off_n[3][12] = 1.39e-14; I_off_n[3][13] = 1.49e-14; I_off_n[3][14] = 1.60e-14; I_off_n[3][15] = 1.72e-14; 
        I_off_n[3][16] = 1.85e-14; I_off_n[3][17] = 1.98e-14; I_off_n[3][18] = 2.12e-14; I_off_n[3][19] = 2.27e-14; I_off_n[3][20] = 2.42e-14;
        I_off_n[3][21] = 2.58e-14; I_off_n[3][22] = 2.76e-14; I_off_n[3][23] = 2.95e-14; I_off_n[3][24] = 3.16e-14; I_off_n[3][25] = 3.37e-14; 
        I_off_n[3][26] = 3.61e-14; I_off_n[3][27] = 3.85e-14; I_off_n[3][28] = 4.10e-14; I_off_n[3][29] = 4.37e-14; I_off_n[3][30] = 4.65e-14;
        I_off_n[3][31] = 4.95e-14; I_off_n[3][32] = 5.27e-14; I_off_n[3][33] = 5.61e-14; I_off_n[3][34] = 5.97e-14; I_off_n[3][35] = 6.36e-14; 
        I_off_n[3][36] = 6.76e-14; I_off_n[3][37] = 7.19e-14; I_off_n[3][38] = 7.64e-14; I_off_n[3][39] = 8.11e-14; I_off_n[3][40] = 8.60e-14;
        I_off_n[3][41] = 9.12e-14; I_off_n[3][42] = 9.68e-14; I_off_n[3][43] = 1.03e-13; I_off_n[3][44] = 1.09e-13; I_off_n[3][45] = 1.16e-13; 
        I_off_n[3][46] = 1.23e-13; I_off_n[3][47] = 1.30e-13; I_off_n[3][48] = 1.38e-13; I_off_n[3][49] = 1.46e-13; I_off_n[3][50] = 1.54e-13;
        I_off_n[3][51] = 1.63e-13; I_off_n[3][52] = 1.72e-13; I_off_n[3][53] = 1.82e-13; I_off_n[3][54] = 1.92e-13; I_off_n[3][55] = 2.03e-13; 
        I_off_n[3][56] = 2.15e-13; I_off_n[3][57] = 2.27e-13; I_off_n[3][58] = 2.39e-13; I_off_n[3][59] = 2.52e-13; I_off_n[3][60] = 2.66e-13;
        I_off_n[3][61] = 2.80e-13; I_off_n[3][62] = 2.95e-13; I_off_n[3][63] = 3.11e-13; I_off_n[3][64] = 3.28e-13; I_off_n[3][65] = 3.46e-13; 
        I_off_n[3][66] = 3.64e-13; I_off_n[3][67] = 3.83e-13; I_off_n[3][68] = 4.03e-13; I_off_n[3][69] = 4.24e-13; I_off_n[3][70] = 4.45e-13;
        I_off_n[3][71] = 4.67e-13; I_off_n[3][72] = 4.91e-13; I_off_n[3][73] = 5.15e-13; I_off_n[3][74] = 5.41e-13; I_off_n[3][75] = 5.68e-13; 
        I_off_n[3][76] = 5.96e-13; I_off_n[3][77] = 6.25e-13; I_off_n[3][78] = 6.54e-13; I_off_n[3][79] = 6.85e-13; I_off_n[3][80] = 7.17e-13;
        I_off_n[3][81] = 7.50e-13; I_off_n[3][82] = 7.84e-13; I_off_n[3][83] = 8.20e-13; I_off_n[3][84] = 8.57e-13; I_off_n[3][85] = 8.96e-13; 
        I_off_n[3][86] = 9.36e-13; I_off_n[3][87] = 9.77e-13; I_off_n[3][88] = 1.02e-12; I_off_n[3][89] = 1.06e-12; I_off_n[3][90] = 1.11e-12;
        I_off_n[3][91] = 1.16e-12; I_off_n[3][92] = 1.21e-12; I_off_n[3][93] = 1.26e-12; I_off_n[3][94] = 1.31e-12; I_off_n[3][95] = 1.37e-12; 
        I_off_n[3][96] = 1.42e-12; I_off_n[3][97] = 1.48e-12; I_off_n[3][98] = 1.54e-12; I_off_n[3][99] = 1.61e-12; I_off_n[3][100] = 1.67e-12;
      }

      //SRAM cell properties
      curr_Wmemcella_sram = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
      curr_area_cell_sram = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_sram = 1.46;
      //CAM cell properties //TODO: data need to be revisited
      curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
      curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;//360
      curr_asp_ratio_cell_cam = 2.92;//2.5
      //Empirical undifferetiated core/FU coefficient
      curr_logic_scaling_co_eff  = 1;
      curr_core_tx_density       = 1.25*0.7*0.7;
      curr_sckt_co_eff           = 1.1539;
      curr_chip_layout_overhead  = 1.2;//die measurement results based on Niagara 1 and 2
      curr_macro_layout_overhead = 1.1;//EDA placement and routing tool rule of thumb


    }

    if (tech == 65)
    { //65nm technology-node. Corresponds to year 2007 in ITRS
      //ITRS HP device type
      SENSE_AMP_D = .2e-9; // s
      SENSE_AMP_P = 5.7e-15; // J
      vdd[0] = 1.1;
      Lphy[0] = 0.025;
      Lelec[0] = 0.019;
      t_ox[0] = 1.1e-3;
      v_th[0] = .19491;
      c_ox[0] = 1.88e-14;
      mobility_eff[0] = 436.24 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[0] = 7.71e-2;
      c_g_ideal[0] = 4.69e-16;
      c_fringe[0] = 0.077e-15;
      c_junc[0] = 1e-15;
      I_on_n[0] = 1197.2e-6;
      I_on_p[0] = 870.8e-6;
      nmos_effective_resistance_multiplier = 1.50;
      n_to_p_eff_curr_drv_ratio[0] = 2.41;
      gmp_to_gmn_multiplier[0] = 1.38;
      Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];
      Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];
      long_channel_leakage_reduction[0] = 1/3.74;
      //Using MASTAR, @380K, increase Lgate until Ion reduces to 90% or Lgate increase by 10%, whichever comes first
      //Ioff(Lgate normal)/Ioff(Lgate long)= 3.74.
      /*
      I_off_n[0][0] = 1.96e-7;
      I_off_n[0][10] = 2.29e-7;
      I_off_n[0][20] = 2.66e-7;
      I_off_n[0][30] = 3.05e-7;
      I_off_n[0][40] = 3.49e-7;
      I_off_n[0][50] = 3.95e-7;
      I_off_n[0][60] = 4.45e-7;
      I_off_n[0][70] = 4.97e-7;
      I_off_n[0][80] = 5.48e-7;
      I_off_n[0][90] = 5.94e-7;
      I_off_n[0][100] = 6.3e-7;
      I_g_on_n[0][0]  = 4.09e-8;//A/micron
      I_g_on_n[0][10] = 4.09e-8;
      I_g_on_n[0][20] = 4.09e-8;
      I_g_on_n[0][30] = 4.09e-8;
      I_g_on_n[0][40] = 4.09e-8;
      I_g_on_n[0][50] = 4.09e-8;
      I_g_on_n[0][60] = 4.09e-8;
      I_g_on_n[0][70] = 4.09e-8;
      I_g_on_n[0][80] = 4.09e-8;
      I_g_on_n[0][90] = 4.09e-8;
      I_g_on_n[0][100] = 4.09e-8;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[0][0] = 4.09e-8; 
      I_g_on_n[0][1] = 4.09e-8; I_g_on_n[0][2] = 4.09e-8; I_g_on_n[0][3] = 4.09e-8; I_g_on_n[0][4] = 4.09e-8; I_g_on_n[0][5] = 4.09e-8; 
      I_g_on_n[0][6] = 4.09e-8; I_g_on_n[0][7] = 4.09e-8; I_g_on_n[0][8] = 4.09e-8; I_g_on_n[0][9] = 4.09e-8; I_g_on_n[0][10] = 4.09e-8;
      I_g_on_n[0][11] = 4.09e-8; I_g_on_n[0][12] = 4.09e-8; I_g_on_n[0][13] = 4.09e-8; I_g_on_n[0][14] = 4.09e-8; I_g_on_n[0][15] = 4.09e-8; 
      I_g_on_n[0][16] = 4.09e-8; I_g_on_n[0][17] = 4.09e-8; I_g_on_n[0][18] = 4.09e-8; I_g_on_n[0][19] = 4.09e-8; I_g_on_n[0][20] = 4.09e-8;
      I_g_on_n[0][21] = 4.09e-8; I_g_on_n[0][22] = 4.09e-8; I_g_on_n[0][23] = 4.09e-8; I_g_on_n[0][24] = 4.09e-8; I_g_on_n[0][25] = 4.09e-8; 
      I_g_on_n[0][26] = 4.09e-8; I_g_on_n[0][27] = 4.09e-8; I_g_on_n[0][28] = 4.09e-8; I_g_on_n[0][29] = 4.09e-8; I_g_on_n[0][30] = 4.09e-8;
      I_g_on_n[0][31] = 4.09e-8; I_g_on_n[0][32] = 4.09e-8; I_g_on_n[0][33] = 4.09e-8; I_g_on_n[0][34] = 4.09e-8; I_g_on_n[0][35] = 4.09e-8; 
      I_g_on_n[0][36] = 4.09e-8; I_g_on_n[0][37] = 4.09e-8; I_g_on_n[0][38] = 4.09e-8; I_g_on_n[0][39] = 4.09e-8; I_g_on_n[0][40] = 4.09e-8;
      I_g_on_n[0][41] = 4.09e-8; I_g_on_n[0][42] = 4.09e-8; I_g_on_n[0][43] = 4.09e-8; I_g_on_n[0][44] = 4.09e-8; I_g_on_n[0][45] = 4.09e-8; 
      I_g_on_n[0][46] = 4.09e-8; I_g_on_n[0][47] = 4.09e-8; I_g_on_n[0][48] = 4.09e-8; I_g_on_n[0][49] = 4.09e-8; I_g_on_n[0][50] = 4.09e-8;
      I_g_on_n[0][51] = 4.09e-8; I_g_on_n[0][52] = 4.09e-8; I_g_on_n[0][53] = 4.09e-8; I_g_on_n[0][54] = 4.09e-8; I_g_on_n[0][55] = 4.09e-8; 
      I_g_on_n[0][56] = 4.09e-8; I_g_on_n[0][57] = 4.09e-8; I_g_on_n[0][58] = 4.09e-8; I_g_on_n[0][59] = 4.09e-8; I_g_on_n[0][60] = 4.09e-8;
      I_g_on_n[0][61] = 4.09e-8; I_g_on_n[0][62] = 4.09e-8; I_g_on_n[0][63] = 4.09e-8; I_g_on_n[0][64] = 4.09e-8; I_g_on_n[0][65] = 4.09e-8; 
      I_g_on_n[0][66] = 4.09e-8; I_g_on_n[0][67] = 4.09e-8; I_g_on_n[0][68] = 4.09e-8; I_g_on_n[0][69] = 4.09e-8; I_g_on_n[0][70] = 4.09e-8;
      I_g_on_n[0][71] = 4.09e-8; I_g_on_n[0][72] = 4.09e-8; I_g_on_n[0][73] = 4.09e-8; I_g_on_n[0][74] = 4.09e-8; I_g_on_n[0][75] = 4.09e-8; 
      I_g_on_n[0][76] = 4.09e-8; I_g_on_n[0][77] = 4.09e-8; I_g_on_n[0][78] = 4.09e-8; I_g_on_n[0][79] = 4.09e-8; I_g_on_n[0][80] = 4.09e-8;
      I_g_on_n[0][81] = 4.09e-8; I_g_on_n[0][82] = 4.09e-8; I_g_on_n[0][83] = 4.09e-8; I_g_on_n[0][84] = 4.09e-8; I_g_on_n[0][85] = 4.09e-8; 
      I_g_on_n[0][86] = 4.09e-8; I_g_on_n[0][87] = 4.09e-8; I_g_on_n[0][88] = 4.09e-8; I_g_on_n[0][89] = 4.09e-8; I_g_on_n[0][90] = 4.09e-8;
      I_g_on_n[0][91] = 4.09e-8; I_g_on_n[0][92] = 4.09e-8; I_g_on_n[0][93] = 4.09e-8; I_g_on_n[0][94] = 4.09e-8; I_g_on_n[0][95] = 4.09e-8; 
      I_g_on_n[0][96] = 4.09e-8; I_g_on_n[0][97] = 4.09e-8; I_g_on_n[0][98] = 4.09e-8; I_g_on_n[0][99] = 4.09e-8; I_g_on_n[0][100] = 4.09e-8;
      I_off_n[0][0] = 1.96e-7; 
      I_off_n[0][1] = 1.99e-7; I_off_n[0][2] = 2.02e-7; I_off_n[0][3] = 2.05e-7; I_off_n[0][4] = 2.09e-7; I_off_n[0][5] = 2.12e-7; 
      I_off_n[0][6] = 2.15e-7; I_off_n[0][7] = 2.19e-7; I_off_n[0][8] = 2.22e-7; I_off_n[0][9] = 2.26e-7; I_off_n[0][10] = 2.29e-7;
      I_off_n[0][11] = 2.33e-7; I_off_n[0][12] = 2.36e-7; I_off_n[0][13] = 2.40e-7; I_off_n[0][14] = 2.43e-7; I_off_n[0][15] = 2.47e-7; 
      I_off_n[0][16] = 2.51e-7; I_off_n[0][17] = 2.55e-7; I_off_n[0][18] = 2.58e-7; I_off_n[0][19] = 2.62e-7; I_off_n[0][20] = 2.66e-7;
      I_off_n[0][21] = 2.70e-7; I_off_n[0][22] = 2.74e-7; I_off_n[0][23] = 2.77e-7; I_off_n[0][24] = 2.81e-7; I_off_n[0][25] = 2.85e-7; 
      I_off_n[0][26] = 2.89e-7; I_off_n[0][27] = 2.93e-7; I_off_n[0][28] = 2.97e-7; I_off_n[0][29] = 3.01e-7; I_off_n[0][30] = 3.05e-7;
      I_off_n[0][31] = 3.09e-7; I_off_n[0][32] = 3.13e-7; I_off_n[0][33] = 3.18e-7; I_off_n[0][34] = 3.22e-7; I_off_n[0][35] = 3.27e-7; 
      I_off_n[0][36] = 3.31e-7; I_off_n[0][37] = 3.35e-7; I_off_n[0][38] = 3.40e-7; I_off_n[0][39] = 3.44e-7; I_off_n[0][40] = 3.49e-7;
      I_off_n[0][41] = 3.53e-7; I_off_n[0][42] = 3.58e-7; I_off_n[0][43] = 3.63e-7; I_off_n[0][44] = 3.67e-7; I_off_n[0][45] = 3.72e-7; 
      I_off_n[0][46] = 3.76e-7; I_off_n[0][47] = 3.81e-7; I_off_n[0][48] = 3.86e-7; I_off_n[0][49] = 3.90e-7; I_off_n[0][50] = 3.95e-7;
      I_off_n[0][51] = 4.00e-7; I_off_n[0][52] = 4.05e-7; I_off_n[0][53] = 4.10e-7; I_off_n[0][54] = 4.15e-7; I_off_n[0][55] = 4.20e-7; 
      I_off_n[0][56] = 4.25e-7; I_off_n[0][57] = 4.30e-7; I_off_n[0][58] = 4.35e-7; I_off_n[0][59] = 4.40e-7; I_off_n[0][60] = 4.45e-7;
      I_off_n[0][61] = 4.50e-7; I_off_n[0][62] = 4.55e-7; I_off_n[0][63] = 4.60e-7; I_off_n[0][64] = 4.66e-7; I_off_n[0][65] = 4.71e-7; 
      I_off_n[0][66] = 4.76e-7; I_off_n[0][67] = 4.81e-7; I_off_n[0][68] = 4.87e-7; I_off_n[0][69] = 4.92e-7; I_off_n[0][70] = 4.97e-7;
      I_off_n[0][71] = 5.02e-7; I_off_n[0][72] = 5.07e-7; I_off_n[0][73] = 5.13e-7; I_off_n[0][74] = 5.18e-7; I_off_n[0][75] = 5.23e-7; 
      I_off_n[0][76] = 5.28e-7; I_off_n[0][77] = 5.33e-7; I_off_n[0][78] = 5.38e-7; I_off_n[0][79] = 5.43e-7; I_off_n[0][80] = 5.48e-7;
      I_off_n[0][81] = 5.53e-7; I_off_n[0][82] = 5.58e-7; I_off_n[0][83] = 5.63e-7; I_off_n[0][84] = 5.67e-7; I_off_n[0][85] = 5.72e-7; 
      I_off_n[0][86] = 5.77e-7; I_off_n[0][87] = 5.81e-7; I_off_n[0][88] = 5.86e-7; I_off_n[0][89] = 5.90e-7; I_off_n[0][90] = 5.94e-7;
      I_off_n[0][91] = 5.98e-7; I_off_n[0][92] = 6.02e-7; I_off_n[0][93] = 6.06e-7; I_off_n[0][94] = 6.10e-7; I_off_n[0][95] = 6.13e-7; 
      I_off_n[0][96] = 6.17e-7; I_off_n[0][97] = 6.20e-7; I_off_n[0][98] = 6.24e-7; I_off_n[0][99] = 6.27e-7; I_off_n[0][100] = 6.30e-7;

      //ITRS LSTP device type
      vdd[1] = 1.2;
      Lphy[1] = 0.045;
      Lelec[1] = 0.0298;
      t_ox[1] = 1.9e-3;
      v_th[1] = 0.52354;
      c_ox[1] = 1.36e-14;
      mobility_eff[1] = 341.21 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[1] = 0.128;
      c_g_ideal[1] = 6.14e-16;
      c_fringe[1] = 0.08e-15;
      c_junc[1] = 1e-15;
      I_on_n[1] = 519.2e-6;
      I_on_p[1] = 266e-6;
      nmos_effective_resistance_multiplier = 1.96;
      n_to_p_eff_curr_drv_ratio[1] = 2.23;
      gmp_to_gmn_multiplier[1] = 0.99;
      Rnchannelon[1] = nmos_effective_resistance_multiplier * vdd[1] / I_on_n[1];
      Rpchannelon[1] = n_to_p_eff_curr_drv_ratio[1] * Rnchannelon[1];
      long_channel_leakage_reduction[1] = 1/2.82;
      /*
      I_off_n[1][0] = 9.12e-12;
      I_off_n[1][10] = 1.49e-11;
      I_off_n[1][20] = 2.36e-11;
      I_off_n[1][30] = 3.64e-11;
      I_off_n[1][40] = 5.48e-11;
      I_off_n[1][50] = 8.05e-11;
      I_off_n[1][60] = 1.15e-10;
      I_off_n[1][70] = 1.59e-10;
      I_off_n[1][80] = 2.1e-10;
      I_off_n[1][90] = 2.62e-10;
      I_off_n[1][100] = 3.21e-10;

      I_g_on_n[1][0]  = 1.09e-10;//A/micron
      I_g_on_n[1][10] = 1.09e-10;
      I_g_on_n[1][20] = 1.09e-10;
      I_g_on_n[1][30] = 1.09e-10;
      I_g_on_n[1][40] = 1.09e-10;
      I_g_on_n[1][50] = 1.09e-10;
      I_g_on_n[1][60] = 1.09e-10;
      I_g_on_n[1][70] = 1.09e-10;
      I_g_on_n[1][80] = 1.09e-10;
      I_g_on_n[1][90] = 1.09e-10;
      I_g_on_n[1][100] = 1.09e-10;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[1][0] = 1.09e-10; 
      I_g_on_n[1][1] = 1.09e-10; I_g_on_n[1][2] = 1.09e-10; I_g_on_n[1][3] = 1.09e-10; I_g_on_n[1][4] = 1.09e-10; I_g_on_n[1][5] = 1.09e-10; 
      I_g_on_n[1][6] = 1.09e-10; I_g_on_n[1][7] = 1.09e-10; I_g_on_n[1][8] = 1.09e-10; I_g_on_n[1][9] = 1.09e-10; I_g_on_n[1][10] = 1.09e-10;
      I_g_on_n[1][11] = 1.09e-10; I_g_on_n[1][12] = 1.09e-10; I_g_on_n[1][13] = 1.09e-10; I_g_on_n[1][14] = 1.09e-10; I_g_on_n[1][15] = 1.09e-10; 
      I_g_on_n[1][16] = 1.09e-10; I_g_on_n[1][17] = 1.09e-10; I_g_on_n[1][18] = 1.09e-10; I_g_on_n[1][19] = 1.09e-10; I_g_on_n[1][20] = 1.09e-10;
      I_g_on_n[1][21] = 1.09e-10; I_g_on_n[1][22] = 1.09e-10; I_g_on_n[1][23] = 1.09e-10; I_g_on_n[1][24] = 1.09e-10; I_g_on_n[1][25] = 1.09e-10; 
      I_g_on_n[1][26] = 1.09e-10; I_g_on_n[1][27] = 1.09e-10; I_g_on_n[1][28] = 1.09e-10; I_g_on_n[1][29] = 1.09e-10; I_g_on_n[1][30] = 1.09e-10;
      I_g_on_n[1][31] = 1.09e-10; I_g_on_n[1][32] = 1.09e-10; I_g_on_n[1][33] = 1.09e-10; I_g_on_n[1][34] = 1.09e-10; I_g_on_n[1][35] = 1.09e-10; 
      I_g_on_n[1][36] = 1.09e-10; I_g_on_n[1][37] = 1.09e-10; I_g_on_n[1][38] = 1.09e-10; I_g_on_n[1][39] = 1.09e-10; I_g_on_n[1][40] = 1.09e-10;
      I_g_on_n[1][41] = 1.09e-10; I_g_on_n[1][42] = 1.09e-10; I_g_on_n[1][43] = 1.09e-10; I_g_on_n[1][44] = 1.09e-10; I_g_on_n[1][45] = 1.09e-10; 
      I_g_on_n[1][46] = 1.09e-10; I_g_on_n[1][47] = 1.09e-10; I_g_on_n[1][48] = 1.09e-10; I_g_on_n[1][49] = 1.09e-10; I_g_on_n[1][50] = 1.09e-10;
      I_g_on_n[1][51] = 1.09e-10; I_g_on_n[1][52] = 1.09e-10; I_g_on_n[1][53] = 1.09e-10; I_g_on_n[1][54] = 1.09e-10; I_g_on_n[1][55] = 1.09e-10; 
      I_g_on_n[1][56] = 1.09e-10; I_g_on_n[1][57] = 1.09e-10; I_g_on_n[1][58] = 1.09e-10; I_g_on_n[1][59] = 1.09e-10; I_g_on_n[1][60] = 1.09e-10;
      I_g_on_n[1][61] = 1.09e-10; I_g_on_n[1][62] = 1.09e-10; I_g_on_n[1][63] = 1.09e-10; I_g_on_n[1][64] = 1.09e-10; I_g_on_n[1][65] = 1.09e-10; 
      I_g_on_n[1][66] = 1.09e-10; I_g_on_n[1][67] = 1.09e-10; I_g_on_n[1][68] = 1.09e-10; I_g_on_n[1][69] = 1.09e-10; I_g_on_n[1][70] = 1.09e-10;
      I_g_on_n[1][71] = 1.09e-10; I_g_on_n[1][72] = 1.09e-10; I_g_on_n[1][73] = 1.09e-10; I_g_on_n[1][74] = 1.09e-10; I_g_on_n[1][75] = 1.09e-10; 
      I_g_on_n[1][76] = 1.09e-10; I_g_on_n[1][77] = 1.09e-10; I_g_on_n[1][78] = 1.09e-10; I_g_on_n[1][79] = 1.09e-10; I_g_on_n[1][80] = 1.09e-10;
      I_g_on_n[1][81] = 1.09e-10; I_g_on_n[1][82] = 1.09e-10; I_g_on_n[1][83] = 1.09e-10; I_g_on_n[1][84] = 1.09e-10; I_g_on_n[1][85] = 1.09e-10; 
      I_g_on_n[1][86] = 1.09e-10; I_g_on_n[1][87] = 1.09e-10; I_g_on_n[1][88] = 1.09e-10; I_g_on_n[1][89] = 1.09e-10; I_g_on_n[1][90] = 1.09e-10;
      I_g_on_n[1][91] = 1.09e-10; I_g_on_n[1][92] = 1.09e-10; I_g_on_n[1][93] = 1.09e-10; I_g_on_n[1][94] = 1.09e-10; I_g_on_n[1][95] = 1.09e-10; 
      I_g_on_n[1][96] = 1.09e-10; I_g_on_n[1][97] = 1.09e-10; I_g_on_n[1][98] = 1.09e-10; I_g_on_n[1][99] = 1.09e-10; I_g_on_n[1][100] = 1.09e-10;
      I_off_n[1][0] = 9.12e-12; 
      I_off_n[1][1] = 9.57e-12; I_off_n[1][2] = 1.01e-11; I_off_n[1][3] = 1.06e-11; I_off_n[1][4] = 1.11e-11; I_off_n[1][5] = 1.17e-11; 
      I_off_n[1][6] = 1.23e-11; I_off_n[1][7] = 1.29e-11; I_off_n[1][8] = 1.35e-11; I_off_n[1][9] = 1.42e-11; I_off_n[1][10] = 1.49e-11;
      I_off_n[1][11] = 1.56e-11; I_off_n[1][12] = 1.64e-11; I_off_n[1][13] = 1.71e-11; I_off_n[1][14] = 1.80e-11; I_off_n[1][15] = 1.88e-11; 
      I_off_n[1][16] = 1.97e-11; I_off_n[1][17] = 2.06e-11; I_off_n[1][18] = 2.16e-11; I_off_n[1][19] = 2.26e-11; I_off_n[1][20] = 2.36e-11;
      I_off_n[1][21] = 2.47e-11; I_off_n[1][22] = 2.58e-11; I_off_n[1][23] = 2.69e-11; I_off_n[1][24] = 2.81e-11; I_off_n[1][25] = 2.94e-11; 
      I_off_n[1][26] = 3.07e-11; I_off_n[1][27] = 3.21e-11; I_off_n[1][28] = 3.35e-11; I_off_n[1][29] = 3.49e-11; I_off_n[1][30] = 3.64e-11;
      I_off_n[1][31] = 3.79e-11; I_off_n[1][32] = 3.96e-11; I_off_n[1][33] = 4.12e-11; I_off_n[1][34] = 4.30e-11; I_off_n[1][35] = 4.48e-11; 
      I_off_n[1][36] = 4.67e-11; I_off_n[1][37] = 4.86e-11; I_off_n[1][38] = 5.06e-11; I_off_n[1][39] = 5.27e-11; I_off_n[1][40] = 5.48e-11;
      I_off_n[1][41] = 5.70e-11; I_off_n[1][42] = 5.93e-11; I_off_n[1][43] = 6.16e-11; I_off_n[1][44] = 6.41e-11; I_off_n[1][45] = 6.66e-11; 
      I_off_n[1][46] = 6.93e-11; I_off_n[1][47] = 7.20e-11; I_off_n[1][48] = 7.47e-11; I_off_n[1][49] = 7.76e-11; I_off_n[1][50] = 8.05e-11;
      I_off_n[1][51] = 8.35e-11; I_off_n[1][52] = 8.66e-11; I_off_n[1][53] = 8.98e-11; I_off_n[1][54] = 9.32e-11; I_off_n[1][55] = 9.66e-11; 
      I_off_n[1][56] = 1.00e-10; I_off_n[1][57] = 1.04e-10; I_off_n[1][58] = 1.07e-10; I_off_n[1][59] = 1.11e-10; I_off_n[1][60] = 1.15e-10;
      I_off_n[1][61] = 1.19e-10; I_off_n[1][62] = 1.23e-10; I_off_n[1][63] = 1.27e-10; I_off_n[1][64] = 1.32e-10; I_off_n[1][65] = 1.36e-10; 
      I_off_n[1][66] = 1.40e-10; I_off_n[1][67] = 1.45e-10; I_off_n[1][68] = 1.50e-10; I_off_n[1][69] = 1.54e-10; I_off_n[1][70] = 1.59e-10;
      I_off_n[1][71] = 1.64e-10; I_off_n[1][72] = 1.69e-10; I_off_n[1][73] = 1.74e-10; I_off_n[1][74] = 1.79e-10; I_off_n[1][75] = 1.84e-10; 
      I_off_n[1][76] = 1.89e-10; I_off_n[1][77] = 1.94e-10; I_off_n[1][78] = 2.00e-10; I_off_n[1][79] = 2.05e-10; I_off_n[1][80] = 2.10e-10;
      I_off_n[1][81] = 2.15e-10; I_off_n[1][82] = 2.20e-10; I_off_n[1][83] = 2.25e-10; I_off_n[1][84] = 2.30e-10; I_off_n[1][85] = 2.36e-10; 
      I_off_n[1][86] = 2.41e-10; I_off_n[1][87] = 2.46e-10; I_off_n[1][88] = 2.51e-10; I_off_n[1][89] = 2.57e-10; I_off_n[1][90] = 2.62e-10;
      I_off_n[1][91] = 2.68e-10; I_off_n[1][92] = 2.73e-10; I_off_n[1][93] = 2.79e-10; I_off_n[1][94] = 2.85e-10; I_off_n[1][95] = 2.91e-10; 
      I_off_n[1][96] = 2.97e-10; I_off_n[1][97] = 3.03e-10; I_off_n[1][98] = 3.09e-10; I_off_n[1][99] = 3.15e-10; I_off_n[1][100] = 3.21e-10;

      //ITRS LOP device type
      vdd[2] = 0.8;
      Lphy[2] = 0.032;
      Lelec[2] = 0.0216;
      t_ox[2] = 1.2e-3;
      v_th[2] = 0.28512;
      c_ox[2] = 1.87e-14;
      mobility_eff[2] = 495.19 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[2] = 0.292;
      c_g_ideal[2] = 6e-16;
      c_fringe[2] = 0.08e-15;
      c_junc[2] = 1e-15;
      I_on_n[2] = 573.1e-6;
      I_on_p[2] = 340.6e-6;
      nmos_effective_resistance_multiplier = 1.82;
      n_to_p_eff_curr_drv_ratio[2] = 2.28;
      gmp_to_gmn_multiplier[2] = 1.11;
      Rnchannelon[2] = nmos_effective_resistance_multiplier * vdd[2] / I_on_n[2];
      Rpchannelon[2] = n_to_p_eff_curr_drv_ratio[2] * Rnchannelon[2];
      long_channel_leakage_reduction[2] = 1/2.05;
      /*
      I_off_n[2][0] = 4.9e-9;
      I_off_n[2][10] = 6.49e-9;
      I_off_n[2][20] = 8.45e-9;
      I_off_n[2][30] = 1.08e-8;
      I_off_n[2][40] = 1.37e-8;
      I_off_n[2][50] = 1.71e-8;
      I_off_n[2][60] = 2.09e-8;
      I_off_n[2][70] = 2.48e-8;
      I_off_n[2][80] = 2.84e-8;
      I_off_n[2][90] = 3.13e-8;
      I_off_n[2][100] = 3.42e-8;

      I_g_on_n[2][0]  = 9.61e-9;//A/micron
      I_g_on_n[2][10] = 9.61e-9;
      I_g_on_n[2][20] = 9.61e-9;
      I_g_on_n[2][30] = 9.61e-9;
      I_g_on_n[2][40] = 9.61e-9;
      I_g_on_n[2][50] = 9.61e-9;
      I_g_on_n[2][60] = 9.61e-9;
      I_g_on_n[2][70] = 9.61e-9;
      I_g_on_n[2][80] = 9.61e-9;
      I_g_on_n[2][90] = 9.61e-9;
      I_g_on_n[2][100] = 9.61e-9;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[2][0] = 9.61e-9; 
      I_g_on_n[2][1] = 9.61e-9; I_g_on_n[2][2] = 9.61e-9; I_g_on_n[2][3] = 9.61e-9; I_g_on_n[2][4] = 9.61e-9; I_g_on_n[2][5] = 9.61e-9; 
      I_g_on_n[2][6] = 9.61e-9; I_g_on_n[2][7] = 9.61e-9; I_g_on_n[2][8] = 9.61e-9; I_g_on_n[2][9] = 9.61e-9; I_g_on_n[2][10] = 9.61e-9;
      I_g_on_n[2][11] = 9.61e-9; I_g_on_n[2][12] = 9.61e-9; I_g_on_n[2][13] = 9.61e-9; I_g_on_n[2][14] = 9.61e-9; I_g_on_n[2][15] = 9.61e-9; 
      I_g_on_n[2][16] = 9.61e-9; I_g_on_n[2][17] = 9.61e-9; I_g_on_n[2][18] = 9.61e-9; I_g_on_n[2][19] = 9.61e-9; I_g_on_n[2][20] = 9.61e-9;
      I_g_on_n[2][21] = 9.61e-9; I_g_on_n[2][22] = 9.61e-9; I_g_on_n[2][23] = 9.61e-9; I_g_on_n[2][24] = 9.61e-9; I_g_on_n[2][25] = 9.61e-9; 
      I_g_on_n[2][26] = 9.61e-9; I_g_on_n[2][27] = 9.61e-9; I_g_on_n[2][28] = 9.61e-9; I_g_on_n[2][29] = 9.61e-9; I_g_on_n[2][30] = 9.61e-9;
      I_g_on_n[2][31] = 9.61e-9; I_g_on_n[2][32] = 9.61e-9; I_g_on_n[2][33] = 9.61e-9; I_g_on_n[2][34] = 9.61e-9; I_g_on_n[2][35] = 9.61e-9; 
      I_g_on_n[2][36] = 9.61e-9; I_g_on_n[2][37] = 9.61e-9; I_g_on_n[2][38] = 9.61e-9; I_g_on_n[2][39] = 9.61e-9; I_g_on_n[2][40] = 9.61e-9;
      I_g_on_n[2][41] = 9.61e-9; I_g_on_n[2][42] = 9.61e-9; I_g_on_n[2][43] = 9.61e-9; I_g_on_n[2][44] = 9.61e-9; I_g_on_n[2][45] = 9.61e-9; 
      I_g_on_n[2][46] = 9.61e-9; I_g_on_n[2][47] = 9.61e-9; I_g_on_n[2][48] = 9.61e-9; I_g_on_n[2][49] = 9.61e-9; I_g_on_n[2][50] = 9.61e-9;
      I_g_on_n[2][51] = 9.61e-9; I_g_on_n[2][52] = 9.61e-9; I_g_on_n[2][53] = 9.61e-9; I_g_on_n[2][54] = 9.61e-9; I_g_on_n[2][55] = 9.61e-9; 
      I_g_on_n[2][56] = 9.61e-9; I_g_on_n[2][57] = 9.61e-9; I_g_on_n[2][58] = 9.61e-9; I_g_on_n[2][59] = 9.61e-9; I_g_on_n[2][60] = 9.61e-9;
      I_g_on_n[2][61] = 9.61e-9; I_g_on_n[2][62] = 9.61e-9; I_g_on_n[2][63] = 9.61e-9; I_g_on_n[2][64] = 9.61e-9; I_g_on_n[2][65] = 9.61e-9; 
      I_g_on_n[2][66] = 9.61e-9; I_g_on_n[2][67] = 9.61e-9; I_g_on_n[2][68] = 9.61e-9; I_g_on_n[2][69] = 9.61e-9; I_g_on_n[2][70] = 9.61e-9;
      I_g_on_n[2][71] = 9.61e-9; I_g_on_n[2][72] = 9.61e-9; I_g_on_n[2][73] = 9.61e-9; I_g_on_n[2][74] = 9.61e-9; I_g_on_n[2][75] = 9.61e-9; 
      I_g_on_n[2][76] = 9.61e-9; I_g_on_n[2][77] = 9.61e-9; I_g_on_n[2][78] = 9.61e-9; I_g_on_n[2][79] = 9.61e-9; I_g_on_n[2][80] = 9.61e-9;
      I_g_on_n[2][81] = 9.61e-9; I_g_on_n[2][82] = 9.61e-9; I_g_on_n[2][83] = 9.61e-9; I_g_on_n[2][84] = 9.61e-9; I_g_on_n[2][85] = 9.61e-9; 
      I_g_on_n[2][86] = 9.61e-9; I_g_on_n[2][87] = 9.61e-9; I_g_on_n[2][88] = 9.61e-9; I_g_on_n[2][89] = 9.61e-9; I_g_on_n[2][90] = 9.61e-9;
      I_g_on_n[2][91] = 9.61e-9; I_g_on_n[2][92] = 9.61e-9; I_g_on_n[2][93] = 9.61e-9; I_g_on_n[2][94] = 9.61e-9; I_g_on_n[2][95] = 9.61e-9; 
      I_g_on_n[2][96] = 9.61e-9; I_g_on_n[2][97] = 9.61e-9; I_g_on_n[2][98] = 9.61e-9; I_g_on_n[2][99] = 9.61e-9; I_g_on_n[2][100] = 9.61e-9;
      I_off_n[2][0] = 4.9e-9; 
      I_off_n[2][1] = 5.04e-9; I_off_n[2][2] = 5.19e-9; I_off_n[2][3] = 5.34e-9; I_off_n[2][4] = 5.49e-9; I_off_n[2][5] = 5.65e-9; 
      I_off_n[2][6] = 5.81e-9; I_off_n[2][7] = 5.98e-9; I_off_n[2][8] = 6.14e-9; I_off_n[2][9] = 6.32e-9; I_off_n[2][10] = 6.49e-9;
      I_off_n[2][11] = 6.67e-9; I_off_n[2][12] = 6.85e-9; I_off_n[2][13] = 7.04e-9; I_off_n[2][14] = 7.23e-9; I_off_n[2][15] = 7.42e-9; 
      I_off_n[2][16] = 7.62e-9; I_off_n[2][17] = 7.82e-9; I_off_n[2][18] = 8.03e-9; I_off_n[2][19] = 8.24e-9; I_off_n[2][20] = 8.45e-9;
      I_off_n[2][21] = 8.67e-9; I_off_n[2][22] = 8.88e-9; I_off_n[2][23] = 9.11e-9; I_off_n[2][24] = 9.34e-9; I_off_n[2][25] = 9.57e-9; 
      I_off_n[2][26] = 9.80e-9; I_off_n[2][27] = 1.00e-8; I_off_n[2][28] = 1.03e-8; I_off_n[2][29] = 1.05e-8; I_off_n[2][30] = 1.08e-8;
      I_off_n[2][31] = 1.11e-8; I_off_n[2][32] = 1.13e-8; I_off_n[2][33] = 1.16e-8; I_off_n[2][34] = 1.19e-8; I_off_n[2][35] = 1.22e-8; 
      I_off_n[2][36] = 1.25e-8; I_off_n[2][37] = 1.28e-8; I_off_n[2][38] = 1.31e-8; I_off_n[2][39] = 1.34e-8; I_off_n[2][40] = 1.37e-8;
      I_off_n[2][41] = 1.40e-8; I_off_n[2][42] = 1.43e-8; I_off_n[2][43] = 1.47e-8; I_off_n[2][44] = 1.50e-8; I_off_n[2][45] = 1.53e-8; 
      I_off_n[2][46] = 1.57e-8; I_off_n[2][47] = 1.60e-8; I_off_n[2][48] = 1.64e-8; I_off_n[2][49] = 1.67e-8; I_off_n[2][50] = 1.71e-8;
      I_off_n[2][51] = 1.75e-8; I_off_n[2][52] = 1.78e-8; I_off_n[2][53] = 1.82e-8; I_off_n[2][54] = 1.86e-8; I_off_n[2][55] = 1.90e-8; 
      I_off_n[2][56] = 1.94e-8; I_off_n[2][57] = 1.97e-8; I_off_n[2][58] = 2.01e-8; I_off_n[2][59] = 2.05e-8; I_off_n[2][60] = 2.09e-8;
      I_off_n[2][61] = 2.13e-8; I_off_n[2][62] = 2.17e-8; I_off_n[2][63] = 2.21e-8; I_off_n[2][64] = 2.25e-8; I_off_n[2][65] = 2.29e-8; 
      I_off_n[2][66] = 2.33e-8; I_off_n[2][67] = 2.36e-8; I_off_n[2][68] = 2.40e-8; I_off_n[2][69] = 2.44e-8; I_off_n[2][70] = 2.48e-8;
      I_off_n[2][71] = 2.52e-8; I_off_n[2][72] = 2.56e-8; I_off_n[2][73] = 2.59e-8; I_off_n[2][74] = 2.63e-8; I_off_n[2][75] = 2.67e-8; 
      I_off_n[2][76] = 2.70e-8; I_off_n[2][77] = 2.74e-8; I_off_n[2][78] = 2.77e-8; I_off_n[2][79] = 2.81e-8; I_off_n[2][80] = 2.84e-8;
      I_off_n[2][81] = 2.87e-8; I_off_n[2][82] = 2.90e-8; I_off_n[2][83] = 2.93e-8; I_off_n[2][84] = 2.96e-8; I_off_n[2][85] = 2.99e-8; 
      I_off_n[2][86] = 3.02e-8; I_off_n[2][87] = 3.04e-8; I_off_n[2][88] = 3.07e-8; I_off_n[2][89] = 3.10e-8; I_off_n[2][90] = 3.13e-8;
      I_off_n[2][91] = 3.16e-8; I_off_n[2][92] = 3.19e-8; I_off_n[2][93] = 3.22e-8; I_off_n[2][94] = 3.25e-8; I_off_n[2][95] = 3.28e-8; 
      I_off_n[2][96] = 3.30e-8; I_off_n[2][97] = 3.33e-8; I_off_n[2][98] = 3.36e-8; I_off_n[2][99] = 3.39e-8; I_off_n[2][100] = 3.42e-8;

      if (ram_cell_tech_type == lp_dram)
      {
        //LP-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.2;
        Lphy[3] = 0.12;
        Lelec[3] = 0.0756;
        curr_v_th_dram_access_transistor = 0.43806;
        width_dram_access_transistor = 0.09;
        curr_I_on_dram_cell = 36e-6;
        curr_I_off_dram_cell_worst_case_length_temp = 19.6e-12;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram = 0;
        curr_area_cell_dram = 0.11;
        curr_asp_ratio_cell_dram = 1.46;
        curr_c_dram_cell = 20e-15;

        //LP-DRAM wordline transistor parameters
        curr_vpp = 1.6;
        t_ox[3] = 2.2e-3;
        v_th[3] = 0.43806;
        c_ox[3] = 1.22e-14;
        mobility_eff[3] =  328.32 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.43806;
        c_g_ideal[3] = 1.46e-15;
        c_fringe[3] = 0.08e-15;
        c_junc[3] = 1e-15 ;
        I_on_n[3] = 399.8e-6;
        I_on_p[3] = 243.4e-6;
        nmos_effective_resistance_multiplier = 1.65;
        n_to_p_eff_curr_drv_ratio[3] = 2.05;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0]  = 2.23e-11;
        I_off_n[3][10] = 3.46e-11;
        I_off_n[3][20] = 5.24e-11;
        I_off_n[3][30] = 7.75e-11;
        I_off_n[3][40] = 1.12e-10;
        I_off_n[3][50] = 1.58e-10;
        I_off_n[3][60] = 2.18e-10;
        I_off_n[3][70] = 2.88e-10;
        I_off_n[3][80] = 3.63e-10;
        I_off_n[3][90] = 4.41e-10;
        I_off_n[3][100] = 5.36e-10;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 2.23e-11; 
        I_off_n[3][1] = 2.33e-11; I_off_n[3][2] = 2.43e-11; I_off_n[3][3] = 2.54e-11; I_off_n[3][4] = 2.66e-11; I_off_n[3][5] = 2.78e-11; 
        I_off_n[3][6] = 2.91e-11; I_off_n[3][7] = 3.04e-11; I_off_n[3][8] = 3.18e-11; I_off_n[3][9] = 3.32e-11; I_off_n[3][10] = 3.46e-11;
        I_off_n[3][11] = 3.61e-11; I_off_n[3][12] = 3.76e-11; I_off_n[3][13] = 3.93e-11; I_off_n[3][14] = 4.10e-11; I_off_n[3][15] = 4.27e-11; 
        I_off_n[3][16] = 4.45e-11; I_off_n[3][17] = 4.64e-11; I_off_n[3][18] = 4.83e-11; I_off_n[3][19] = 5.03e-11; I_off_n[3][20] = 5.24e-11;
        I_off_n[3][21] = 5.45e-11; I_off_n[3][22] = 5.67e-11; I_off_n[3][23] = 5.91e-11; I_off_n[3][24] = 6.14e-11; I_off_n[3][25] = 6.39e-11; 
        I_off_n[3][26] = 6.65e-11; I_off_n[3][27] = 6.91e-11; I_off_n[3][28] = 7.18e-11; I_off_n[3][29] = 7.46e-11; I_off_n[3][30] = 7.75e-11;
        I_off_n[3][31] = 8.05e-11; I_off_n[3][32] = 8.35e-11; I_off_n[3][33] = 8.67e-11; I_off_n[3][34] = 9.00e-11; I_off_n[3][35] = 9.35e-11; 
        I_off_n[3][36] = 9.70e-11; I_off_n[3][37] = 1.01e-10; I_off_n[3][38] = 1.04e-10; I_off_n[3][39] = 1.08e-10; I_off_n[3][40] = 1.12e-10;
        I_off_n[3][41] = 1.16e-10; I_off_n[3][42] = 1.20e-10; I_off_n[3][43] = 1.24e-10; I_off_n[3][44] = 1.29e-10; I_off_n[3][45] = 1.33e-10; 
        I_off_n[3][46] = 1.38e-10; I_off_n[3][47] = 1.43e-10; I_off_n[3][48] = 1.48e-10; I_off_n[3][49] = 1.53e-10; I_off_n[3][50] = 1.58e-10;
        I_off_n[3][51] = 1.63e-10; I_off_n[3][52] = 1.69e-10; I_off_n[3][53] = 1.75e-10; I_off_n[3][54] = 1.80e-10; I_off_n[3][55] = 1.86e-10; 
        I_off_n[3][56] = 1.93e-10; I_off_n[3][57] = 1.99e-10; I_off_n[3][58] = 2.05e-10; I_off_n[3][59] = 2.12e-10; I_off_n[3][60] = 2.18e-10;
        I_off_n[3][61] = 2.25e-10; I_off_n[3][62] = 2.31e-10; I_off_n[3][63] = 2.38e-10; I_off_n[3][64] = 2.45e-10; I_off_n[3][65] = 2.52e-10; 
        I_off_n[3][66] = 2.59e-10; I_off_n[3][67] = 2.66e-10; I_off_n[3][68] = 2.74e-10; I_off_n[3][69] = 2.81e-10; I_off_n[3][70] = 2.88e-10;
        I_off_n[3][71] = 2.95e-10; I_off_n[3][72] = 3.03e-10; I_off_n[3][73] = 3.10e-10; I_off_n[3][74] = 3.17e-10; I_off_n[3][75] = 3.25e-10; 
        I_off_n[3][76] = 3.33e-10; I_off_n[3][77] = 3.40e-10; I_off_n[3][78] = 3.48e-10; I_off_n[3][79] = 3.55e-10; I_off_n[3][80] = 3.63e-10;
        I_off_n[3][81] = 3.71e-10; I_off_n[3][82] = 3.78e-10; I_off_n[3][83] = 3.86e-10; I_off_n[3][84] = 3.93e-10; I_off_n[3][85] = 4.01e-10; 
        I_off_n[3][86] = 4.09e-10; I_off_n[3][87] = 4.16e-10; I_off_n[3][88] = 4.24e-10; I_off_n[3][89] = 4.33e-10; I_off_n[3][90] = 4.41e-10;
        I_off_n[3][91] = 4.50e-10; I_off_n[3][92] = 4.59e-10; I_off_n[3][93] = 4.68e-10; I_off_n[3][94] = 4.77e-10; I_off_n[3][95] = 4.86e-10; 
        I_off_n[3][96] = 4.96e-10; I_off_n[3][97] = 5.06e-10; I_off_n[3][98] = 5.16e-10; I_off_n[3][99] = 5.26e-10; I_off_n[3][100] = 5.36e-10;
      }
      else if (ram_cell_tech_type == comm_dram)
      {
        //COMM-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.3;
        Lphy[3] = 0.065;
        Lelec[3] = 0.0426;
        curr_v_th_dram_access_transistor = 1;
        width_dram_access_transistor = 0.065;
        curr_I_on_dram_cell = 20e-6;
        curr_I_off_dram_cell_worst_case_length_temp = 1e-15;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram = 0;
        curr_area_cell_dram = 6*0.065*0.065;
        curr_asp_ratio_cell_dram = 1.5;
        curr_c_dram_cell = 30e-15;

        //COMM-DRAM wordline transistor parameters
        curr_vpp = 3.3;
        t_ox[3] = 5e-3;
        v_th[3] = 1.0;
        c_ox[3] = 6.16e-15;
        mobility_eff[3] =  303.44 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.385;
        c_g_ideal[3] = 4e-16;
        c_fringe[3] = 0.08e-15;
        c_junc[3] = 1e-15 ;
        I_on_n[3] = 1031e-6;
        I_on_p[3] = I_on_n[3] / 2;
        nmos_effective_resistance_multiplier = 1.69;
        n_to_p_eff_curr_drv_ratio[3] = 2.39;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0]  = 1.80e-14;
        I_off_n[3][10] = 3.64e-14;
        I_off_n[3][20] = 7.03e-14;
        I_off_n[3][30] = 1.31e-13;
        I_off_n[3][40] = 2.35e-13;
        I_off_n[3][50] = 4.09e-13;
        I_off_n[3][60] = 6.89e-13;
        I_off_n[3][70] = 1.13e-12;
        I_off_n[3][80] = 1.78e-12;
        I_off_n[3][90] = 2.71e-12;
        I_off_n[3][100] = 3.99e-12;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 1.80e-14; 
        I_off_n[3][1] = 1.92e-14; I_off_n[3][2] = 2.05e-14; I_off_n[3][3] = 2.20e-14; I_off_n[3][4] = 2.37e-14; I_off_n[3][5] = 2.55e-14; 
        I_off_n[3][6] = 2.75e-14; I_off_n[3][7] = 2.96e-14; I_off_n[3][8] = 3.18e-14; I_off_n[3][9] = 3.40e-14; I_off_n[3][10] = 3.64e-14;
        I_off_n[3][11] = 3.89e-14; I_off_n[3][12] = 4.16e-14; I_off_n[3][13] = 4.45e-14; I_off_n[3][14] = 4.76e-14; I_off_n[3][15] = 5.09e-14; 
        I_off_n[3][16] = 5.44e-14; I_off_n[3][17] = 5.81e-14; I_off_n[3][18] = 6.20e-14; I_off_n[3][19] = 6.60e-14; I_off_n[3][20] = 7.03e-14;
        I_off_n[3][21] = 7.48e-14; I_off_n[3][22] = 7.97e-14; I_off_n[3][23] = 8.50e-14; I_off_n[3][24] = 9.06e-14; I_off_n[3][25] = 9.65e-14; 
        I_off_n[3][26] = 1.03e-13; I_off_n[3][27] = 1.09e-13; I_off_n[3][28] = 1.16e-13; I_off_n[3][29] = 1.23e-13; I_off_n[3][30] = 1.31e-13;
        I_off_n[3][31] = 1.39e-13; I_off_n[3][32] = 1.47e-13; I_off_n[3][33] = 1.57e-13; I_off_n[3][34] = 1.66e-13; I_off_n[3][35] = 1.76e-13; 
        I_off_n[3][36] = 1.87e-13; I_off_n[3][37] = 1.98e-13; I_off_n[3][38] = 2.10e-13; I_off_n[3][39] = 2.22e-13; I_off_n[3][40] = 2.35e-13;
        I_off_n[3][41] = 2.48e-13; I_off_n[3][42] = 2.63e-13; I_off_n[3][43] = 2.78e-13; I_off_n[3][44] = 2.94e-13; I_off_n[3][45] = 3.11e-13; 
        I_off_n[3][46] = 3.29e-13; I_off_n[3][47] = 3.48e-13; I_off_n[3][48] = 3.68e-13; I_off_n[3][49] = 3.88e-13; I_off_n[3][50] = 4.09e-13;
        I_off_n[3][51] = 4.31e-13; I_off_n[3][52] = 4.55e-13; I_off_n[3][53] = 4.79e-13; I_off_n[3][54] = 5.06e-13; I_off_n[3][55] = 5.33e-13; 
        I_off_n[3][56] = 5.62e-13; I_off_n[3][57] = 5.92e-13; I_off_n[3][58] = 6.23e-13; I_off_n[3][59] = 6.55e-13; I_off_n[3][60] = 6.89e-13;
        I_off_n[3][61] = 7.24e-13; I_off_n[3][62] = 7.62e-13; I_off_n[3][63] = 8.02e-13; I_off_n[3][64] = 8.43e-13; I_off_n[3][65] = 8.87e-13; 
        I_off_n[3][66] = 9.32e-13; I_off_n[3][67] = 9.79e-13; I_off_n[3][68] = 1.03e-12; I_off_n[3][69] = 1.08e-12; I_off_n[3][70] = 1.13e-12;
        I_off_n[3][71] = 1.18e-12; I_off_n[3][72] = 1.24e-12; I_off_n[3][73] = 1.30e-12; I_off_n[3][74] = 1.36e-12; I_off_n[3][75] = 1.43e-12; 
        I_off_n[3][76] = 1.49e-12; I_off_n[3][77] = 1.56e-12; I_off_n[3][78] = 1.63e-12; I_off_n[3][79] = 1.70e-12; I_off_n[3][80] = 1.78e-12;
        I_off_n[3][81] = 1.86e-12; I_off_n[3][82] = 1.94e-12; I_off_n[3][83] = 2.03e-12; I_off_n[3][84] = 2.11e-12; I_off_n[3][85] = 2.21e-12; 
        I_off_n[3][86] = 2.30e-12; I_off_n[3][87] = 2.40e-12; I_off_n[3][88] = 2.50e-12; I_off_n[3][89] = 2.60e-12; I_off_n[3][90] = 2.71e-12;
        I_off_n[3][91] = 2.82e-12; I_off_n[3][92] = 2.93e-12; I_off_n[3][93] = 3.05e-12; I_off_n[3][94] = 3.18e-12; I_off_n[3][95] = 3.30e-12; 
        I_off_n[3][96] = 3.43e-12; I_off_n[3][97] = 3.57e-12; I_off_n[3][98] = 3.71e-12; I_off_n[3][99] = 3.85e-12; I_off_n[3][100] = 3.99e-12;
      }

      //SRAM cell properties
      curr_Wmemcella_sram = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
      curr_area_cell_sram = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_sram = 1.46;
      //CAM cell properties //TODO: data need to be revisited
      curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
      curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_cam = 2.92;
      //Empirical undifferetiated core/FU coefficient
      curr_logic_scaling_co_eff = 0.7; //Rather than scale proportionally to square of feature size, only scale linearly according to IBM cell processor
      curr_core_tx_density      = 1.25*0.7;
      curr_sckt_co_eff           = 1.1359;
      curr_chip_layout_overhead  = 1.2;//die measurement results based on Niagara 1 and 2
      curr_macro_layout_overhead = 1.1;//EDA placement and routing tool rule of thumb
    }

    if (tech == 45)
    { //45nm technology-node. Corresponds to year 2010 in ITRS
      //ITRS HP device type
      SENSE_AMP_D = .04e-9; // s
      SENSE_AMP_P = 2.7e-15; // J
      vdd[0] = 1.0;
      Lphy[0] = 0.018;
      Lelec[0] = 0.01345;
      t_ox[0] = 0.65e-3;
      v_th[0] = .18035;
      c_ox[0] = 3.77e-14;
      mobility_eff[0] = 266.68 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[0] = 9.38E-2;
      c_g_ideal[0] = 6.78e-16;
      c_fringe[0] = 0.05e-15;
      c_junc[0] = 1e-15;
      I_on_n[0] = 2046.6e-6;
      //There are certain problems with the ITRS PMOS numbers in MASTAR for 45nm. So we are using 65nm values of
      //n_to_p_eff_curr_drv_ratio and gmp_to_gmn_multiplier for 45nm
      I_on_p[0] = I_on_n[0] / 2;//This value is fixed arbitrarily but I_on_p is not being used in CACTI
      nmos_effective_resistance_multiplier = 1.51;
      n_to_p_eff_curr_drv_ratio[0] = 2.41;
      gmp_to_gmn_multiplier[0] = 1.38;
      Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];
      Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];
      long_channel_leakage_reduction[0] = 1/3.546;//Using MASTAR, @380K, increase Lgate until Ion reduces to 90%, Ioff(Lgate normal)/Ioff(Lgate long)= 3.74
      /*
      I_off_n[0][0] = 2.8e-7;
      I_off_n[0][10] = 3.28e-7;
      I_off_n[0][20] = 3.81e-7;
      I_off_n[0][30] = 4.39e-7;
      I_off_n[0][40] = 5.02e-7;
      I_off_n[0][50] = 5.69e-7;
      I_off_n[0][60] = 6.42e-7;
      I_off_n[0][70] = 7.2e-7;
      I_off_n[0][80] = 8.03e-7;
      I_off_n[0][90] = 8.91e-7;
      I_off_n[0][100] = 9.84e-7;

      I_g_on_n[0][0]  = 3.59e-8;//A/micron
      I_g_on_n[0][10] = 3.59e-8;
      I_g_on_n[0][20] = 3.59e-8;
      I_g_on_n[0][30] = 3.59e-8;
      I_g_on_n[0][40] = 3.59e-8;
      I_g_on_n[0][50] = 3.59e-8;
      I_g_on_n[0][60] = 3.59e-8;
      I_g_on_n[0][70] = 3.59e-8;
      I_g_on_n[0][80] = 3.59e-8;
      I_g_on_n[0][90] = 3.59e-8;
      I_g_on_n[0][100] = 3.59e-8;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[0][0] = 3.59e-8; 
        I_g_on_n[0][1] = 3.59e-8; I_g_on_n[0][2] = 3.59e-8; I_g_on_n[0][3] = 3.59e-8; I_g_on_n[0][4] = 3.59e-8; I_g_on_n[0][5] = 3.59e-8; 
        I_g_on_n[0][6] = 3.59e-8; I_g_on_n[0][7] = 3.59e-8; I_g_on_n[0][8] = 3.59e-8; I_g_on_n[0][9] = 3.59e-8; I_g_on_n[0][10] = 3.59e-8;
        I_g_on_n[0][11] = 3.59e-8; I_g_on_n[0][12] = 3.59e-8; I_g_on_n[0][13] = 3.59e-8; I_g_on_n[0][14] = 3.59e-8; I_g_on_n[0][15] = 3.59e-8; 
        I_g_on_n[0][16] = 3.59e-8; I_g_on_n[0][17] = 3.59e-8; I_g_on_n[0][18] = 3.59e-8; I_g_on_n[0][19] = 3.59e-8; I_g_on_n[0][20] = 3.59e-8;
        I_g_on_n[0][21] = 3.59e-8; I_g_on_n[0][22] = 3.59e-8; I_g_on_n[0][23] = 3.59e-8; I_g_on_n[0][24] = 3.59e-8; I_g_on_n[0][25] = 3.59e-8; 
        I_g_on_n[0][26] = 3.59e-8; I_g_on_n[0][27] = 3.59e-8; I_g_on_n[0][28] = 3.59e-8; I_g_on_n[0][29] = 3.59e-8; I_g_on_n[0][30] = 3.59e-8;
        I_g_on_n[0][31] = 3.59e-8; I_g_on_n[0][32] = 3.59e-8; I_g_on_n[0][33] = 3.59e-8; I_g_on_n[0][34] = 3.59e-8; I_g_on_n[0][35] = 3.59e-8; 
        I_g_on_n[0][36] = 3.59e-8; I_g_on_n[0][37] = 3.59e-8; I_g_on_n[0][38] = 3.59e-8; I_g_on_n[0][39] = 3.59e-8; I_g_on_n[0][40] = 3.59e-8;
        I_g_on_n[0][41] = 3.59e-8; I_g_on_n[0][42] = 3.59e-8; I_g_on_n[0][43] = 3.59e-8; I_g_on_n[0][44] = 3.59e-8; I_g_on_n[0][45] = 3.59e-8; 
        I_g_on_n[0][46] = 3.59e-8; I_g_on_n[0][47] = 3.59e-8; I_g_on_n[0][48] = 3.59e-8; I_g_on_n[0][49] = 3.59e-8; I_g_on_n[0][50] = 3.59e-8;
        I_g_on_n[0][51] = 3.59e-8; I_g_on_n[0][52] = 3.59e-8; I_g_on_n[0][53] = 3.59e-8; I_g_on_n[0][54] = 3.59e-8; I_g_on_n[0][55] = 3.59e-8; 
        I_g_on_n[0][56] = 3.59e-8; I_g_on_n[0][57] = 3.59e-8; I_g_on_n[0][58] = 3.59e-8; I_g_on_n[0][59] = 3.59e-8; I_g_on_n[0][60] = 3.59e-8;
        I_g_on_n[0][61] = 3.59e-8; I_g_on_n[0][62] = 3.59e-8; I_g_on_n[0][63] = 3.59e-8; I_g_on_n[0][64] = 3.59e-8; I_g_on_n[0][65] = 3.59e-8; 
        I_g_on_n[0][66] = 3.59e-8; I_g_on_n[0][67] = 3.59e-8; I_g_on_n[0][68] = 3.59e-8; I_g_on_n[0][69] = 3.59e-8; I_g_on_n[0][70] = 3.59e-8;
        I_g_on_n[0][71] = 3.59e-8; I_g_on_n[0][72] = 3.59e-8; I_g_on_n[0][73] = 3.59e-8; I_g_on_n[0][74] = 3.59e-8; I_g_on_n[0][75] = 3.59e-8; 
        I_g_on_n[0][76] = 3.59e-8; I_g_on_n[0][77] = 3.59e-8; I_g_on_n[0][78] = 3.59e-8; I_g_on_n[0][79] = 3.59e-8; I_g_on_n[0][80] = 3.59e-8;
        I_g_on_n[0][81] = 3.59e-8; I_g_on_n[0][82] = 3.59e-8; I_g_on_n[0][83] = 3.59e-8; I_g_on_n[0][84] = 3.59e-8; I_g_on_n[0][85] = 3.59e-8; 
        I_g_on_n[0][86] = 3.59e-8; I_g_on_n[0][87] = 3.59e-8; I_g_on_n[0][88] = 3.59e-8; I_g_on_n[0][89] = 3.59e-8; I_g_on_n[0][90] = 3.59e-8;
        I_g_on_n[0][91] = 3.59e-8; I_g_on_n[0][92] = 3.59e-8; I_g_on_n[0][93] = 3.59e-8; I_g_on_n[0][94] = 3.59e-8; I_g_on_n[0][95] = 3.59e-8; 
        I_g_on_n[0][96] = 3.59e-8; I_g_on_n[0][97] = 3.59e-8; I_g_on_n[0][98] = 3.59e-8; I_g_on_n[0][99] = 3.59e-8; I_g_on_n[0][100] = 3.59e-8;
        I_off_n[0][0] = 2.80e-7; 
        I_off_n[0][1] = 2.85e-7; I_off_n[0][2] = 2.89e-7; I_off_n[0][3] = 2.94e-7; I_off_n[0][4] = 2.99e-7; I_off_n[0][5] = 3.03e-7; 
        I_off_n[0][6] = 3.08e-7; I_off_n[0][7] = 3.13e-7; I_off_n[0][8] = 3.18e-7; I_off_n[0][9] = 3.23e-7; I_off_n[0][10] = 3.28e-7;
        I_off_n[0][11] = 3.33e-7; I_off_n[0][12] = 3.38e-7; I_off_n[0][13] = 3.43e-7; I_off_n[0][14] = 3.49e-7; I_off_n[0][15] = 3.54e-7; 
        I_off_n[0][16] = 3.59e-7; I_off_n[0][17] = 3.65e-7; I_off_n[0][18] = 3.70e-7; I_off_n[0][19] = 3.75e-7; I_off_n[0][20] = 3.81e-7;
        I_off_n[0][21] = 3.87e-7; I_off_n[0][22] = 3.92e-7; I_off_n[0][23] = 3.98e-7; I_off_n[0][24] = 4.04e-7; I_off_n[0][25] = 4.09e-7; 
        I_off_n[0][26] = 4.15e-7; I_off_n[0][27] = 4.21e-7; I_off_n[0][28] = 4.27e-7; I_off_n[0][29] = 4.33e-7; I_off_n[0][30] = 4.39e-7;
        I_off_n[0][31] = 4.45e-7; I_off_n[0][32] = 4.51e-7; I_off_n[0][33] = 4.57e-7; I_off_n[0][34] = 4.64e-7; I_off_n[0][35] = 4.70e-7; 
        I_off_n[0][36] = 4.76e-7; I_off_n[0][37] = 4.83e-7; I_off_n[0][38] = 4.89e-7; I_off_n[0][39] = 4.96e-7; I_off_n[0][40] = 5.02e-7;
        I_off_n[0][41] = 5.09e-7; I_off_n[0][42] = 5.15e-7; I_off_n[0][43] = 5.22e-7; I_off_n[0][44] = 5.28e-7; I_off_n[0][45] = 5.35e-7; 
        I_off_n[0][46] = 5.42e-7; I_off_n[0][47] = 5.48e-7; I_off_n[0][48] = 5.55e-7; I_off_n[0][49] = 5.62e-7; I_off_n[0][50] = 5.69e-7;
        I_off_n[0][51] = 5.76e-7; I_off_n[0][52] = 5.83e-7; I_off_n[0][53] = 5.90e-7; I_off_n[0][54] = 5.98e-7; I_off_n[0][55] = 6.05e-7; 
        I_off_n[0][56] = 6.12e-7; I_off_n[0][57] = 6.20e-7; I_off_n[0][58] = 6.27e-7; I_off_n[0][59] = 6.34e-7; I_off_n[0][60] = 6.42e-7;
        I_off_n[0][61] = 6.50e-7; I_off_n[0][62] = 6.57e-7; I_off_n[0][63] = 6.65e-7; I_off_n[0][64] = 6.73e-7; I_off_n[0][65] = 6.80e-7; 
        I_off_n[0][66] = 6.88e-7; I_off_n[0][67] = 6.96e-7; I_off_n[0][68] = 7.04e-7; I_off_n[0][69] = 7.12e-7; I_off_n[0][70] = 7.20e-7;
        I_off_n[0][71] = 7.28e-7; I_off_n[0][72] = 7.36e-7; I_off_n[0][73] = 7.44e-7; I_off_n[0][74] = 7.53e-7; I_off_n[0][75] = 7.61e-7; 
        I_off_n[0][76] = 7.69e-7; I_off_n[0][77] = 7.78e-7; I_off_n[0][78] = 7.86e-7; I_off_n[0][79] = 7.94e-7; I_off_n[0][80] = 8.03e-7;
        I_off_n[0][81] = 8.12e-7; I_off_n[0][82] = 8.20e-7; I_off_n[0][83] = 8.29e-7; I_off_n[0][84] = 8.38e-7; I_off_n[0][85] = 8.46e-7; 
        I_off_n[0][86] = 8.55e-7; I_off_n[0][87] = 8.64e-7; I_off_n[0][88] = 8.73e-7; I_off_n[0][89] = 8.82e-7; I_off_n[0][90] = 8.91e-7;
        I_off_n[0][91] = 9.00e-7; I_off_n[0][92] = 9.09e-7; I_off_n[0][93] = 9.18e-7; I_off_n[0][94] = 9.28e-7; I_off_n[0][95] = 9.37e-7; 
        I_off_n[0][96] = 9.46e-7; I_off_n[0][97] = 9.56e-7; I_off_n[0][98] = 9.65e-7; I_off_n[0][99] = 9.74e-7; I_off_n[0][100] = 9.84e-7;

      //ITRS LSTP device type
      vdd[1] = 1.1;
      Lphy[1] =  0.028;
      Lelec[1] = 0.0212;
      t_ox[1] = 1.4e-3;
      v_th[1] = 0.50245;
      c_ox[1] = 2.01e-14;
      mobility_eff[1] =  363.96 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[1] = 9.12e-2;
      c_g_ideal[1] = 5.18e-16;
      c_fringe[1] = 0.08e-15;
      c_junc[1] = 1e-15;
      I_on_n[1] = 666.2e-6;
      I_on_p[1] = I_on_n[1] / 2;
      nmos_effective_resistance_multiplier = 1.99;
      n_to_p_eff_curr_drv_ratio[1] = 2.23;
      gmp_to_gmn_multiplier[1] = 0.99;
      Rnchannelon[1] = nmos_effective_resistance_multiplier * vdd[1] / I_on_n[1];
      Rpchannelon[1] = n_to_p_eff_curr_drv_ratio[1] * Rnchannelon[1];
      long_channel_leakage_reduction[1] = 1/2.08;
      /*
      I_off_n[1][0] = 1.01e-11;
      I_off_n[1][10] = 1.65e-11;
      I_off_n[1][20] = 2.62e-11;
      I_off_n[1][30] = 4.06e-11;
      I_off_n[1][40] = 6.12e-11;
      I_off_n[1][50] = 9.02e-11;
      I_off_n[1][60] = 1.3e-10;
      I_off_n[1][70] = 1.83e-10;
      I_off_n[1][80] = 2.51e-10;
      I_off_n[1][90] = 3.29e-10;
      I_off_n[1][100] = 4.1e-10;

      I_g_on_n[1][0]  = 9.47e-12;//A/micron
      I_g_on_n[1][10] = 9.47e-12;
      I_g_on_n[1][20] = 9.47e-12;
      I_g_on_n[1][30] = 9.47e-12;
      I_g_on_n[1][40] = 9.47e-12;
      I_g_on_n[1][50] = 9.47e-12;
      I_g_on_n[1][60] = 9.47e-12;
      I_g_on_n[1][70] = 9.47e-12;
      I_g_on_n[1][80] = 9.47e-12;
      I_g_on_n[1][90] = 9.47e-12;
      I_g_on_n[1][100] = 9.47e-12;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[1][0] = 9.47e-12; 
        I_g_on_n[1][1] = 9.47e-12; I_g_on_n[1][2] = 9.47e-12; I_g_on_n[1][3] = 9.47e-12; I_g_on_n[1][4] = 9.47e-12; I_g_on_n[1][5] = 9.47e-12; 
        I_g_on_n[1][6] = 9.47e-12; I_g_on_n[1][7] = 9.47e-12; I_g_on_n[1][8] = 9.47e-12; I_g_on_n[1][9] = 9.47e-12; I_g_on_n[1][10] = 9.47e-12;
        I_g_on_n[1][11] = 9.47e-12; I_g_on_n[1][12] = 9.47e-12; I_g_on_n[1][13] = 9.47e-12; I_g_on_n[1][14] = 9.47e-12; I_g_on_n[1][15] = 9.47e-12; 
        I_g_on_n[1][16] = 9.47e-12; I_g_on_n[1][17] = 9.47e-12; I_g_on_n[1][18] = 9.47e-12; I_g_on_n[1][19] = 9.47e-12; I_g_on_n[1][20] = 9.47e-12;
        I_g_on_n[1][21] = 9.47e-12; I_g_on_n[1][22] = 9.47e-12; I_g_on_n[1][23] = 9.47e-12; I_g_on_n[1][24] = 9.47e-12; I_g_on_n[1][25] = 9.47e-12; 
        I_g_on_n[1][26] = 9.47e-12; I_g_on_n[1][27] = 9.47e-12; I_g_on_n[1][28] = 9.47e-12; I_g_on_n[1][29] = 9.47e-12; I_g_on_n[1][30] = 9.47e-12;
        I_g_on_n[1][31] = 9.47e-12; I_g_on_n[1][32] = 9.47e-12; I_g_on_n[1][33] = 9.47e-12; I_g_on_n[1][34] = 9.47e-12; I_g_on_n[1][35] = 9.47e-12; 
        I_g_on_n[1][36] = 9.47e-12; I_g_on_n[1][37] = 9.47e-12; I_g_on_n[1][38] = 9.47e-12; I_g_on_n[1][39] = 9.47e-12; I_g_on_n[1][40] = 9.47e-12;
        I_g_on_n[1][41] = 9.47e-12; I_g_on_n[1][42] = 9.47e-12; I_g_on_n[1][43] = 9.47e-12; I_g_on_n[1][44] = 9.47e-12; I_g_on_n[1][45] = 9.47e-12; 
        I_g_on_n[1][46] = 9.47e-12; I_g_on_n[1][47] = 9.47e-12; I_g_on_n[1][48] = 9.47e-12; I_g_on_n[1][49] = 9.47e-12; I_g_on_n[1][50] = 9.47e-12;
        I_g_on_n[1][51] = 9.47e-12; I_g_on_n[1][52] = 9.47e-12; I_g_on_n[1][53] = 9.47e-12; I_g_on_n[1][54] = 9.47e-12; I_g_on_n[1][55] = 9.47e-12; 
        I_g_on_n[1][56] = 9.47e-12; I_g_on_n[1][57] = 9.47e-12; I_g_on_n[1][58] = 9.47e-12; I_g_on_n[1][59] = 9.47e-12; I_g_on_n[1][60] = 9.47e-12;
        I_g_on_n[1][61] = 9.47e-12; I_g_on_n[1][62] = 9.47e-12; I_g_on_n[1][63] = 9.47e-12; I_g_on_n[1][64] = 9.47e-12; I_g_on_n[1][65] = 9.47e-12; 
        I_g_on_n[1][66] = 9.47e-12; I_g_on_n[1][67] = 9.47e-12; I_g_on_n[1][68] = 9.47e-12; I_g_on_n[1][69] = 9.47e-12; I_g_on_n[1][70] = 9.47e-12;
        I_g_on_n[1][71] = 9.47e-12; I_g_on_n[1][72] = 9.47e-12; I_g_on_n[1][73] = 9.47e-12; I_g_on_n[1][74] = 9.47e-12; I_g_on_n[1][75] = 9.47e-12; 
        I_g_on_n[1][76] = 9.47e-12; I_g_on_n[1][77] = 9.47e-12; I_g_on_n[1][78] = 9.47e-12; I_g_on_n[1][79] = 9.47e-12; I_g_on_n[1][80] = 9.47e-12;
        I_g_on_n[1][81] = 9.47e-12; I_g_on_n[1][82] = 9.47e-12; I_g_on_n[1][83] = 9.47e-12; I_g_on_n[1][84] = 9.47e-12; I_g_on_n[1][85] = 9.47e-12; 
        I_g_on_n[1][86] = 9.47e-12; I_g_on_n[1][87] = 9.47e-12; I_g_on_n[1][88] = 9.47e-12; I_g_on_n[1][89] = 9.47e-12; I_g_on_n[1][90] = 9.47e-12;
        I_g_on_n[1][91] = 9.47e-12; I_g_on_n[1][92] = 9.47e-12; I_g_on_n[1][93] = 9.47e-12; I_g_on_n[1][94] = 9.47e-12; I_g_on_n[1][95] = 9.47e-12; 
        I_g_on_n[1][96] = 9.47e-12; I_g_on_n[1][97] = 9.47e-12; I_g_on_n[1][98] = 9.47e-12; I_g_on_n[1][99] = 9.47e-12; I_g_on_n[1][100] = 9.47e-12;
        I_off_n[1][0] = 1.01e-11; 
        I_off_n[1][1] = 1.06e-11; I_off_n[1][2] = 1.11e-11; I_off_n[1][3] = 1.17e-11; I_off_n[1][4] = 1.23e-11; I_off_n[1][5] = 1.29e-11; 
        I_off_n[1][6] = 1.36e-11; I_off_n[1][7] = 1.43e-11; I_off_n[1][8] = 1.50e-11; I_off_n[1][9] = 1.57e-11; I_off_n[1][10] = 1.65e-11;
        I_off_n[1][11] = 1.73e-11; I_off_n[1][12] = 1.81e-11; I_off_n[1][13] = 1.90e-11; I_off_n[1][14] = 1.99e-11; I_off_n[1][15] = 2.09e-11; 
        I_off_n[1][16] = 2.19e-11; I_off_n[1][17] = 2.29e-11; I_off_n[1][18] = 2.40e-11; I_off_n[1][19] = 2.51e-11; I_off_n[1][20] = 2.62e-11;
        I_off_n[1][21] = 2.74e-11; I_off_n[1][22] = 2.86e-11; I_off_n[1][23] = 2.99e-11; I_off_n[1][24] = 3.13e-11; I_off_n[1][25] = 3.27e-11; 
        I_off_n[1][26] = 3.42e-11; I_off_n[1][27] = 3.57e-11; I_off_n[1][28] = 3.73e-11; I_off_n[1][29] = 3.89e-11; I_off_n[1][30] = 4.06e-11;
        I_off_n[1][31] = 4.23e-11; I_off_n[1][32] = 4.41e-11; I_off_n[1][33] = 4.60e-11; I_off_n[1][34] = 4.80e-11; I_off_n[1][35] = 5.00e-11; 
        I_off_n[1][36] = 5.21e-11; I_off_n[1][37] = 5.43e-11; I_off_n[1][38] = 5.65e-11; I_off_n[1][39] = 5.88e-11; I_off_n[1][40] = 6.12e-11;
        I_off_n[1][41] = 6.37e-11; I_off_n[1][42] = 6.62e-11; I_off_n[1][43] = 6.89e-11; I_off_n[1][44] = 7.17e-11; I_off_n[1][45] = 7.45e-11; 
        I_off_n[1][46] = 7.75e-11; I_off_n[1][47] = 8.05e-11; I_off_n[1][48] = 8.37e-11; I_off_n[1][49] = 8.69e-11; I_off_n[1][50] = 9.02e-11;
        I_off_n[1][51] = 9.36e-11; I_off_n[1][52] = 9.72e-11; I_off_n[1][53] = 1.01e-10; I_off_n[1][54] = 1.05e-10; I_off_n[1][55] = 1.09e-10; 
        I_off_n[1][56] = 1.13e-10; I_off_n[1][57] = 1.17e-10; I_off_n[1][58] = 1.21e-10; I_off_n[1][59] = 1.26e-10; I_off_n[1][60] = 1.30e-10;
        I_off_n[1][61] = 1.35e-10; I_off_n[1][62] = 1.39e-10; I_off_n[1][63] = 1.44e-10; I_off_n[1][64] = 1.49e-10; I_off_n[1][65] = 1.55e-10; 
        I_off_n[1][66] = 1.60e-10; I_off_n[1][67] = 1.66e-10; I_off_n[1][68] = 1.71e-10; I_off_n[1][69] = 1.77e-10; I_off_n[1][70] = 1.83e-10;
        I_off_n[1][71] = 1.89e-10; I_off_n[1][72] = 1.95e-10; I_off_n[1][73] = 2.02e-10; I_off_n[1][74] = 2.09e-10; I_off_n[1][75] = 2.15e-10; 
        I_off_n[1][76] = 2.22e-10; I_off_n[1][77] = 2.29e-10; I_off_n[1][78] = 2.37e-10; I_off_n[1][79] = 2.44e-10; I_off_n[1][80] = 2.51e-10;
        I_off_n[1][81] = 2.58e-10; I_off_n[1][82] = 2.66e-10; I_off_n[1][83] = 2.74e-10; I_off_n[1][84] = 2.81e-10; I_off_n[1][85] = 2.89e-10; 
        I_off_n[1][86] = 2.97e-10; I_off_n[1][87] = 3.05e-10; I_off_n[1][88] = 3.13e-10; I_off_n[1][89] = 3.21e-10; I_off_n[1][90] = 3.29e-10;
        I_off_n[1][91] = 3.37e-10; I_off_n[1][92] = 3.45e-10; I_off_n[1][93] = 3.53e-10; I_off_n[1][94] = 3.61e-10; I_off_n[1][95] = 3.69e-10; 
        I_off_n[1][96] = 3.77e-10; I_off_n[1][97] = 3.85e-10; I_off_n[1][98] = 3.94e-10; I_off_n[1][99] = 4.02e-10; I_off_n[1][100] = 4.10e-10;

      //ITRS LOP device type
      vdd[2] = 0.7;
      Lphy[2] = 0.022;
      Lelec[2] = 0.016;
      t_ox[2] = 0.9e-3;
      v_th[2] = 0.22599;
      c_ox[2] = 2.82e-14;//F/micron2
      mobility_eff[2] = 508.9 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[2] = 5.71e-2;
      c_g_ideal[2] = 6.2e-16;
      c_fringe[2] = 0.073e-15;
      c_junc[2] = 1e-15;
      I_on_n[2] = 748.9e-6;
      I_on_p[2] = I_on_n[2] / 2;
      nmos_effective_resistance_multiplier = 1.76;
      n_to_p_eff_curr_drv_ratio[2] = 2.28;
      gmp_to_gmn_multiplier[2] = 1.11;
      Rnchannelon[2] = nmos_effective_resistance_multiplier * vdd[2] / I_on_n[2];
      Rpchannelon[2] = n_to_p_eff_curr_drv_ratio[2] * Rnchannelon[2];
      long_channel_leakage_reduction[2] = 1/1.92;
      /*
      I_off_n[2][0] = 4.03e-9;
      I_off_n[2][10] = 5.02e-9;
      I_off_n[2][20] = 6.18e-9;
      I_off_n[2][30] = 7.51e-9;
      I_off_n[2][40] = 9.04e-9;
      I_off_n[2][50] = 1.08e-8;
      I_off_n[2][60] = 1.27e-8;
      I_off_n[2][70] = 1.47e-8;
      I_off_n[2][80] = 1.66e-8;
      I_off_n[2][90] = 1.84e-8;
      I_off_n[2][100] = 2.03e-8;

      I_g_on_n[2][0]  = 3.24e-8;//A/micron
      I_g_on_n[2][10] = 4.01e-8;
      I_g_on_n[2][20] = 4.90e-8;
      I_g_on_n[2][30] = 5.92e-8;
      I_g_on_n[2][40] = 7.08e-8;
      I_g_on_n[2][50] = 8.38e-8;
      I_g_on_n[2][60] = 9.82e-8;
      I_g_on_n[2][70] = 1.14e-7;
      I_g_on_n[2][80] = 1.29e-7;
      I_g_on_n[2][90] = 1.43e-7;
      I_g_on_n[2][100] = 1.54e-7;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[2][0] = 3.24e-8; 
        I_g_on_n[2][1] = 3.31e-8; I_g_on_n[2][2] = 3.38e-8; I_g_on_n[2][3] = 3.46e-8; I_g_on_n[2][4] = 3.53e-8; I_g_on_n[2][5] = 3.61e-8; 
        I_g_on_n[2][6] = 3.69e-8; I_g_on_n[2][7] = 3.77e-8; I_g_on_n[2][8] = 3.85e-8; I_g_on_n[2][9] = 3.93e-8; I_g_on_n[2][10] = 4.01e-8;
        I_g_on_n[2][11] = 4.09e-8; I_g_on_n[2][12] = 4.18e-8; I_g_on_n[2][13] = 4.26e-8; I_g_on_n[2][14] = 4.35e-8; I_g_on_n[2][15] = 4.44e-8; 
        I_g_on_n[2][16] = 4.53e-8; I_g_on_n[2][17] = 4.62e-8; I_g_on_n[2][18] = 4.71e-8; I_g_on_n[2][19] = 4.81e-8; I_g_on_n[2][20] = 4.90e-8;
        I_g_on_n[2][21] = 5.00e-8; I_g_on_n[2][22] = 5.09e-8; I_g_on_n[2][23] = 5.19e-8; I_g_on_n[2][24] = 5.29e-8; I_g_on_n[2][25] = 5.39e-8; 
        I_g_on_n[2][26] = 5.50e-8; I_g_on_n[2][27] = 5.60e-8; I_g_on_n[2][28] = 5.71e-8; I_g_on_n[2][29] = 5.81e-8; I_g_on_n[2][30] = 5.92e-8;
        I_g_on_n[2][31] = 6.03e-8; I_g_on_n[2][32] = 6.14e-8; I_g_on_n[2][33] = 6.25e-8; I_g_on_n[2][34] = 6.37e-8; I_g_on_n[2][35] = 6.48e-8; 
        I_g_on_n[2][36] = 6.60e-8; I_g_on_n[2][37] = 6.72e-8; I_g_on_n[2][38] = 6.84e-8; I_g_on_n[2][39] = 6.96e-8; I_g_on_n[2][40] = 7.08e-8;
        I_g_on_n[2][41] = 7.20e-8; I_g_on_n[2][42] = 7.33e-8; I_g_on_n[2][43] = 7.45e-8; I_g_on_n[2][44] = 7.58e-8; I_g_on_n[2][45] = 7.71e-8; 
        I_g_on_n[2][46] = 7.84e-8; I_g_on_n[2][47] = 7.98e-8; I_g_on_n[2][48] = 8.11e-8; I_g_on_n[2][49] = 8.24e-8; I_g_on_n[2][50] = 8.38e-8;
        I_g_on_n[2][51] = 8.52e-8; I_g_on_n[2][52] = 8.66e-8; I_g_on_n[2][53] = 8.80e-8; I_g_on_n[2][54] = 8.94e-8; I_g_on_n[2][55] = 9.08e-8; 
        I_g_on_n[2][56] = 9.23e-8; I_g_on_n[2][57] = 9.37e-8; I_g_on_n[2][58] = 9.52e-8; I_g_on_n[2][59] = 9.67e-8; I_g_on_n[2][60] = 9.82e-8;
        I_g_on_n[2][61] = 9.97e-8; I_g_on_n[2][62] = 1.01e-7; I_g_on_n[2][63] = 1.03e-7; I_g_on_n[2][64] = 1.04e-7; I_g_on_n[2][65] = 1.06e-7; 
        I_g_on_n[2][66] = 1.08e-7; I_g_on_n[2][67] = 1.09e-7; I_g_on_n[2][68] = 1.11e-7; I_g_on_n[2][69] = 1.12e-7; I_g_on_n[2][70] = 1.14e-7;
        I_g_on_n[2][71] = 1.16e-7; I_g_on_n[2][72] = 1.17e-7; I_g_on_n[2][73] = 1.19e-7; I_g_on_n[2][74] = 1.20e-7; I_g_on_n[2][75] = 1.22e-7; 
        I_g_on_n[2][76] = 1.23e-7; I_g_on_n[2][77] = 1.25e-7; I_g_on_n[2][78] = 1.26e-7; I_g_on_n[2][79] = 1.28e-7; I_g_on_n[2][80] = 1.29e-7;
        I_g_on_n[2][81] = 1.30e-7; I_g_on_n[2][82] = 1.32e-7; I_g_on_n[2][83] = 1.33e-7; I_g_on_n[2][84] = 1.35e-7; I_g_on_n[2][85] = 1.36e-7; 
        I_g_on_n[2][86] = 1.38e-7; I_g_on_n[2][87] = 1.39e-7; I_g_on_n[2][88] = 1.40e-7; I_g_on_n[2][89] = 1.42e-7; I_g_on_n[2][90] = 1.43e-7;
        I_g_on_n[2][91] = 1.44e-7; I_g_on_n[2][92] = 1.45e-7; I_g_on_n[2][93] = 1.47e-7; I_g_on_n[2][94] = 1.48e-7; I_g_on_n[2][95] = 1.49e-7; 
        I_g_on_n[2][96] = 1.50e-7; I_g_on_n[2][97] = 1.51e-7; I_g_on_n[2][98] = 1.52e-7; I_g_on_n[2][99] = 1.53e-7; I_g_on_n[2][100] = 1.54e-7;
        I_off_n[2][0] = 4.03e-9; 
        I_off_n[2][1] = 4.12e-9; I_off_n[2][2] = 4.21e-9; I_off_n[2][3] = 4.31e-9; I_off_n[2][4] = 4.41e-9; I_off_n[2][5] = 4.50e-9; 
        I_off_n[2][6] = 4.60e-9; I_off_n[2][7] = 4.71e-9; I_off_n[2][8] = 4.81e-9; I_off_n[2][9] = 4.91e-9; I_off_n[2][10] = 5.02e-9;
        I_off_n[2][11] = 5.13e-9; I_off_n[2][12] = 5.24e-9; I_off_n[2][13] = 5.35e-9; I_off_n[2][14] = 5.46e-9; I_off_n[2][15] = 5.58e-9; 
        I_off_n[2][16] = 5.70e-9; I_off_n[2][17] = 5.81e-9; I_off_n[2][18] = 5.93e-9; I_off_n[2][19] = 6.06e-9; I_off_n[2][20] = 6.18e-9;
        I_off_n[2][21] = 6.30e-9; I_off_n[2][22] = 6.43e-9; I_off_n[2][23] = 6.56e-9; I_off_n[2][24] = 6.69e-9; I_off_n[2][25] = 6.82e-9; 
        I_off_n[2][26] = 6.96e-9; I_off_n[2][27] = 7.09e-9; I_off_n[2][28] = 7.23e-9; I_off_n[2][29] = 7.37e-9; I_off_n[2][30] = 7.51e-9;
        I_off_n[2][31] = 7.65e-9; I_off_n[2][32] = 7.80e-9; I_off_n[2][33] = 7.95e-9; I_off_n[2][34] = 8.10e-9; I_off_n[2][35] = 8.25e-9; 
        I_off_n[2][36] = 8.40e-9; I_off_n[2][37] = 8.56e-9; I_off_n[2][38] = 8.72e-9; I_off_n[2][39] = 8.88e-9; I_off_n[2][40] = 9.04e-9;
        I_off_n[2][41] = 9.21e-9; I_off_n[2][42] = 9.37e-9; I_off_n[2][43] = 9.55e-9; I_off_n[2][44] = 9.72e-9; I_off_n[2][45] = 9.90e-9; 
        I_off_n[2][46] = 1.01e-8; I_off_n[2][47] = 1.03e-8; I_off_n[2][48] = 1.04e-8; I_off_n[2][49] = 1.06e-8; I_off_n[2][50] = 1.08e-8;
        I_off_n[2][51] = 1.10e-8; I_off_n[2][52] = 1.12e-8; I_off_n[2][53] = 1.14e-8; I_off_n[2][54] = 1.15e-8; I_off_n[2][55] = 1.17e-8; 
        I_off_n[2][56] = 1.19e-8; I_off_n[2][57] = 1.21e-8; I_off_n[2][58] = 1.23e-8; I_off_n[2][59] = 1.25e-8; I_off_n[2][60] = 1.27e-8;
        I_off_n[2][61] = 1.29e-8; I_off_n[2][62] = 1.31e-8; I_off_n[2][63] = 1.33e-8; I_off_n[2][64] = 1.35e-8; I_off_n[2][65] = 1.37e-8; 
        I_off_n[2][66] = 1.39e-8; I_off_n[2][67] = 1.41e-8; I_off_n[2][68] = 1.43e-8; I_off_n[2][69] = 1.45e-8; I_off_n[2][70] = 1.47e-8;
        I_off_n[2][71] = 1.49e-8; I_off_n[2][72] = 1.51e-8; I_off_n[2][73] = 1.53e-8; I_off_n[2][74] = 1.55e-8; I_off_n[2][75] = 1.57e-8; 
        I_off_n[2][76] = 1.59e-8; I_off_n[2][77] = 1.60e-8; I_off_n[2][78] = 1.62e-8; I_off_n[2][79] = 1.64e-8; I_off_n[2][80] = 1.66e-8;
        I_off_n[2][81] = 1.68e-8; I_off_n[2][82] = 1.70e-8; I_off_n[2][83] = 1.71e-8; I_off_n[2][84] = 1.73e-8; I_off_n[2][85] = 1.75e-8; 
        I_off_n[2][86] = 1.77e-8; I_off_n[2][87] = 1.79e-8; I_off_n[2][88] = 1.80e-8; I_off_n[2][89] = 1.82e-8; I_off_n[2][90] = 1.84e-8;
        I_off_n[2][91] = 1.86e-8; I_off_n[2][92] = 1.88e-8; I_off_n[2][93] = 1.90e-8; I_off_n[2][94] = 1.91e-8; I_off_n[2][95] = 1.93e-8; 
        I_off_n[2][96] = 1.95e-8; I_off_n[2][97] = 1.97e-8; I_off_n[2][98] = 1.99e-8; I_off_n[2][99] = 2.01e-8; I_off_n[2][100] = 2.03e-8;

      if (ram_cell_tech_type == lp_dram)
      {
        //LP-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.1;
        Lphy[3] = 0.078;
        Lelec[3] = 0.0504;// Assume Lelec is 30% lesser than Lphy for DRAM access and wordline transistors.
        curr_v_th_dram_access_transistor = 0.44559;
        width_dram_access_transistor = 0.079;
        curr_I_on_dram_cell = 36e-6;//A
        curr_I_off_dram_cell_worst_case_length_temp = 19.5e-12;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram  = 0;
        curr_area_cell_dram = width_dram_access_transistor * Lphy[3] * 10.0;
        curr_asp_ratio_cell_dram = 1.46;
        curr_c_dram_cell = 20e-15;

        //LP-DRAM wordline transistor parameters
        curr_vpp = 1.5;
        t_ox[3] = 2.1e-3;
        v_th[3] = 0.44559;
        c_ox[3] = 1.41e-14;
        mobility_eff[3] =   426.30 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.181;
        c_g_ideal[3] = 1.10e-15;
        c_fringe[3] = 0.08e-15;
        c_junc[3] = 1e-15;
        I_on_n[3] = 456e-6;
        I_on_p[3] = I_on_n[3] / 2;
        nmos_effective_resistance_multiplier = 1.65;
        n_to_p_eff_curr_drv_ratio[3] = 2.05;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0] = 2.54e-11;
        I_off_n[3][10] = 3.94e-11;
        I_off_n[3][20] = 5.95e-11;
        I_off_n[3][30] = 8.79e-11;
        I_off_n[3][40] = 1.27e-10;
        I_off_n[3][50] = 1.79e-10;
        I_off_n[3][60] = 2.47e-10;
        I_off_n[3][70] = 3.31e-10;
        I_off_n[3][80] = 4.26e-10;
        I_off_n[3][90] = 5.27e-10;
        I_off_n[3][100] = 6.46e-10;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 2.54e-11; 
        I_off_n[3][1] = 2.65e-11; I_off_n[3][2] = 2.77e-11; I_off_n[3][3] = 2.90e-11; I_off_n[3][4] = 3.03e-11; I_off_n[3][5] = 3.17e-11; 
        I_off_n[3][6] = 3.31e-11; I_off_n[3][7] = 3.46e-11; I_off_n[3][8] = 3.62e-11; I_off_n[3][9] = 3.78e-11; I_off_n[3][10] = 3.94e-11;
        I_off_n[3][11] = 4.11e-11; I_off_n[3][12] = 4.28e-11; I_off_n[3][13] = 4.47e-11; I_off_n[3][14] = 4.66e-11; I_off_n[3][15] = 4.86e-11; 
        I_off_n[3][16] = 5.06e-11; I_off_n[3][17] = 5.27e-11; I_off_n[3][18] = 5.49e-11; I_off_n[3][19] = 5.72e-11; I_off_n[3][20] = 5.95e-11;
        I_off_n[3][21] = 6.19e-11; I_off_n[3][22] = 6.44e-11; I_off_n[3][23] = 6.70e-11; I_off_n[3][24] = 6.97e-11; I_off_n[3][25] = 7.25e-11; 
        I_off_n[3][26] = 7.54e-11; I_off_n[3][27] = 7.84e-11; I_off_n[3][28] = 8.15e-11; I_off_n[3][29] = 8.47e-11; I_off_n[3][30] = 8.79e-11;
        I_off_n[3][31] = 9.13e-11; I_off_n[3][32] = 9.47e-11; I_off_n[3][33] = 9.84e-11; I_off_n[3][34] = 1.02e-10; I_off_n[3][35] = 1.06e-10; 
        I_off_n[3][36] = 1.10e-10; I_off_n[3][37] = 1.14e-10; I_off_n[3][38] = 1.18e-10; I_off_n[3][39] = 1.23e-10; I_off_n[3][40] = 1.27e-10;
        I_off_n[3][41] = 1.32e-10; I_off_n[3][42] = 1.36e-10; I_off_n[3][43] = 1.41e-10; I_off_n[3][44] = 1.46e-10; I_off_n[3][45] = 1.51e-10; 
        I_off_n[3][46] = 1.56e-10; I_off_n[3][47] = 1.62e-10; I_off_n[3][48] = 1.67e-10; I_off_n[3][49] = 1.73e-10; I_off_n[3][50] = 1.79e-10;
        I_off_n[3][51] = 1.85e-10; I_off_n[3][52] = 1.91e-10; I_off_n[3][53] = 1.98e-10; I_off_n[3][54] = 2.04e-10; I_off_n[3][55] = 2.11e-10; 
        I_off_n[3][56] = 2.18e-10; I_off_n[3][57] = 2.25e-10; I_off_n[3][58] = 2.32e-10; I_off_n[3][59] = 2.40e-10; I_off_n[3][60] = 2.47e-10;
        I_off_n[3][61] = 2.55e-10; I_off_n[3][62] = 2.63e-10; I_off_n[3][63] = 2.71e-10; I_off_n[3][64] = 2.79e-10; I_off_n[3][65] = 2.87e-10; 
        I_off_n[3][66] = 2.96e-10; I_off_n[3][67] = 3.04e-10; I_off_n[3][68] = 3.13e-10; I_off_n[3][69] = 3.22e-10; I_off_n[3][70] = 3.31e-10;
        I_off_n[3][71] = 3.40e-10; I_off_n[3][72] = 3.49e-10; I_off_n[3][73] = 3.58e-10; I_off_n[3][74] = 3.68e-10; I_off_n[3][75] = 3.77e-10; 
        I_off_n[3][76] = 3.87e-10; I_off_n[3][77] = 3.97e-10; I_off_n[3][78] = 4.06e-10; I_off_n[3][79] = 4.16e-10; I_off_n[3][80] = 4.26e-10;
        I_off_n[3][81] = 4.36e-10; I_off_n[3][82] = 4.46e-10; I_off_n[3][83] = 4.55e-10; I_off_n[3][84] = 4.65e-10; I_off_n[3][85] = 4.75e-10; 
        I_off_n[3][86] = 4.85e-10; I_off_n[3][87] = 4.95e-10; I_off_n[3][88] = 5.06e-10; I_off_n[3][89] = 5.16e-10; I_off_n[3][90] = 5.27e-10;
        I_off_n[3][91] = 5.38e-10; I_off_n[3][92] = 5.49e-10; I_off_n[3][93] = 5.61e-10; I_off_n[3][94] = 5.72e-10; I_off_n[3][95] = 5.84e-10; 
        I_off_n[3][96] = 5.96e-10; I_off_n[3][97] = 6.08e-10; I_off_n[3][98] = 6.21e-10; I_off_n[3][99] = 6.33e-10; I_off_n[3][100] = 6.46e-10;
      }
      else if (ram_cell_tech_type == comm_dram)
      {
        //COMM-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.1;
        Lphy[3] = 0.045;
        Lelec[3] = 0.0298;
        curr_v_th_dram_access_transistor = 1;
        width_dram_access_transistor = 0.045;
        curr_I_on_dram_cell = 20e-6;//A
        curr_I_off_dram_cell_worst_case_length_temp = 1e-15;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram  = 0;
        curr_area_cell_dram = 6*0.045*0.045;
        curr_asp_ratio_cell_dram = 1.5;
        curr_c_dram_cell = 30e-15;

        //COMM-DRAM wordline transistor parameters
        curr_vpp = 2.7;
        t_ox[3] = 4e-3;
        v_th[3] = 1.0;
        c_ox[3] = 7.98e-15;
        mobility_eff[3] = 368.58 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.147;
        c_g_ideal[3] = 3.59e-16;
        c_fringe[3] = 0.08e-15;
        c_junc[3] = 1e-15;
        I_on_n[3] = 999.4e-6;
        I_on_p[3] = I_on_n[3] / 2;
        nmos_effective_resistance_multiplier = 1.69;
        n_to_p_eff_curr_drv_ratio[3] = 1.95;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0] = 1.31e-14;
        I_off_n[3][10] = 2.68e-14;
        I_off_n[3][20] = 5.25e-14;
        I_off_n[3][30] = 9.88e-14;
        I_off_n[3][40] = 1.79e-13;
        I_off_n[3][50] = 3.15e-13;
        I_off_n[3][60] = 5.36e-13;
        I_off_n[3][70] = 8.86e-13;
        I_off_n[3][80] = 1.42e-12;
        I_off_n[3][90] = 2.20e-12;
        I_off_n[3][100] = 3.29e-12;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 1.31e-14; 
        I_off_n[3][1] = 1.39e-14; I_off_n[3][2] = 1.49e-14; I_off_n[3][3] = 1.61e-14; I_off_n[3][4] = 1.73e-14; I_off_n[3][5] = 1.87e-14; 
        I_off_n[3][6] = 2.01e-14; I_off_n[3][7] = 2.17e-14; I_off_n[3][8] = 2.33e-14; I_off_n[3][9] = 2.50e-14; I_off_n[3][10] = 2.68e-14;
        I_off_n[3][11] = 2.87e-14; I_off_n[3][12] = 3.07e-14; I_off_n[3][13] = 3.29e-14; I_off_n[3][14] = 3.52e-14; I_off_n[3][15] = 3.78e-14; 
        I_off_n[3][16] = 4.04e-14; I_off_n[3][17] = 4.32e-14; I_off_n[3][18] = 4.62e-14; I_off_n[3][19] = 4.93e-14; I_off_n[3][20] = 5.25e-14;
        I_off_n[3][21] = 5.59e-14; I_off_n[3][22] = 5.97e-14; I_off_n[3][23] = 6.37e-14; I_off_n[3][24] = 6.79e-14; I_off_n[3][25] = 7.24e-14; 
        I_off_n[3][26] = 7.72e-14; I_off_n[3][27] = 8.23e-14; I_off_n[3][28] = 8.75e-14; I_off_n[3][29] = 9.30e-14; I_off_n[3][30] = 9.88e-14;
        I_off_n[3][31] = 1.05e-13; I_off_n[3][32] = 1.11e-13; I_off_n[3][33] = 1.18e-13; I_off_n[3][34] = 1.26e-13; I_off_n[3][35] = 1.34e-13; 
        I_off_n[3][36] = 1.42e-13; I_off_n[3][37] = 1.51e-13; I_off_n[3][38] = 1.60e-13; I_off_n[3][39] = 1.69e-13; I_off_n[3][40] = 1.79e-13;
        I_off_n[3][41] = 1.89e-13; I_off_n[3][42] = 2.01e-13; I_off_n[3][43] = 2.13e-13; I_off_n[3][44] = 2.25e-13; I_off_n[3][45] = 2.39e-13; 
        I_off_n[3][46] = 2.53e-13; I_off_n[3][47] = 2.67e-13; I_off_n[3][48] = 2.83e-13; I_off_n[3][49] = 2.98e-13; I_off_n[3][50] = 3.15e-13;
        I_off_n[3][51] = 3.32e-13; I_off_n[3][52] = 3.51e-13; I_off_n[3][53] = 3.70e-13; I_off_n[3][54] = 3.91e-13; I_off_n[3][55] = 4.13e-13; 
        I_off_n[3][56] = 4.35e-13; I_off_n[3][57] = 4.59e-13; I_off_n[3][58] = 4.84e-13; I_off_n[3][59] = 5.09e-13; I_off_n[3][60] = 5.36e-13;
        I_off_n[3][61] = 5.64e-13; I_off_n[3][62] = 5.94e-13; I_off_n[3][63] = 6.25e-13; I_off_n[3][64] = 6.58e-13; I_off_n[3][65] = 6.92e-13; 
        I_off_n[3][66] = 7.28e-13; I_off_n[3][67] = 7.65e-13; I_off_n[3][68] = 8.04e-13; I_off_n[3][69] = 8.44e-13; I_off_n[3][70] = 8.86e-13;
        I_off_n[3][71] = 9.29e-13; I_off_n[3][72] = 9.75e-13; I_off_n[3][73] = 1.02e-12; I_off_n[3][74] = 1.07e-12; I_off_n[3][75] = 1.13e-12; 
        I_off_n[3][76] = 1.18e-12; I_off_n[3][77] = 1.24e-12; I_off_n[3][78] = 1.30e-12; I_off_n[3][79] = 1.36e-12; I_off_n[3][80] = 1.42e-12;
        I_off_n[3][81] = 1.49e-12; I_off_n[3][82] = 1.55e-12; I_off_n[3][83] = 1.62e-12; I_off_n[3][84] = 1.70e-12; I_off_n[3][85] = 1.78e-12; 
        I_off_n[3][86] = 1.86e-12; I_off_n[3][87] = 1.94e-12; I_off_n[3][88] = 2.02e-12; I_off_n[3][89] = 2.11e-12; I_off_n[3][90] = 2.20e-12;
        I_off_n[3][91] = 2.29e-12; I_off_n[3][92] = 2.39e-12; I_off_n[3][93] = 2.49e-12; I_off_n[3][94] = 2.60e-12; I_off_n[3][95] = 2.70e-12; 
        I_off_n[3][96] = 2.81e-12; I_off_n[3][97] = 2.93e-12; I_off_n[3][98] = 3.05e-12; I_off_n[3][99] = 3.17e-12; I_off_n[3][100] = 3.29e-12;
      }


      //SRAM cell properties
      curr_Wmemcella_sram = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
      curr_area_cell_sram = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_sram = 1.46;
      //CAM cell properties //TODO: data need to be revisited
      curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
      curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_cam = 2.92;
      //Empirical undifferetiated core/FU coefficient
      curr_logic_scaling_co_eff = 0.7*0.7;
      curr_core_tx_density      = 1.25;
      curr_sckt_co_eff           = 1.1387;
      curr_chip_layout_overhead  = 1.2;//die measurement results based on Niagara 1 and 2
      curr_macro_layout_overhead = 1.1;//EDA placement and routing tool rule of thumb
    }

    if (tech == 32)
    {
      SENSE_AMP_D = .03e-9; // s
      SENSE_AMP_P = 2.16e-15; // J
      //For 2013, MPU/ASIC stagger-contacted M1 half-pitch is 32 nm (so this is 32 nm
      //technology i.e. FEATURESIZE = 0.032). Using the SOI process numbers for
      //HP and LSTP.
      vdd[0] = 0.9;
      Lphy[0] = 0.013;
      Lelec[0] = 0.01013;
      t_ox[0] = 0.5e-3;
      v_th[0] = 0.21835;
      c_ox[0] = 4.11e-14;
      mobility_eff[0] = 361.84 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[0] = 5.09E-2;
      c_g_ideal[0] = 5.34e-16;
      c_fringe[0] = 0.04e-15;
      c_junc[0] = 1e-15;
      I_on_n[0] =  2211.7e-6;
      I_on_p[0] = I_on_n[0] / 2;
      nmos_effective_resistance_multiplier = 1.49;
      n_to_p_eff_curr_drv_ratio[0] = 2.41;
      gmp_to_gmn_multiplier[0] = 1.38;
      Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];//ohm-micron
      Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];//ohm-micron
      long_channel_leakage_reduction[0] = 1/3.706;
      //Using MASTAR, @300K (380K does not work in MASTAR), increase Lgate until Ion reduces to 95% or Lgate increase by 5% (DG device can only increase by 5%),
      //whichever comes first
      /*
      I_off_n[0][0] = 1.52e-7;
      I_off_n[0][10] = 1.55e-7;
      I_off_n[0][20] = 1.59e-7;
      I_off_n[0][30] = 1.68e-7;
      I_off_n[0][40] = 1.90e-7;
      I_off_n[0][50] = 2.69e-7;
      I_off_n[0][60] = 5.32e-7;
      I_off_n[0][70] = 1.02e-6;
      I_off_n[0][80] = 1.62e-6;
      I_off_n[0][90] = 2.73e-6;
      I_off_n[0][100] = 6.1e-6;

      I_g_on_n[0][0]  = 6.55e-8;//A/micron
      I_g_on_n[0][10] = 6.55e-8;
      I_g_on_n[0][20] = 6.55e-8;
      I_g_on_n[0][30] = 6.55e-8;
      I_g_on_n[0][40] = 6.55e-8;
      I_g_on_n[0][50] = 6.55e-8;
      I_g_on_n[0][60] = 6.55e-8;
      I_g_on_n[0][70] = 6.55e-8;
      I_g_on_n[0][80] = 6.55e-8;
      I_g_on_n[0][90] = 6.55e-8;
      I_g_on_n[0][100] = 6.55e-8;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[0][0] = 6.55e-8; 
      I_g_on_n[0][1] = 6.55e-8; I_g_on_n[0][2] = 6.55e-8; I_g_on_n[0][3] = 6.55e-8; I_g_on_n[0][4] = 6.55e-8; I_g_on_n[0][5] = 6.55e-8; 
      I_g_on_n[0][6] = 6.55e-8; I_g_on_n[0][7] = 6.55e-8; I_g_on_n[0][8] = 6.55e-8; I_g_on_n[0][9] = 6.55e-8; I_g_on_n[0][10] = 6.55e-8;
      I_g_on_n[0][11] = 6.55e-8; I_g_on_n[0][12] = 6.55e-8; I_g_on_n[0][13] = 6.55e-8; I_g_on_n[0][14] = 6.55e-8; I_g_on_n[0][15] = 6.55e-8; 
      I_g_on_n[0][16] = 6.55e-8; I_g_on_n[0][17] = 6.55e-8; I_g_on_n[0][18] = 6.55e-8; I_g_on_n[0][19] = 6.55e-8; I_g_on_n[0][20] = 6.55e-8;
      I_g_on_n[0][21] = 6.55e-8; I_g_on_n[0][22] = 6.55e-8; I_g_on_n[0][23] = 6.55e-8; I_g_on_n[0][24] = 6.55e-8; I_g_on_n[0][25] = 6.55e-8; 
      I_g_on_n[0][26] = 6.55e-8; I_g_on_n[0][27] = 6.55e-8; I_g_on_n[0][28] = 6.55e-8; I_g_on_n[0][29] = 6.55e-8; I_g_on_n[0][30] = 6.55e-8;
      I_g_on_n[0][31] = 6.55e-8; I_g_on_n[0][32] = 6.55e-8; I_g_on_n[0][33] = 6.55e-8; I_g_on_n[0][34] = 6.55e-8; I_g_on_n[0][35] = 6.55e-8; 
      I_g_on_n[0][36] = 6.55e-8; I_g_on_n[0][37] = 6.55e-8; I_g_on_n[0][38] = 6.55e-8; I_g_on_n[0][39] = 6.55e-8; I_g_on_n[0][40] = 6.55e-8;
      I_g_on_n[0][41] = 6.55e-8; I_g_on_n[0][42] = 6.55e-8; I_g_on_n[0][43] = 6.55e-8; I_g_on_n[0][44] = 6.55e-8; I_g_on_n[0][45] = 6.55e-8; 
      I_g_on_n[0][46] = 6.55e-8; I_g_on_n[0][47] = 6.55e-8; I_g_on_n[0][48] = 6.55e-8; I_g_on_n[0][49] = 6.55e-8; I_g_on_n[0][50] = 6.55e-8;
      I_g_on_n[0][51] = 6.55e-8; I_g_on_n[0][52] = 6.55e-8; I_g_on_n[0][53] = 6.55e-8; I_g_on_n[0][54] = 6.55e-8; I_g_on_n[0][55] = 6.55e-8; 
      I_g_on_n[0][56] = 6.55e-8; I_g_on_n[0][57] = 6.55e-8; I_g_on_n[0][58] = 6.55e-8; I_g_on_n[0][59] = 6.55e-8; I_g_on_n[0][60] = 6.55e-8;
      I_g_on_n[0][61] = 6.55e-8; I_g_on_n[0][62] = 6.55e-8; I_g_on_n[0][63] = 6.55e-8; I_g_on_n[0][64] = 6.55e-8; I_g_on_n[0][65] = 6.55e-8; 
      I_g_on_n[0][66] = 6.55e-8; I_g_on_n[0][67] = 6.55e-8; I_g_on_n[0][68] = 6.55e-8; I_g_on_n[0][69] = 6.55e-8; I_g_on_n[0][70] = 6.55e-8;
      I_g_on_n[0][71] = 6.55e-8; I_g_on_n[0][72] = 6.55e-8; I_g_on_n[0][73] = 6.55e-8; I_g_on_n[0][74] = 6.55e-8; I_g_on_n[0][75] = 6.55e-8; 
      I_g_on_n[0][76] = 6.55e-8; I_g_on_n[0][77] = 6.55e-8; I_g_on_n[0][78] = 6.55e-8; I_g_on_n[0][79] = 6.55e-8; I_g_on_n[0][80] = 6.55e-8;
      I_g_on_n[0][81] = 6.55e-8; I_g_on_n[0][82] = 6.55e-8; I_g_on_n[0][83] = 6.55e-8; I_g_on_n[0][84] = 6.55e-8; I_g_on_n[0][85] = 6.55e-8; 
      I_g_on_n[0][86] = 6.55e-8; I_g_on_n[0][87] = 6.55e-8; I_g_on_n[0][88] = 6.55e-8; I_g_on_n[0][89] = 6.55e-8; I_g_on_n[0][90] = 6.55e-8;
      I_g_on_n[0][91] = 6.55e-8; I_g_on_n[0][92] = 6.55e-8; I_g_on_n[0][93] = 6.55e-8; I_g_on_n[0][94] = 6.55e-8; I_g_on_n[0][95] = 6.55e-8; 
      I_g_on_n[0][96] = 6.55e-8; I_g_on_n[0][97] = 6.55e-8; I_g_on_n[0][98] = 6.55e-8; I_g_on_n[0][99] = 6.55e-8; I_g_on_n[0][100] = 6.55e-8;
      I_off_n[0][0] = 1.52e-7; 
      I_off_n[0][1] = 1.52e-7; I_off_n[0][2] = 1.53e-7; I_off_n[0][3] = 1.53e-7; I_off_n[0][4] = 1.53e-7; I_off_n[0][5] = 1.53e-7; 
      I_off_n[0][6] = 1.54e-7; I_off_n[0][7] = 1.54e-7; I_off_n[0][8] = 1.54e-7; I_off_n[0][9] = 1.55e-7; I_off_n[0][10] = 1.55e-7;
      I_off_n[0][11] = 1.55e-7; I_off_n[0][12] = 1.56e-7; I_off_n[0][13] = 1.56e-7; I_off_n[0][14] = 1.56e-7; I_off_n[0][15] = 1.57e-7; 
      I_off_n[0][16] = 1.57e-7; I_off_n[0][17] = 1.58e-7; I_off_n[0][18] = 1.58e-7; I_off_n[0][19] = 1.58e-7; I_off_n[0][20] = 1.59e-7;
      I_off_n[0][21] = 1.60e-7; I_off_n[0][22] = 1.60e-7; I_off_n[0][23] = 1.61e-7; I_off_n[0][24] = 1.62e-7; I_off_n[0][25] = 1.63e-7; 
      I_off_n[0][26] = 1.64e-7; I_off_n[0][27] = 1.65e-7; I_off_n[0][28] = 1.66e-7; I_off_n[0][29] = 1.67e-7; I_off_n[0][30] = 1.68e-7;
      I_off_n[0][31] = 1.69e-7; I_off_n[0][32] = 1.71e-7; I_off_n[0][33] = 1.72e-7; I_off_n[0][34] = 1.74e-7; I_off_n[0][35] = 1.76e-7; 
      I_off_n[0][36] = 1.79e-7; I_off_n[0][37] = 1.81e-7; I_off_n[0][38] = 1.84e-7; I_off_n[0][39] = 1.87e-7; I_off_n[0][40] = 1.90e-7;
      I_off_n[0][41] = 1.94e-7; I_off_n[0][42] = 1.99e-7; I_off_n[0][43] = 2.04e-7; I_off_n[0][44] = 2.11e-7; I_off_n[0][45] = 2.19e-7; 
      I_off_n[0][46] = 2.27e-7; I_off_n[0][47] = 2.36e-7; I_off_n[0][48] = 2.46e-7; I_off_n[0][49] = 2.57e-7; I_off_n[0][50] = 2.69e-7;
      I_off_n[0][51] = 2.83e-7; I_off_n[0][52] = 3.01e-7; I_off_n[0][53] = 3.22e-7; I_off_n[0][54] = 3.46e-7; I_off_n[0][55] = 3.73e-7; 
      I_off_n[0][56] = 4.02e-7; I_off_n[0][57] = 4.33e-7; I_off_n[0][58] = 4.65e-7; I_off_n[0][59] = 4.98e-7; I_off_n[0][60] = 5.32e-7;
      I_off_n[0][61] = 5.69e-7; I_off_n[0][62] = 6.09e-7; I_off_n[0][63] = 6.54e-7; I_off_n[0][64] = 7.01e-7; I_off_n[0][65] = 7.51e-7; 
      I_off_n[0][66] = 8.04e-7; I_off_n[0][67] = 8.57e-7; I_off_n[0][68] = 9.11e-7; I_off_n[0][69] = 9.66e-7; I_off_n[0][70] = 1.02e-6;
      I_off_n[0][71] = 1.07e-6; I_off_n[0][72] = 1.13e-6; I_off_n[0][73] = 1.18e-6; I_off_n[0][74] = 1.23e-6; I_off_n[0][75] = 1.29e-6; 
      I_off_n[0][76] = 1.35e-6; I_off_n[0][77] = 1.41e-6; I_off_n[0][78] = 1.48e-6; I_off_n[0][79] = 1.54e-6; I_off_n[0][80] = 1.62e-6;
      I_off_n[0][81] = 1.70e-6; I_off_n[0][82] = 1.78e-6; I_off_n[0][83] = 1.87e-6; I_off_n[0][84] = 1.96e-6; I_off_n[0][85] = 2.06e-6; 
      I_off_n[0][86] = 2.17e-6; I_off_n[0][87] = 2.29e-6; I_off_n[0][88] = 2.43e-6; I_off_n[0][89] = 2.57e-6; I_off_n[0][90] = 2.73e-6;
      I_off_n[0][91] = 2.92e-6; I_off_n[0][92] = 3.15e-6; I_off_n[0][93] = 3.42e-6; I_off_n[0][94] = 3.72e-6; I_off_n[0][95] = 4.06e-6; 
      I_off_n[0][96] = 4.43e-6; I_off_n[0][97] = 4.82e-6; I_off_n[0][98] = 5.23e-6; I_off_n[0][99] = 5.66e-6; I_off_n[0][100] = 6.10e-6;
//      32 DG
//      I_g_on_n[0][0]  = 2.71e-9;//A/micron
//      I_g_on_n[0][10] = 2.71e-9;
//      I_g_on_n[0][20] = 2.71e-9;
//      I_g_on_n[0][30] = 2.71e-9;
//      I_g_on_n[0][40] = 2.71e-9;
//      I_g_on_n[0][50] = 2.71e-9;
//      I_g_on_n[0][60] = 2.71e-9;
//      I_g_on_n[0][70] = 2.71e-9;
//      I_g_on_n[0][80] = 2.71e-9;
//      I_g_on_n[0][90] = 2.71e-9;
//      I_g_on_n[0][100] = 2.71e-9;

      //LSTP device type
      vdd[1] = 1;
      Lphy[1] = 0.020;
      Lelec[1] = 0.0173;
      t_ox[1] = 1.2e-3;
      v_th[1] = 0.513;
      c_ox[1] = 2.29e-14;
      mobility_eff[1] =  347.46 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[1] = 8.64e-2;
      c_g_ideal[1] = 4.58e-16;
      c_fringe[1] = 0.053e-15;
      c_junc[1] = 1e-15;
      I_on_n[1] = 683.6e-6;
      I_on_p[1] = I_on_n[1] / 2;
      nmos_effective_resistance_multiplier = 1.99;
      n_to_p_eff_curr_drv_ratio[1] = 2.23;
      gmp_to_gmn_multiplier[1] = 0.99;
      Rnchannelon[1] = nmos_effective_resistance_multiplier * vdd[1] / I_on_n[1];
      Rpchannelon[1] = n_to_p_eff_curr_drv_ratio[1] * Rnchannelon[1];
      long_channel_leakage_reduction[1] = 1/1.93;
      /*
      I_off_n[1][0] = 2.06e-11;
      I_off_n[1][10] = 3.30e-11;
      I_off_n[1][20] = 5.15e-11;
      I_off_n[1][30] = 7.83e-11;
      I_off_n[1][40] = 1.16e-10;
      I_off_n[1][50] = 1.69e-10;
      I_off_n[1][60] = 2.40e-10;
      I_off_n[1][70] = 3.34e-10;
      I_off_n[1][80] = 4.54e-10;
      I_off_n[1][90] = 5.96e-10;
      I_off_n[1][100] = 7.44e-10;

      I_g_on_n[1][0]  = 3.73e-11;//A/micron
      I_g_on_n[1][10] = 3.73e-11;
      I_g_on_n[1][20] = 3.73e-11;
      I_g_on_n[1][30] = 3.73e-11;
      I_g_on_n[1][40] = 3.73e-11;
      I_g_on_n[1][50] = 3.73e-11;
      I_g_on_n[1][60] = 3.73e-11;
      I_g_on_n[1][70] = 3.73e-11;
      I_g_on_n[1][80] = 3.73e-11;
      I_g_on_n[1][90] = 3.73e-11;
      I_g_on_n[1][100] = 3.73e-11;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      I_g_on_n[1][0] = 3.73e-11; 
      I_g_on_n[1][1] = 3.73e-11; I_g_on_n[1][2] = 3.73e-11; I_g_on_n[1][3] = 3.73e-11; I_g_on_n[1][4] = 3.73e-11; I_g_on_n[1][5] = 3.73e-11; 
      I_g_on_n[1][6] = 3.73e-11; I_g_on_n[1][7] = 3.73e-11; I_g_on_n[1][8] = 3.73e-11; I_g_on_n[1][9] = 3.73e-11; I_g_on_n[1][10] = 3.73e-11;
      I_g_on_n[1][11] = 3.73e-11; I_g_on_n[1][12] = 3.73e-11; I_g_on_n[1][13] = 3.73e-11; I_g_on_n[1][14] = 3.73e-11; I_g_on_n[1][15] = 3.73e-11; 
      I_g_on_n[1][16] = 3.73e-11; I_g_on_n[1][17] = 3.73e-11; I_g_on_n[1][18] = 3.73e-11; I_g_on_n[1][19] = 3.73e-11; I_g_on_n[1][20] = 3.73e-11;
      I_g_on_n[1][21] = 3.73e-11; I_g_on_n[1][22] = 3.73e-11; I_g_on_n[1][23] = 3.73e-11; I_g_on_n[1][24] = 3.73e-11; I_g_on_n[1][25] = 3.73e-11; 
      I_g_on_n[1][26] = 3.73e-11; I_g_on_n[1][27] = 3.73e-11; I_g_on_n[1][28] = 3.73e-11; I_g_on_n[1][29] = 3.73e-11; I_g_on_n[1][30] = 3.73e-11;
      I_g_on_n[1][31] = 3.73e-11; I_g_on_n[1][32] = 3.73e-11; I_g_on_n[1][33] = 3.73e-11; I_g_on_n[1][34] = 3.73e-11; I_g_on_n[1][35] = 3.73e-11; 
      I_g_on_n[1][36] = 3.73e-11; I_g_on_n[1][37] = 3.73e-11; I_g_on_n[1][38] = 3.73e-11; I_g_on_n[1][39] = 3.73e-11; I_g_on_n[1][40] = 3.73e-11;
      I_g_on_n[1][41] = 3.73e-11; I_g_on_n[1][42] = 3.73e-11; I_g_on_n[1][43] = 3.73e-11; I_g_on_n[1][44] = 3.73e-11; I_g_on_n[1][45] = 3.73e-11; 
      I_g_on_n[1][46] = 3.73e-11; I_g_on_n[1][47] = 3.73e-11; I_g_on_n[1][48] = 3.73e-11; I_g_on_n[1][49] = 3.73e-11; I_g_on_n[1][50] = 3.73e-11;
      I_g_on_n[1][51] = 3.73e-11; I_g_on_n[1][52] = 3.73e-11; I_g_on_n[1][53] = 3.73e-11; I_g_on_n[1][54] = 3.73e-11; I_g_on_n[1][55] = 3.73e-11; 
      I_g_on_n[1][56] = 3.73e-11; I_g_on_n[1][57] = 3.73e-11; I_g_on_n[1][58] = 3.73e-11; I_g_on_n[1][59] = 3.73e-11; I_g_on_n[1][60] = 3.73e-11;
      I_g_on_n[1][61] = 3.73e-11; I_g_on_n[1][62] = 3.73e-11; I_g_on_n[1][63] = 3.73e-11; I_g_on_n[1][64] = 3.73e-11; I_g_on_n[1][65] = 3.73e-11; 
      I_g_on_n[1][66] = 3.73e-11; I_g_on_n[1][67] = 3.73e-11; I_g_on_n[1][68] = 3.73e-11; I_g_on_n[1][69] = 3.73e-11; I_g_on_n[1][70] = 3.73e-11;
      I_g_on_n[1][71] = 3.73e-11; I_g_on_n[1][72] = 3.73e-11; I_g_on_n[1][73] = 3.73e-11; I_g_on_n[1][74] = 3.73e-11; I_g_on_n[1][75] = 3.73e-11; 
      I_g_on_n[1][76] = 3.73e-11; I_g_on_n[1][77] = 3.73e-11; I_g_on_n[1][78] = 3.73e-11; I_g_on_n[1][79] = 3.73e-11; I_g_on_n[1][80] = 3.73e-11;
      I_g_on_n[1][81] = 3.73e-11; I_g_on_n[1][82] = 3.73e-11; I_g_on_n[1][83] = 3.73e-11; I_g_on_n[1][84] = 3.73e-11; I_g_on_n[1][85] = 3.73e-11; 
      I_g_on_n[1][86] = 3.73e-11; I_g_on_n[1][87] = 3.73e-11; I_g_on_n[1][88] = 3.73e-11; I_g_on_n[1][89] = 3.73e-11; I_g_on_n[1][90] = 3.73e-11;
      I_g_on_n[1][91] = 3.73e-11; I_g_on_n[1][92] = 3.73e-11; I_g_on_n[1][93] = 3.73e-11; I_g_on_n[1][94] = 3.73e-11; I_g_on_n[1][95] = 3.73e-11; 
      I_g_on_n[1][96] = 3.73e-11; I_g_on_n[1][97] = 3.73e-11; I_g_on_n[1][98] = 3.73e-11; I_g_on_n[1][99] = 3.73e-11; I_g_on_n[1][100] = 3.73e-11;
      I_off_n[1][0] = 2.06e-11; 
      I_off_n[1][1] = 2.16e-11; I_off_n[1][2] = 2.26e-11; I_off_n[1][3] = 2.37e-11; I_off_n[1][4] = 2.49e-11; I_off_n[1][5] = 2.61e-11; 
      I_off_n[1][6] = 2.74e-11; I_off_n[1][7] = 2.87e-11; I_off_n[1][8] = 3.01e-11; I_off_n[1][9] = 3.15e-11; I_off_n[1][10] = 3.30e-11;
      I_off_n[1][11] = 3.45e-11; I_off_n[1][12] = 3.61e-11; I_off_n[1][13] = 3.78e-11; I_off_n[1][14] = 3.95e-11; I_off_n[1][15] = 4.14e-11; 
      I_off_n[1][16] = 4.33e-11; I_off_n[1][17] = 4.52e-11; I_off_n[1][18] = 4.72e-11; I_off_n[1][19] = 4.93e-11; I_off_n[1][20] = 5.15e-11;
      I_off_n[1][21] = 5.37e-11; I_off_n[1][22] = 5.61e-11; I_off_n[1][23] = 5.85e-11; I_off_n[1][24] = 6.11e-11; I_off_n[1][25] = 6.37e-11; 
      I_off_n[1][26] = 6.65e-11; I_off_n[1][27] = 6.93e-11; I_off_n[1][28] = 7.22e-11; I_off_n[1][29] = 7.52e-11; I_off_n[1][30] = 7.83e-11;
      I_off_n[1][31] = 8.15e-11; I_off_n[1][32] = 8.48e-11; I_off_n[1][33] = 8.83e-11; I_off_n[1][34] = 9.19e-11; I_off_n[1][35] = 9.56e-11; 
      I_off_n[1][36] = 9.94e-11; I_off_n[1][37] = 1.03e-10; I_off_n[1][38] = 1.07e-10; I_off_n[1][39] = 1.12e-10; I_off_n[1][40] = 1.16e-10;
      I_off_n[1][41] = 1.21e-10; I_off_n[1][42] = 1.25e-10; I_off_n[1][43] = 1.30e-10; I_off_n[1][44] = 1.35e-10; I_off_n[1][45] = 1.40e-10; 
      I_off_n[1][46] = 1.46e-10; I_off_n[1][47] = 1.51e-10; I_off_n[1][48] = 1.57e-10; I_off_n[1][49] = 1.63e-10; I_off_n[1][50] = 1.69e-10;
      I_off_n[1][51] = 1.75e-10; I_off_n[1][52] = 1.82e-10; I_off_n[1][53] = 1.88e-10; I_off_n[1][54] = 1.95e-10; I_off_n[1][55] = 2.02e-10; 
      I_off_n[1][56] = 2.09e-10; I_off_n[1][57] = 2.17e-10; I_off_n[1][58] = 2.24e-10; I_off_n[1][59] = 2.32e-10; I_off_n[1][60] = 2.40e-10;
      I_off_n[1][61] = 2.48e-10; I_off_n[1][62] = 2.57e-10; I_off_n[1][63] = 2.66e-10; I_off_n[1][64] = 2.75e-10; I_off_n[1][65] = 2.84e-10; 
      I_off_n[1][66] = 2.93e-10; I_off_n[1][67] = 3.03e-10; I_off_n[1][68] = 3.13e-10; I_off_n[1][69] = 3.24e-10; I_off_n[1][70] = 3.34e-10;
      I_off_n[1][71] = 3.45e-10; I_off_n[1][72] = 3.56e-10; I_off_n[1][73] = 3.67e-10; I_off_n[1][74] = 3.79e-10; I_off_n[1][75] = 3.91e-10; 
      I_off_n[1][76] = 4.03e-10; I_off_n[1][77] = 4.16e-10; I_off_n[1][78] = 4.28e-10; I_off_n[1][79] = 4.41e-10; I_off_n[1][80] = 4.54e-10;
      I_off_n[1][81] = 4.67e-10; I_off_n[1][82] = 4.81e-10; I_off_n[1][83] = 4.95e-10; I_off_n[1][84] = 5.09e-10; I_off_n[1][85] = 5.23e-10; 
      I_off_n[1][86] = 5.38e-10; I_off_n[1][87] = 5.52e-10; I_off_n[1][88] = 5.67e-10; I_off_n[1][89] = 5.81e-10; I_off_n[1][90] = 5.96e-10;
      I_off_n[1][91] = 6.11e-10; I_off_n[1][92] = 6.25e-10; I_off_n[1][93] = 6.40e-10; I_off_n[1][94] = 6.54e-10; I_off_n[1][95] = 6.69e-10; 
      I_off_n[1][96] = 6.84e-10; I_off_n[1][97] = 6.99e-10; I_off_n[1][98] = 7.14e-10; I_off_n[1][99] = 7.29e-10; I_off_n[1][100] = 7.44e-10;
      //LOP device type
      vdd[2] = 0.6;
      Lphy[2] = 0.016;
      Lelec[2] = 0.01232;
      t_ox[2] = 0.9e-3;
      v_th[2] = 0.24227;
      c_ox[2] = 2.84e-14;
      mobility_eff[2] =  513.52 * (1e-2 * 1e6 * 1e-2 * 1e6);
      Vdsat[2] = 4.64e-2;
      c_g_ideal[2] = 4.54e-16;
      c_fringe[2] = 0.057e-15;
      c_junc[2] = 1e-15;
      I_on_n[2] = 827.8e-6;
      I_on_p[2] = I_on_n[2] / 2;
      nmos_effective_resistance_multiplier = 1.73;
      n_to_p_eff_curr_drv_ratio[2] = 2.28;
      gmp_to_gmn_multiplier[2] = 1.11;
      Rnchannelon[2] = nmos_effective_resistance_multiplier * vdd[2] / I_on_n[2];
      Rpchannelon[2] = n_to_p_eff_curr_drv_ratio[2] * Rnchannelon[2];
      long_channel_leakage_reduction[2] = 1/1.89;
      /*
      I_off_n[2][0] = 5.94e-8;
      I_off_n[2][10] = 7.23e-8;
      I_off_n[2][20] = 8.7e-8;
      I_off_n[2][30] = 1.04e-7;
      I_off_n[2][40] = 1.22e-7;
      I_off_n[2][50] = 1.43e-7;
      I_off_n[2][60] = 1.65e-7;
      I_off_n[2][70] = 1.90e-7;
      I_off_n[2][80] = 2.15e-7;
      I_off_n[2][90] = 2.39e-7;
      I_off_n[2][100] = 2.63e-7;

      I_g_on_n[2][0]  = 2.93e-9;//A/micron
      I_g_on_n[2][10] = 2.93e-9;
      I_g_on_n[2][20] = 2.93e-9;
      I_g_on_n[2][30] = 2.93e-9;
      I_g_on_n[2][40] = 2.93e-9;
      I_g_on_n[2][50] = 2.93e-9;
      I_g_on_n[2][60] = 2.93e-9;
      I_g_on_n[2][70] = 2.93e-9;
      I_g_on_n[2][80] = 2.93e-9;
      I_g_on_n[2][90] = 2.93e-9;
      I_g_on_n[2][100] = 2.93e-9;
      */
      /* Leakage current data were interpolated to support full-range leakage feedback.
         Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[2][0] = 2.93e-9; 
        I_g_on_n[2][1] = 2.93e-9; I_g_on_n[2][2] = 2.93e-9; I_g_on_n[2][3] = 2.93e-9; I_g_on_n[2][4] = 2.93e-9; I_g_on_n[2][5] = 2.93e-9; 
        I_g_on_n[2][6] = 2.93e-9; I_g_on_n[2][7] = 2.93e-9; I_g_on_n[2][8] = 2.93e-9; I_g_on_n[2][9] = 2.93e-9; I_g_on_n[2][10] = 2.93e-9;
        I_g_on_n[2][11] = 2.93e-9; I_g_on_n[2][12] = 2.93e-9; I_g_on_n[2][13] = 2.93e-9; I_g_on_n[2][14] = 2.93e-9; I_g_on_n[2][15] = 2.93e-9; 
        I_g_on_n[2][16] = 2.93e-9; I_g_on_n[2][17] = 2.93e-9; I_g_on_n[2][18] = 2.93e-9; I_g_on_n[2][19] = 2.93e-9; I_g_on_n[2][20] = 2.93e-9;
        I_g_on_n[2][21] = 2.93e-9; I_g_on_n[2][22] = 2.93e-9; I_g_on_n[2][23] = 2.93e-9; I_g_on_n[2][24] = 2.93e-9; I_g_on_n[2][25] = 2.93e-9; 
        I_g_on_n[2][26] = 2.93e-9; I_g_on_n[2][27] = 2.93e-9; I_g_on_n[2][28] = 2.93e-9; I_g_on_n[2][29] = 2.93e-9; I_g_on_n[2][30] = 2.93e-9;
        I_g_on_n[2][31] = 2.93e-9; I_g_on_n[2][32] = 2.93e-9; I_g_on_n[2][33] = 2.93e-9; I_g_on_n[2][34] = 2.93e-9; I_g_on_n[2][35] = 2.93e-9; 
        I_g_on_n[2][36] = 2.93e-9; I_g_on_n[2][37] = 2.93e-9; I_g_on_n[2][38] = 2.93e-9; I_g_on_n[2][39] = 2.93e-9; I_g_on_n[2][40] = 2.93e-9;
        I_g_on_n[2][41] = 2.93e-9; I_g_on_n[2][42] = 2.93e-9; I_g_on_n[2][43] = 2.93e-9; I_g_on_n[2][44] = 2.93e-9; I_g_on_n[2][45] = 2.93e-9; 
        I_g_on_n[2][46] = 2.93e-9; I_g_on_n[2][47] = 2.93e-9; I_g_on_n[2][48] = 2.93e-9; I_g_on_n[2][49] = 2.93e-9; I_g_on_n[2][50] = 2.93e-9;
        I_g_on_n[2][51] = 2.93e-9; I_g_on_n[2][52] = 2.93e-9; I_g_on_n[2][53] = 2.93e-9; I_g_on_n[2][54] = 2.93e-9; I_g_on_n[2][55] = 2.93e-9; 
        I_g_on_n[2][56] = 2.93e-9; I_g_on_n[2][57] = 2.93e-9; I_g_on_n[2][58] = 2.93e-9; I_g_on_n[2][59] = 2.93e-9; I_g_on_n[2][60] = 2.93e-9;
        I_g_on_n[2][61] = 2.93e-9; I_g_on_n[2][62] = 2.93e-9; I_g_on_n[2][63] = 2.93e-9; I_g_on_n[2][64] = 2.93e-9; I_g_on_n[2][65] = 2.93e-9; 
        I_g_on_n[2][66] = 2.93e-9; I_g_on_n[2][67] = 2.93e-9; I_g_on_n[2][68] = 2.93e-9; I_g_on_n[2][69] = 2.93e-9; I_g_on_n[2][70] = 2.93e-9;
        I_g_on_n[2][71] = 2.93e-9; I_g_on_n[2][72] = 2.93e-9; I_g_on_n[2][73] = 2.93e-9; I_g_on_n[2][74] = 2.93e-9; I_g_on_n[2][75] = 2.93e-9; 
        I_g_on_n[2][76] = 2.93e-9; I_g_on_n[2][77] = 2.93e-9; I_g_on_n[2][78] = 2.93e-9; I_g_on_n[2][79] = 2.93e-9; I_g_on_n[2][80] = 2.93e-9;
        I_g_on_n[2][81] = 2.93e-9; I_g_on_n[2][82] = 2.93e-9; I_g_on_n[2][83] = 2.93e-9; I_g_on_n[2][84] = 2.93e-9; I_g_on_n[2][85] = 2.93e-9; 
        I_g_on_n[2][86] = 2.93e-9; I_g_on_n[2][87] = 2.93e-9; I_g_on_n[2][88] = 2.93e-9; I_g_on_n[2][89] = 2.93e-9; I_g_on_n[2][90] = 2.93e-9;
        I_g_on_n[2][91] = 2.93e-9; I_g_on_n[2][92] = 2.93e-9; I_g_on_n[2][93] = 2.93e-9; I_g_on_n[2][94] = 2.93e-9; I_g_on_n[2][95] = 2.93e-9; 
        I_g_on_n[2][96] = 2.93e-9; I_g_on_n[2][97] = 2.93e-9; I_g_on_n[2][98] = 2.93e-9; I_g_on_n[2][99] = 2.93e-9; I_g_on_n[2][100] = 2.93e-9;
        I_off_n[2][0] = 5.94e-8; 
        I_off_n[2][1] = 6.06e-8; I_off_n[2][2] = 6.18e-8; I_off_n[2][3] = 6.31e-8; I_off_n[2][4] = 6.43e-8; I_off_n[2][5] = 6.56e-8; 
        I_off_n[2][6] = 6.69e-8; I_off_n[2][7] = 6.82e-8; I_off_n[2][8] = 6.96e-8; I_off_n[2][9] = 7.09e-8; I_off_n[2][10] = 7.23e-8;
        I_off_n[2][11] = 7.37e-8; I_off_n[2][12] = 7.51e-8; I_off_n[2][13] = 7.65e-8; I_off_n[2][14] = 7.79e-8; I_off_n[2][15] = 7.94e-8; 
        I_off_n[2][16] = 8.09e-8; I_off_n[2][17] = 8.24e-8; I_off_n[2][18] = 8.39e-8; I_off_n[2][19] = 8.54e-8; I_off_n[2][20] = 8.70e-8;
        I_off_n[2][21] = 8.86e-8; I_off_n[2][22] = 9.02e-8; I_off_n[2][23] = 9.19e-8; I_off_n[2][24] = 9.36e-8; I_off_n[2][25] = 9.53e-8; 
        I_off_n[2][26] = 9.70e-8; I_off_n[2][27] = 9.88e-8; I_off_n[2][28] = 1.00e-7; I_off_n[2][29] = 1.02e-7; I_off_n[2][30] = 1.04e-7;
        I_off_n[2][31] = 1.06e-7; I_off_n[2][32] = 1.07e-7; I_off_n[2][33] = 1.09e-7; I_off_n[2][34] = 1.11e-7; I_off_n[2][35] = 1.13e-7; 
        I_off_n[2][36] = 1.15e-7; I_off_n[2][37] = 1.16e-7; I_off_n[2][38] = 1.18e-7; I_off_n[2][39] = 1.20e-7; I_off_n[2][40] = 1.22e-7;
        I_off_n[2][41] = 1.24e-7; I_off_n[2][42] = 1.26e-7; I_off_n[2][43] = 1.28e-7; I_off_n[2][44] = 1.30e-7; I_off_n[2][45] = 1.32e-7; 
        I_off_n[2][46] = 1.34e-7; I_off_n[2][47] = 1.37e-7; I_off_n[2][48] = 1.39e-7; I_off_n[2][49] = 1.41e-7; I_off_n[2][50] = 1.43e-7;
        I_off_n[2][51] = 1.45e-7; I_off_n[2][52] = 1.47e-7; I_off_n[2][53] = 1.49e-7; I_off_n[2][54] = 1.52e-7; I_off_n[2][55] = 1.54e-7; 
        I_off_n[2][56] = 1.56e-7; I_off_n[2][57] = 1.58e-7; I_off_n[2][58] = 1.60e-7; I_off_n[2][59] = 1.63e-7; I_off_n[2][60] = 1.65e-7;
        I_off_n[2][61] = 1.67e-7; I_off_n[2][62] = 1.70e-7; I_off_n[2][63] = 1.72e-7; I_off_n[2][64] = 1.75e-7; I_off_n[2][65] = 1.77e-7; 
        I_off_n[2][66] = 1.80e-7; I_off_n[2][67] = 1.82e-7; I_off_n[2][68] = 1.85e-7; I_off_n[2][69] = 1.87e-7; I_off_n[2][70] = 1.90e-7;
        I_off_n[2][71] = 1.93e-7; I_off_n[2][72] = 1.95e-7; I_off_n[2][73] = 1.98e-7; I_off_n[2][74] = 2.00e-7; I_off_n[2][75] = 2.03e-7; 
        I_off_n[2][76] = 2.05e-7; I_off_n[2][77] = 2.08e-7; I_off_n[2][78] = 2.10e-7; I_off_n[2][79] = 2.13e-7; I_off_n[2][80] = 2.15e-7;
        I_off_n[2][81] = 2.17e-7; I_off_n[2][82] = 2.20e-7; I_off_n[2][83] = 2.22e-7; I_off_n[2][84] = 2.25e-7; I_off_n[2][85] = 2.27e-7; 
        I_off_n[2][86] = 2.29e-7; I_off_n[2][87] = 2.32e-7; I_off_n[2][88] = 2.34e-7; I_off_n[2][89] = 2.37e-7; I_off_n[2][90] = 2.39e-7;
        I_off_n[2][91] = 2.41e-7; I_off_n[2][92] = 2.44e-7; I_off_n[2][93] = 2.46e-7; I_off_n[2][94] = 2.49e-7; I_off_n[2][95] = 2.51e-7; 
        I_off_n[2][96] = 2.53e-7; I_off_n[2][97] = 2.56e-7; I_off_n[2][98] = 2.58e-7; I_off_n[2][99] = 2.61e-7; I_off_n[2][100] = 2.63e-7;

      if (ram_cell_tech_type == lp_dram)
      {
        //LP-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.0;
        Lphy[3] = 0.056;
        Lelec[3] = 0.0419;//Assume Lelec is 30% lesser than Lphy for DRAM access and wordline transistors.
        curr_v_th_dram_access_transistor = 0.44129;
        width_dram_access_transistor = 0.056;
        curr_I_on_dram_cell = 36e-6;
        curr_I_off_dram_cell_worst_case_length_temp = 18.9e-12;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram = 0;
        curr_area_cell_dram = width_dram_access_transistor * Lphy[3] * 10.0;
        curr_asp_ratio_cell_dram = 1.46;
        curr_c_dram_cell = 20e-15;

        //LP-DRAM wordline transistor parameters
        curr_vpp = 1.5;
        t_ox[3] = 2e-3;
        v_th[3] = 0.44467;
        c_ox[3] = 1.48e-14;
        mobility_eff[3] =  408.12 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.174;
        c_g_ideal[3] = 7.45e-16;
        c_fringe[3] = 0.053e-15;
        c_junc[3] = 1e-15;
        I_on_n[3] = 1055.4e-6;
        I_on_p[3] = I_on_n[3] / 2;
        nmos_effective_resistance_multiplier = 1.65;
        n_to_p_eff_curr_drv_ratio[3] = 2.05;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0]  = 3.57e-11;
        I_off_n[3][10] = 5.51e-11;
        I_off_n[3][20] = 8.27e-11;
        I_off_n[3][30] = 1.21e-10;
        I_off_n[3][40] = 1.74e-10;
        I_off_n[3][50] = 2.45e-10;
        I_off_n[3][60] = 3.38e-10;
        I_off_n[3][70] = 4.53e-10;
        I_off_n[3][80] = 5.87e-10;
        I_off_n[3][90] = 7.29e-10;
        I_off_n[3][100] = 8.87e-10;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 3.57e-11; 
        I_off_n[3][1] = 3.73e-11; I_off_n[3][2] = 3.89e-11; I_off_n[3][3] = 4.07e-11; I_off_n[3][4] = 4.25e-11; I_off_n[3][5] = 4.45e-11; 
        I_off_n[3][6] = 4.65e-11; I_off_n[3][7] = 4.85e-11; I_off_n[3][8] = 5.07e-11; I_off_n[3][9] = 5.28e-11; I_off_n[3][10] = 5.51e-11;
        I_off_n[3][11] = 5.74e-11; I_off_n[3][12] = 5.99e-11; I_off_n[3][13] = 6.24e-11; I_off_n[3][14] = 6.50e-11; I_off_n[3][15] = 6.77e-11; 
        I_off_n[3][16] = 7.06e-11; I_off_n[3][17] = 7.35e-11; I_off_n[3][18] = 7.65e-11; I_off_n[3][19] = 7.95e-11; I_off_n[3][20] = 8.27e-11;
        I_off_n[3][21] = 8.60e-11; I_off_n[3][22] = 8.94e-11; I_off_n[3][23] = 9.29e-11; I_off_n[3][24] = 9.65e-11; I_off_n[3][25] = 1.00e-10; 
        I_off_n[3][26] = 1.04e-10; I_off_n[3][27] = 1.08e-10; I_off_n[3][28] = 1.12e-10; I_off_n[3][29] = 1.17e-10; I_off_n[3][30] = 1.21e-10;
        I_off_n[3][31] = 1.26e-10; I_off_n[3][32] = 1.30e-10; I_off_n[3][33] = 1.35e-10; I_off_n[3][34] = 1.40e-10; I_off_n[3][35] = 1.45e-10; 
        I_off_n[3][36] = 1.51e-10; I_off_n[3][37] = 1.56e-10; I_off_n[3][38] = 1.62e-10; I_off_n[3][39] = 1.68e-10; I_off_n[3][40] = 1.74e-10;
        I_off_n[3][41] = 1.80e-10; I_off_n[3][42] = 1.87e-10; I_off_n[3][43] = 1.93e-10; I_off_n[3][44] = 2.00e-10; I_off_n[3][45] = 2.07e-10; 
        I_off_n[3][46] = 2.14e-10; I_off_n[3][47] = 2.22e-10; I_off_n[3][48] = 2.29e-10; I_off_n[3][49] = 2.37e-10; I_off_n[3][50] = 2.45e-10;
        I_off_n[3][51] = 2.53e-10; I_off_n[3][52] = 2.62e-10; I_off_n[3][53] = 2.70e-10; I_off_n[3][54] = 2.79e-10; I_off_n[3][55] = 2.89e-10; 
        I_off_n[3][56] = 2.98e-10; I_off_n[3][57] = 3.08e-10; I_off_n[3][58] = 3.18e-10; I_off_n[3][59] = 3.28e-10; I_off_n[3][60] = 3.38e-10;
        I_off_n[3][61] = 3.48e-10; I_off_n[3][62] = 3.59e-10; I_off_n[3][63] = 3.70e-10; I_off_n[3][64] = 3.81e-10; I_off_n[3][65] = 3.93e-10; 
        I_off_n[3][66] = 4.05e-10; I_off_n[3][67] = 4.16e-10; I_off_n[3][68] = 4.28e-10; I_off_n[3][69] = 4.41e-10; I_off_n[3][70] = 4.53e-10;
        I_off_n[3][71] = 4.66e-10; I_off_n[3][72] = 4.78e-10; I_off_n[3][73] = 4.91e-10; I_off_n[3][74] = 5.05e-10; I_off_n[3][75] = 5.18e-10; 
        I_off_n[3][76] = 5.32e-10; I_off_n[3][77] = 5.46e-10; I_off_n[3][78] = 5.59e-10; I_off_n[3][79] = 5.73e-10; I_off_n[3][80] = 5.87e-10;
        I_off_n[3][81] = 6.01e-10; I_off_n[3][82] = 6.15e-10; I_off_n[3][83] = 6.29e-10; I_off_n[3][84] = 6.42e-10; I_off_n[3][85] = 6.57e-10; 
        I_off_n[3][86] = 6.71e-10; I_off_n[3][87] = 6.85e-10; I_off_n[3][88] = 6.99e-10; I_off_n[3][89] = 7.14e-10; I_off_n[3][90] = 7.29e-10;
        I_off_n[3][91] = 7.44e-10; I_off_n[3][92] = 7.59e-10; I_off_n[3][93] = 7.75e-10; I_off_n[3][94] = 7.90e-10; I_off_n[3][95] = 8.06e-10; 
        I_off_n[3][96] = 8.22e-10; I_off_n[3][97] = 8.38e-10; I_off_n[3][98] = 8.54e-10; I_off_n[3][99] = 8.70e-10; I_off_n[3][100] = 8.87e-10;
      }
      else if (ram_cell_tech_type == comm_dram)
      {
        //COMM-DRAM cell access transistor technology parameters
        curr_vdd_dram_cell = 1.0;
        Lphy[3] = 0.032;
        Lelec[3] = 0.0205;//Assume Lelec is 30% lesser than Lphy for DRAM access and wordline transistors.
        curr_v_th_dram_access_transistor = 1;
        width_dram_access_transistor = 0.032;
        curr_I_on_dram_cell = 20e-6;
        curr_I_off_dram_cell_worst_case_length_temp = 1e-15;
        curr_Wmemcella_dram = width_dram_access_transistor;
        curr_Wmemcellpmos_dram = 0;
        curr_Wmemcellnmos_dram = 0;
        curr_area_cell_dram = 6*0.032*0.032;
        curr_asp_ratio_cell_dram = 1.5;
        curr_c_dram_cell = 30e-15;

        //COMM-DRAM wordline transistor parameters
        curr_vpp = 2.6;
        t_ox[3] = 4e-3;
        v_th[3] = 1.0;
        c_ox[3] = 7.99e-15;
        mobility_eff[3] =  380.76 * (1e-2 * 1e6 * 1e-2 * 1e6);
        Vdsat[3] = 0.129;
        c_g_ideal[3] = 2.56e-16;
        c_fringe[3] = 0.053e-15;
        c_junc[3] = 1e-15;
        I_on_n[3] = 1024.5e-6;
        I_on_p[3] = I_on_n[3] / 2;
        nmos_effective_resistance_multiplier = 1.69;
        n_to_p_eff_curr_drv_ratio[3] = 1.95;
        gmp_to_gmn_multiplier[3] = 0.90;
        Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp / I_on_n[3];
        Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];
        long_channel_leakage_reduction[3] = 1;
        /*
        I_off_n[3][0]  = 3.63e-14;
        I_off_n[3][10] = 7.18e-14;
        I_off_n[3][20] = 1.36e-13;
        I_off_n[3][30] = 2.49e-13;
        I_off_n[3][40] = 4.41e-13;
        I_off_n[3][50] = 7.55e-13;
        I_off_n[3][60] = 1.26e-12;
        I_off_n[3][70] = 2.03e-12;
        I_off_n[3][80] = 3.19e-12;
        I_off_n[3][90] = 4.87e-12;
        I_off_n[3][100] = 7.16e-12;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_off_n[3][0] = 3.63e-14; 
        I_off_n[3][1] = 3.86e-14; I_off_n[3][2] = 4.12e-14; I_off_n[3][3] = 4.42e-14; I_off_n[3][4] = 4.75e-14; I_off_n[3][5] = 5.10e-14; 
        I_off_n[3][6] = 5.48e-14; I_off_n[3][7] = 5.87e-14; I_off_n[3][8] = 6.29e-14; I_off_n[3][9] = 6.73e-14; I_off_n[3][10] = 7.18e-14;
        I_off_n[3][11] = 7.66e-14; I_off_n[3][12] = 8.17e-14; I_off_n[3][13] = 8.72e-14; I_off_n[3][14] = 9.31e-14; I_off_n[3][15] = 9.94e-14; 
        I_off_n[3][16] = 1.06e-13; I_off_n[3][17] = 1.13e-13; I_off_n[3][18] = 1.20e-13; I_off_n[3][19] = 1.28e-13; I_off_n[3][20] = 1.36e-13;
        I_off_n[3][21] = 1.45e-13; I_off_n[3][22] = 1.54e-13; I_off_n[3][23] = 1.63e-13; I_off_n[3][24] = 1.74e-13; I_off_n[3][25] = 1.85e-13; 
        I_off_n[3][26] = 1.97e-13; I_off_n[3][27] = 2.09e-13; I_off_n[3][28] = 2.22e-13; I_off_n[3][29] = 2.35e-13; I_off_n[3][30] = 2.49e-13;
        I_off_n[3][31] = 2.64e-13; I_off_n[3][32] = 2.80e-13; I_off_n[3][33] = 2.96e-13; I_off_n[3][34] = 3.14e-13; I_off_n[3][35] = 3.33e-13; 
        I_off_n[3][36] = 3.53e-13; I_off_n[3][37] = 3.73e-13; I_off_n[3][38] = 3.95e-13; I_off_n[3][39] = 4.18e-13; I_off_n[3][40] = 4.41e-13;
        I_off_n[3][41] = 4.66e-13; I_off_n[3][42] = 4.92e-13; I_off_n[3][43] = 5.19e-13; I_off_n[3][44] = 5.49e-13; I_off_n[3][45] = 5.79e-13; 
        I_off_n[3][46] = 6.12e-13; I_off_n[3][47] = 6.45e-13; I_off_n[3][48] = 6.80e-13; I_off_n[3][49] = 7.17e-13; I_off_n[3][50] = 7.55e-13;
        I_off_n[3][51] = 7.95e-13; I_off_n[3][52] = 8.38e-13; I_off_n[3][53] = 8.83e-13; I_off_n[3][54] = 9.30e-13; I_off_n[3][55] = 9.80e-13; 
        I_off_n[3][56] = 1.03e-12; I_off_n[3][57] = 1.09e-12; I_off_n[3][58] = 1.14e-12; I_off_n[3][59] = 1.20e-12; I_off_n[3][60] = 1.26e-12;
        I_off_n[3][61] = 1.32e-12; I_off_n[3][62] = 1.39e-12; I_off_n[3][63] = 1.46e-12; I_off_n[3][64] = 1.53e-12; I_off_n[3][65] = 1.61e-12; 
        I_off_n[3][66] = 1.68e-12; I_off_n[3][67] = 1.77e-12; I_off_n[3][68] = 1.85e-12; I_off_n[3][69] = 1.94e-12; I_off_n[3][70] = 2.03e-12;
        I_off_n[3][71] = 2.13e-12; I_off_n[3][72] = 2.23e-12; I_off_n[3][73] = 2.33e-12; I_off_n[3][74] = 2.44e-12; I_off_n[3][75] = 2.55e-12; 
        I_off_n[3][76] = 2.67e-12; I_off_n[3][77] = 2.80e-12; I_off_n[3][78] = 2.92e-12; I_off_n[3][79] = 3.05e-12; I_off_n[3][80] = 3.19e-12;
        I_off_n[3][81] = 3.33e-12; I_off_n[3][82] = 3.48e-12; I_off_n[3][83] = 3.63e-12; I_off_n[3][84] = 3.79e-12; I_off_n[3][85] = 3.96e-12; 
        I_off_n[3][86] = 4.13e-12; I_off_n[3][87] = 4.31e-12; I_off_n[3][88] = 4.49e-12; I_off_n[3][89] = 4.68e-12; I_off_n[3][90] = 4.87e-12;
        I_off_n[3][91] = 5.07e-12; I_off_n[3][92] = 5.27e-12; I_off_n[3][93] = 5.49e-12; I_off_n[3][94] = 5.71e-12; I_off_n[3][95] = 5.93e-12; 
        I_off_n[3][96] = 6.17e-12; I_off_n[3][97] = 6.41e-12; I_off_n[3][98] = 6.65e-12; I_off_n[3][99] = 6.90e-12; I_off_n[3][100] = 7.16e-12;
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
      }

      //SRAM cell properties
      curr_Wmemcella_sram    = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
      curr_area_cell_sram    = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_sram = 1.46;
      //CAM cell properties //TODO: data need to be revisited
      curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
      curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
      curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
      curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;
      curr_asp_ratio_cell_cam = 2.92;
      //Empirical undifferetiated core/FU coefficient
      curr_logic_scaling_co_eff = 0.7*0.7*0.7;
      curr_core_tx_density      = 1.25/0.7;
      curr_sckt_co_eff           = 1.1111;
      curr_chip_layout_overhead  = 1.2;//die measurement results based on Niagara 1 and 2
      curr_macro_layout_overhead = 1.1;//EDA placement and routing tool rule of thumb
    }

    if(tech == 22){
        SENSE_AMP_D = .03e-9; // s
	SENSE_AMP_P = 2.16e-15; // J
    	//For 2016, MPU/ASIC stagger-contacted M1 half-pitch is 22 nm (so this is 22 nm
    	//technology i.e. FEATURESIZE = 0.022). Using the DG process numbers for HP.
    	//22 nm HP
    	vdd[0] = 0.8;
    	Lphy[0] = 0.009;//Lphy is the physical gate-length.
    	Lelec[0] = 0.00468;//Lelec is the electrical gate-length.
    	t_ox[0] = 0.55e-3;//micron
    	v_th[0] = 0.1395;//V
    	c_ox[0] = 3.63e-14;//F/micron2
    	mobility_eff[0] = 426.07 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
    	Vdsat[0] = 2.33e-2; //V/micron
    	c_g_ideal[0] = 3.27e-16;//F/micron
    	c_fringe[0] = 0.06e-15;//F/micron
    	c_junc[0] = 0;//F/micron2
    	I_on_n[0] =  2626.4e-6;//A/micron
    	I_on_p[0] = I_on_n[0] / 2;//A/micron //This value for I_on_p is not really used.
        nmos_effective_resistance_multiplier = 1.45;
        n_to_p_eff_curr_drv_ratio[0] = 2; //Wpmos/Wnmos = 2 in 2007 MASTAR. Look in
    	//"Dynamic" tab of Device workspace.
        gmp_to_gmn_multiplier[0] = 1.38; //Just using the 32nm SOI value.
        Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];//ohm-micron
        Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];//ohm-micron
        long_channel_leakage_reduction[0] = 1/3.274;
        /*
        I_off_n[0][0] = 1.52e-7/1.5*1.2;//From 22nm, leakage current are directly from ITRS report rather than MASTAR, since MASTAR has serious bugs there.
        I_off_n[0][10] = 1.55e-7/1.5*1.2;
        I_off_n[0][20] = 1.59e-7/1.5*1.2;
        I_off_n[0][30] = 1.68e-7/1.5*1.2;
        I_off_n[0][40] = 1.90e-7/1.5*1.2;
        I_off_n[0][50] = 2.69e-7/1.5*1.2;
        I_off_n[0][60] = 5.32e-7/1.5*1.2;
        I_off_n[0][70] = 1.02e-6/1.5*1.2;
        I_off_n[0][80] = 1.62e-6/1.5*1.2;
        I_off_n[0][90] = 2.73e-6/1.5*1.2;
        I_off_n[0][100] = 6.1e-6/1.5*1.2;
        //for 22nm DG HP
        I_g_on_n[0][0]  = 1.81e-9;//A/micron
        I_g_on_n[0][10] = 1.81e-9;
        I_g_on_n[0][20] = 1.81e-9;
        I_g_on_n[0][30] = 1.81e-9;
        I_g_on_n[0][40] = 1.81e-9;
        I_g_on_n[0][50] = 1.81e-9;
        I_g_on_n[0][60] = 1.81e-9;
        I_g_on_n[0][70] = 1.81e-9;
        I_g_on_n[0][80] = 1.81e-9;
        I_g_on_n[0][90] = 1.81e-9;
        I_g_on_n[0][100] = 1.81e-9;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[0][0] = 1.81e-9; 
        I_g_on_n[0][1] = 1.81e-9; I_g_on_n[0][2] = 1.81e-9; I_g_on_n[0][3] = 1.81e-9; I_g_on_n[0][4] = 1.81e-9; I_g_on_n[0][5] = 1.81e-9; 
        I_g_on_n[0][6] = 1.81e-9; I_g_on_n[0][7] = 1.81e-9; I_g_on_n[0][8] = 1.81e-9; I_g_on_n[0][9] = 1.81e-9; I_g_on_n[0][10] = 1.81e-9;
        I_g_on_n[0][11] = 1.81e-9; I_g_on_n[0][12] = 1.81e-9; I_g_on_n[0][13] = 1.81e-9; I_g_on_n[0][14] = 1.81e-9; I_g_on_n[0][15] = 1.81e-9; 
        I_g_on_n[0][16] = 1.81e-9; I_g_on_n[0][17] = 1.81e-9; I_g_on_n[0][18] = 1.81e-9; I_g_on_n[0][19] = 1.81e-9; I_g_on_n[0][20] = 1.81e-9;
        I_g_on_n[0][21] = 1.81e-9; I_g_on_n[0][22] = 1.81e-9; I_g_on_n[0][23] = 1.81e-9; I_g_on_n[0][24] = 1.81e-9; I_g_on_n[0][25] = 1.81e-9; 
        I_g_on_n[0][26] = 1.81e-9; I_g_on_n[0][27] = 1.81e-9; I_g_on_n[0][28] = 1.81e-9; I_g_on_n[0][29] = 1.81e-9; I_g_on_n[0][30] = 1.81e-9;
        I_g_on_n[0][31] = 1.81e-9; I_g_on_n[0][32] = 1.81e-9; I_g_on_n[0][33] = 1.81e-9; I_g_on_n[0][34] = 1.81e-9; I_g_on_n[0][35] = 1.81e-9; 
        I_g_on_n[0][36] = 1.81e-9; I_g_on_n[0][37] = 1.81e-9; I_g_on_n[0][38] = 1.81e-9; I_g_on_n[0][39] = 1.81e-9; I_g_on_n[0][40] = 1.81e-9;
        I_g_on_n[0][41] = 1.81e-9; I_g_on_n[0][42] = 1.81e-9; I_g_on_n[0][43] = 1.81e-9; I_g_on_n[0][44] = 1.81e-9; I_g_on_n[0][45] = 1.81e-9; 
        I_g_on_n[0][46] = 1.81e-9; I_g_on_n[0][47] = 1.81e-9; I_g_on_n[0][48] = 1.81e-9; I_g_on_n[0][49] = 1.81e-9; I_g_on_n[0][50] = 1.81e-9;
        I_g_on_n[0][51] = 1.81e-9; I_g_on_n[0][52] = 1.81e-9; I_g_on_n[0][53] = 1.81e-9; I_g_on_n[0][54] = 1.81e-9; I_g_on_n[0][55] = 1.81e-9; 
        I_g_on_n[0][56] = 1.81e-9; I_g_on_n[0][57] = 1.81e-9; I_g_on_n[0][58] = 1.81e-9; I_g_on_n[0][59] = 1.81e-9; I_g_on_n[0][60] = 1.81e-9;
        I_g_on_n[0][61] = 1.81e-9; I_g_on_n[0][62] = 1.81e-9; I_g_on_n[0][63] = 1.81e-9; I_g_on_n[0][64] = 1.81e-9; I_g_on_n[0][65] = 1.81e-9; 
        I_g_on_n[0][66] = 1.81e-9; I_g_on_n[0][67] = 1.81e-9; I_g_on_n[0][68] = 1.81e-9; I_g_on_n[0][69] = 1.81e-9; I_g_on_n[0][70] = 1.81e-9;
        I_g_on_n[0][71] = 1.81e-9; I_g_on_n[0][72] = 1.81e-9; I_g_on_n[0][73] = 1.81e-9; I_g_on_n[0][74] = 1.81e-9; I_g_on_n[0][75] = 1.81e-9; 
        I_g_on_n[0][76] = 1.81e-9; I_g_on_n[0][77] = 1.81e-9; I_g_on_n[0][78] = 1.81e-9; I_g_on_n[0][79] = 1.81e-9; I_g_on_n[0][80] = 1.81e-9;
        I_g_on_n[0][81] = 1.81e-9; I_g_on_n[0][82] = 1.81e-9; I_g_on_n[0][83] = 1.81e-9; I_g_on_n[0][84] = 1.81e-9; I_g_on_n[0][85] = 1.81e-9; 
        I_g_on_n[0][86] = 1.81e-9; I_g_on_n[0][87] = 1.81e-9; I_g_on_n[0][88] = 1.81e-9; I_g_on_n[0][89] = 1.81e-9; I_g_on_n[0][90] = 1.81e-9;
        I_g_on_n[0][91] = 1.81e-9; I_g_on_n[0][92] = 1.81e-9; I_g_on_n[0][93] = 1.81e-9; I_g_on_n[0][94] = 1.81e-9; I_g_on_n[0][95] = 1.81e-9; 
        I_g_on_n[0][96] = 1.81e-9; I_g_on_n[0][97] = 1.81e-9; I_g_on_n[0][98] = 1.81e-9; I_g_on_n[0][99] = 1.81e-9; I_g_on_n[0][100] = 1.81e-9;
        I_off_n[0][0] = 1.22e-7; 
        I_off_n[0][1] = 1.22e-7; I_off_n[0][2] = 1.22e-7; I_off_n[0][3] = 1.22e-7; I_off_n[0][4] = 1.22e-7; I_off_n[0][5] = 1.23e-7; 
        I_off_n[0][6] = 1.23e-7; I_off_n[0][7] = 1.23e-7; I_off_n[0][8] = 1.23e-7; I_off_n[0][9] = 1.24e-7; I_off_n[0][10] = 1.24e-7;
        I_off_n[0][11] = 1.24e-7; I_off_n[0][12] = 1.25e-7; I_off_n[0][13] = 1.25e-7; I_off_n[0][14] = 1.25e-7; I_off_n[0][15] = 1.25e-7; 
        I_off_n[0][16] = 1.26e-7; I_off_n[0][17] = 1.26e-7; I_off_n[0][18] = 1.26e-7; I_off_n[0][19] = 1.27e-7; I_off_n[0][20] = 1.27e-7;
        I_off_n[0][21] = 1.28e-7; I_off_n[0][22] = 1.28e-7; I_off_n[0][23] = 1.29e-7; I_off_n[0][24] = 1.29e-7; I_off_n[0][25] = 1.30e-7; 
        I_off_n[0][26] = 1.31e-7; I_off_n[0][27] = 1.32e-7; I_off_n[0][28] = 1.32e-7; I_off_n[0][29] = 1.33e-7; I_off_n[0][30] = 1.34e-7;
        I_off_n[0][31] = 1.35e-7; I_off_n[0][32] = 1.37e-7; I_off_n[0][33] = 1.38e-7; I_off_n[0][34] = 1.39e-7; I_off_n[0][35] = 1.41e-7; 
        I_off_n[0][36] = 1.43e-7; I_off_n[0][37] = 1.45e-7; I_off_n[0][38] = 1.47e-7; I_off_n[0][39] = 1.49e-7; I_off_n[0][40] = 1.52e-7;
        I_off_n[0][41] = 1.55e-7; I_off_n[0][42] = 1.59e-7; I_off_n[0][43] = 1.64e-7; I_off_n[0][44] = 1.69e-7; I_off_n[0][45] = 1.75e-7; 
        I_off_n[0][46] = 1.82e-7; I_off_n[0][47] = 1.89e-7; I_off_n[0][48] = 1.97e-7; I_off_n[0][49] = 2.06e-7; I_off_n[0][50] = 2.15e-7;
        I_off_n[0][51] = 2.27e-7; I_off_n[0][52] = 2.41e-7; I_off_n[0][53] = 2.58e-7; I_off_n[0][54] = 2.77e-7; I_off_n[0][55] = 2.98e-7; 
        I_off_n[0][56] = 3.21e-7; I_off_n[0][57] = 3.46e-7; I_off_n[0][58] = 3.72e-7; I_off_n[0][59] = 3.98e-7; I_off_n[0][60] = 4.26e-7;
        I_off_n[0][61] = 4.55e-7; I_off_n[0][62] = 4.87e-7; I_off_n[0][63] = 5.23e-7; I_off_n[0][64] = 5.61e-7; I_off_n[0][65] = 6.01e-7; 
        I_off_n[0][66] = 6.43e-7; I_off_n[0][67] = 6.86e-7; I_off_n[0][68] = 7.29e-7; I_off_n[0][69] = 7.73e-7; I_off_n[0][70] = 8.16e-7;
        I_off_n[0][71] = 8.59e-7; I_off_n[0][72] = 9.01e-7; I_off_n[0][73] = 9.44e-7; I_off_n[0][74] = 9.87e-7; I_off_n[0][75] = 1.03e-6; 
        I_off_n[0][76] = 1.08e-6; I_off_n[0][77] = 1.13e-6; I_off_n[0][78] = 1.18e-6; I_off_n[0][79] = 1.24e-6; I_off_n[0][80] = 1.30e-6;
        I_off_n[0][81] = 1.36e-6; I_off_n[0][82] = 1.43e-6; I_off_n[0][83] = 1.50e-6; I_off_n[0][84] = 1.57e-6; I_off_n[0][85] = 1.65e-6; 
        I_off_n[0][86] = 1.74e-6; I_off_n[0][87] = 1.84e-6; I_off_n[0][88] = 1.94e-6; I_off_n[0][89] = 2.06e-6; I_off_n[0][90] = 2.18e-6;
        I_off_n[0][91] = 2.34e-6; I_off_n[0][92] = 2.52e-6; I_off_n[0][93] = 2.74e-6; I_off_n[0][94] = 2.98e-6; I_off_n[0][95] = 3.25e-6; 
        I_off_n[0][96] = 3.54e-6; I_off_n[0][97] = 3.85e-6; I_off_n[0][98] = 4.18e-6; I_off_n[0][99] = 4.52e-6; I_off_n[0][100] = 4.88e-6;

    	//22 nm LSTP DG
    	vdd[1] = 0.8;
    	Lphy[1] = 0.014;
    	Lelec[1] = 0.008;//Lelec is the electrical gate-length.
    	t_ox[1] = 1.1e-3;//micron
    	v_th[1] = 0.40126;//V
    	c_ox[1] = 2.30e-14;//F/micron2
    	mobility_eff[1] =  738.09 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
    	Vdsat[1] = 6.64e-2; //V/micron
    	c_g_ideal[1] = 3.22e-16;//F/micron
    	c_fringe[1] = 0.08e-15;
    	c_junc[1] = 0;//F/micron2
    	I_on_n[1] = 727.6e-6;//A/micron
    	I_on_p[1] = I_on_n[1] / 2;
    	nmos_effective_resistance_multiplier = 1.99;
    	n_to_p_eff_curr_drv_ratio[1] = 2;
    	gmp_to_gmn_multiplier[1] = 0.99;
    	Rnchannelon[1] = nmos_effective_resistance_multiplier * vdd[1] / I_on_n[1];//ohm-micron
    	Rpchannelon[1] = n_to_p_eff_curr_drv_ratio[1] * Rnchannelon[1];//ohm-micron
    	long_channel_leakage_reduction[1] = 1/1.89;
        /*
    	I_off_n[1][0] = 2.43e-11;
    	I_off_n[1][10] = 4.85e-11;
    	I_off_n[1][20] = 9.68e-11;
    	I_off_n[1][30] = 1.94e-10;
    	I_off_n[1][40] = 3.87e-10;
    	I_off_n[1][50] = 7.73e-10;
    	I_off_n[1][60] = 3.55e-10;
    	I_off_n[1][70] = 3.09e-9;
    	I_off_n[1][80] = 6.19e-9;
    	I_off_n[1][90] = 1.24e-8;
    	I_off_n[1][100]= 2.48e-8;

    	I_g_on_n[1][0]  = 4.51e-10;//A/micron
    	I_g_on_n[1][10] = 4.51e-10;
    	I_g_on_n[1][20] = 4.51e-10;
    	I_g_on_n[1][30] = 4.51e-10;
    	I_g_on_n[1][40] = 4.51e-10;
    	I_g_on_n[1][50] = 4.51e-10;
    	I_g_on_n[1][60] = 4.51e-10;
    	I_g_on_n[1][70] = 4.51e-10;
    	I_g_on_n[1][80] = 4.51e-10;
    	I_g_on_n[1][90] = 4.51e-10;
    	I_g_on_n[1][100] = 4.51e-10;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[1][0] = 4.51e-10; 
        I_g_on_n[1][1] = 4.51e-10; I_g_on_n[1][2] = 4.51e-10; I_g_on_n[1][3] = 4.51e-10; I_g_on_n[1][4] = 4.51e-10; I_g_on_n[1][5] = 4.51e-10; 
        I_g_on_n[1][6] = 4.51e-10; I_g_on_n[1][7] = 4.51e-10; I_g_on_n[1][8] = 4.51e-10; I_g_on_n[1][9] = 4.51e-10; I_g_on_n[1][10] = 4.51e-10;
        I_g_on_n[1][11] = 4.51e-10; I_g_on_n[1][12] = 4.51e-10; I_g_on_n[1][13] = 4.51e-10; I_g_on_n[1][14] = 4.51e-10; I_g_on_n[1][15] = 4.51e-10; 
        I_g_on_n[1][16] = 4.51e-10; I_g_on_n[1][17] = 4.51e-10; I_g_on_n[1][18] = 4.51e-10; I_g_on_n[1][19] = 4.51e-10; I_g_on_n[1][20] = 4.51e-10;
        I_g_on_n[1][21] = 4.51e-10; I_g_on_n[1][22] = 4.51e-10; I_g_on_n[1][23] = 4.51e-10; I_g_on_n[1][24] = 4.51e-10; I_g_on_n[1][25] = 4.51e-10; 
        I_g_on_n[1][26] = 4.51e-10; I_g_on_n[1][27] = 4.51e-10; I_g_on_n[1][28] = 4.51e-10; I_g_on_n[1][29] = 4.51e-10; I_g_on_n[1][30] = 4.51e-10;
        I_g_on_n[1][31] = 4.51e-10; I_g_on_n[1][32] = 4.51e-10; I_g_on_n[1][33] = 4.51e-10; I_g_on_n[1][34] = 4.51e-10; I_g_on_n[1][35] = 4.51e-10; 
        I_g_on_n[1][36] = 4.51e-10; I_g_on_n[1][37] = 4.51e-10; I_g_on_n[1][38] = 4.51e-10; I_g_on_n[1][39] = 4.51e-10; I_g_on_n[1][40] = 4.51e-10;
        I_g_on_n[1][41] = 4.51e-10; I_g_on_n[1][42] = 4.51e-10; I_g_on_n[1][43] = 4.51e-10; I_g_on_n[1][44] = 4.51e-10; I_g_on_n[1][45] = 4.51e-10; 
        I_g_on_n[1][46] = 4.51e-10; I_g_on_n[1][47] = 4.51e-10; I_g_on_n[1][48] = 4.51e-10; I_g_on_n[1][49] = 4.51e-10; I_g_on_n[1][50] = 4.51e-10;
        I_g_on_n[1][51] = 4.51e-10; I_g_on_n[1][52] = 4.51e-10; I_g_on_n[1][53] = 4.51e-10; I_g_on_n[1][54] = 4.51e-10; I_g_on_n[1][55] = 4.51e-10; 
        I_g_on_n[1][56] = 4.51e-10; I_g_on_n[1][57] = 4.51e-10; I_g_on_n[1][58] = 4.51e-10; I_g_on_n[1][59] = 4.51e-10; I_g_on_n[1][60] = 4.51e-10;
        I_g_on_n[1][61] = 4.51e-10; I_g_on_n[1][62] = 4.51e-10; I_g_on_n[1][63] = 4.51e-10; I_g_on_n[1][64] = 4.51e-10; I_g_on_n[1][65] = 4.51e-10; 
        I_g_on_n[1][66] = 4.51e-10; I_g_on_n[1][67] = 4.51e-10; I_g_on_n[1][68] = 4.51e-10; I_g_on_n[1][69] = 4.51e-10; I_g_on_n[1][70] = 4.51e-10;
        I_g_on_n[1][71] = 4.51e-10; I_g_on_n[1][72] = 4.51e-10; I_g_on_n[1][73] = 4.51e-10; I_g_on_n[1][74] = 4.51e-10; I_g_on_n[1][75] = 4.51e-10; 
        I_g_on_n[1][76] = 4.51e-10; I_g_on_n[1][77] = 4.51e-10; I_g_on_n[1][78] = 4.51e-10; I_g_on_n[1][79] = 4.51e-10; I_g_on_n[1][80] = 4.51e-10;
        I_g_on_n[1][81] = 4.51e-10; I_g_on_n[1][82] = 4.51e-10; I_g_on_n[1][83] = 4.51e-10; I_g_on_n[1][84] = 4.51e-10; I_g_on_n[1][85] = 4.51e-10; 
        I_g_on_n[1][86] = 4.51e-10; I_g_on_n[1][87] = 4.51e-10; I_g_on_n[1][88] = 4.51e-10; I_g_on_n[1][89] = 4.51e-10; I_g_on_n[1][90] = 4.51e-10;
        I_g_on_n[1][91] = 4.51e-10; I_g_on_n[1][92] = 4.51e-10; I_g_on_n[1][93] = 4.51e-10; I_g_on_n[1][94] = 4.51e-10; I_g_on_n[1][95] = 4.51e-10; 
        I_g_on_n[1][96] = 4.51e-10; I_g_on_n[1][97] = 4.51e-10; I_g_on_n[1][98] = 4.51e-10; I_g_on_n[1][99] = 4.51e-10; I_g_on_n[1][100] = 4.51e-10;
        I_off_n[1][0] = 2.43e-11; 
        I_off_n[1][1] = 2.57e-11; I_off_n[1][2] = 2.73e-11; I_off_n[1][3] = 2.93e-11; I_off_n[1][4] = 3.15e-11; I_off_n[1][5] = 3.39e-11; 
        I_off_n[1][6] = 3.65e-11; I_off_n[1][7] = 3.93e-11; I_off_n[1][8] = 4.22e-11; I_off_n[1][9] = 4.53e-11; I_off_n[1][10] = 4.85e-11;
        I_off_n[1][11] = 5.19e-11; I_off_n[1][12] = 5.56e-11; I_off_n[1][13] = 5.96e-11; I_off_n[1][14] = 6.39e-11; I_off_n[1][15] = 6.86e-11; 
        I_off_n[1][16] = 7.36e-11; I_off_n[1][17] = 7.89e-11; I_off_n[1][18] = 8.45e-11; I_off_n[1][19] = 9.05e-11; I_off_n[1][20] = 9.68e-11;
        I_off_n[1][21] = 1.04e-10; I_off_n[1][22] = 1.11e-10; I_off_n[1][23] = 1.19e-10; I_off_n[1][24] = 1.28e-10; I_off_n[1][25] = 1.37e-10; 
        I_off_n[1][26] = 1.47e-10; I_off_n[1][27] = 1.58e-10; I_off_n[1][28] = 1.69e-10; I_off_n[1][29] = 1.81e-10; I_off_n[1][30] = 1.94e-10;
        I_off_n[1][31] = 2.08e-10; I_off_n[1][32] = 2.22e-10; I_off_n[1][33] = 2.38e-10; I_off_n[1][34] = 2.56e-10; I_off_n[1][35] = 2.74e-10; 
        I_off_n[1][36] = 2.94e-10; I_off_n[1][37] = 3.16e-10; I_off_n[1][38] = 3.38e-10; I_off_n[1][39] = 3.62e-10; I_off_n[1][40] = 3.87e-10;
        I_off_n[1][41] = 4.14e-10; I_off_n[1][42] = 4.44e-10; I_off_n[1][43] = 4.76e-10; I_off_n[1][44] = 5.10e-10; I_off_n[1][45] = 5.48e-10; 
        I_off_n[1][46] = 5.88e-10; I_off_n[1][47] = 6.30e-10; I_off_n[1][48] = 6.75e-10; I_off_n[1][49] = 7.23e-10; I_off_n[1][50] = 7.73e-10;
        I_off_n[1][51] = 8.27e-10; I_off_n[1][52] = 8.87e-10; I_off_n[1][53] = 9.52e-10; I_off_n[1][54] = 1.02e-9; I_off_n[1][55] = 1.10e-9; 
        I_off_n[1][56] = 1.18e-9; I_off_n[1][57] = 1.26e-9; I_off_n[1][58] = 1.35e-9; I_off_n[1][59] = 1.45e-9; I_off_n[1][60] = 1.55e-9;
        I_off_n[1][61] = 1.66e-9; I_off_n[1][62] = 1.78e-9; I_off_n[1][63] = 1.90e-9; I_off_n[1][64] = 2.04e-9; I_off_n[1][65] = 2.19e-9; 
        I_off_n[1][66] = 2.35e-9; I_off_n[1][67] = 2.52e-9; I_off_n[1][68] = 2.70e-9; I_off_n[1][69] = 2.89e-9; I_off_n[1][70] = 3.09e-9;
        I_off_n[1][71] = 3.31e-9; I_off_n[1][72] = 3.54e-9; I_off_n[1][73] = 3.80e-9; I_off_n[1][74] = 4.08e-9; I_off_n[1][75] = 4.38e-9; 
        I_off_n[1][76] = 4.70e-9; I_off_n[1][77] = 5.04e-9; I_off_n[1][78] = 5.40e-9; I_off_n[1][79] = 5.79e-9; I_off_n[1][80] = 6.19e-9;
        I_off_n[1][81] = 6.62e-9; I_off_n[1][82] = 7.10e-9; I_off_n[1][83] = 7.62e-9; I_off_n[1][84] = 8.18e-9; I_off_n[1][85] = 8.78e-9; 
        I_off_n[1][86] = 9.42e-9; I_off_n[1][87] = 1.01e-8; I_off_n[1][88] = 1.08e-8; I_off_n[1][89] = 1.16e-8; I_off_n[1][90] = 1.24e-8;
        I_off_n[1][91] = 1.33e-8; I_off_n[1][92] = 1.43e-8; I_off_n[1][93] = 1.53e-8; I_off_n[1][94] = 1.65e-8; I_off_n[1][95] = 1.77e-8; 
        I_off_n[1][96] = 1.90e-8; I_off_n[1][97] = 2.04e-8; I_off_n[1][98] = 2.18e-8; I_off_n[1][99] = 2.33e-8; I_off_n[1][100] = 2.48e-8;
    	//22 nm LOP
    	vdd[2] = 0.6;
    	Lphy[2] = 0.011;
    	Lelec[2] = 0.00604;//Lelec is the electrical gate-length.
    	t_ox[2] = 0.8e-3;//micron
    	v_th[2] = 0.2315;//V
    	c_ox[2] = 2.87e-14;//F/micron2
    	mobility_eff[2] =  698.37 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
    	Vdsat[2] = 1.81e-2; //V/micron
    	c_g_ideal[2] = 3.16e-16;//F/micron
    	c_fringe[2] = 0.08e-15;
    	c_junc[2] = 0;//F/micron2 This is Cj0 not Cjunc in MASTAR results->Dynamic Tab
    	I_on_n[2] = 916.1e-6;//A/micron
    	I_on_p[2] = I_on_n[2] / 2;
    	nmos_effective_resistance_multiplier = 1.73;
    	n_to_p_eff_curr_drv_ratio[2] = 2;
    	gmp_to_gmn_multiplier[2] = 1.11;
    	Rnchannelon[2] = nmos_effective_resistance_multiplier * vdd[2] / I_on_n[2];//ohm-micron
    	Rpchannelon[2] = n_to_p_eff_curr_drv_ratio[2] * Rnchannelon[2];//ohm-micron
    	long_channel_leakage_reduction[2] = 1/2.38;
        /*
    	I_off_n[2][0] = 1.31e-8;
    	I_off_n[2][10] = 2.60e-8;
    	I_off_n[2][20] = 5.14e-8;
    	I_off_n[2][30] = 1.02e-7;
    	I_off_n[2][40] = 2.02e-7;
    	I_off_n[2][50] = 3.99e-7;
    	I_off_n[2][60] = 7.91e-7;
    	I_off_n[2][70] = 1.09e-6;
    	I_off_n[2][80] = 2.09e-6;
    	I_off_n[2][90] = 4.04e-6;
    	I_off_n[2][100]= 4.48e-6;

    	I_g_on_n[2][0]  = 2.74e-9;//A/micron
    	I_g_on_n[2][10] = 2.74e-9;
    	I_g_on_n[2][20] = 2.74e-9;
    	I_g_on_n[2][30] = 2.74e-9;
    	I_g_on_n[2][40] = 2.74e-9;
    	I_g_on_n[2][50] = 2.74e-9;
    	I_g_on_n[2][60] = 2.74e-9;
    	I_g_on_n[2][70] = 2.74e-9;
    	I_g_on_n[2][80] = 2.74e-9;
    	I_g_on_n[2][90] = 2.74e-9;
    	I_g_on_n[2][100] = 2.74e-9;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[2][0] = 2.74e-9; 
        I_g_on_n[2][1] = 2.74e-9; I_g_on_n[2][2] = 2.74e-9; I_g_on_n[2][3] = 2.74e-9; I_g_on_n[2][4] = 2.74e-9; I_g_on_n[2][5] = 2.74e-9; 
        I_g_on_n[2][6] = 2.74e-9; I_g_on_n[2][7] = 2.74e-9; I_g_on_n[2][8] = 2.74e-9; I_g_on_n[2][9] = 2.74e-9; I_g_on_n[2][10] = 2.74e-9;
        I_g_on_n[2][11] = 2.74e-9; I_g_on_n[2][12] = 2.74e-9; I_g_on_n[2][13] = 2.74e-9; I_g_on_n[2][14] = 2.74e-9; I_g_on_n[2][15] = 2.74e-9; 
        I_g_on_n[2][16] = 2.74e-9; I_g_on_n[2][17] = 2.74e-9; I_g_on_n[2][18] = 2.74e-9; I_g_on_n[2][19] = 2.74e-9; I_g_on_n[2][20] = 2.74e-9;
        I_g_on_n[2][21] = 2.74e-9; I_g_on_n[2][22] = 2.74e-9; I_g_on_n[2][23] = 2.74e-9; I_g_on_n[2][24] = 2.74e-9; I_g_on_n[2][25] = 2.74e-9; 
        I_g_on_n[2][26] = 2.74e-9; I_g_on_n[2][27] = 2.74e-9; I_g_on_n[2][28] = 2.74e-9; I_g_on_n[2][29] = 2.74e-9; I_g_on_n[2][30] = 2.74e-9;
        I_g_on_n[2][31] = 2.74e-9; I_g_on_n[2][32] = 2.74e-9; I_g_on_n[2][33] = 2.74e-9; I_g_on_n[2][34] = 2.74e-9; I_g_on_n[2][35] = 2.74e-9; 
        I_g_on_n[2][36] = 2.74e-9; I_g_on_n[2][37] = 2.74e-9; I_g_on_n[2][38] = 2.74e-9; I_g_on_n[2][39] = 2.74e-9; I_g_on_n[2][40] = 2.74e-9;
        I_g_on_n[2][41] = 2.74e-9; I_g_on_n[2][42] = 2.74e-9; I_g_on_n[2][43] = 2.74e-9; I_g_on_n[2][44] = 2.74e-9; I_g_on_n[2][45] = 2.74e-9; 
        I_g_on_n[2][46] = 2.74e-9; I_g_on_n[2][47] = 2.74e-9; I_g_on_n[2][48] = 2.74e-9; I_g_on_n[2][49] = 2.74e-9; I_g_on_n[2][50] = 2.74e-9;
        I_g_on_n[2][51] = 2.74e-9; I_g_on_n[2][52] = 2.74e-9; I_g_on_n[2][53] = 2.74e-9; I_g_on_n[2][54] = 2.74e-9; I_g_on_n[2][55] = 2.74e-9; 
        I_g_on_n[2][56] = 2.74e-9; I_g_on_n[2][57] = 2.74e-9; I_g_on_n[2][58] = 2.74e-9; I_g_on_n[2][59] = 2.74e-9; I_g_on_n[2][60] = 2.74e-9;
        I_g_on_n[2][61] = 2.74e-9; I_g_on_n[2][62] = 2.74e-9; I_g_on_n[2][63] = 2.74e-9; I_g_on_n[2][64] = 2.74e-9; I_g_on_n[2][65] = 2.74e-9; 
        I_g_on_n[2][66] = 2.74e-9; I_g_on_n[2][67] = 2.74e-9; I_g_on_n[2][68] = 2.74e-9; I_g_on_n[2][69] = 2.74e-9; I_g_on_n[2][70] = 2.74e-9;
        I_g_on_n[2][71] = 2.74e-9; I_g_on_n[2][72] = 2.74e-9; I_g_on_n[2][73] = 2.74e-9; I_g_on_n[2][74] = 2.74e-9; I_g_on_n[2][75] = 2.74e-9; 
        I_g_on_n[2][76] = 2.74e-9; I_g_on_n[2][77] = 2.74e-9; I_g_on_n[2][78] = 2.74e-9; I_g_on_n[2][79] = 2.74e-9; I_g_on_n[2][80] = 2.74e-9;
        I_g_on_n[2][81] = 2.74e-9; I_g_on_n[2][82] = 2.74e-9; I_g_on_n[2][83] = 2.74e-9; I_g_on_n[2][84] = 2.74e-9; I_g_on_n[2][85] = 2.74e-9; 
        I_g_on_n[2][86] = 2.74e-9; I_g_on_n[2][87] = 2.74e-9; I_g_on_n[2][88] = 2.74e-9; I_g_on_n[2][89] = 2.74e-9; I_g_on_n[2][90] = 2.74e-9;
        I_g_on_n[2][91] = 2.74e-9; I_g_on_n[2][92] = 2.74e-9; I_g_on_n[2][93] = 2.74e-9; I_g_on_n[2][94] = 2.74e-9; I_g_on_n[2][95] = 2.74e-9; 
        I_g_on_n[2][96] = 2.74e-9; I_g_on_n[2][97] = 2.74e-9; I_g_on_n[2][98] = 2.74e-9; I_g_on_n[2][99] = 2.74e-9; I_g_on_n[2][100] = 2.74e-9;
        I_off_n[2][0] = 1.31e-8; 
        I_off_n[2][1] = 1.38e-8; I_off_n[2][2] = 1.47e-8; I_off_n[2][3] = 1.58e-8; I_off_n[2][4] = 1.70e-8; I_off_n[2][5] = 1.82e-8; 
        I_off_n[2][6] = 1.96e-8; I_off_n[2][7] = 2.11e-8; I_off_n[2][8] = 2.27e-8; I_off_n[2][9] = 2.43e-8; I_off_n[2][10] = 2.60e-8;
        I_off_n[2][11] = 2.27e-8; I_off_n[2][12] = 2.97e-8; I_off_n[2][13] = 3.19e-8; I_off_n[2][14] = 3.42e-8; I_off_n[2][15] = 3.66e-8; 
        I_off_n[2][16] = 3.92e-8; I_off_n[2][17] = 4.20e-8; I_off_n[2][18] = 4.50e-8; I_off_n[2][19] = 4.81e-8; I_off_n[2][20] = 5.14e-8;
        I_off_n[2][21] = 5.50e-8; I_off_n[2][22] = 5.88e-8; I_off_n[2][23] = 6.31e-8; I_off_n[2][24] = 6.76e-8; I_off_n[2][25] = 7.25e-8; 
        I_off_n[2][26] = 7.78e-8; I_off_n[2][27] = 8.33e-8; I_off_n[2][28] = 8.92e-8; I_off_n[2][29] = 9.54e-8; I_off_n[2][30] = 1.02e-7;
        I_off_n[2][31] = 1.09e-7; I_off_n[2][32] = 1.17e-7; I_off_n[2][33] = 1.25e-7; I_off_n[2][34] = 1.34e-7; I_off_n[2][35] = 1.44e-7; 
        I_off_n[2][36] = 1.54e-7; I_off_n[2][37] = 1.65e-7; I_off_n[2][38] = 1.77e-7; I_off_n[2][39] = 1.89e-7; I_off_n[2][40] = 2.02e-7;
        I_off_n[2][41] = 2.16e-7; I_off_n[2][42] = 2.31e-7; I_off_n[2][43] = 2.48e-7; I_off_n[2][44] = 2.65e-7; I_off_n[2][45] = 2.84e-7; 
        I_off_n[2][46] = 3.05e-7; I_off_n[2][47] = 3.26e-7; I_off_n[2][48] = 3.49e-7; I_off_n[2][49] = 3.73e-7; I_off_n[2][50] = 3.99e-7;
        I_off_n[2][51] = 4.28e-7; I_off_n[2][52] = 4.62e-7; I_off_n[2][53] = 5.01e-7; I_off_n[2][54] = 5.42e-7; I_off_n[2][55] = 5.85e-7; 
        I_off_n[2][56] = 6.29e-7; I_off_n[2][57] = 6.73e-7; I_off_n[2][58] = 7.15e-7; I_off_n[2][59] = 7.55e-7; I_off_n[2][60] = 7.91e-7;
        I_off_n[2][61] = 8.23e-7; I_off_n[2][62] = 8.51e-7; I_off_n[2][63] = 8.76e-7; I_off_n[2][64] = 9.01e-7; I_off_n[2][65] = 9.25e-7; 
        I_off_n[2][66] = 9.51e-7; I_off_n[2][67] = 9.79e-7; I_off_n[2][68] = 1.01e-6; I_off_n[2][69] = 1.05e-6; I_off_n[2][70] = 1.09e-6;
        I_off_n[2][71] = 1.14e-6; I_off_n[2][72] = 1.21e-6; I_off_n[2][73] = 1.29e-6; I_off_n[2][74] = 1.38e-6; I_off_n[2][75] = 1.48e-6; 
        I_off_n[2][76] = 1.59e-6; I_off_n[2][77] = 1.71e-6; I_off_n[2][78] = 1.83e-6; I_off_n[2][79] = 1.96e-6; I_off_n[2][80] = 2.09e-6;
        I_off_n[2][81] = 2.25e-6; I_off_n[2][82] = 2.44e-6; I_off_n[2][83] = 2.66e-6; I_off_n[2][84] = 2.90e-6; I_off_n[2][85] = 3.14e-6; 
        I_off_n[2][86] = 3.38e-6; I_off_n[2][87] = 3.60e-6; I_off_n[2][88] = 3.79e-6; I_off_n[2][89] = 3.94e-6; I_off_n[2][90] = 4.04e-6;
        I_off_n[2][91] = 4.11e-6; I_off_n[2][92] = 4.18e-6; I_off_n[2][93] = 4.24e-6; I_off_n[2][94] = 4.30e-6; I_off_n[2][95] = 4.35e-6; 
        I_off_n[2][96] = 4.39e-6; I_off_n[2][97] = 4.43e-6; I_off_n[2][98] = 4.46e-6; I_off_n[2][99] = 4.47e-6; I_off_n[2][100] = 4.48e-6;

        if (ram_cell_tech_type == 3)
              {}
        else if (ram_cell_tech_type == 4)
        {
    	//22 nm commodity DRAM cell access transistor technology parameters.
    		//parameters
        	curr_vdd_dram_cell = 0.9;//0.45;//This value has reduced greatly in 2007 ITRS for all technology nodes. In
    		//2005 ITRS, the value was about twice the value in 2007 ITRS
    		Lphy[3] = 0.022;//micron
    		Lelec[3] = 0.0181;//micron.
    		curr_v_th_dram_access_transistor = 1;//V
    		width_dram_access_transistor = 0.022;//micron
    		curr_I_on_dram_cell = 20e-6; //This is a typical value that I have always
    		//kept constant. In reality this could perhaps be lower
    		curr_I_off_dram_cell_worst_case_length_temp = 1e-15;//A
    		curr_Wmemcella_dram = width_dram_access_transistor;
    		curr_Wmemcellpmos_dram = 0;
    		curr_Wmemcellnmos_dram = 0;
    		curr_area_cell_dram = 6*0.022*0.022;//micron2.
    		curr_asp_ratio_cell_dram = 0.667;
    		curr_c_dram_cell = 30e-15;//This is a typical value that I have alwaus
    		//kept constant.

    	//22 nm commodity DRAM wordline transistor parameters obtained using MASTAR.
    		curr_vpp = 2.3;//vpp. V
    		t_ox[3] = 3.5e-3;//micron
    		v_th[3] = 1.0;//V
    		c_ox[3] = 9.06e-15;//F/micron2
    		mobility_eff[3] =  367.29 * (1e-2 * 1e6 * 1e-2 * 1e6);//micron2 / Vs
    		Vdsat[3] = 0.0972; //V/micron
    		c_g_ideal[3] = 1.99e-16;//F/micron
    		c_fringe[3] = 0.053e-15;//F/micron
    		c_junc[3] = 1e-15;//F/micron2
    		I_on_n[3] = 910.5e-6;//A/micron
    		I_on_p[3] = I_on_n[3] / 2;//This value for I_on_p is not really used.
    		nmos_effective_resistance_multiplier = 1.69;//Using the value from 32nm.
    		//
    		n_to_p_eff_curr_drv_ratio[3] = 1.95;//Using the value from 32nm
    		gmp_to_gmn_multiplier[3] = 0.90;
    		Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp  / I_on_n[3];//ohm-micron
    		Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];//ohm-micron
    		long_channel_leakage_reduction[3] = 1;
                /*
    		I_off_n[3][0] = 1.1e-13; //A/micron
    		I_off_n[3][10] = 2.11e-13;
    		I_off_n[3][20] = 3.88e-13;
    		I_off_n[3][30] = 6.9e-13;
    		I_off_n[3][40] = 1.19e-12;
    		I_off_n[3][50] = 1.98e-12;
    		I_off_n[3][60] = 3.22e-12;
    		I_off_n[3][70] = 5.09e-12;
    		I_off_n[3][80] = 7.85e-12;
    		I_off_n[3][90] = 1.18e-11;
    		I_off_n[3][100] = 1.72e-11;
                */
                /* Leakage current data were interpolated to support full-range leakage feedback.
                   Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
                I_off_n[3][0] = 1.10e-13; 
                I_off_n[3][1] = 1.17e-13; I_off_n[3][2] = 1.24e-13; I_off_n[3][3] = 1.33e-13; I_off_n[3][4] = 1.42e-13; I_off_n[3][5] = 1.52e-13; 
                I_off_n[3][6] = 1.63e-13; I_off_n[3][7] = 1.74e-13; I_off_n[3][8] = 1.86e-13; I_off_n[3][9] = 1.98e-13; I_off_n[3][10] = 2.11e-13;
                I_off_n[3][11] = 2.24e-13; I_off_n[3][12] = 2.39e-13; I_off_n[3][13] = 2.54e-13; I_off_n[3][14] = 2.70e-13; I_off_n[3][15] = 2.88e-13; 
                I_off_n[3][16] = 3.06e-13; I_off_n[3][17] = 3.25e-13; I_off_n[3][18] = 3.45e-13; I_off_n[3][19] = 3.66e-13; I_off_n[3][20] = 3.88e-13;
                I_off_n[3][21] = 4.11e-13; I_off_n[3][22] = 4.36e-13; I_off_n[3][23] = 4.62e-13; I_off_n[3][24] = 4.90e-13; I_off_n[3][25] = 5.20e-13; 
                I_off_n[3][26] = 5.51e-13; I_off_n[3][27] = 5.83e-13; I_off_n[3][28] = 6.18e-13; I_off_n[3][29] = 6.53e-13; I_off_n[3][30] = 6.90e-13;
                I_off_n[3][31] = 7.29e-13; I_off_n[3][32] = 7.71e-13; I_off_n[3][33] = 8.15e-13; I_off_n[3][34] = 8.61e-13; I_off_n[3][35] = 9.11e-13; 
                I_off_n[3][36] = 9.62e-13; I_off_n[3][37] = 1.02e-12; I_off_n[3][38] = 1.07e-12; I_off_n[3][39] = 1.13e-12; I_off_n[3][40] = 1.19e-12;
                I_off_n[3][41] = 1.25e-12; I_off_n[3][42] = 1.32e-12; I_off_n[3][43] = 1.39e-12; I_off_n[3][44] = 1.46e-12; I_off_n[3][45] = 1.54e-12; 
                I_off_n[3][46] = 1.62e-12; I_off_n[3][47] = 1.71e-12; I_off_n[3][48] = 1.79e-12; I_off_n[3][49] = 1.89e-12; I_off_n[3][50] = 1.98e-12;
                I_off_n[3][51] = 2.08e-12; I_off_n[3][52] = 2.18e-12; I_off_n[3][53] = 2.30e-12; I_off_n[3][54] = 2.41e-12; I_off_n[3][55] = 2.53e-12; 
                I_off_n[3][56] = 2.66e-12; I_off_n[3][57] = 2.79e-12; I_off_n[3][58] = 2.93e-12; I_off_n[3][59] = 3.07e-12; I_off_n[3][60] = 3.22e-12;
                I_off_n[3][61] = 3.37e-12; I_off_n[3][62] = 3.53e-12; I_off_n[3][63] = 3.70e-12; I_off_n[3][64] = 3.88e-12; I_off_n[3][65] = 4.06e-12; 
                I_off_n[3][66] = 4.25e-12; I_off_n[3][67] = 4.45e-12; I_off_n[3][68] = 4.66e-12; I_off_n[3][69] = 4.87e-12; I_off_n[3][70] = 5.09e-12;
                I_off_n[3][71] = 5.32e-12; I_off_n[3][72] = 5.56e-12; I_off_n[3][73] = 5.81e-12; I_off_n[3][74] = 6.07e-12; I_off_n[3][75] = 6.34e-12; 
                I_off_n[3][76] = 6.62e-12; I_off_n[3][77] = 6.92e-12; I_off_n[3][78] = 7.22e-12; I_off_n[3][79] = 7.53e-12; I_off_n[3][80] = 7.85e-12;
                I_off_n[3][81] = 8.18e-12; I_off_n[3][82] = 8.53e-12; I_off_n[3][83] = 8.89e-12; I_off_n[3][84] = 9.27e-12; I_off_n[3][85] = 9.66e-12; 
                I_off_n[3][86] = 1.01e-11; I_off_n[3][87] = 1.05e-11; I_off_n[3][88] = 1.09e-11; I_off_n[3][89] = 1.13e-11; I_off_n[3][90] = 1.18e-11;
                I_off_n[3][91] = 1.23e-11; I_off_n[3][92] = 1.27e-11; I_off_n[3][93] = 1.33e-11; I_off_n[3][94] = 1.38e-11; I_off_n[3][95] = 1.43e-11; 
                I_off_n[3][96] = 1.49e-11; I_off_n[3][97] = 1.54e-11; I_off_n[3][98] = 1.60e-11; I_off_n[3][99] = 1.66e-11; I_off_n[3][100] = 1.72e-11;
    	}
        else
        {
      	  //some error handler
        }

        //SRAM cell properties
        curr_Wmemcella_sram    = 1.31 * g_ip->F_sz_um;
        curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
        curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
        curr_area_cell_sram    = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
        curr_asp_ratio_cell_sram = 1.46;
        //CAM cell properties //TODO: data need to be revisited
        curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
        curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
        curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
        curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;
        curr_asp_ratio_cell_cam = 2.92;
        //Empirical undifferetiated core/FU coefficient
        curr_logic_scaling_co_eff = 0.7*0.7*0.7*0.7;
        curr_core_tx_density      = 1.25/0.7/0.7;
        curr_sckt_co_eff           = 1.1296;
        curr_chip_layout_overhead  = 1.2;//die measurement results based on Niagara 1 and 2
        curr_macro_layout_overhead = 1.1;//EDA placement and routing tool rule of thumb
    	}

    if(tech == 16){
    	//For 2019, MPU/ASIC stagger-contacted M1 half-pitch is 16 nm (so this is 16 nm
    	//technology i.e. FEATURESIZE = 0.016). Using the DG process numbers for HP.
    	//16 nm HP
    	vdd[0] = 0.7;
    	Lphy[0] = 0.006;//Lphy is the physical gate-length.
    	Lelec[0] = 0.00315;//Lelec is the electrical gate-length.
    	t_ox[0] = 0.5e-3;//micron
    	v_th[0] = 0.1489;//V
    	c_ox[0] = 3.83e-14;//F/micron2 Cox_elec in MASTAR
    	mobility_eff[0] = 476.15 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
    	Vdsat[0] = 1.42e-2; //V/micron calculated in spreadsheet
    	c_g_ideal[0] = 2.30e-16;//F/micron
    	c_fringe[0] = 0.06e-15;//F/micron MASTAR inputdynamic/3
    	c_junc[0] = 0;//F/micron2 MASTAR result dynamic
    	I_on_n[0] =  2768.4e-6;//A/micron
    	I_on_p[0] = I_on_n[0] / 2;//A/micron //This value for I_on_p is not really used.
        nmos_effective_resistance_multiplier = 1.48;//nmos_effective_resistance_multiplier  is the ratio of Ieff to Idsat where Ieff is the effective NMOS current and Idsat is the saturation current.
        n_to_p_eff_curr_drv_ratio[0] = 2; //Wpmos/Wnmos = 2 in 2007 MASTAR. Look in
    	//"Dynamic" tab of Device workspace.
        gmp_to_gmn_multiplier[0] = 1.38; //Just using the 32nm SOI value.
        Rnchannelon[0] = nmos_effective_resistance_multiplier * vdd[0] / I_on_n[0];//ohm-micron
        Rpchannelon[0] = n_to_p_eff_curr_drv_ratio[0] * Rnchannelon[0];//ohm-micron
        long_channel_leakage_reduction[0] = 1/2.655;
        /*
        I_off_n[0][0] = 1.52e-7/1.5*1.2*1.07;
        I_off_n[0][10] = 1.55e-7/1.5*1.2*1.07;
        I_off_n[0][20] = 1.59e-7/1.5*1.2*1.07;
        I_off_n[0][30] = 1.68e-7/1.5*1.2*1.07;
        I_off_n[0][40] = 1.90e-7/1.5*1.2*1.07;
        I_off_n[0][50] = 2.69e-7/1.5*1.2*1.07;
        I_off_n[0][60] = 5.32e-7/1.5*1.2*1.07;
        I_off_n[0][70] = 1.02e-6/1.5*1.2*1.07;
        I_off_n[0][80] = 1.62e-6/1.5*1.2*1.07;
        I_off_n[0][90] = 2.73e-6/1.5*1.2*1.07;
        I_off_n[0][100] = 6.1e-6/1.5*1.2*1.07;
        //for 16nm DG HP
        I_g_on_n[0][0]  = 1.07e-9;//A/micron
        I_g_on_n[0][10] = 1.07e-9;
        I_g_on_n[0][20] = 1.07e-9;
        I_g_on_n[0][30] = 1.07e-9;
        I_g_on_n[0][40] = 1.07e-9;
        I_g_on_n[0][50] = 1.07e-9;
        I_g_on_n[0][60] = 1.07e-9;
        I_g_on_n[0][70] = 1.07e-9;
        I_g_on_n[0][80] = 1.07e-9;
        I_g_on_n[0][90] = 1.07e-9;
        I_g_on_n[0][100] = 1.07e-9;
        */
        /* Leakage current data were interpolated to support full-range leakage feedback.
           Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
        I_g_on_n[0][0] = 1.07e-9; 
        I_g_on_n[0][1] = 1.07e-9; I_g_on_n[0][2] = 1.07e-9; I_g_on_n[0][3] = 1.07e-9; I_g_on_n[0][4] = 1.07e-9; I_g_on_n[0][5] = 1.07e-9; 
        I_g_on_n[0][6] = 1.07e-9; I_g_on_n[0][7] = 1.07e-9; I_g_on_n[0][8] = 1.07e-9; I_g_on_n[0][9] = 1.07e-9; I_g_on_n[0][10] = 1.07e-9;
        I_g_on_n[0][11] = 1.07e-9; I_g_on_n[0][12] = 1.07e-9; I_g_on_n[0][13] = 1.07e-9; I_g_on_n[0][14] = 1.07e-9; I_g_on_n[0][15] = 1.07e-9; 
        I_g_on_n[0][16] = 1.07e-9; I_g_on_n[0][17] = 1.07e-9; I_g_on_n[0][18] = 1.07e-9; I_g_on_n[0][19] = 1.07e-9; I_g_on_n[0][20] = 1.07e-9;
        I_g_on_n[0][21] = 1.07e-9; I_g_on_n[0][22] = 1.07e-9; I_g_on_n[0][23] = 1.07e-9; I_g_on_n[0][24] = 1.07e-9; I_g_on_n[0][25] = 1.07e-9; 
        I_g_on_n[0][26] = 1.07e-9; I_g_on_n[0][27] = 1.07e-9; I_g_on_n[0][28] = 1.07e-9; I_g_on_n[0][29] = 1.07e-9; I_g_on_n[0][30] = 1.07e-9;
        I_g_on_n[0][31] = 1.07e-9; I_g_on_n[0][32] = 1.07e-9; I_g_on_n[0][33] = 1.07e-9; I_g_on_n[0][34] = 1.07e-9; I_g_on_n[0][35] = 1.07e-9; 
        I_g_on_n[0][36] = 1.07e-9; I_g_on_n[0][37] = 1.07e-9; I_g_on_n[0][38] = 1.07e-9; I_g_on_n[0][39] = 1.07e-9; I_g_on_n[0][40] = 1.07e-9;
        I_g_on_n[0][41] = 1.07e-9; I_g_on_n[0][42] = 1.07e-9; I_g_on_n[0][43] = 1.07e-9; I_g_on_n[0][44] = 1.07e-9; I_g_on_n[0][45] = 1.07e-9; 
        I_g_on_n[0][46] = 1.07e-9; I_g_on_n[0][47] = 1.07e-9; I_g_on_n[0][48] = 1.07e-9; I_g_on_n[0][49] = 1.07e-9; I_g_on_n[0][50] = 1.07e-9;
        I_g_on_n[0][51] = 1.07e-9; I_g_on_n[0][52] = 1.07e-9; I_g_on_n[0][53] = 1.07e-9; I_g_on_n[0][54] = 1.07e-9; I_g_on_n[0][55] = 1.07e-9; 
        I_g_on_n[0][56] = 1.07e-9; I_g_on_n[0][57] = 1.07e-9; I_g_on_n[0][58] = 1.07e-9; I_g_on_n[0][59] = 1.07e-9; I_g_on_n[0][60] = 1.07e-9;
        I_g_on_n[0][61] = 1.07e-9; I_g_on_n[0][62] = 1.07e-9; I_g_on_n[0][63] = 1.07e-9; I_g_on_n[0][64] = 1.07e-9; I_g_on_n[0][65] = 1.07e-9; 
        I_g_on_n[0][66] = 1.07e-9; I_g_on_n[0][67] = 1.07e-9; I_g_on_n[0][68] = 1.07e-9; I_g_on_n[0][69] = 1.07e-9; I_g_on_n[0][70] = 1.07e-9;
        I_g_on_n[0][71] = 1.07e-9; I_g_on_n[0][72] = 1.07e-9; I_g_on_n[0][73] = 1.07e-9; I_g_on_n[0][74] = 1.07e-9; I_g_on_n[0][75] = 1.07e-9; 
        I_g_on_n[0][76] = 1.07e-9; I_g_on_n[0][77] = 1.07e-9; I_g_on_n[0][78] = 1.07e-9; I_g_on_n[0][79] = 1.07e-9; I_g_on_n[0][80] = 1.07e-9;
        I_g_on_n[0][81] = 1.07e-9; I_g_on_n[0][82] = 1.07e-9; I_g_on_n[0][83] = 1.07e-9; I_g_on_n[0][84] = 1.07e-9; I_g_on_n[0][85] = 1.07e-9; 
        I_g_on_n[0][86] = 1.07e-9; I_g_on_n[0][87] = 1.07e-9; I_g_on_n[0][88] = 1.07e-9; I_g_on_n[0][89] = 1.07e-9; I_g_on_n[0][90] = 1.07e-9;
        I_g_on_n[0][91] = 1.07e-9; I_g_on_n[0][92] = 1.07e-9; I_g_on_n[0][93] = 1.07e-9; I_g_on_n[0][94] = 1.07e-9; I_g_on_n[0][95] = 1.07e-9; 
        I_g_on_n[0][96] = 1.07e-9; I_g_on_n[0][97] = 1.07e-9; I_g_on_n[0][98] = 1.07e-9; I_g_on_n[0][99] = 1.07e-9; I_g_on_n[0][100] = 1.07e-9;
        I_off_n[0][0] = 1.30e-7; 
        I_off_n[0][1] = 1.30e-7; I_off_n[0][2] = 1.31e-7; I_off_n[0][3] = 1.31e-7; I_off_n[0][4] = 1.31e-7; I_off_n[0][5] = 1.31e-7; 
        I_off_n[0][6] = 1.32e-7; I_off_n[0][7] = 1.32e-7; I_off_n[0][8] = 1.32e-7; I_off_n[0][9] = 1.32e-7; I_off_n[0][10] = 1.33e-7;
        I_off_n[0][11] = 1.33e-7; I_off_n[0][12] = 1.33e-7; I_off_n[0][13] = 1.34e-7; I_off_n[0][14] = 1.34e-7; I_off_n[0][15] = 1.34e-7; 
        I_off_n[0][16] = 1.34e-7; I_off_n[0][17] = 1.35e-7; I_off_n[0][18] = 1.35e-7; I_off_n[0][19] = 1.36e-7; I_off_n[0][20] = 1.36e-7;
        I_off_n[0][21] = 1.37e-7; I_off_n[0][22] = 1.37e-7; I_off_n[0][23] = 1.38e-7; I_off_n[0][24] = 1.38e-7; I_off_n[0][25] = 1.39e-7; 
        I_off_n[0][26] = 1.40e-7; I_off_n[0][27] = 1.41e-7; I_off_n[0][28] = 1.42e-7; I_off_n[0][29] = 1.43e-7; I_off_n[0][30] = 1.44e-7;
        I_off_n[0][31] = 1.45e-7; I_off_n[0][32] = 1.46e-7; I_off_n[0][33] = 1.48e-7; I_off_n[0][34] = 1.49e-7; I_off_n[0][35] = 1.51e-7; 
        I_off_n[0][36] = 1.53e-7; I_off_n[0][37] = 1.55e-7; I_off_n[0][38] = 1.57e-7; I_off_n[0][39] = 1.60e-7; I_off_n[0][40] = 1.63e-7;
        I_off_n[0][41] = 1.66e-7; I_off_n[0][42] = 1.70e-7; I_off_n[0][43] = 1.75e-7; I_off_n[0][44] = 1.81e-7; I_off_n[0][45] = 1.87e-7; 
        I_off_n[0][46] = 1.94e-7; I_off_n[0][47] = 2.02e-7; I_off_n[0][48] = 2.11e-7; I_off_n[0][49] = 2.20e-7; I_off_n[0][50] = 2.30e-7;
        I_off_n[0][51] = 2.42e-7; I_off_n[0][52] = 2.58e-7; I_off_n[0][53] = 2.76e-7; I_off_n[0][54] = 2.96e-7; I_off_n[0][55] = 3.19e-7; 
        I_off_n[0][56] = 3.44e-7; I_off_n[0][57] = 3.70e-7; I_off_n[0][58] = 3.98e-7; I_off_n[0][59] = 4.26e-7; I_off_n[0][60] = 4.55e-7;
        I_off_n[0][61] = 4.87e-7; I_off_n[0][62] = 5.22e-7; I_off_n[0][63] = 5.60e-7; I_off_n[0][64] = 6.00e-7; I_off_n[0][65] = 6.43e-7; 
        I_off_n[0][66] = 6.88e-7; I_off_n[0][67] = 7.34e-7; I_off_n[0][68] = 7.80e-7; I_off_n[0][69] = 8.27e-7; I_off_n[0][70] = 8.73e-7;
        I_off_n[0][71] = 9.19e-7; I_off_n[0][72] = 9.64e-7; I_off_n[0][73] = 1.01e-6; I_off_n[0][74] = 1.06e-6; I_off_n[0][75] = 1.10e-6; 
        I_off_n[0][76] = 1.15e-6; I_off_n[0][77] = 1.21e-6; I_off_n[0][78] = 1.26e-6; I_off_n[0][79] = 1.32e-6; I_off_n[0][80] = 1.39e-6;
        I_off_n[0][81] = 1.45e-6; I_off_n[0][82] = 1.53e-6; I_off_n[0][83] = 1.60e-6; I_off_n[0][84] = 1.68e-6; I_off_n[0][85] = 1.77e-6; 
        I_off_n[0][86] = 1.86e-6; I_off_n[0][87] = 1.96e-6; I_off_n[0][88] = 2.08e-6; I_off_n[0][89] = 2.20e-6; I_off_n[0][90] = 2.34e-6;
        I_off_n[0][91] = 2.50e-6; I_off_n[0][92] = 2.70e-6; I_off_n[0][93] = 2.93e-6; I_off_n[0][94] = 3.19e-6; I_off_n[0][95] = 3.48e-6; 
        I_off_n[0][96] = 3.79e-6; I_off_n[0][97] = 4.12e-6; I_off_n[0][98] = 4.47e-6; I_off_n[0][99] = 4.84e-6; I_off_n[0][100] = 5.22e-6;

//    	//16 nm LSTP DG
//    	vdd[1] = 0.8;
//    	Lphy[1] = 0.014;
//    	Lelec[1] = 0.008;//Lelec is the electrical gate-length.
//    	t_ox[1] = 1.1e-3;//micron
//    	v_th[1] = 0.40126;//V
//    	c_ox[1] = 2.30e-14;//F/micron2
//    	mobility_eff[1] =  738.09 * (1e-2 * 1e6 * 1e-2 * 1e6); //micron2 / Vs
//    	Vdsat[1] = 6.64e-2; //V/micron
//    	c_g_ideal[1] = 3.22e-16;//F/micron
//    	c_fringe[1] = 0.008e-15;
//    	c_junc[1] = 0;//F/micron2
//    	I_on_n[1] = 727.6e-6;//A/micron
//    	I_on_p[1] = I_on_n[1] / 2;
//    	nmos_effective_resistance_multiplier = 1.99;
//    	n_to_p_eff_curr_drv_ratio[1] = 2;
//    	gmp_to_gmn_multiplier[1] = 0.99;
//    	Rnchannelon[1] = nmos_effective_resistance_multiplier * vdd[1] / I_on_n[1];//ohm-micron
//    	Rpchannelon[1] = n_to_p_eff_curr_drv_ratio[1] * Rnchannelon[1];//ohm-micron
//    	I_off_n[1][0] = 2.43e-11;
//    	I_off_n[1][10] = 4.85e-11;
//    	I_off_n[1][20] = 9.68e-11;
//    	I_off_n[1][30] = 1.94e-10;
//    	I_off_n[1][40] = 3.87e-10;
//    	I_off_n[1][50] = 7.73e-10;
//    	I_off_n[1][60] = 3.55e-10;
//    	I_off_n[1][70] = 3.09e-9;
//    	I_off_n[1][80] = 6.19e-9;
//    	I_off_n[1][90] = 1.24e-8;
//    	I_off_n[1][100]= 2.48e-8;
//
//    	//    for 22nm LSTP HP
//    	I_g_on_n[1][0]  = 4.51e-10;//A/micron
//    	I_g_on_n[1][10] = 4.51e-10;
//    	I_g_on_n[1][20] = 4.51e-10;
//    	I_g_on_n[1][30] = 4.51e-10;
//    	I_g_on_n[1][40] = 4.51e-10;
//    	I_g_on_n[1][50] = 4.51e-10;
//    	I_g_on_n[1][60] = 4.51e-10;
//    	I_g_on_n[1][70] = 4.51e-10;
//    	I_g_on_n[1][80] = 4.51e-10;
//    	I_g_on_n[1][90] = 4.51e-10;
//    	I_g_on_n[1][100] = 4.51e-10;


        if (ram_cell_tech_type == 3)
              {}
        else if (ram_cell_tech_type == 4)
        {
    	//22 nm commodity DRAM cell access transistor technology parameters.
    		//parameters
        	curr_vdd_dram_cell = 0.9;//0.45;//This value has reduced greatly in 2007 ITRS for all technology nodes. In
    		//2005 ITRS, the value was about twice the value in 2007 ITRS
    		Lphy[3] = 0.022;//micron
    		Lelec[3] = 0.0181;//micron.
    		curr_v_th_dram_access_transistor = 1;//V
    		width_dram_access_transistor = 0.022;//micron
    		curr_I_on_dram_cell = 20e-6; //This is a typical value that I have always
    		//kept constant. In reality this could perhaps be lower
    		curr_I_off_dram_cell_worst_case_length_temp = 1e-15;//A
    		curr_Wmemcella_dram = width_dram_access_transistor;
    		curr_Wmemcellpmos_dram = 0;
    		curr_Wmemcellnmos_dram = 0;
    		curr_area_cell_dram = 6*0.022*0.022;//micron2.
    		curr_asp_ratio_cell_dram = 0.667;
    		curr_c_dram_cell = 30e-15;//This is a typical value that I have alwaus
    		//kept constant.

    	//22 nm commodity DRAM wordline transistor parameters obtained using MASTAR.
    		curr_vpp = 2.3;//vpp. V
    		t_ox[3] = 3.5e-3;//micron
    		v_th[3] = 1.0;//V
    		c_ox[3] = 9.06e-15;//F/micron2
    		mobility_eff[3] =  367.29 * (1e-2 * 1e6 * 1e-2 * 1e6);//micron2 / Vs
    		Vdsat[3] = 0.0972; //V/micron
    		c_g_ideal[3] = 1.99e-16;//F/micron
    		c_fringe[3] = 0.053e-15;//F/micron
    		c_junc[3] = 1e-15;//F/micron2
    		I_on_n[3] = 910.5e-6;//A/micron
    		I_on_p[3] = I_on_n[3] / 2;//This value for I_on_p is not really used.
    		nmos_effective_resistance_multiplier = 1.69;//Using the value from 32nm.
    		//
    		n_to_p_eff_curr_drv_ratio[3] = 1.95;//Using the value from 32nm
    		gmp_to_gmn_multiplier[3] = 0.90;
    		Rnchannelon[3] = nmos_effective_resistance_multiplier * curr_vpp  / I_on_n[3];//ohm-micron
    		Rpchannelon[3] = n_to_p_eff_curr_drv_ratio[3] * Rnchannelon[3];//ohm-micron
    		long_channel_leakage_reduction[3] = 1;
                /*
    		I_off_n[3][0] = 1.1e-13; //A/micron
    		I_off_n[3][10] = 2.11e-13;
    		I_off_n[3][20] = 3.88e-13;
    		I_off_n[3][30] = 6.9e-13;
    		I_off_n[3][40] = 1.19e-12;
    		I_off_n[3][50] = 1.98e-12;
    		I_off_n[3][60] = 3.22e-12;
    		I_off_n[3][70] = 5.09e-12;
    		I_off_n[3][80] = 7.85e-12;
    		I_off_n[3][90] = 1.18e-11;
    		I_off_n[3][100] = 1.72e-11;
                */
                /* Leakage current data were interpolated to support full-range leakage feedback.
                   Modified by William Song (wjhsong@gatech.edu), Georgia Tech and Sandia National Laboratories. */
                I_off_n[3][0] = 1.10e-13; 
                I_off_n[3][1] = 1.17e-13; I_off_n[3][2] = 1.24e-13; I_off_n[3][3] = 1.33e-13; I_off_n[3][4] = 1.42e-13; I_off_n[3][5] = 1.52e-13; 
                I_off_n[3][6] = 1.63e-13; I_off_n[3][7] = 1.74e-13; I_off_n[3][8] = 1.86e-13; I_off_n[3][9] = 1.98e-13; I_off_n[3][10] = 2.11e-13;
                I_off_n[3][11] = 2.24e-13; I_off_n[3][12] = 2.39e-13; I_off_n[3][13] = 2.54e-13; I_off_n[3][14] = 2.70e-13; I_off_n[3][15] = 2.88e-13; 
                I_off_n[3][16] = 3.06e-13; I_off_n[3][17] = 3.25e-13; I_off_n[3][18] = 3.45e-13; I_off_n[3][19] = 3.66e-13; I_off_n[3][20] = 3.88e-13;
                I_off_n[3][21] = 4.11e-13; I_off_n[3][22] = 4.36e-13; I_off_n[3][23] = 4.62e-13; I_off_n[3][24] = 4.90e-13; I_off_n[3][25] = 5.20e-13; 
                I_off_n[3][26] = 5.51e-13; I_off_n[3][27] = 5.83e-13; I_off_n[3][28] = 6.18e-13; I_off_n[3][29] = 6.53e-13; I_off_n[3][30] = 6.90e-13;
                I_off_n[3][31] = 7.29e-13; I_off_n[3][32] = 7.71e-13; I_off_n[3][33] = 8.15e-13; I_off_n[3][34] = 8.61e-13; I_off_n[3][35] = 9.11e-13; 
                I_off_n[3][36] = 9.62e-13; I_off_n[3][37] = 1.02e-12; I_off_n[3][38] = 1.07e-12; I_off_n[3][39] = 1.13e-12; I_off_n[3][40] = 1.19e-12;
                I_off_n[3][41] = 1.25e-12; I_off_n[3][42] = 1.32e-12; I_off_n[3][43] = 1.39e-12; I_off_n[3][44] = 1.46e-12; I_off_n[3][45] = 1.54e-12; 
                I_off_n[3][46] = 1.62e-12; I_off_n[3][47] = 1.71e-12; I_off_n[3][48] = 1.79e-12; I_off_n[3][49] = 1.89e-12; I_off_n[3][50] = 1.98e-12;
                I_off_n[3][51] = 2.08e-12; I_off_n[3][52] = 2.18e-12; I_off_n[3][53] = 2.30e-12; I_off_n[3][54] = 2.41e-12; I_off_n[3][55] = 2.53e-12; 
                I_off_n[3][56] = 2.66e-12; I_off_n[3][57] = 2.79e-12; I_off_n[3][58] = 2.93e-12; I_off_n[3][59] = 3.07e-12; I_off_n[3][60] = 3.22e-12;
                I_off_n[3][61] = 3.37e-12; I_off_n[3][62] = 3.53e-12; I_off_n[3][63] = 3.70e-12; I_off_n[3][64] = 3.88e-12; I_off_n[3][65] = 4.06e-12; 
                I_off_n[3][66] = 4.25e-12; I_off_n[3][67] = 4.45e-12; I_off_n[3][68] = 4.66e-12; I_off_n[3][69] = 4.87e-12; I_off_n[3][70] = 5.09e-12;
                I_off_n[3][71] = 5.32e-12; I_off_n[3][72] = 5.56e-12; I_off_n[3][73] = 5.81e-12; I_off_n[3][74] = 6.07e-12; I_off_n[3][75] = 6.34e-12; 
                I_off_n[3][76] = 6.62e-12; I_off_n[3][77] = 6.92e-12; I_off_n[3][78] = 7.22e-12; I_off_n[3][79] = 7.53e-12; I_off_n[3][80] = 7.85e-12;
                I_off_n[3][81] = 8.18e-12; I_off_n[3][82] = 8.53e-12; I_off_n[3][83] = 8.89e-12; I_off_n[3][84] = 9.27e-12; I_off_n[3][85] = 9.66e-12; 
                I_off_n[3][86] = 1.01e-11; I_off_n[3][87] = 1.05e-11; I_off_n[3][88] = 1.09e-11; I_off_n[3][89] = 1.13e-11; I_off_n[3][90] = 1.18e-11;
                I_off_n[3][91] = 1.23e-11; I_off_n[3][92] = 1.27e-11; I_off_n[3][93] = 1.33e-11; I_off_n[3][94] = 1.38e-11; I_off_n[3][95] = 1.43e-11; 
                I_off_n[3][96] = 1.49e-11; I_off_n[3][97] = 1.54e-11; I_off_n[3][98] = 1.60e-11; I_off_n[3][99] = 1.66e-11; I_off_n[3][100] = 1.72e-11;

    	}
        else
        {
      	  //some error handler
        }

        //SRAM cell properties
        curr_Wmemcella_sram    = 1.31 * g_ip->F_sz_um;
        curr_Wmemcellpmos_sram = 1.23 * g_ip->F_sz_um;
        curr_Wmemcellnmos_sram = 2.08 * g_ip->F_sz_um;
        curr_area_cell_sram    = 146 * g_ip->F_sz_um * g_ip->F_sz_um;
        curr_asp_ratio_cell_sram = 1.46;
        //CAM cell properties //TODO: data need to be revisited
        curr_Wmemcella_cam = 1.31 * g_ip->F_sz_um;
        curr_Wmemcellpmos_cam = 1.23 * g_ip->F_sz_um;
        curr_Wmemcellnmos_cam = 2.08 * g_ip->F_sz_um;
        curr_area_cell_cam = 292 * g_ip->F_sz_um * g_ip->F_sz_um;
        curr_asp_ratio_cell_cam = 2.92;
        //Empirical undifferetiated core/FU coefficient
        curr_logic_scaling_co_eff = 0.7*0.7*0.7*0.7*0.7;
        curr_core_tx_density      = 1.25/0.7/0.7/0.7;
        curr_sckt_co_eff           = 1.1296;
        curr_chip_layout_overhead  = 1.2;//die measurement results based on Niagara 1 and 2
        curr_macro_layout_overhead = 1.1;//EDA placement and routing tool rule of thumb
    	}


    g_tp.peri_global.Vdd       += curr_alpha * vdd[peri_global_tech_type];
    g_tp.peri_global.t_ox      += curr_alpha * t_ox[peri_global_tech_type];
    g_tp.peri_global.Vth       += curr_alpha * v_th[peri_global_tech_type];
    g_tp.peri_global.C_ox      += curr_alpha * c_ox[peri_global_tech_type];
    g_tp.peri_global.C_g_ideal += curr_alpha * c_g_ideal[peri_global_tech_type];
    g_tp.peri_global.C_fringe  += curr_alpha * c_fringe[peri_global_tech_type];
    g_tp.peri_global.C_junc    += curr_alpha * c_junc[peri_global_tech_type];
    g_tp.peri_global.C_junc_sidewall = 0.25e-15;  // F/micron
    g_tp.peri_global.l_phy     += curr_alpha * Lphy[peri_global_tech_type];
    g_tp.peri_global.l_elec    += curr_alpha * Lelec[peri_global_tech_type];
    g_tp.peri_global.I_on_n    += curr_alpha * I_on_n[peri_global_tech_type];
    g_tp.peri_global.R_nch_on  += curr_alpha * Rnchannelon[peri_global_tech_type];
    g_tp.peri_global.R_pch_on  += curr_alpha * Rpchannelon[peri_global_tech_type];
    g_tp.peri_global.n_to_p_eff_curr_drv_ratio
      += curr_alpha * n_to_p_eff_curr_drv_ratio[peri_global_tech_type];
    g_tp.peri_global.long_channel_leakage_reduction
      += curr_alpha * long_channel_leakage_reduction[peri_global_tech_type];
    g_tp.peri_global.I_off_n   += curr_alpha * I_off_n[peri_global_tech_type][g_ip->temp - 300];
    g_tp.peri_global.I_off_p   += curr_alpha * I_off_n[peri_global_tech_type][g_ip->temp - 300];
    g_tp.peri_global.I_g_on_n   += curr_alpha * I_g_on_n[peri_global_tech_type][g_ip->temp - 300];
    g_tp.peri_global.I_g_on_p   += curr_alpha * I_g_on_n[peri_global_tech_type][g_ip->temp - 300];
    gmp_to_gmn_multiplier_periph_global += curr_alpha * gmp_to_gmn_multiplier[peri_global_tech_type];

    g_tp.sram_cell.Vdd       += curr_alpha * vdd[ram_cell_tech_type];
    g_tp.sram_cell.l_phy     += curr_alpha * Lphy[ram_cell_tech_type];
    g_tp.sram_cell.l_elec    += curr_alpha * Lelec[ram_cell_tech_type];
    g_tp.sram_cell.t_ox      += curr_alpha * t_ox[ram_cell_tech_type];
    g_tp.sram_cell.Vth       += curr_alpha * v_th[ram_cell_tech_type];
    g_tp.sram_cell.C_g_ideal += curr_alpha * c_g_ideal[ram_cell_tech_type];
    g_tp.sram_cell.C_fringe  += curr_alpha * c_fringe[ram_cell_tech_type];
    g_tp.sram_cell.C_junc    += curr_alpha * c_junc[ram_cell_tech_type];
    g_tp.sram_cell.C_junc_sidewall = 0.25e-15;  // F/micron
    g_tp.sram_cell.I_on_n    += curr_alpha * I_on_n[ram_cell_tech_type];
    g_tp.sram_cell.R_nch_on  += curr_alpha * Rnchannelon[ram_cell_tech_type];
    g_tp.sram_cell.R_pch_on  += curr_alpha * Rpchannelon[ram_cell_tech_type];
    g_tp.sram_cell.n_to_p_eff_curr_drv_ratio += curr_alpha * n_to_p_eff_curr_drv_ratio[ram_cell_tech_type];
    g_tp.sram_cell.long_channel_leakage_reduction += curr_alpha * long_channel_leakage_reduction[ram_cell_tech_type];
    g_tp.sram_cell.I_off_n   += curr_alpha * I_off_n[ram_cell_tech_type][g_ip->temp - 300];
    g_tp.sram_cell.I_off_p   += curr_alpha * I_off_n[ram_cell_tech_type][g_ip->temp - 300];
    g_tp.sram_cell.I_g_on_n   += curr_alpha * I_g_on_n[ram_cell_tech_type][g_ip->temp - 300];
    g_tp.sram_cell.I_g_on_p   += curr_alpha * I_g_on_n[ram_cell_tech_type][g_ip->temp - 300];

    g_tp.dram_cell_Vdd      += curr_alpha * curr_vdd_dram_cell;
    g_tp.dram_acc.Vth       += curr_alpha * curr_v_th_dram_access_transistor;
    g_tp.dram_acc.l_phy     += curr_alpha * Lphy[dram_cell_tech_flavor];
    g_tp.dram_acc.l_elec    += curr_alpha * Lelec[dram_cell_tech_flavor];
    g_tp.dram_acc.C_g_ideal += curr_alpha * c_g_ideal[dram_cell_tech_flavor];
    g_tp.dram_acc.C_fringe  += curr_alpha * c_fringe[dram_cell_tech_flavor];
    g_tp.dram_acc.C_junc    += curr_alpha * c_junc[dram_cell_tech_flavor];
    g_tp.dram_acc.C_junc_sidewall = 0.25e-15;  // F/micron
    g_tp.dram_cell_I_on     += curr_alpha * curr_I_on_dram_cell;
    g_tp.dram_cell_I_off_worst_case_len_temp += curr_alpha * curr_I_off_dram_cell_worst_case_length_temp;
    g_tp.dram_acc.I_on_n    += curr_alpha * I_on_n[dram_cell_tech_flavor];
    g_tp.dram_cell_C        += curr_alpha * curr_c_dram_cell;
    g_tp.vpp                += curr_alpha * curr_vpp;
    g_tp.dram_wl.l_phy      += curr_alpha * Lphy[dram_cell_tech_flavor];
    g_tp.dram_wl.l_elec     += curr_alpha * Lelec[dram_cell_tech_flavor];
    g_tp.dram_wl.C_g_ideal  += curr_alpha * c_g_ideal[dram_cell_tech_flavor];
    g_tp.dram_wl.C_fringe   += curr_alpha * c_fringe[dram_cell_tech_flavor];
    g_tp.dram_wl.C_junc     += curr_alpha * c_junc[dram_cell_tech_flavor];
    g_tp.dram_wl.C_junc_sidewall = 0.25e-15;  // F/micron
    g_tp.dram_wl.I_on_n     += curr_alpha * I_on_n[dram_cell_tech_flavor];
    g_tp.dram_wl.R_nch_on   += curr_alpha * Rnchannelon[dram_cell_tech_flavor];
    g_tp.dram_wl.R_pch_on   += curr_alpha * Rpchannelon[dram_cell_tech_flavor];
    g_tp.dram_wl.n_to_p_eff_curr_drv_ratio += curr_alpha * n_to_p_eff_curr_drv_ratio[dram_cell_tech_flavor];
    g_tp.dram_wl.long_channel_leakage_reduction += curr_alpha * long_channel_leakage_reduction[dram_cell_tech_flavor];
    g_tp.dram_wl.I_off_n    += curr_alpha * I_off_n[dram_cell_tech_flavor][g_ip->temp - 300];
    g_tp.dram_wl.I_off_p    += curr_alpha * I_off_n[dram_cell_tech_flavor][g_ip->temp - 300];

    g_tp.cam_cell.Vdd       += curr_alpha * vdd[ram_cell_tech_type];
    g_tp.cam_cell.l_phy     += curr_alpha * Lphy[ram_cell_tech_type];
    g_tp.cam_cell.l_elec    += curr_alpha * Lelec[ram_cell_tech_type];
    g_tp.cam_cell.t_ox      += curr_alpha * t_ox[ram_cell_tech_type];
    g_tp.cam_cell.Vth       += curr_alpha * v_th[ram_cell_tech_type];
    g_tp.cam_cell.C_g_ideal += curr_alpha * c_g_ideal[ram_cell_tech_type];
    g_tp.cam_cell.C_fringe  += curr_alpha * c_fringe[ram_cell_tech_type];
    g_tp.cam_cell.C_junc    += curr_alpha * c_junc[ram_cell_tech_type];
    g_tp.cam_cell.C_junc_sidewall = 0.25e-15;  // F/micron
    g_tp.cam_cell.I_on_n    += curr_alpha * I_on_n[ram_cell_tech_type];
    g_tp.cam_cell.R_nch_on  += curr_alpha * Rnchannelon[ram_cell_tech_type];
    g_tp.cam_cell.R_pch_on  += curr_alpha * Rpchannelon[ram_cell_tech_type];
    g_tp.cam_cell.n_to_p_eff_curr_drv_ratio += curr_alpha * n_to_p_eff_curr_drv_ratio[ram_cell_tech_type];
    g_tp.cam_cell.long_channel_leakage_reduction += curr_alpha * long_channel_leakage_reduction[ram_cell_tech_type];
    g_tp.cam_cell.I_off_n   += curr_alpha * I_off_n[ram_cell_tech_type][g_ip->temp - 300];
    g_tp.cam_cell.I_off_p   += curr_alpha * I_off_n[ram_cell_tech_type][g_ip->temp - 300];
    g_tp.cam_cell.I_g_on_n   += curr_alpha * I_g_on_n[ram_cell_tech_type][g_ip->temp - 300];
    g_tp.cam_cell.I_g_on_p   += curr_alpha * I_g_on_n[ram_cell_tech_type][g_ip->temp - 300];

    g_tp.dram.cell_a_w    += curr_alpha * curr_Wmemcella_dram;
    g_tp.dram.cell_pmos_w += curr_alpha * curr_Wmemcellpmos_dram;
    g_tp.dram.cell_nmos_w += curr_alpha * curr_Wmemcellnmos_dram;
    area_cell_dram        += curr_alpha * curr_area_cell_dram;
    asp_ratio_cell_dram   += curr_alpha * curr_asp_ratio_cell_dram;

    g_tp.sram.cell_a_w    += curr_alpha * curr_Wmemcella_sram;
    g_tp.sram.cell_pmos_w += curr_alpha * curr_Wmemcellpmos_sram;
    g_tp.sram.cell_nmos_w += curr_alpha * curr_Wmemcellnmos_sram;
    area_cell_sram += curr_alpha * curr_area_cell_sram;
    asp_ratio_cell_sram += curr_alpha * curr_asp_ratio_cell_sram;

    g_tp.cam.cell_a_w    += curr_alpha * curr_Wmemcella_cam;//sheng
    g_tp.cam.cell_pmos_w += curr_alpha * curr_Wmemcellpmos_cam;
    g_tp.cam.cell_nmos_w += curr_alpha * curr_Wmemcellnmos_cam;
    area_cell_cam += curr_alpha * curr_area_cell_cam;
    asp_ratio_cell_cam += curr_alpha * curr_asp_ratio_cell_cam;

    //Sense amplifier latch Gm calculation
    mobility_eff_periph_global += curr_alpha * mobility_eff[peri_global_tech_type];
    Vdsat_periph_global += curr_alpha * Vdsat[peri_global_tech_type];

    //Empirical undifferetiated core/FU coefficient
    g_tp.scaling_factor.logic_scaling_co_eff += curr_alpha * curr_logic_scaling_co_eff;
    g_tp.scaling_factor.core_tx_density += curr_alpha * curr_core_tx_density;
    g_tp.chip_layout_overhead  += curr_alpha * curr_chip_layout_overhead;
    g_tp.macro_layout_overhead += curr_alpha * curr_macro_layout_overhead;
    g_tp.sckt_co_eff           += curr_alpha * curr_sckt_co_eff;
  }


  //Currently we are not modeling the resistance/capacitance of poly anywhere.
  //Continuous function (or date have been processed) does not need linear interpolation
  g_tp.w_comp_inv_p1 = 12.5 * g_ip->F_sz_um;//this was 10 micron for the 0.8 micron process
  g_tp.w_comp_inv_n1 =  7.5 * g_ip->F_sz_um;//this was  6 micron for the 0.8 micron process
  g_tp.w_comp_inv_p2 =   25 * g_ip->F_sz_um;//this was 20 micron for the 0.8 micron process
  g_tp.w_comp_inv_n2 =   15 * g_ip->F_sz_um;//this was 12 micron for the 0.8 micron process
  g_tp.w_comp_inv_p3 =   50 * g_ip->F_sz_um;//this was 40 micron for the 0.8 micron process
  g_tp.w_comp_inv_n3 =   30 * g_ip->F_sz_um;//this was 24 micron for the 0.8 micron process
  g_tp.w_eval_inv_p  =  100 * g_ip->F_sz_um;//this was 80 micron for the 0.8 micron process
  g_tp.w_eval_inv_n  =   50 * g_ip->F_sz_um;//this was 40 micron for the 0.8 micron process
  g_tp.w_comp_n     = 12.5 * g_ip->F_sz_um;//this was 10 micron for the 0.8 micron process
  g_tp.w_comp_p     = 37.5 * g_ip->F_sz_um;//this was 30 micron for the 0.8 micron process

  g_tp.MIN_GAP_BET_P_AND_N_DIFFS = 5 * g_ip->F_sz_um;
  g_tp.MIN_GAP_BET_SAME_TYPE_DIFFS = 1.5 * g_ip->F_sz_um;
  g_tp.HPOWERRAIL = 2 * g_ip->F_sz_um;
  g_tp.cell_h_def = 50 * g_ip->F_sz_um;
  g_tp.w_poly_contact = g_ip->F_sz_um;
  g_tp.spacing_poly_to_contact = g_ip->F_sz_um;
  g_tp.spacing_poly_to_poly = 1.5 * g_ip->F_sz_um;
  g_tp.ram_wl_stitching_overhead_ = 7.5 * g_ip->F_sz_um;

  g_tp.min_w_nmos_ = 3 * g_ip->F_sz_um / 2;
  g_tp.max_w_nmos_ = 100 * g_ip->F_sz_um;
  g_tp.w_iso       = 12.5*g_ip->F_sz_um;//was 10 micron for the 0.8 micron process
  g_tp.w_sense_n   = 3.75*g_ip->F_sz_um; // sense amplifier N-trans; was 3 micron for the 0.8 micron process
  g_tp.w_sense_p   = 7.5*g_ip->F_sz_um; // sense amplifier P-trans; was 6 micron for the 0.8 micron process
  g_tp.w_sense_en  = 5*g_ip->F_sz_um; // Sense enable transistor of the sense amplifier; was 4 micron for the 0.8 micron process
  g_tp.w_nmos_b_mux  = 6 * g_tp.min_w_nmos_;
  g_tp.w_nmos_sa_mux = 6 * g_tp.min_w_nmos_;

  if (ram_cell_tech_type == comm_dram)
  {
    g_tp.max_w_nmos_dec = 8 * g_ip->F_sz_um;
    g_tp.h_dec          = 8;  // in the unit of memory cell height
  }
  else
  {
    g_tp.max_w_nmos_dec = g_tp.max_w_nmos_;
    g_tp.h_dec          = 4;  // in the unit of memory cell height
  }

  g_tp.peri_global.C_overlap = 0.2 * g_tp.peri_global.C_g_ideal;
  g_tp.sram_cell.C_overlap   = 0.2 * g_tp.sram_cell.C_g_ideal;
  g_tp.cam_cell.C_overlap    = 0.2 * g_tp.cam_cell.C_g_ideal;

  g_tp.dram_acc.C_overlap = 0.2 * g_tp.dram_acc.C_g_ideal;
  g_tp.dram_acc.R_nch_on = g_tp.dram_cell_Vdd / g_tp.dram_acc.I_on_n;
  //g_tp.dram_acc.R_pch_on = g_tp.dram_cell_Vdd / g_tp.dram_acc.I_on_p;

  g_tp.dram_wl.C_overlap = 0.2 * g_tp.dram_wl.C_g_ideal;

  double gmn_sense_amp_latch = (mobility_eff_periph_global / 2) * g_tp.peri_global.C_ox * (g_tp.w_sense_n / g_tp.peri_global.l_elec) * Vdsat_periph_global;
  double gmp_sense_amp_latch = gmp_to_gmn_multiplier_periph_global * gmn_sense_amp_latch;
  g_tp.gm_sense_amp_latch = gmn_sense_amp_latch + gmp_sense_amp_latch;

  g_tp.dram.b_w = sqrt(area_cell_dram / (asp_ratio_cell_dram));
  g_tp.dram.b_h = asp_ratio_cell_dram * g_tp.dram.b_w;
  g_tp.sram.b_w = sqrt(area_cell_sram / (asp_ratio_cell_sram));
  g_tp.sram.b_h = asp_ratio_cell_sram * g_tp.sram.b_w;
  g_tp.cam.b_w =  sqrt(area_cell_cam / (asp_ratio_cell_cam));//Sheng
  g_tp.cam.b_h = asp_ratio_cell_cam * g_tp.cam.b_w;

  g_tp.dram.Vbitpre = g_tp.dram_cell_Vdd;
  g_tp.sram.Vbitpre = vdd[ram_cell_tech_type];
  g_tp.cam.Vbitpre = vdd[ram_cell_tech_type];//Sheng
  pmos_to_nmos_sizing_r = pmos_to_nmos_sz_ratio();
  g_tp.w_pmos_bl_precharge = 6 * pmos_to_nmos_sizing_r * g_tp.min_w_nmos_;
  g_tp.w_pmos_bl_eq = pmos_to_nmos_sizing_r * g_tp.min_w_nmos_;


  double wire_pitch       [NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         wire_r_per_micron[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         wire_c_per_micron[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         horiz_dielectric_constant[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         vert_dielectric_constant[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         aspect_ratio[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         miller_value[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES],
         ild_thickness[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUMBER_WIRE_TYPES];

  for (iter=0; iter<=1; ++iter)
  {
    // linear interpolation
    if (iter == 0)
    {
      tech = tech_lo;
      if (tech_lo == tech_hi)
      {
        curr_alpha = 1;
      }
      else
      {
        curr_alpha = (technology - tech_hi)/(tech_lo - tech_hi);
      }
    }
    else
    {
      tech = tech_hi;
      if (tech_lo == tech_hi)
      {
        break;
      }
      else
      {
        curr_alpha = (tech_lo - technology)/(tech_lo - tech_hi);
      }
    }

    if (tech == 180)
    {
    	//Aggressive projections
    	wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;//micron
    	aspect_ratio[0][0] = 2.0;
    	wire_width = wire_pitch[0][0] / 2; //micron
    	wire_thickness = aspect_ratio[0][0] * wire_width;//micron
    	wire_spacing = wire_pitch[0][0] - wire_width;//micron
    	barrier_thickness = 0.017;//micron
    	dishing_thickness = 0;//micron
    	alpha_scatter = 1;
    	wire_r_per_micron[0][0] = wire_resistance(CU_RESISTIVITY, wire_width,
    			wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);//ohm/micron
    	ild_thickness[0][0] = 0.75;//micron
    	miller_value[0][0] = 1.5;
    	horiz_dielectric_constant[0][0] = 2.709;
    	vert_dielectric_constant[0][0] = 3.9;
    	fringe_cap = 0.115e-15; //F/micron
        wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][0], miller_value[0][0], horiz_dielectric_constant[0][0],
          vert_dielectric_constant[0][0],
          fringe_cap);//F/micron.

    	wire_pitch[0][1] = 4 * g_ip->F_sz_um;
    	wire_width = wire_pitch[0][1] / 2;
    	aspect_ratio[0][1] = 2.4;
    	wire_thickness = aspect_ratio[0][1] * wire_width;
    	wire_spacing = wire_pitch[0][1] - wire_width;
    	wire_r_per_micron[0][1] = wire_resistance(CU_RESISTIVITY, wire_width,
    			wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
    	ild_thickness[0][1] = 0.75;//micron
    	miller_value[0][1] = 1.5;
    	horiz_dielectric_constant[0][1] = 2.709;
    	vert_dielectric_constant[0][1] = 3.9;
    	fringe_cap = 0.115e-15; //F/micron
        wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1],
          vert_dielectric_constant[0][1],
          fringe_cap);

    	wire_pitch[0][2] = 8 * g_ip->F_sz_um;
    	aspect_ratio[0][2] = 2.2;
    	wire_width = wire_pitch[0][2] / 2;
    	wire_thickness = aspect_ratio[0][2] * wire_width;
    	wire_spacing = wire_pitch[0][2] - wire_width;
    	wire_r_per_micron[0][2] = wire_resistance(CU_RESISTIVITY, wire_width,
    			wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
    	ild_thickness[0][2] = 1.5;
    	miller_value[0][2] = 1.5;
        horiz_dielectric_constant[0][2] = 2.709;
        vert_dielectric_constant[0][2] = 3.9;
        wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
          fringe_cap);

    	//Conservative projections
    	wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
    	aspect_ratio[1][0]= 2.0;
    	wire_width = wire_pitch[1][0] / 2;
    	wire_thickness = aspect_ratio[1][0] * wire_width;
    	wire_spacing = wire_pitch[1][0] - wire_width;
    	barrier_thickness = 0.017;
    	dishing_thickness = 0;
    	alpha_scatter = 1;
    	wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
    			wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
    	ild_thickness[1][0] = 0.75;
    	miller_value[1][0] = 1.5;
    	horiz_dielectric_constant[1][0] = 3.038;
    	vert_dielectric_constant[1][0] = 3.9;
    	fringe_cap = 0.115e-15;
        wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0],
          vert_dielectric_constant[1][0],
          fringe_cap);

    	wire_pitch[1][1] = 4 * g_ip->F_sz_um;
    	wire_width = wire_pitch[1][1] / 2;
    	aspect_ratio[1][1] = 2.0;
    	wire_thickness = aspect_ratio[1][1] * wire_width;
    	wire_spacing = wire_pitch[1][1] - wire_width;
    	wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
    			wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
    	ild_thickness[1][1] = 0.75;
    	miller_value[1][1] = 1.5;
    	horiz_dielectric_constant[1][1] = 3.038;
    	vert_dielectric_constant[1][1] = 3.9;
        wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1],
          vert_dielectric_constant[1][1],
          fringe_cap);

    	wire_pitch[1][2] = 8 * g_ip->F_sz_um;
    	aspect_ratio[1][2] = 2.2;
    	wire_width = wire_pitch[1][2] / 2;
    	wire_thickness = aspect_ratio[1][2] * wire_width;
    	wire_spacing = wire_pitch[1][2] - wire_width;
    	dishing_thickness = 0.1 *  wire_thickness;
    	wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
    			wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
    	ild_thickness[1][2]  = 1.98;
    	miller_value[1][2]  = 1.5;
        horiz_dielectric_constant[1][2]  = 3.038;
        vert_dielectric_constant[1][2]  = 3.9;
        wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][2] , miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
          fringe_cap);
    	//Nominal projections for commodity DRAM wordline/bitline
    	wire_pitch[1][3] = 2 * 0.18;
    	wire_c_per_micron[1][3] = 60e-15 / (256 * 2 * 0.18);
    	wire_r_per_micron[1][3] = 12 / 0.18;
    }
    else if (tech == 90)
    {
      //Aggressive projections
      wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;//micron
      aspect_ratio[0][0] = 2.4;
      wire_width = wire_pitch[0][0] / 2; //micron
      wire_thickness = aspect_ratio[0][0] * wire_width;//micron
      wire_spacing = wire_pitch[0][0] - wire_width;//micron
      barrier_thickness = 0.01;//micron
      dishing_thickness = 0;//micron
      alpha_scatter = 1;
      wire_r_per_micron[0][0] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);//ohm/micron
      ild_thickness[0][0] = 0.48;//micron
      miller_value[0][0] = 1.5;
      horiz_dielectric_constant[0][0] = 2.709;
      vert_dielectric_constant[0][0] = 3.9;
      fringe_cap = 0.115e-15; //F/micron
      wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][0], miller_value[0][0], horiz_dielectric_constant[0][0],
          vert_dielectric_constant[0][0],
          fringe_cap);//F/micron.

      wire_pitch[0][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[0][1] / 2;
      aspect_ratio[0][1] = 2.4;
      wire_thickness = aspect_ratio[0][1] * wire_width;
      wire_spacing = wire_pitch[0][1] - wire_width;
      wire_r_per_micron[0][1] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][1] = 0.48;//micron
      miller_value[0][1] = 1.5;
      horiz_dielectric_constant[0][1] = 2.709;
      vert_dielectric_constant[0][1] = 3.9;
      wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1],
          vert_dielectric_constant[0][1],
          fringe_cap);

      wire_pitch[0][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[0][2] = 2.7;
      wire_width = wire_pitch[0][2] / 2;
      wire_thickness = aspect_ratio[0][2] * wire_width;
      wire_spacing = wire_pitch[0][2] - wire_width;
      wire_r_per_micron[0][2] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][2] = 0.96;
      miller_value[0][2] = 1.5;
      horiz_dielectric_constant[0][2] = 2.709;
      vert_dielectric_constant[0][2] = 3.9;
      wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
          fringe_cap);

      //Conservative projections
      wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[1][0]  = 2.0;
      wire_width = wire_pitch[1][0] / 2;
      wire_thickness = aspect_ratio[1][0] * wire_width;
      wire_spacing = wire_pitch[1][0] - wire_width;
      barrier_thickness = 0.008;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][0]  = 0.48;
      miller_value[1][0]  = 1.5;
      horiz_dielectric_constant[1][0]  = 3.038;
      vert_dielectric_constant[1][0]  = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0],
          vert_dielectric_constant[1][0],
          fringe_cap);

      wire_pitch[1][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[1][1] / 2;
      aspect_ratio[1][1] = 2.0;
      wire_thickness = aspect_ratio[1][1] * wire_width;
      wire_spacing = wire_pitch[1][1] - wire_width;
      wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][1]  = 0.48;
      miller_value[1][1]  = 1.5;
      horiz_dielectric_constant[1][1]  = 3.038;
      vert_dielectric_constant[1][1]  = 3.9;
      wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1],
          vert_dielectric_constant[1][1],
          fringe_cap);

      wire_pitch[1][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[1][2]  = 2.2;
      wire_width = wire_pitch[1][2] / 2;
      wire_thickness = aspect_ratio[1][2] * wire_width;
      wire_spacing = wire_pitch[1][2] - wire_width;
      dishing_thickness = 0.1 *  wire_thickness;
      wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][2]  = 1.1;
      miller_value[1][2]  = 1.5;
      horiz_dielectric_constant[1][2]  = 3.038;
      vert_dielectric_constant[1][2]  = 3.9;
      wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][2] , miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
          fringe_cap);
      //Nominal projections for commodity DRAM wordline/bitline
      wire_pitch[1][3] = 2 * 0.09;
      wire_c_per_micron[1][3] = 60e-15 / (256 * 2 * 0.09);
      wire_r_per_micron[1][3] = 12 / 0.09;
    }
    else if (tech == 65)
    {
      //Aggressive projections
      wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[0][0]  = 2.7;
      wire_width = wire_pitch[0][0] / 2;
      wire_thickness = aspect_ratio[0][0]  * wire_width;
      wire_spacing = wire_pitch[0][0] - wire_width;
      barrier_thickness = 0;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[0][0] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][0]  = 0.405;
      miller_value[0][0]   = 1.5;
      horiz_dielectric_constant[0][0]  = 2.303;
      vert_dielectric_constant[0][0]   = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][0] , miller_value[0][0] , horiz_dielectric_constant[0][0] , vert_dielectric_constant[0][0] ,
          fringe_cap);

      wire_pitch[0][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[0][1] / 2;
      aspect_ratio[0][1]  = 2.7;
      wire_thickness = aspect_ratio[0][1]  * wire_width;
      wire_spacing = wire_pitch[0][1] - wire_width;
      wire_r_per_micron[0][1] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][1]  = 0.405;
      miller_value[0][1]   = 1.5;
      horiz_dielectric_constant[0][1]  = 2.303;
      vert_dielectric_constant[0][1]   = 3.9;
      wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1],
          vert_dielectric_constant[0][1],
          fringe_cap);

      wire_pitch[0][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[0][2] = 2.8;
      wire_width = wire_pitch[0][2] / 2;
      wire_thickness = aspect_ratio[0][2] * wire_width;
      wire_spacing = wire_pitch[0][2] - wire_width;
      wire_r_per_micron[0][2] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][2] = 0.81;
      miller_value[0][2]   = 1.5;
      horiz_dielectric_constant[0][2]  = 2.303;
      vert_dielectric_constant[0][2]   = 3.9;
      wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
          fringe_cap);

      //Conservative projections
      wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[1][0] = 2.0;
      wire_width = wire_pitch[1][0] / 2;
      wire_thickness = aspect_ratio[1][0] * wire_width;
      wire_spacing = wire_pitch[1][0] - wire_width;
      barrier_thickness = 0.006;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][0] = 0.405;
      miller_value[1][0] = 1.5;
      horiz_dielectric_constant[1][0] = 2.734;
      vert_dielectric_constant[1][0] = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0], vert_dielectric_constant[1][0],
          fringe_cap);

      wire_pitch[1][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[1][1] / 2;
      aspect_ratio[1][1] = 2.0;
      wire_thickness = aspect_ratio[1][1] * wire_width;
      wire_spacing = wire_pitch[1][1] - wire_width;
      wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][1] = 0.405;
      miller_value[1][1] = 1.5;
      horiz_dielectric_constant[1][1] = 2.734;
      vert_dielectric_constant[1][1] = 3.9;
      wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1], vert_dielectric_constant[1][1],
          fringe_cap);

      wire_pitch[1][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[1][2] = 2.2;
      wire_width = wire_pitch[1][2] / 2;
      wire_thickness = aspect_ratio[1][2] * wire_width;
      wire_spacing = wire_pitch[1][2] - wire_width;
      dishing_thickness = 0.1 *  wire_thickness;
      wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][2] = 0.77;
      miller_value[1][2] = 1.5;
      horiz_dielectric_constant[1][2] = 2.734;
      vert_dielectric_constant[1][2] = 3.9;
      wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][2], miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
          fringe_cap);
      //Nominal projections for commodity DRAM wordline/bitline
      wire_pitch[1][3] = 2 * 0.065;
      wire_c_per_micron[1][3] = 52.5e-15 / (256 * 2 * 0.065);
      wire_r_per_micron[1][3] = 12 / 0.065;
    }
    else if (tech == 45)
    {
      //Aggressive projections.
      wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[0][0]  = 3.0;
      wire_width = wire_pitch[0][0] / 2;
      wire_thickness = aspect_ratio[0][0]  * wire_width;
      wire_spacing = wire_pitch[0][0] - wire_width;
      barrier_thickness = 0;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[0][0] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][0]  = 0.315;
      miller_value[0][0]  = 1.5;
      horiz_dielectric_constant[0][0]  = 1.958;
      vert_dielectric_constant[0][0]  = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][0] , miller_value[0][0] , horiz_dielectric_constant[0][0] , vert_dielectric_constant[0][0] ,
          fringe_cap);

      wire_pitch[0][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[0][1] / 2;
      aspect_ratio[0][1]  = 3.0;
      wire_thickness = aspect_ratio[0][1] * wire_width;
      wire_spacing = wire_pitch[0][1] - wire_width;
      wire_r_per_micron[0][1] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][1]  = 0.315;
      miller_value[0][1]  = 1.5;
      horiz_dielectric_constant[0][1]  = 1.958;
      vert_dielectric_constant[0][1]  = 3.9;
      wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1], vert_dielectric_constant[0][1],
          fringe_cap);

      wire_pitch[0][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[0][2] = 3.0;
      wire_width = wire_pitch[0][2] / 2;
      wire_thickness = aspect_ratio[0][2] * wire_width;
      wire_spacing = wire_pitch[0][2] - wire_width;
      wire_r_per_micron[0][2] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][2] = 0.63;
      miller_value[0][2]  = 1.5;
      horiz_dielectric_constant[0][2]  = 1.958;
      vert_dielectric_constant[0][2]  = 3.9;
      wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
          fringe_cap);

      //Conservative projections
      wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[1][0] = 2.0;
      wire_width = wire_pitch[1][0] / 2;
      wire_thickness = aspect_ratio[1][0] * wire_width;
      wire_spacing = wire_pitch[1][0] - wire_width;
      barrier_thickness = 0.004;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][0] = 0.315;
      miller_value[1][0] = 1.5;
      horiz_dielectric_constant[1][0] = 2.46;
      vert_dielectric_constant[1][0] = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0], vert_dielectric_constant[1][0],
          fringe_cap);

      wire_pitch[1][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[1][1] / 2;
      aspect_ratio[1][1] = 2.0;
      wire_thickness = aspect_ratio[1][1] * wire_width;
      wire_spacing = wire_pitch[1][1] - wire_width;
      wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][1] = 0.315;
      miller_value[1][1] = 1.5;
      horiz_dielectric_constant[1][1] = 2.46;
      vert_dielectric_constant[1][1] = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1], vert_dielectric_constant[1][1],
          fringe_cap);

      wire_pitch[1][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[1][2] = 2.2;
      wire_width = wire_pitch[1][2] / 2;
      wire_thickness = aspect_ratio[1][2] * wire_width;
      wire_spacing = wire_pitch[1][2] - wire_width;
      dishing_thickness = 0.1 * wire_thickness;
      wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][2] = 0.55;
      miller_value[1][2] = 1.5;
      horiz_dielectric_constant[1][2] = 2.46;
      vert_dielectric_constant[1][2] = 3.9;
      wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][2], miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
          fringe_cap);
      //Nominal projections for commodity DRAM wordline/bitline
      wire_pitch[1][3] = 2 * 0.045;
      wire_c_per_micron[1][3] = 37.5e-15 / (256 * 2 * 0.045);
      wire_r_per_micron[1][3] = 12 / 0.045;
    }
    else if (tech == 32)
    {
      //Aggressive projections.
      wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[0][0] = 3.0;
      wire_width = wire_pitch[0][0] / 2;
      wire_thickness = aspect_ratio[0][0] * wire_width;
      wire_spacing = wire_pitch[0][0] - wire_width;
      barrier_thickness = 0;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[0][0] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][0] = 0.21;
      miller_value[0][0] = 1.5;
      horiz_dielectric_constant[0][0] = 1.664;
      vert_dielectric_constant[0][0] = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][0], miller_value[0][0], horiz_dielectric_constant[0][0], vert_dielectric_constant[0][0],
          fringe_cap);

      wire_pitch[0][1] = 4 * g_ip->F_sz_um;
      wire_width = wire_pitch[0][1] / 2;
      aspect_ratio[0][1] = 3.0;
      wire_thickness = aspect_ratio[0][1] * wire_width;
      wire_spacing = wire_pitch[0][1] - wire_width;
      wire_r_per_micron[0][1] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][1] = 0.21;
      miller_value[0][1] = 1.5;
      horiz_dielectric_constant[0][1] = 1.664;
      vert_dielectric_constant[0][1] = 3.9;
      wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1], vert_dielectric_constant[0][1],
          fringe_cap);

      wire_pitch[0][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[0][2] = 3.0;
      wire_width = wire_pitch[0][2] / 2;
      wire_thickness = aspect_ratio[0][2] * wire_width;
      wire_spacing = wire_pitch[0][2] - wire_width;
      wire_r_per_micron[0][2] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[0][2] = 0.42;
      miller_value[0][2] = 1.5;
      horiz_dielectric_constant[0][2] = 1.664;
      vert_dielectric_constant[0][2] = 3.9;
      wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
          fringe_cap);

      //Conservative projections
      wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
      aspect_ratio[1][0] = 2.0;
      wire_width = wire_pitch[1][0] / 2;
      wire_thickness = aspect_ratio[1][0] * wire_width;
      wire_spacing = wire_pitch[1][0] - wire_width;
      barrier_thickness = 0.003;
      dishing_thickness = 0;
      alpha_scatter = 1;
      wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][0] = 0.21;
      miller_value[1][0] = 1.5;
      horiz_dielectric_constant[1][0] = 2.214;
      vert_dielectric_constant[1][0] = 3.9;
      fringe_cap = 0.115e-15;
      wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0], vert_dielectric_constant[1][0],
          fringe_cap);

      wire_pitch[1][1] = 4 * g_ip->F_sz_um;
      aspect_ratio[1][1] = 2.0;
      wire_width = wire_pitch[1][1] / 2;
      wire_thickness = aspect_ratio[1][1] * wire_width;
      wire_spacing = wire_pitch[1][1] - wire_width;
      wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][1] = 0.21;
      miller_value[1][1] = 1.5;
      horiz_dielectric_constant[1][1] = 2.214;
      vert_dielectric_constant[1][1] = 3.9;
      wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1], vert_dielectric_constant[1][1],
          fringe_cap);

      wire_pitch[1][2] = 8 * g_ip->F_sz_um;
      aspect_ratio[1][2] = 2.2;
      wire_width = wire_pitch[1][2] / 2;
      wire_thickness = aspect_ratio[1][2] * wire_width;
      wire_spacing = wire_pitch[1][2] - wire_width;
      dishing_thickness = 0.1 *  wire_thickness;
      wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
          wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
      ild_thickness[1][2] = 0.385;
      miller_value[1][2] = 1.5;
      horiz_dielectric_constant[1][2] = 2.214;
      vert_dielectric_constant[1][2] = 3.9;
      wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
          ild_thickness[1][2], miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
          fringe_cap);
      //Nominal projections for commodity DRAM wordline/bitline
      wire_pitch[1][3] = 2 * 0.032;//micron
      wire_c_per_micron[1][3] = 31e-15 / (256 * 2 * 0.032);//F/micron
      wire_r_per_micron[1][3] = 12 / 0.032;//ohm/micron
    }
    else if (tech == 22)
        {
          //Aggressive projections.
          wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;//local
          aspect_ratio[0][0] = 3.0;
          wire_width = wire_pitch[0][0] / 2;
          wire_thickness = aspect_ratio[0][0] * wire_width;
          wire_spacing = wire_pitch[0][0] - wire_width;
          barrier_thickness = 0;
          dishing_thickness = 0;
          alpha_scatter = 1;
          wire_r_per_micron[0][0] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[0][0] = 0.15;
          miller_value[0][0] = 1.5;
          horiz_dielectric_constant[0][0] = 1.414;
          vert_dielectric_constant[0][0] = 3.9;
          fringe_cap = 0.115e-15;
          wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[0][0], miller_value[0][0], horiz_dielectric_constant[0][0], vert_dielectric_constant[0][0],
            fringe_cap);

          wire_pitch[0][1] = 4 * g_ip->F_sz_um;//semi-global
          wire_width = wire_pitch[0][1] / 2;
          aspect_ratio[0][1] = 3.0;
          wire_thickness = aspect_ratio[0][1] * wire_width;
          wire_spacing = wire_pitch[0][1] - wire_width;
          wire_r_per_micron[0][1] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[0][1] = 0.15;
          miller_value[0][1] = 1.5;
          horiz_dielectric_constant[0][1] = 1.414;
          vert_dielectric_constant[0][1] = 3.9;
          wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1], vert_dielectric_constant[0][1],
            fringe_cap);

          wire_pitch[0][2] = 8 * g_ip->F_sz_um;//global
          aspect_ratio[0][2] = 3.0;
          wire_width = wire_pitch[0][2] / 2;
          wire_thickness = aspect_ratio[0][2] * wire_width;
          wire_spacing = wire_pitch[0][2] - wire_width;
          wire_r_per_micron[0][2] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[0][2] = 0.3;
          miller_value[0][2] = 1.5;
          horiz_dielectric_constant[0][2] = 1.414;
          vert_dielectric_constant[0][2] = 3.9;
          wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
        		  ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
        		  fringe_cap);

//          //*************************
//          wire_pitch[0][4] = 16 * g_ip.F_sz_um;//global
//          aspect_ratio = 3.0;
//          wire_width = wire_pitch[0][4] / 2;
//          wire_thickness = aspect_ratio * wire_width;
//          wire_spacing = wire_pitch[0][4] - wire_width;
//          wire_r_per_micron[0][4] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
//        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//          ild_thickness = 0.3;
//          wire_c_per_micron[0][4] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//        		  ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//        		  fringe_cap);
//
//          wire_pitch[0][5] = 24 * g_ip.F_sz_um;//global
//          aspect_ratio = 3.0;
//          wire_width = wire_pitch[0][5] / 2;
//          wire_thickness = aspect_ratio * wire_width;
//          wire_spacing = wire_pitch[0][5] - wire_width;
//          wire_r_per_micron[0][5] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
//        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//          ild_thickness = 0.3;
//          wire_c_per_micron[0][5] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//        		  ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//        		  fringe_cap);
//
//          wire_pitch[0][6] = 32 * g_ip.F_sz_um;//global
//          aspect_ratio = 3.0;
//          wire_width = wire_pitch[0][6] / 2;
//          wire_thickness = aspect_ratio * wire_width;
//          wire_spacing = wire_pitch[0][6] - wire_width;
//          wire_r_per_micron[0][6] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
//        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//          ild_thickness = 0.3;
//          wire_c_per_micron[0][6] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//        		  ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//        		  fringe_cap);
          //*************************

          //Conservative projections
          wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
          aspect_ratio[1][0] = 2.0;
          wire_width = wire_pitch[1][0] / 2;
          wire_thickness = aspect_ratio[1][0] * wire_width;
          wire_spacing = wire_pitch[1][0] - wire_width;
          barrier_thickness = 0.003;
          dishing_thickness = 0;
          alpha_scatter = 1.05;
          wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[1][0] = 0.15;
          miller_value[1][0] = 1.5;
          horiz_dielectric_constant[1][0] = 2.104;
          vert_dielectric_constant[1][0] = 3.9;
          fringe_cap = 0.115e-15;
          wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0], vert_dielectric_constant[1][0],
            fringe_cap);

          wire_pitch[1][1] = 4 * g_ip->F_sz_um;
          wire_width = wire_pitch[1][1] / 2;
          aspect_ratio[1][1] = 2.0;
          wire_thickness = aspect_ratio[1][1] * wire_width;
          wire_spacing = wire_pitch[1][1] - wire_width;
          wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[1][1] = 0.15;
          miller_value[1][1] = 1.5;
          horiz_dielectric_constant[1][1] = 2.104;
          vert_dielectric_constant[1][1] = 3.9;
          wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1], vert_dielectric_constant[1][1],
            fringe_cap);

            wire_pitch[1][2] = 8 * g_ip->F_sz_um;
            aspect_ratio[1][2] = 2.2;
            wire_width = wire_pitch[1][2] / 2;
            wire_thickness = aspect_ratio[1][2] * wire_width;
            wire_spacing = wire_pitch[1][2] - wire_width;
            dishing_thickness = 0.1 *  wire_thickness;
            wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
            ild_thickness[1][2] = 0.275;
            miller_value[1][2] = 1.5;
            horiz_dielectric_constant[1][2] = 2.104;
            vert_dielectric_constant[1][2] = 3.9;
            wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            		ild_thickness[1][2], miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
            		fringe_cap);
            //Nominal projections for commodity DRAM wordline/bitline
            wire_pitch[1][3] = 2 * 0.022;//micron
            wire_c_per_micron[1][3] = 31e-15 / (256 * 2 * 0.022);//F/micron
            wire_r_per_micron[1][3] = 12 / 0.022;//ohm/micron

            //******************
//            wire_pitch[1][4] = 16 * g_ip.F_sz_um;
//            aspect_ratio = 2.2;
//            wire_width = wire_pitch[1][4] / 2;
//            wire_thickness = aspect_ratio * wire_width;
//            wire_spacing = wire_pitch[1][4] - wire_width;
//            dishing_thickness = 0.1 *  wire_thickness;
//            wire_r_per_micron[1][4] = wire_resistance(CU_RESISTIVITY, wire_width,
//            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//            ild_thickness = 0.275;
//            wire_c_per_micron[1][4] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//            		ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//            		fringe_cap);
//
//            wire_pitch[1][5] = 24 * g_ip.F_sz_um;
//            aspect_ratio = 2.2;
//            wire_width = wire_pitch[1][5] / 2;
//            wire_thickness = aspect_ratio * wire_width;
//            wire_spacing = wire_pitch[1][5] - wire_width;
//            dishing_thickness = 0.1 *  wire_thickness;
//            wire_r_per_micron[1][5] = wire_resistance(CU_RESISTIVITY, wire_width,
//            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//            ild_thickness = 0.275;
//            wire_c_per_micron[1][5] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//            		ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//            		fringe_cap);
//
//            wire_pitch[1][6] = 32 * g_ip.F_sz_um;
//            aspect_ratio = 2.2;
//            wire_width = wire_pitch[1][6] / 2;
//            wire_thickness = aspect_ratio * wire_width;
//            wire_spacing = wire_pitch[1][6] - wire_width;
//            dishing_thickness = 0.1 *  wire_thickness;
//            wire_r_per_micron[1][6] = wire_resistance(CU_RESISTIVITY, wire_width,
//            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//            ild_thickness = 0.275;
//            wire_c_per_micron[1][6] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//            		ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//            		fringe_cap);
        }

    else if (tech == 16)
        {
          //Aggressive projections.
          wire_pitch[0][0] = 2.5 * g_ip->F_sz_um;//local
          aspect_ratio[0][0] = 3.0;
          wire_width = wire_pitch[0][0] / 2;
          wire_thickness = aspect_ratio[0][0] * wire_width;
          wire_spacing = wire_pitch[0][0] - wire_width;
          barrier_thickness = 0;
          dishing_thickness = 0;
          alpha_scatter = 1;
          wire_r_per_micron[0][0] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[0][0] = 0.108;
          miller_value[0][0] = 1.5;
          horiz_dielectric_constant[0][0] = 1.202;
          vert_dielectric_constant[0][0] = 3.9;
          fringe_cap = 0.115e-15;
          wire_c_per_micron[0][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[0][0], miller_value[0][0], horiz_dielectric_constant[0][0], vert_dielectric_constant[0][0],
            fringe_cap);

          wire_pitch[0][1] = 4 * g_ip->F_sz_um;//semi-global
          aspect_ratio[0][1] = 3.0;
          wire_width = wire_pitch[0][1] / 2;
          wire_thickness = aspect_ratio[0][1] * wire_width;
          wire_spacing = wire_pitch[0][1] - wire_width;
          wire_r_per_micron[0][1] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[0][1] = 0.108;
          miller_value[0][1] = 1.5;
          horiz_dielectric_constant[0][1] = 1.202;
          vert_dielectric_constant[0][1] = 3.9;
          wire_c_per_micron[0][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[0][1], miller_value[0][1], horiz_dielectric_constant[0][1], vert_dielectric_constant[0][1],
            fringe_cap);

          wire_pitch[0][2] = 8 * g_ip->F_sz_um;//global
          aspect_ratio[0][2] = 3.0;
          wire_width = wire_pitch[0][2] / 2;
          wire_thickness = aspect_ratio[0][2] * wire_width;
          wire_spacing = wire_pitch[0][2] - wire_width;
          wire_r_per_micron[0][2] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[0][2] = 0.216;
          miller_value[0][2] = 1.5;
          horiz_dielectric_constant[0][2] = 1.202;
          vert_dielectric_constant[0][2] = 3.9;
          wire_c_per_micron[0][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
        		  ild_thickness[0][2], miller_value[0][2], horiz_dielectric_constant[0][2], vert_dielectric_constant[0][2],
        		  fringe_cap);

//          //*************************
//          wire_pitch[0][4] = 16 * g_ip.F_sz_um;//global
//          aspect_ratio = 3.0;
//          wire_width = wire_pitch[0][4] / 2;
//          wire_thickness = aspect_ratio * wire_width;
//          wire_spacing = wire_pitch[0][4] - wire_width;
//          wire_r_per_micron[0][4] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
//        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//          ild_thickness = 0.3;
//          wire_c_per_micron[0][4] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//        		  ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//        		  fringe_cap);
//
//          wire_pitch[0][5] = 24 * g_ip.F_sz_um;//global
//          aspect_ratio = 3.0;
//          wire_width = wire_pitch[0][5] / 2;
//          wire_thickness = aspect_ratio * wire_width;
//          wire_spacing = wire_pitch[0][5] - wire_width;
//          wire_r_per_micron[0][5] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
//        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//          ild_thickness = 0.3;
//          wire_c_per_micron[0][5] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//        		  ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//        		  fringe_cap);
//
//          wire_pitch[0][6] = 32 * g_ip.F_sz_um;//global
//          aspect_ratio = 3.0;
//          wire_width = wire_pitch[0][6] / 2;
//          wire_thickness = aspect_ratio * wire_width;
//          wire_spacing = wire_pitch[0][6] - wire_width;
//          wire_r_per_micron[0][6] = wire_resistance(BULK_CU_RESISTIVITY, wire_width,
//        		  wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//          ild_thickness = 0.3;
//          wire_c_per_micron[0][6] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//        		  ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//        		  fringe_cap);
          //*************************

          //Conservative projections
          wire_pitch[1][0] = 2.5 * g_ip->F_sz_um;
          aspect_ratio[1][0] = 2.0;
          wire_width = wire_pitch[1][0] / 2;
          wire_thickness = aspect_ratio[1][0] * wire_width;
          wire_spacing = wire_pitch[1][0] - wire_width;
          barrier_thickness = 0.002;
          dishing_thickness = 0;
          alpha_scatter = 1.05;
          wire_r_per_micron[1][0] = wire_resistance(CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[1][0] = 0.108;
          miller_value[1][0] = 1.5;
          horiz_dielectric_constant[1][0] = 1.998;
          vert_dielectric_constant[1][0] = 3.9;
          fringe_cap = 0.115e-15;
          wire_c_per_micron[1][0] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[1][0], miller_value[1][0], horiz_dielectric_constant[1][0], vert_dielectric_constant[1][0],
            fringe_cap);

          wire_pitch[1][1] = 4 * g_ip->F_sz_um;
          wire_width = wire_pitch[1][1] / 2;
          aspect_ratio[1][1] = 2.0;
          wire_thickness = aspect_ratio[1][1] * wire_width;
          wire_spacing = wire_pitch[1][1] - wire_width;
          wire_r_per_micron[1][1] = wire_resistance(CU_RESISTIVITY, wire_width,
            wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
          ild_thickness[1][1] = 0.108;
          miller_value[1][1] = 1.5;
          horiz_dielectric_constant[1][1] = 1.998;
          vert_dielectric_constant[1][1] = 3.9;
            wire_c_per_micron[1][1] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            ild_thickness[1][1], miller_value[1][1], horiz_dielectric_constant[1][1], vert_dielectric_constant[1][1],
            fringe_cap);

            wire_pitch[1][2] = 8 * g_ip->F_sz_um;
            aspect_ratio[1][2] = 2.2;
            wire_width = wire_pitch[1][2] / 2;
            wire_thickness = aspect_ratio[1][2] * wire_width;
            wire_spacing = wire_pitch[1][2] - wire_width;
            dishing_thickness = 0.1 *  wire_thickness;
            wire_r_per_micron[1][2] = wire_resistance(CU_RESISTIVITY, wire_width,
            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
            ild_thickness[1][2] = 0.198;
            miller_value[1][2] = 1.5;
            horiz_dielectric_constant[1][2] = 1.998;
            vert_dielectric_constant[1][2] = 3.9;
            wire_c_per_micron[1][2] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
            		ild_thickness[1][2], miller_value[1][2], horiz_dielectric_constant[1][2], vert_dielectric_constant[1][2],
            		fringe_cap);
            //Nominal projections for commodity DRAM wordline/bitline
            wire_pitch[1][3] = 2 * 0.016;//micron
            wire_c_per_micron[1][3] = 31e-15 / (256 * 2 * 0.016);//F/micron
            wire_r_per_micron[1][3] = 12 / 0.016;//ohm/micron

            //******************
//            wire_pitch[1][4] = 16 * g_ip.F_sz_um;
//            aspect_ratio = 2.2;
//            wire_width = wire_pitch[1][4] / 2;
//            wire_thickness = aspect_ratio * wire_width;
//            wire_spacing = wire_pitch[1][4] - wire_width;
//            dishing_thickness = 0.1 *  wire_thickness;
//            wire_r_per_micron[1][4] = wire_resistance(CU_RESISTIVITY, wire_width,
//            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//            ild_thickness = 0.275;
//            wire_c_per_micron[1][4] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//            		ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//            		fringe_cap);
//
//            wire_pitch[1][5] = 24 * g_ip.F_sz_um;
//            aspect_ratio = 2.2;
//            wire_width = wire_pitch[1][5] / 2;
//            wire_thickness = aspect_ratio * wire_width;
//            wire_spacing = wire_pitch[1][5] - wire_width;
//            dishing_thickness = 0.1 *  wire_thickness;
//            wire_r_per_micron[1][5] = wire_resistance(CU_RESISTIVITY, wire_width,
//            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//            ild_thickness = 0.275;
//            wire_c_per_micron[1][5] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//            		ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//            		fringe_cap);
//
//            wire_pitch[1][6] = 32 * g_ip.F_sz_um;
//            aspect_ratio = 2.2;
//            wire_width = wire_pitch[1][6] / 2;
//            wire_thickness = aspect_ratio * wire_width;
//            wire_spacing = wire_pitch[1][6] - wire_width;
//            dishing_thickness = 0.1 *  wire_thickness;
//            wire_r_per_micron[1][6] = wire_resistance(CU_RESISTIVITY, wire_width,
//            		wire_thickness, barrier_thickness, dishing_thickness, alpha_scatter);
//            ild_thickness = 0.275;
//            wire_c_per_micron[1][6] = wire_capacitance(wire_width, wire_thickness, wire_spacing,
//            		ild_thickness, miller_value, horiz_dielectric_constant, vert_dielectric_constant,
//            		fringe_cap);
        }
    g_tp.wire_local.pitch    += curr_alpha * wire_pitch[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.R_per_um += curr_alpha * wire_r_per_micron[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.C_per_um += curr_alpha * wire_c_per_micron[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.aspect_ratio  += curr_alpha * aspect_ratio[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.ild_thickness += curr_alpha * ild_thickness[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.miller_value   += curr_alpha * miller_value[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.horiz_dielectric_constant += curr_alpha* horiz_dielectric_constant[g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];
    g_tp.wire_local.vert_dielectric_constant  += curr_alpha* vert_dielectric_constant [g_ip->ic_proj_type][(ram_cell_tech_type == comm_dram)?3:0];

    g_tp.wire_inside_mat.pitch     += curr_alpha * wire_pitch[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.R_per_um  += curr_alpha* wire_r_per_micron[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.C_per_um  += curr_alpha* wire_c_per_micron[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.aspect_ratio  += curr_alpha * aspect_ratio[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.ild_thickness += curr_alpha * ild_thickness[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.miller_value   += curr_alpha * miller_value[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.horiz_dielectric_constant += curr_alpha* horiz_dielectric_constant[g_ip->ic_proj_type][g_ip->wire_is_mat_type];
    g_tp.wire_inside_mat.vert_dielectric_constant  += curr_alpha* vert_dielectric_constant [g_ip->ic_proj_type][g_ip->wire_is_mat_type];

    g_tp.wire_outside_mat.pitch    += curr_alpha * wire_pitch[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.R_per_um += curr_alpha*wire_r_per_micron[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.C_per_um += curr_alpha*wire_c_per_micron[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.aspect_ratio  += curr_alpha * aspect_ratio[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.ild_thickness += curr_alpha * ild_thickness[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.miller_value   += curr_alpha * miller_value[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.horiz_dielectric_constant += curr_alpha* horiz_dielectric_constant[g_ip->ic_proj_type][g_ip->wire_os_mat_type];
    g_tp.wire_outside_mat.vert_dielectric_constant  += curr_alpha* vert_dielectric_constant [g_ip->ic_proj_type][g_ip->wire_os_mat_type];

    g_tp.unit_len_wire_del = g_tp.wire_inside_mat.R_per_um * g_tp.wire_inside_mat.C_per_um / 2;

    g_tp.sense_delay               += curr_alpha *SENSE_AMP_D;
    g_tp.sense_dy_power            += curr_alpha *SENSE_AMP_P;
//    g_tp.horiz_dielectric_constant += horiz_dielectric_constant;
//    g_tp.vert_dielectric_constant  += vert_dielectric_constant;
//    g_tp.aspect_ratio              += aspect_ratio;
//    g_tp.miller_value              += miller_value;
//    g_tp.ild_thickness             += ild_thickness;

  }
  g_tp.fringe_cap = fringe_cap;

  double rd = tr_R_on(g_tp.min_w_nmos_, NCH, 1);
  double p_to_n_sizing_r = pmos_to_nmos_sz_ratio();
  double c_load = gate_C(g_tp.min_w_nmos_ * (1 + p_to_n_sizing_r), 0.0);
  double tf = rd * c_load;
  g_tp.kinv = horowitz(0, tf, 0.5, 0.5, RISE);
  double KLOAD = 1;
  c_load = KLOAD * (drain_C_(g_tp.min_w_nmos_, NCH, 1, 1, g_tp.cell_h_def) +
                    drain_C_(g_tp.min_w_nmos_ * p_to_n_sizing_r, PCH, 1, 1, g_tp.cell_h_def) +
                    gate_C(g_tp.min_w_nmos_ * 4 * (1 + p_to_n_sizing_r), 0.0));
  tf = rd * c_load;
  g_tp.FO4 = horowitz(0, tf, 0.5, 0.5, RISE);
}

