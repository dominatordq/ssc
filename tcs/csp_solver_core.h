/*******************************************************************************************************
*  Copyright 2017 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  (�Alliance�) under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as �System Advisor Model� or �SAM�. Except
*  to comply with the foregoing, the terms �System Advisor Model�, �SAM�, or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/

#ifndef __csp_solver_core_
#define __csp_solver_core_

#include <numeric>
#include <limits>
#include <memory>

#include "lib_weatherfile.h"
#include "csp_solver_util.h"

#include "numeric_solvers.h"

class C_csp_solver_steam_state
{
public:
	double m_temp;	//[K]
	double m_pres;	//[bar]
	double m_enth;	//[kJ/kg]
	double m_x;		//[-]

	C_csp_solver_steam_state()
	{
		m_temp = m_pres = m_enth = m_x = std::numeric_limits<double>::quiet_NaN();
	}
};

class C_csp_solver_htf_1state
{
public:
	double m_temp;	//[C]
	double m_pres;	//[kPa]
	double m_qual;	//[-]
	double m_m_dot;	//[kg/s]

	C_csp_solver_htf_1state()
	{
		m_temp = m_pres = m_qual = m_m_dot = std::numeric_limits<double>::quiet_NaN();
	}
};

class C_csp_solver_htf_state
{
public:
	double m_m_dot;		//[kg/hr]
	double m_temp_in;	//[C]
	double m_pres_in;	//[kPa]
	double m_qual_in;	//[-]
	double m_temp_out;	//[C]
	double m_pres_out;	//[kPa]
	double m_qual_out;	//[-]

	C_csp_solver_htf_state()
	{
		m_m_dot = 
			m_temp_in = m_pres_in = m_qual_in =
			m_temp_out = m_pres_out = m_qual_out = std::numeric_limits<double>::quiet_NaN();
	}
};

struct S_timestep
{
	// Obviously, only need to know 2 out of 3...
	double m_time_start;	//[s] Time at beginning of timestep
	double m_time;			//[s] Time at *end* of timestep
	double m_step;			//[s] Duration of timestep

	S_timestep()
	{
		m_time_start = m_time = m_step = std::numeric_limits<double>::quiet_NaN();
	}
};

class C_timestep_fixed
{
	private:
		S_timestep ms_timestep;

	public:
		void init(double time_start /*s*/, double step /*s*/);
		double get_end_time();
		double get_step();
		void step_forward();

	C_timestep_fixed(){};

	~C_timestep_fixed(){};
};

class C_csp_solver_sim_info
{
public:
	
	S_timestep ms_ts;
	
	//double m_time;		//[s] Time at end of timestep
	//double m_step;		//[s] Duration of timestep

	int m_tou;		//[-] Time-Of-Use Period

	C_csp_solver_sim_info()
	{
		//m_time = m_step = std::numeric_limits<double>::quiet_NaN();

		m_tou = -1;
	}
};

class C_csp_weatherreader
{
private:
	bool m_first;		// flag to indicate whether this is the first call

	// member string for exception messages
	std::string m_error_msg;

	int m_ncall;

	int day_prev;

	bool m_is_wf_init;

public:
	std::shared_ptr<weather_data_provider> m_weather_data_provider;
	weather_header* m_hdr;
	weather_record m_rec;

	C_csp_weatherreader();

	~C_csp_weatherreader(){};

	void init();

	void timestep_call(const C_csp_solver_sim_info &p_sim_info);

	void converged();

    bool read_time_step(int time_step, C_csp_solver_sim_info &p_sim_info);

	// Class to save messages for up stream classes
	C_csp_messages mc_csp_messages;

	struct S_csp_weatherreader_solved_params
	{
		double m_lat;			//[deg]
		double m_lon;			//[deg]
		double m_tz;			//[deg]
		double m_shift;			//[deg]
		double m_elev;			//[m]
        bool m_leapyear;        //true/false

		S_csp_weatherreader_solved_params()
		{
			m_lat = m_lon = m_tz = m_shift = m_elev = std::numeric_limits<double>::quiet_NaN();
            m_leapyear = false;
		}
	};

	struct S_outputs
	{
		int m_year;				//[yr]
		int m_month;			//[mn]
		int m_day;				//[day]
		int m_hour;				//[hr]
		double m_minute;		//[min]

		double m_global;		//[W/m2]
		double m_beam;			//[W/m2]
		double m_hor_beam;		//[W/m2]
		double m_diffuse;		//[W/m2]
		double m_tdry;			//[C]
		double m_twet;			//[C]
		double m_tdew;			//[C]
		double m_wspd;			//[m/s]
		double m_wdir;			//[deg]
		double m_rhum;			//[%]
		double m_pres;			//[mbar]
		double m_snow;			//[cm]
		double m_albedo;		//[-] (0..1)
		double m_aod;		    //[m]

		double m_poa;			//[W/m2]
		double m_solazi;		//[deg]
		double m_solzen;		//[deg]
		double m_lat;			//[deg]
		double m_lon;			//[deg]
		double m_tz;			//[deg]
		double m_shift;			//[deg]
		double m_elev;			//[m]

		double m_time_rise;		//[hr]
		double m_time_set;		//[hr]

		S_outputs()
		{
			m_year = m_month = m_day = m_hour = -1;

			m_global = m_beam = m_hor_beam = m_diffuse = m_tdry = m_twet = m_tdew = m_wspd = 
				m_wdir = m_rhum = m_pres = m_snow = m_albedo =
				m_poa = m_solazi = m_solzen = m_lat = m_lon = m_tz = m_shift = m_elev =
				m_time_rise = m_time_set = m_aod = std::numeric_limits<double>::quiet_NaN();
		}
	};

	// Member data - public so can be set from up stream code
	std::string m_filename;
	int m_trackmode;
	double m_tilt;
	double m_azimuth;

	S_outputs ms_outputs;
	S_csp_weatherreader_solved_params ms_solved_params;

	bool has_error(){ return (m_error_msg.size() > 0); }
	std::string get_error(){ return m_error_msg; }
};

class C_csp_tou
{

public:

    struct S_csp_tou_params
    {
        bool m_isleapyear;
		bool m_dispatch_optimize;
        int m_optimize_frequency;
        int m_disp_steps_per_hour;
        int m_optimize_horizon;
        double m_solver_timeout;
        double m_mip_gap;
        int m_presolve_type;
        int m_bb_type;
        int m_disp_reporting;
        int m_scaling_type;
        int m_max_iterations;
        double m_disp_time_weighting;
        double m_rsu_cost;
        double m_csu_cost;
        double m_q_rec_standby;
        double m_pen_delta_w;
		double m_w_rec_ht;
		std::vector<double> m_w_lim_full;

		//Hybrid 
		bool m_hybrid_optimize;
		double m_batt_slope_coeff;
		double m_batt_int_coeff;
		double m_slope_int_plus;
		double m_slope_int_minus;
		double m_slope_plus;
		double m_slope_minus;
		double m_batt_capacity;
		double m_i_expected;
		double m_batt_charge_lb;
		double m_batt_charge_ub;
		double m_batt_discharge_lb;
		double m_batt_discharge_ub;
		double m_batt_pr_min;
		double m_batt_pr_max;
		double m_batt_res;
		double m_batt_soc_min;
		double m_batt_soc_max;

		double m_w_dc_pl;
		double m_w_ramp_limit;
		double m_alpha_pv;
		double m_beta_pv;

		int m_counter;

		//Hybrid costs
		double m_rec_helio_cost;		//[$/MWht]
		double m_pc_cost;				//[$/MWhe]
		double m_pc_sb_cost;			//[$/MWht]
		double m_pv_field_cost;			//[$/MWhe]
		double m_batt_charge_cost;		//[$/MWhe]
		double m_batt_discharge_cost;	//[$/MWhe]
		double m_batt_lifecycle_cost;	//[$/lifecycle]
		double m_pen_rec_hot_su;		//[$/start]
		double m_pen_pc_hot_su;			//[$/start]

		double m_alpha;

		bool m_is_write_ampl_dat;
        bool m_is_ampl_engine;
        std::string m_ampl_data_dir;
        std::string m_ampl_exec_call;
		
		bool m_is_block_dispatch;

		bool m_use_rule_1;
		double m_standby_off_buffer;

		bool m_use_rule_2;
		double m_q_dot_rec_des_mult;
		double m_f_q_dot_pc_overwrite;


        S_csp_tou_params()
        {
            m_isleapyear = false;
            m_dispatch_optimize = false;        //Do dispatch optimization
            m_optimize_frequency = 24;          //[hr] Optimization occurs every X hours
            m_disp_steps_per_hour = 1;          //[-] Steps per hour for dispatch optimization
            m_optimize_horizon = 48;            //[hr] Optimization time horizon
            m_solver_timeout = 5.;
            m_mip_gap = 0.055;
            m_max_iterations = 10000;
            m_bb_type = -1;
            m_disp_reporting = -1;
            m_presolve_type = -1;
            m_scaling_type = -1;

            m_disp_time_weighting = 0.99;	//
            m_rsu_cost = 5650.;	//
            m_csu_cost = 6520.;		//
            m_pen_delta_w = 0.00064;	//
            m_q_rec_standby = 9.e09;	//
			m_w_rec_ht = 0.0;	//
			m_w_lim_full.resize(8760);
			m_w_lim_full.assign(8760, 9.e99);

			//Hybrid params (may not need to explicitly define values here, but in cmod_tcsmolten_salt)
			m_hybrid_optimize = false;
			m_batt_slope_coeff = 46.36980;
			m_batt_int_coeff = 176.9472;
			m_slope_int_plus = 236.10597;
			m_slope_int_minus = 236.10597;
			m_slope_plus = 0.02928;
			m_slope_minus = 0.02928;
			m_batt_capacity = 749.50817;	//[kAh]
			m_i_expected = 749508.17153;	//[A]
			m_batt_charge_lb = 0;
			m_batt_discharge_lb = 0;
			m_batt_pr_min = 0;
			m_batt_pr_max = 150000;
			m_batt_res = 3.81034e-05;	//[Ohm]
			m_batt_soc_min = 0.2;
			m_batt_soc_max = 0.8;

			m_w_dc_pl = 265055.22;	//[kWac] 
			m_w_ramp_limit = 3;	//[kWe]
			m_alpha_pv = 53.37;	//[kWe]
			m_beta_pv = 0.02743;

			//Hybrid costs
			m_rec_helio_cost = 3;	//[$/MWht]
			m_pc_cost = 2; //[$/MWhe]
			m_pc_sb_cost = 0.8; //[$/MWht]
			m_pv_field_cost = 0.5; //[$/MWhe]
			m_batt_charge_cost = 1; //[$/MWhe]
			m_batt_discharge_cost = 1; //[$/MWhe]
			m_batt_lifecycle_cost = 250; //[$/lifecycle]
			m_pen_rec_hot_su = 10;	//[$/MWt]
			m_pen_pc_hot_su = 40;	//[$/MWe]

			m_alpha = 50;
			m_counter = 0;

			m_is_write_ampl_dat = false;        //write ampl data files?	false originally
            m_is_ampl_engine = false;           //run dispatch with external AMPL engine?
            m_ampl_data_dir = "";               //directory where files should be written 
            m_ampl_exec_call = "";
			
			m_is_block_dispatch = true;			// Either this or m_dispatch_optimize must be true
			
			// Rule 1: if the sun sets (or does not rise) in m_standby_off_buffer [hours], then do not allow power cycle standby
			m_use_rule_1 = false;				
			m_standby_off_buffer = -1.23;		//[hr]


			// Rule 2: If both:
			//   1) Block Dispatch calls for PC OFF
			//   2) Thermal storage charge capacity is less than the product of 'm_q_dot_rec_des_mult' and the receiver design output
			//
			//   THEN: Run power cycle at 'm_f_q_dot_pc_overwrite' until either:
			//   1) the Block Dispatch target fraction calls for PC ON
			//   2) the PC shuts off due to lack of thermal resource
			//   
			m_use_rule_2 = false;
			m_q_dot_rec_des_mult = -1.23;
			m_f_q_dot_pc_overwrite = 1.23;

        };

    } mc_dispatch_params;

	struct S_csp_tou_outputs
	{
        int m_csp_op_tou;
		int m_pricing_tou;
		double m_f_turbine;
		double m_price_mult;

		S_csp_tou_outputs()
		{
            m_csp_op_tou = m_pricing_tou = -1;

			m_f_turbine = m_price_mult = std::numeric_limits<double>::quiet_NaN();
		}
	};

	C_csp_tou(){};

	~C_csp_tou(){};

	void init_parent();

	virtual void init() = 0;

	virtual void call(double time_s, C_csp_tou::S_csp_tou_outputs & tou_outputs) = 0;
};

class C_csp_collector_receiver
{

public:
	
	// Class to save messages for up stream classes
	C_csp_messages mc_csp_messages;

	// Maximum step for collector-receiver model
	double m_max_step;		//[s]

	// Collector-receiver technology type
	bool m_is_sensible_htf;

	C_csp_collector_receiver()
	{
		m_max_step = -1.0;			//[s]
		m_is_sensible_htf = true;	//[-]
	};

	~C_csp_collector_receiver(){};

	enum E_csp_cr_modes
	{
		OFF = 0,
		STARTUP,
		ON,
		STEADY_STATE
	};

	struct S_csp_cr_init_inputs
	{
		double m_latitude;		//[deg]
		double m_longitude;		//[deg]
        double m_tz;            //[hr]
		double m_shift;			//[deg]
        double m_elev;          //[m]

		S_csp_cr_init_inputs()
		{
			m_latitude = m_longitude = m_shift = m_tz = m_elev = std::numeric_limits<double>::quiet_NaN();
		}
	};
	
	struct S_csp_cr_solved_params
	{
		double m_T_htf_cold_des;		//[K]
		double m_P_cold_des;			//[kPa]
		double m_x_cold_des;			//[-]
		double m_q_dot_rec_des;			//[MW]
		double m_A_aper_total;			//[m^2] Total solar field aperture area

		S_csp_cr_solved_params()
		{
			m_T_htf_cold_des = m_P_cold_des = m_x_cold_des =
				m_q_dot_rec_des = m_A_aper_total = std::numeric_limits<double>::quiet_NaN();
		}
	};

	struct S_csp_cr_inputs
	{	
		double m_field_control;			//[-] Defocus signal from controller (can PC and TES accept all receiver output?)
		int m_input_operation_mode;		//[-]
        double m_adjust;                //[-] Field availability / adjustment factor

		S_csp_cr_inputs()
		{
			m_field_control = std::numeric_limits<double>::quiet_NaN();
			m_adjust = std::numeric_limits<double>::quiet_NaN();

			m_input_operation_mode = -1;
		}
	};
	
	struct S_csp_cr_out_solver
	{	
		// Collector receiver outputs that must be defined in the CR call() for the solver to succeed.
			// The controller only checks whether this value is > 0. 
			// If it is <= 0.0, then the controller assumes that startup failed
			// This value is also reported as a modeled timestep output
		double m_q_startup;				//[MWt-hr] 
		double m_time_required_su;		//[s] Time required for receiver to startup
		double m_m_dot_salt_tot;		//[kg/hr] Molten salt mass flow rate
		double m_q_thermal;				//[MWt] 'Available' receiver thermal output
		double m_T_salt_hot;			//[C] Hot HTF from receiver
		double m_component_defocus;		//[-] Defocus applied by component model to stay within mass flow or other constraints
			
		// These are used for the parasitic class call(), so could be zero...
		double m_E_fp_total;			//[MW] Solar field freeze protection power
		double m_W_dot_col_tracking;	//[MWe] Collector tracking power
		double m_W_dot_htf_pump;		//[MWe] HTF pumping power

		// 07/08/2016, GZ: add new variables for DSG LF 
		int m_standby_control;		//[-]
		double m_dP_sf_sh;			//[bar] Pressure drop across the solar field superheater
		double m_h_htf_hot;			//[kJ/kg]
		double m_xb_htf_hot;		//[-]
		double m_P_htf_hot;			//[kPa]
			
		S_csp_cr_out_solver()
		{
			m_q_thermal = m_q_startup = m_m_dot_salt_tot = m_T_salt_hot = m_W_dot_htf_pump =
				m_W_dot_col_tracking = m_time_required_su = m_E_fp_total =
				m_dP_sf_sh = m_h_htf_hot = m_xb_htf_hot = m_P_htf_hot = std::numeric_limits<double>::quiet_NaN();

			m_component_defocus = 1.0;

			m_standby_control = -1;
		}
	};

	virtual void init( const C_csp_collector_receiver::S_csp_cr_init_inputs init_inputs,
		C_csp_collector_receiver::S_csp_cr_solved_params & solved_params) = 0;

	virtual int get_operating_state() = 0;

    virtual double get_startup_time() = 0;
    virtual double get_startup_energy() = 0; //MWh
    virtual double get_pumping_parasitic_coef() = 0;  //MWe/MWt
    virtual double get_min_power_delivery() = 0;    //MWt
	virtual double get_tracking_power() = 0;		//MWe
	virtual double get_col_startup_power() = 0;		//MWe-hr

	virtual void off(const C_csp_weatherreader::S_outputs &weather,
		const C_csp_solver_htf_1state &htf_state_in,
		C_csp_collector_receiver::S_csp_cr_out_solver &cr_out_solver,
		const C_csp_solver_sim_info &sim_info) = 0;

	virtual void startup(const C_csp_weatherreader::S_outputs &weather,
		const C_csp_solver_htf_1state &htf_state_in,
		C_csp_collector_receiver::S_csp_cr_out_solver &cr_out_solver,
		const C_csp_solver_sim_info &sim_info) = 0;

	virtual void on(const C_csp_weatherreader::S_outputs &weather,
		const C_csp_solver_htf_1state &htf_state_in,
		double field_control,
		C_csp_collector_receiver::S_csp_cr_out_solver &cr_out_solver,
		const C_csp_solver_sim_info &sim_info) = 0;

	struct S_csp_cr_est_out
	{
		double m_q_startup_avail;	//[MWt] Estimate startup thermal power. Only > 0 if solar avail AND cr is OFF or Starting Up
		double m_q_dot_avail;		//[MWt] Estimated output if cr is ON and producing useful thermal power
		double m_m_dot_avail;		//[kg/hr] Estimated output mass flow rate if cr is ON and producing useful thermal power
		double m_T_htf_hot;			//[C] Estimated timestep-average outlet temperature

		S_csp_cr_est_out()
		{
			m_q_startup_avail = m_q_dot_avail =
				m_m_dot_avail = m_T_htf_hot = std::numeric_limits<double>::quiet_NaN();
		}
	};

	virtual void estimates(const C_csp_weatherreader::S_outputs &weather,
		const C_csp_solver_htf_1state &htf_state_in,
		C_csp_collector_receiver::S_csp_cr_est_out &est_out,
		const C_csp_solver_sim_info &sim_info) = 0;

	virtual void converged() = 0;

	virtual void write_output_intervals(double report_time_start,
		const std::vector<double> & v_temp_ts_time_end, double report_time_end) = 0;

    virtual double calculate_optical_efficiency( const C_csp_weatherreader::S_outputs &weather, const C_csp_solver_sim_info &sim ) = 0;

    virtual double calculate_thermal_efficiency_approx( const C_csp_weatherreader::S_outputs &weather, double q_incident ) = 0; //very approximate thermal efficiency for optimization projections

    virtual double get_collector_area() = 0;
};


class C_csp_power_cycle
{

public:
	
	// Collector-receiver technology type
	bool m_is_sensible_htf;		//[-] True = indirect, sensible HTF, e.g. molten salt. False = direct steam

	C_csp_power_cycle()
	{
		m_is_sensible_htf = true;
	};

	~C_csp_power_cycle(){};

	enum E_csp_power_cycle_modes
	{
		STARTUP = 0,
		ON,
		STANDBY,
		OFF,
		STARTUP_CONTROLLED
	};

	struct S_control_inputs
	{
		int m_standby_control;		//[-] Control signal indicating standby mode
		double m_m_dot;				//[kg/hr] HTF mass flow rate to power cycle
		//int m_tou;					//[-] Time-of-use period: ONE BASED, converted to 0-based in code

		S_control_inputs()
		{
			m_standby_control = /*m_tou =*/ -1;
		}
	};

	struct S_solved_params
	{
		double m_W_dot_des;			//[MW]
		double m_eta_des;			//[-]
		double m_q_dot_des;			//[MW]
		double m_q_startup;			//[MWt-hr]
		double m_max_frac;			//[-]
		double m_cutoff_frac;		//[-]
		double m_sb_frac;			//[-]
		double m_T_htf_hot_ref;		//[C]
		double m_m_dot_design;		//[kg/hr]
		double m_m_dot_max;			//[kg/hr]
		double m_m_dot_min;			//[kg/hr]

		// The following may not be set for sensible HTF systems
		double m_P_hot_des;			//[kPa]
		double m_x_hot_des;			//[-]

		S_solved_params()
		{
			m_W_dot_des = m_eta_des = m_q_dot_des = m_q_startup = m_max_frac = m_cutoff_frac = 
				m_sb_frac = m_T_htf_hot_ref = 
				m_m_dot_design = m_m_dot_max = m_m_dot_min = std::numeric_limits<double>::quiet_NaN();
		}
	};

	struct S_csp_pc_out_solver
	{
		double m_time_required_su;	//[s] Time required for receiver to startup MIN(controller timestep, calculated time to startup during call)
		double m_P_cycle;			//[MWe] Cycle power output
		double m_T_htf_cold;		//[C] Heat transfer fluid outlet temperature
		double m_q_dot_htf;			//[MWt] Thermal power from HTF (= thermal power into cycle)
		double m_m_dot_htf;			//[kg/hr] Actual HTF flow rate passing through the power cycle

			// Parasitics, plant net power equation
		double m_W_dot_htf_pump;	//[MWe] HTF pumping power
		double m_W_cool_par;		//[MWe] Cooling system parasitic load

		bool m_was_method_successful;	//[-] Return false if method did not solve as expected but can be handled by solver/controller

		S_csp_pc_out_solver()
		{
			m_time_required_su = m_P_cycle = m_T_htf_cold = m_q_dot_htf = m_m_dot_htf =
				m_W_dot_htf_pump = m_W_cool_par = std::numeric_limits<double>::quiet_NaN();

			m_was_method_successful = false;
		}
	};

	virtual void init(C_csp_power_cycle::S_solved_params &solved_params) = 0;

	virtual int get_operating_state() = 0;

    //required gets
    virtual double get_cold_startup_time() = 0;
    virtual double get_warm_startup_time() = 0;
    virtual double get_hot_startup_time() = 0;
    virtual double get_standby_energy_requirement() = 0; //[MW]
    virtual double get_cold_startup_energy() = 0;    //[MWh]
    virtual double get_warm_startup_energy() = 0;    //[MWh]
    virtual double get_hot_startup_energy() = 0;    //[MWh]
    virtual double get_max_thermal_power() = 0;     //MW
    virtual double get_min_thermal_power() = 0;     //MW
	virtual void get_max_power_output_operation_constraints(double T_amb /*C*/, double & m_dot_HTF_ND_max, double & W_dot_ND_max) = 0;	//[-] Normalized over design power
    virtual double get_efficiency_at_TPH(double T_degC, double P_atm, double relhum_pct, double *w_dot_condenser=0) = 0; //-
    virtual double get_efficiency_at_load(double load_frac, double *w_dot_condenser=0) = 0;
	virtual double get_htf_pumping_parasitic_coef() = 0;	//[kWe/kWt]
	
	// This can vary between timesteps for Type224, depending on remaining startup energy and time
	virtual double get_max_q_pc_startup() = 0;		//[MWt]

	virtual void call(const C_csp_weatherreader::S_outputs &weather,
		C_csp_solver_htf_1state &htf_state_in,
		const C_csp_power_cycle::S_control_inputs &inputs,
		C_csp_power_cycle::S_csp_pc_out_solver &out_solver,
		//C_csp_power_cycle::S_csp_pc_out_report &out_report,
		const C_csp_solver_sim_info &sim_info) = 0;

	virtual void converged() = 0;

	virtual void write_output_intervals(double report_time_start,
		const std::vector<double> & v_temp_ts_time_end, double report_time_end) = 0;

	virtual void assign(int index, double *p_reporting_ts_array, size_t n_reporting_ts_array) = 0;

};

class C_csp_tes
{

public:
	C_csp_tes(){};

	~C_csp_tes(){};

	struct S_csp_tes_outputs
	{
		double m_q_heater;			//[MWe]  Heating power required to keep tanks at a minimum temperature
        double m_m_dot;             //[kg/s] Hot tank mass flow rate, valid for direct and indirect systems
		double m_W_dot_rhtf_pump;	//[MWe]  Pumping power for Receiver HTF thru storage
		double m_q_dot_loss;		//[MWt]  Storage thermal losses
		double m_q_dot_dc_to_htf;	//[MWt]  Thermal power to the HTF from storage
		double m_q_dot_ch_from_htf;	//[MWt]  Thermal power from the HTF to storage
		double m_T_hot_ave;		    //[K]    Average hot tank temperature over timestep
		double m_T_cold_ave;	    //[K]    Average cold tank temperature over timestep
		double m_T_hot_final;	    //[K]    Hot tank temperature at end of timestep
		double m_T_cold_final;	    //[K]    Cold tank temperature at end of timestep
	
		S_csp_tes_outputs()
		{
			m_q_heater = m_m_dot = m_W_dot_rhtf_pump = m_q_dot_loss = m_q_dot_dc_to_htf = m_q_dot_ch_from_htf = 
            m_T_hot_ave = m_T_cold_ave = m_T_hot_final = m_T_cold_final = std::numeric_limits<double>::quiet_NaN();
		}
	};

	virtual void init() = 0;

	virtual bool does_tes_exist() = 0;

	virtual double get_hot_temp() = 0;

	virtual double get_cold_temp() = 0;

    virtual double get_initial_charge_energy() = 0; //MWh

    virtual double get_min_charge_energy() = 0; //MWh

    virtual double get_max_charge_energy() = 0; //MWh

    virtual double get_degradation_rate() = 0;  // s^-1

    virtual void discharge_avail_est(double T_cold_K, double step_s, double &q_dot_dc_est, double &m_dot_field_est, double &T_hot_field_est) = 0;
	
	virtual void charge_avail_est(double T_hot_K, double step_s, double &q_dot_ch_est, double &m_dot_field_est, double &T_cold_field_est) = 0;

	virtual bool discharge(double timestep /*s*/, double T_amb /*K*/, double m_dot_htf_in /*kg/s*/, double T_htf_cold_in, double & T_htf_hot_out /*K*/, C_csp_tes::S_csp_tes_outputs &outputs) = 0;
	
	virtual void discharge_full(double timestep /*s*/, double T_amb /*K*/, double T_htf_cold_in, double & T_htf_hot_out /*K*/, double & m_dot_htf_out /*kg/s*/, C_csp_tes::S_csp_tes_outputs &outputs) = 0;

	virtual bool charge(double timestep /*s*/, double T_amb /*K*/, double m_dot_htf_in /*kg/s*/, double T_htf_hot_in, double & T_htf_cold_out /*K*/, C_csp_tes::S_csp_tes_outputs &outputs) = 0;

	virtual void charge_full(double timestep /*s*/, double T_amb /*K*/, double T_htf_hot_in /*K*/, double & T_htf_cold_out /*K*/, double & m_dot_htf_out /*kg/s*/, C_csp_tes::S_csp_tes_outputs &outputs) = 0;
	
	virtual void idle(double timestep, double T_amb, C_csp_tes::S_csp_tes_outputs &outputs) = 0;
	
	virtual void converged() = 0;
};

class C_csp_solver
{

public:
	
	class C_solver_outputs
	{
	public:
		enum
		{
			// Ouputs that are NOT reported as weighted averages
				// Simulation
			TIME_FINAL,       //[hr] Simulation timestep
				// Weather Reader
			MONTH,            //[-] Month of year
			HOUR_DAY,         //[hr] hour of day
				// Controller
			ERR_M_DOT,        //[-] Relative mass conservation error
			ERR_Q_DOT,        //[-] Relative energy conservation error
			N_OP_MODES,       //[-] Number of subtimesteps in reporting timestep
			OP_MODE_1,        //[-] First operating mode in reporting timestep - always should be valid
			OP_MODE_2,        //[-] 2nd operating mode in reporting timestep - not always populated
			OP_MODE_3,        //[-] 3rd operating mode in reporting timestep - usually NOT populated

			// **************************************************************
			//      ONLY instantaneous outputs that are reported as the first value
			//        if multiple csp-timesteps for one reporting timestep
			// **************************************************************
			TOU_PERIOD,                 //[-] CSP operating TOU period
			PRICING_MULT,               //[-] PPA price multiplier
			PC_Q_DOT_SB,                //[MWt] PC required standby thermal power
			PC_Q_DOT_MIN,               //[MWt] PC required min thermal power
			PC_Q_DOT_TARGET,            //[MWt] PC target thermal power
			PC_Q_DOT_MAX,               //[MWt] PC allowable max thermal power
			CTRL_IS_REC_SU,             //[-] Control decision: is receiver startup allowed?
			CTRL_IS_PC_SU,              //[-] Control decision: is power cycle startup allowed?
			CTRL_IS_PC_SB,              //[-] Control decision: is power cycle standby allowed?
			EST_Q_DOT_CR_SU,            //[MWt] Estimate receiver startup thermal power
			EST_Q_DOT_CR_ON,            //[MWt] Estimate receiver thermal power to HTF
			EST_Q_DOT_DC,               //[MWt] Estimate max TES dc thermal power
			EST_Q_DOT_CH,               //[MWt] Estimate max TES ch thermal power
			CTRL_OP_MODE_SEQ_A,         //[-] First 3 operating modes tried
			CTRL_OP_MODE_SEQ_B,         //[-] Next 3 operating modes tried
			CTRL_OP_MODE_SEQ_C,         //[-] Final 3 operating modes tried
			DISPATCH_SOLVE_STATE,       //[-] The status of the dispatch optimization solver
			DISPATCH_SOLVE_ITER,        //[-] Number of iterations before completing dispatch optimization
			DISPATCH_SOLVE_OBJ,         //[?] Objective function value achieved by the dispatch optimization solver
			DISPATCH_SOLVE_OBJ_RELAX,   //[?] Objective function value for the relaxed continuous problem 
			DISPATCH_QSF_EXPECT,        //[MWt] Expected total solar field energy generation in dispatch model
			DISPATCH_QSFPROD_EXPECT,    //[MWt] Expected useful solar field energy generation in dispatch model
			DISPATCH_QSFSU_EXPECT,      //[MWt] Solar field startup energy in dispatch model
			DISPATCH_TES_EXPECT,        //[MWht] Thermal energy storage charge state in dispatch model
			DISPATCH_PCEFF_EXPECT,      //[-] Expected power cycle efficiency adjustment in dispatch model
			DISPATCH_SFEFF_EXPECT,      //[-] Expected solar field thermal efficiency adjustment in dispatch model
			DISPATCH_QPBSU_EXPECT,      //[MWt] Power cycle startup energy consumption in dispatch model
			DISPATCH_WPB_EXPECT,        //[MWe] Power cycle electricity production in dispatch model
			DISPATCH_REV_EXPECT,        //[MWe*fact] Power cycle electricity production times revenue factor in dispatch model
			DISPATCH_PRES_NCONSTR,      //[-] Number of constraint relationships in dispatch model formulation
			DISPATCH_PRES_NVAR,         //[-] Number of variables in dispatch model formulation
			DISPATCH_SOLVE_TIME,        //[sec]   Time required to solve the dispatch model at each instance

			// **************************************************************
			//      Outputs that are reported as weighted averages if 
			//       multiple csp-timesteps for one reporting timestep
			// **************************************************************
				// Weather Reader			
			SOLZEN,           //[deg] Solar zenith angle
			SOLAZ,            //[deg] Solar azimuth angle
			BEAM,			  //[W/m^2] Resource beam normal irradiance
			TDRY,             //[C] Dry bulb temperature
			TWET,             //[C] Wet bulb temperature
			RH,	              //[-] Relative humidity
			WSPD,             //[m/s] Wind speed
			PRES,             //[mbar] Atmospheric pressure
				// Controller and Storage
			CR_DEFOCUS,       //[-] Field optical focus fraction
			TES_Q_DOT_LOSS,       //[MWt] TES thermal losses
			TES_W_DOT_HEATER,     //[MWe] TES freeze protection power
			TES_T_HOT,            //[C] TES final hot tank temperature
			TES_T_COLD,           //[C] TES final cold tank temperature
			TES_Q_DOT_DC,         //[MWt] TES discharge thermal power
			TES_Q_DOT_CH,         //[MWt] TES charge thermal power
			TES_E_CH_STATE,       //[MWht] TES charge state at the end of the time step
			TES_M_DOT_DC,         //[MWt] TES discharge mass flow rate
			TES_M_DOT_CH,         //[MWt] TES charge mass flow rate
			COL_W_DOT_TRACK,      //[MWe] Parasitic collector tracking, startup, stow power consumption
			CR_W_DOT_PUMP,        //[MWe] Parasitic tower HTF pump power
			SYS_W_DOT_PUMP,       //[MWe] Parasitic PC and TES HTF pump power
			PC_W_DOT_COOLING,     //[MWe] Parasitic condenser operation power
			SYS_W_DOT_FIXED,      //[MWe] Parasitic fixed power consumption
			SYS_W_DOT_BOP,        //[MWe] Parasitic BOP power consumption
			W_DOT_NET             //[MWe] System total electric power to grid
		};
	};
	
	C_csp_reported_outputs mc_reported_outputs;

	struct S_sim_setup
	{
		double m_sim_time_start;	//[s]
		double m_sim_time_end;		//[s]
		double m_report_step;		//[s]

		S_sim_setup()
		{
			m_sim_time_start = m_sim_time_end = m_report_step = std::numeric_limits<double>::quiet_NaN();
		}
	};

	class C_csp_solver_kernel
	{
	private:
		S_sim_setup ms_sim_setup;

		C_timestep_fixed mc_ts_weatherfile;

		C_timestep_fixed mc_ts_sim_baseline;

	public:
			
		C_csp_solver_sim_info mc_sim_info;

		void init(C_csp_solver::S_sim_setup & sim_setup, double wf_step /*s*/, 
			double baseline_step /*s*/, C_csp_messages & csp_messages);

		void wf_step_forward();

		void baseline_step_forward();

		double get_wf_end_time();
		double get_wf_step();

		double get_baseline_end_time();
		double get_baseline_step();		

		const S_sim_setup * get_sim_setup();

	};
	
	struct S_csp_system_params
	{
		double m_pb_fixed_par;		//[MWe/MWcap]
		
		double m_bop_par;			//[MWe/MWcap]
		double m_bop_par_f;			//[-]
		double m_bop_par_0;			//[-]
		double m_bop_par_1;			//[-]
		double m_bop_par_2;			//[-]

		S_csp_system_params()
		{
			m_pb_fixed_par =

			m_bop_par = m_bop_par_f = m_bop_par_0 = m_bop_par_1 = m_bop_par_2 = std::numeric_limits<double>::quiet_NaN();
		}
	};

private:
	C_csp_weatherreader &mc_weather;
	C_csp_collector_receiver &mc_collector_receiver;
	C_csp_power_cycle &mc_power_cycle;
	C_csp_tes &mc_tes;
	C_csp_tou &mc_tou;

	S_csp_system_params & ms_system_params;

	C_csp_solver_htf_1state mc_cr_htf_state_in;
	C_csp_collector_receiver::S_csp_cr_out_solver mc_cr_out_solver;

	C_csp_solver_htf_1state mc_pc_htf_state_in;
	C_csp_power_cycle::S_control_inputs mc_pc_inputs;
	C_csp_power_cycle::S_csp_pc_out_solver mc_pc_out_solver;

	C_csp_solver_htf_state mc_tes_ch_htf_state;
	C_csp_solver_htf_state mc_tes_dc_htf_state;
	C_csp_tes::S_csp_tes_outputs mc_tes_outputs;

    C_csp_tou::S_csp_tou_outputs mc_tou_outputs;

	C_csp_solver::C_csp_solver_kernel mc_kernel;

	// Hierarchy logic
	bool m_is_CR_SU__PC_OFF__TES_OFF__AUX_OFF_avail;
	bool m_is_CR_ON__PC_SB__TES_OFF__AUX_OFF_avail;
	bool m_is_CR_ON__PC_SU__TES_OFF__AUX_OFF_avail;
	bool m_is_CR_ON__PC_OFF__TES_CH__AUX_OFF_avail;
	bool m_is_CR_OFF__PC_SU__TES_DC__AUX_OFF_avail;
	bool m_is_CR_DF__PC_MAX__TES_OFF__AUX_OFF_avail;
	
	bool m_is_CR_ON__PC_RM_HI__TES_OFF__AUX_OFF_avail_HI_SIDE;
	bool m_is_CR_ON__PC_RM_HI__TES_OFF__AUX_OFF_avail_LO_SIDE;

	bool m_is_CR_ON__PC_RM_LO__TES_OFF__AUX_OFF_avail;

	bool m_is_CR_ON__PC_TARGET__TES_CH__AUX_OFF_avail_HI_SIDE;
	bool m_is_CR_ON__PC_TARGET__TES_CH__AUX_OFF_avail_LO_SIDE;

	bool m_is_CR_ON__PC_TARGET__TES_DC__AUX_OFF_avail;
	bool m_is_CR_ON__PC_RM_LO__TES_EMPTY__AUX_OFF_avail;

	bool m_is_CR_DF__PC_OFF__TES_FULL__AUX_OFF_avail;

	bool m_is_CR_OFF__PC_SB__TES_DC__AUX_OFF_avail;
	bool m_is_CR_OFF__PC_MIN__TES_EMPTY__AUX_OFF_avail;
	bool m_is_CR_OFF__PC_RM_LO__TES_EMPTY__AUX_OFF_avail;

	bool m_is_CR_ON__PC_SB__TES_CH__AUX_OFF_avail;
	bool m_is_CR_SU__PC_MIN__TES_EMPTY__AUX_OFF_avail;
	bool m_is_CR_SU__PC_SB__TES_DC__AUX_OFF_avail;
	bool m_is_CR_ON__PC_SB__TES_DC__AUX_OFF_avail;

	bool m_is_CR_OFF__PC_TARGET__TES_DC__AUX_OFF_avail;
	bool m_is_CR_SU__PC_TARGET__TES_DC__AUX_OFF_avail;
	bool m_is_CR_ON__PC_RM_HI__TES_FULL__AUX_OFF_avail;

	bool m_is_CR_ON__PC_MIN__TES_EMPTY__AUX_OFF_avail;

	bool m_is_CR_SU__PC_RM_LO__TES_EMPTY__AUX_OFF_avail;

	bool m_is_CR_DF__PC_MAX__TES_FULL__AUX_OFF_avail;

	bool m_is_CR_ON__PC_SB__TES_FULL__AUX_OFF_avail;

	bool m_is_CR_SU__PC_SU__TES_DC__AUX_OFF_avail;

	bool m_is_CR_ON__PC_SU__TES_CH__AUX_OFF_avail;

	bool m_is_CR_DF__PC_SU__TES_FULL__AUX_OFF_avail;

	bool m_is_CR_DF__PC_SU__TES_OFF__AUX_OFF_avail;

	// member string for exception messages
	std::string error_msg;

		// Collector receiver design parameters
	double m_T_htf_cold_des;			//[K]
	double m_P_cold_des;				//[kPa]
	double m_x_cold_des;				//[-]
	double m_q_dot_rec_des;				//[MW]
	double m_A_aperture;				//[m2]

		// Power cycle design parameters
	double m_cycle_W_dot_des;			//[MW]
	double m_cycle_eta_des;				//[-]
	double m_cycle_q_dot_des;			//[MW]
	double m_cycle_max_frac;			//[-]
	double m_cycle_cutoff_frac;			//[-]
	double m_cycle_sb_frac_des;			//[-]
	double m_cycle_T_htf_hot_des;		//[K]
	double m_cycle_P_hot_des;			//[kPa]
	double m_cycle_x_hot_des;			//[-]
	double m_m_dot_pc_des;				//[kg/hr]
	double m_m_dot_pc_min;				//[kg/hr]
	double m_m_dot_pc_max;				//[kg/hr]

		// Storage logic
	bool m_is_tes;			//[-] True: plant has storage

		// Reporting and Output Tracking
	int m_i_reporting;					//[-]
	double m_report_time_start;			//[s]
	double m_report_time_end;			//[s]
	double m_report_step;				//[s]

		// Estimates to use
	double m_T_htf_pc_cold_est;			//[C]

		// Reset hierarchy logic
	void reset_hierarchy_logic();
	void turn_off_plant();

	void solver_cr_to_pc_to_cr(int pc_mode, double field_control_in, double tol, int &exit_mode, double &exit_tolerance);
	 
	void solver_pc_su_controlled__tes_dc(double step_tol /*s*/,
		double &time_pc_su /*s*/, 
		int & exit_mode, double &T_pc_in_exit_tolerance);

	int solver_pc_fixed__tes_empty(double q_dot_pc_fixed /*MWt*/,
		double tol,
		double & time_tes_dc);

	int solver_cr_on__pc_match__tes_full(int pc_mode, double defocus_in);

	enum E_solver_outcomes
	{
		CSP_NO_SOLUTION,	// Models did not provide enough information with which to iterate on T_rec_in
		POOR_CONVERGENCE,	// Models solved, but convergence on T_rec_in was not within specified tolerance
		CSP_CONVERGED,		// Models solved; convergence within specified tolerance
		KNOW_NEXT_MODE,		// Models did not solve, but failure mode allowed next mode to be determined
		UNDER_TARGET_PC,	// Models solved, but could not converge because the operating mode did not allow enough thermal power to go to power cycle
		OVER_TARGET_PC,		// Models solved, but could not converge because the operating mode could not reduce the mass flow rate enough to the power cycle
		REC_IS_OFF			// Collector-receiver model did not produce power
	};

	// Solved Controller Variables
	double m_defocus;		//[-] (1..0) Should only be less than 1 if receiver is on, but defocused
	
	std::vector<double> mv_time_local;

	bool(*mpf_callback)(std::string &log_msg, std::string &progress_msg, void *data, double progress, int log_type);
	void *mp_cmod_active;

	void send_callback(double percent);

public:

	// Class to save messages for up stream classes
	C_csp_messages mc_csp_messages;

	// Vector to track operating modes
	std::vector<int> m_op_mode_tracking;

	enum tech_operating_modes
	{
		ENTRY_MODE = 0,
		
		CR_OFF__PC_OFF__TES_OFF__AUX_OFF,
		CR_SU__PC_OFF__TES_OFF__AUX_OFF,
		CR_ON__PC_SU__TES_OFF__AUX_OFF,
		CR_ON__PC_SB__TES_OFF__AUX_OFF,
		
		CR_ON__PC_RM_HI__TES_OFF__AUX_OFF,
		CR_ON__PC_RM_LO__TES_OFF__AUX_OFF,
		
		CR_DF__PC_MAX__TES_OFF__AUX_OFF,

		CR_OFF__PC_SU__TES_DC__AUX_OFF,
		CR_ON__PC_OFF__TES_CH__AUX_OFF,

		SKIP_10,

		CR_ON__PC_TARGET__TES_CH__AUX_OFF,
		CR_ON__PC_TARGET__TES_DC__AUX_OFF,

		CR_ON__PC_RM_LO__TES_EMPTY__AUX_OFF,

		CR_DF__PC_OFF__TES_FULL__AUX_OFF,
		
		CR_OFF__PC_SB__TES_DC__AUX_OFF,
		CR_OFF__PC_MIN__TES_EMPTY__AUX_OFF,
		CR_OFF__PC_RM_LO__TES_EMPTY__AUX_OFF,

		CR_ON__PC_SB__TES_CH__AUX_OFF,
		CR_SU__PC_MIN__TES_EMPTY__AUX_OFF,

		SKIP_20,

		CR_SU__PC_SB__TES_DC__AUX_OFF,
		CR_ON__PC_SB__TES_DC__AUX_OFF,
		CR_OFF__PC_TARGET__TES_DC__AUX_OFF,
		CR_SU__PC_TARGET__TES_DC__AUX_OFF,
		CR_ON__PC_RM_HI__TES_FULL__AUX_OFF,

		CR_ON__PC_MIN__TES_EMPTY__AUX_OFF,

		CR_SU__PC_RM_LO__TES_EMPTY__AUX_OFF,

		CR_DF__PC_MAX__TES_FULL__AUX_OFF,

		CR_ON__PC_SB__TES_FULL__AUX_OFF,

		SKIP_30,

		CR_SU__PC_SU__TES_DC__AUX_OFF,

		CR_ON__PC_SU__TES_CH__AUX_OFF,

		CR_DF__PC_SU__TES_FULL__AUX_OFF,

		CR_DF__PC_SU__TES_OFF__AUX_OFF
	};

    static std::string tech_operating_modes_str[];
    
	C_csp_solver(C_csp_weatherreader &weather,
		C_csp_collector_receiver &collector_receiver,
		C_csp_power_cycle &power_cycle,
		C_csp_tes &tes,
		C_csp_tou &tou,
		S_csp_system_params &system,
		bool(*pf_callback)(std::string &log_msg, std::string &progress_msg, void *data, double progress, int out_type) = 0,
		void *p_cmod_active = 0);

	~C_csp_solver(){};

	void init();

	void Ssimulate(C_csp_solver::S_sim_setup & sim_setup);

	int steps_per_hour();

	double get_cr_aperture_area();

	// Output vectors
	// Need to be sure these are always up-to-date as multiple operating modes are tested during one timestep
	std::vector< std::vector< double > > mvv_outputs_temp;

	// *****************************
	// *****************************
	// Solvers
	class C_MEQ_cr_on__pc_q_dot_max__tes_off__defocus : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;				//[-]
		double m_q_dot_max;			//[MWt]

	public:
		C_MEQ_cr_on__pc_q_dot_max__tes_off__defocus(C_csp_solver *pc_csp_solver, int pc_mode /*-*/, double q_dot_max /*MWt*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_q_dot_max = q_dot_max;			
		}

		virtual int operator()(double defocus /*-*/, double *q_dot_pc /*MWt*/);
	};

	class C_mono_eq_cr_to_pc_to_cr : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;					//[-]
		double m_P_field_in;			//[kPa]
		double m_x_field_in;			//[-]
		double m_field_control_in;		//[-]

	public:
		C_mono_eq_cr_to_pc_to_cr(C_csp_solver *pc_csp_solver, int pc_mode /*-*/,
			double P_field_in /*kPa*/, double x_field_in /*-*/, double field_control_in /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_P_field_in = P_field_in;
			m_x_field_in = x_field_in;
			m_field_control_in = field_control_in;
		}
		
		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_pc_su_cont_tes_dc : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;

	public:
		C_mono_eq_pc_su_cont_tes_dc(C_csp_solver *pc_csp_solver)
		{
			mpc_csp_solver = pc_csp_solver;
			m_time_pc_su = std::numeric_limits<double>::quiet_NaN();
		}

		double m_time_pc_su;		//[s] power cycle model returns MIN(time required to completely startup, full timestep duration)

		virtual int operator()(double T_htf_hot /*C*/, double *diff_T_htf_hot /*-*/);
	};

	class C_mono_eq_pc_target_tes_dc__m_dot : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_T_htf_cold;		//[C]
		int m_pc_mode;				//[-]

	public:
		C_mono_eq_pc_target_tes_dc__m_dot(C_csp_solver *pc_csp_solver, 
				int pc_mode /*-*/, double T_htf_cold /*C*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_T_htf_cold = T_htf_cold;
			m_pc_mode = pc_mode;
		}

		virtual int operator()(double m_dot_htf /*kg/hr*/, double *q_dot_pc /*MWt*/);
	};

	class C_mono_eq_pc_target_tes_dc__T_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;			//[-]
		double m_q_dot_target;		//[MWt]

	public:
		C_mono_eq_pc_target_tes_dc__T_cold(C_csp_solver *pc_csp_solver, 
				int pc_mode /*-*/, double q_dot_target /*MWt*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;	//[-]
			m_q_dot_target = q_dot_target;		//[MWt]
			m_q_dot_calc = std::numeric_limits<double>::quiet_NaN();	//[MWt]
			m_m_dot_calc = std::numeric_limits<double>::quiet_NaN();	//[kg/hr]
		}

		double m_q_dot_calc;		//[MWt]
		double m_m_dot_calc;		//[kg/hr]

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_pc_match_tes_empty : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;

	public:
		C_mono_eq_pc_match_tes_empty(C_csp_solver *pc_csp_solver)
		{
			mpc_csp_solver = pc_csp_solver;
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_cr_on_pc_su_tes_ch : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;

	public:
		C_mono_eq_cr_on_pc_su_tes_ch(C_csp_solver *pc_csp_solver)
		{
			mpc_csp_solver = pc_csp_solver;
			m_step_pc_su = std::numeric_limits<double>::quiet_NaN();
		}

		double m_step_pc_su;	//[s]

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_pc_target__m_dot : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;		//[-]
		double m_T_htf_hot;	//[C]

	public:
		C_mono_eq_pc_target__m_dot(C_csp_solver *pc_csp_solver, int pc_mode, double T_htf_hot /*C*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_T_htf_hot = T_htf_hot;	//[C]
		}

		virtual int operator()(double m_dot_htf_pc /*kg/hr*/, double *q_dot_pc /*MWt*/);
	};

	class C_mono_eq_cr_on_pc_target_tes_ch__T_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;			//[-]
		double m_q_dot_target;		//[MWt]
		double m_defocus;			//[-]

	public:
		C_mono_eq_cr_on_pc_target_tes_ch__T_cold(C_csp_solver *pc_csp_solver, 
									int pc_mode, double q_dot_target /*MWt*/, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;				//[-]
			m_q_dot_target = q_dot_target;		//[MWt]
			m_defocus = defocus;				//[-]
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_cr_on_pc_match_tes_empty : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_defocus;		//[-]
		
	public:
		C_mono_eq_cr_on_pc_match_tes_empty(C_csp_solver *pc_csp_solver, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_defocus = defocus;	//[-]
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_cr_on_pc_target_tes_dc : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;			//[-]
		double m_defocus;		//[-]
		double m_q_dot_target;	//[MWt]

	public:
		C_mono_eq_cr_on_pc_target_tes_dc(C_csp_solver *pc_csp_solver,
			int pc_mode, double q_dot_target /*MWt*/, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;				//[-]
			m_defocus = defocus;				//[-]
			m_q_dot_target = q_dot_target;		//[MWt]
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_pc_target__m_dot_fixed_plus_tes_dc : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;			//[-]
		double m_T_htf_cold;	//[C]
		double m_T_htf_fixed_hot;	//[C]
		double m_m_dot_htf_fixed;	//[kg/hr]

	public:
		C_mono_eq_pc_target__m_dot_fixed_plus_tes_dc(C_csp_solver *pc_csp_solver,
						int pc_mode, double T_htf_cold /*C*/,
						double T_htf_fixed_hot /*C*/, double m_dot_htf_fixed /*kg/hr*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_T_htf_cold = T_htf_cold;				//[C]
			m_T_htf_fixed_hot = T_htf_fixed_hot;	//[C]
			m_m_dot_htf_fixed = m_dot_htf_fixed;	//[kg/hr]
		}

		virtual int operator()(double m_dot_tes_dc /*kg/hr*/, double *q_dot_pc);
	};

	class C_mono_eq_pc_target_tes_empty__x_step : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_T_htf_cold;			//[C]

	public:
		C_mono_eq_pc_target_tes_empty__x_step(C_csp_solver *pc_csp_solver,
									double T_htf_cold /*C*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_T_htf_cold = T_htf_cold;				//[C]
		}

		virtual int operator()(double step /*s*/, double *q_dot_pc /*MWt*/);
	};

	class C_mono_eq_pc_target_tes_empty__T_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_q_dot_pc_target;		//[MWt]

	public:
		C_mono_eq_pc_target_tes_empty__T_cold(C_csp_solver *pc_csp_solver,
								double q_dot_pc_target /*MWt*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_q_dot_pc_target = q_dot_pc_target;	//[MWt]
			m_step = std::numeric_limits<double>::quiet_NaN();
		}

		double m_step;		//[s]

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);

		void solve_pc(double step /*s*/);
	};

	class C_mono_eq_cr_on__pc_match__tes_full : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;					//[-]
		double m_defocus;				//[-]

	public:
		C_mono_eq_cr_on__pc_match__tes_full(C_csp_solver *pc_csp_solver,
										int pc_mode, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_defocus = defocus;
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_cr_on__pc_max_m_dot__tes_full : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;					//[-]
		double m_defocus;				//[-]

	public:
		C_mono_eq_cr_on__pc_max_m_dot__tes_full(C_csp_solver *pc_csp_solver,
									int pc_mode, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_defocus = defocus;
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_mono_eq_cr_on__pc_target__tes_full__defocus : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;			//[-]
		double m_q_dot_max;		//[MWt]

	public:
		C_mono_eq_cr_on__pc_target__tes_full__defocus(C_csp_solver *pc_csp_solver,
			int pc_mode, double q_dot_max /*MWt*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_q_dot_max = q_dot_max;		//[MWt]
		}

		virtual int operator()(double defocus /*-*/, double *q_dot_pc /*MWt*/);
	};

	class C_mono_eq_cr_on__pc_m_dot_max__tes_full_defocus : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;		//[-]

	public:
		C_mono_eq_cr_on__pc_m_dot_max__tes_full_defocus(C_csp_solver *pc_csp_solver,
			int pc_mode)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
		}

		virtual int operator()(double defocus /*-*/, double *m_dot_bal /*-*/);
	};

	class C_mono_eq_cr_on__pc_match_m_dot_ceil__tes_full : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;					//[-]
		double m_defocus;				//[-]

	public:
		C_mono_eq_cr_on__pc_match_m_dot_ceil__tes_full(C_csp_solver *pc_csp_solver,
			int pc_mode, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_defocus = defocus;
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_MEQ_cr_on__pc_m_dot_max__tes_off__defocus : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;		//[-]

	public:
		C_MEQ_cr_on__pc_m_dot_max__tes_off__defocus(C_csp_solver *pc_csp_solver,
			int pc_mode)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
		}

		virtual int operator()(double defocus /*-*/, double *m_dot_bal /*-*/);
	};

	class C_MEQ_cr_on__pc_max_m_dot__tes_off__T_htf_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		int m_pc_mode;					//[-]
		double m_defocus;				//[-]

	public:
		C_MEQ_cr_on__pc_max_m_dot__tes_off__T_htf_cold(C_csp_solver *pc_csp_solver,
			int pc_mode, double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_pc_mode = pc_mode;
			m_defocus = defocus;
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_MEQ_cr_on__pc_off__tes_ch__T_htf_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_defocus;

	public:
		C_MEQ_cr_on__pc_off__tes_ch__T_htf_cold(C_csp_solver *pc_csp_solver,
							double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_defocus = defocus;				//[-]
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_MEQ_cr_on__pc_target__tes_empty__T_htf_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_defocus;
		double m_q_dot_pc_target;	//[MWt]

	public:
		C_MEQ_cr_on__pc_target__tes_empty__T_htf_cold(C_csp_solver *pc_csp_solver,
			double defocus /*-*/, double q_dot_pc_target /*MWt*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_defocus = defocus;
			m_q_dot_pc_target = q_dot_pc_target;
			m_step = std::numeric_limits<double>::quiet_NaN();
		}

		double m_step;	//[s]

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);

		void solve_pc(double step /*s*/, double T_htf_pc_hot /*C*/, double m_dot_htf_pc /*kg/hr*/);
	};

	class C_MEQ_cr_on__pc_target__tes_empty__step : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_defocus;				//[-]
		double m_T_htf_cold;			//[C]

	public:
		C_MEQ_cr_on__pc_target__tes_empty__step(C_csp_solver *pc_csp_solver,
			double defocus /*-*/, double T_htf_cold /*C*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_defocus = defocus;
			m_T_htf_cold = T_htf_cold;				//[C]
			m_m_dot_pc = std::numeric_limits<double>::quiet_NaN();		//[kg/hr]
			m_T_htf_pc_hot = std::numeric_limits<double>::quiet_NaN();	//[MWt]
		}

		double m_m_dot_pc;			//[kg/hr]
		double m_T_htf_pc_hot;		//[C]

		virtual int operator()(double step /*s*/, double *q_dot_pc /*MWt*/);
	};

	class C_MEQ_cr_df__pc_off__tes_full__T_cold : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;
		double m_defocus;		//[-]

	public:
		C_MEQ_cr_df__pc_off__tes_full__T_cold(C_csp_solver *pc_csp_solver,
			double defocus /*-*/)
		{
			mpc_csp_solver = pc_csp_solver;
			m_defocus = defocus;
		}

		virtual int operator()(double T_htf_cold /*C*/, double *diff_T_htf_cold /*-*/);
	};

	class C_MEQ_cr_df__pc_off__tes_full__defocus : public C_monotonic_equation
	{
	private:
		C_csp_solver *mpc_csp_solver;

	public:
		C_MEQ_cr_df__pc_off__tes_full__defocus(C_csp_solver *pc_csp_solver)
		{
			mpc_csp_solver = pc_csp_solver;
		}

		virtual int operator()(double defocus /*-*/, double *diff_m_dot /*-*/);
	};

};


#endif //__csp_solver_core_
