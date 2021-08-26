#pragma once
#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(CONFIGREG) {
	sc_in <   bool		>		clk;
	sc_in <   bool		>		rst;
	sc_in <	  bool		>		cpu_trigger;
	sc_in<  sc_bv<32>   >		conf_bits;
	sc_out<	  bool		>		conf_done;
	sc_in<	  bool		>		layer_done;
	//sc_out< sc_bv<64>	>		configReg;
	sc_out< sc_bv<4>	>		filter_height_cf;
	sc_out< sc_bv<4>	>		filter_width_cf;
	sc_out< sc_bv<10>	>		ifmap_height_cf;
	sc_out< sc_bv<10>	>		ifmap_width_cf;
	sc_out< sc_bv<10>	>		num_channel_cf;
	sc_out< sc_bv<10>	>		num_filter_cf;
	sc_out< sc_bv<3>	>		stride_cf;
	sc_out< sc_bv<3>	>		padding_cf;
	sc_out< sc_bv<2>	>		dataflow_cf;
	sc_out< sc_bv<2>	>		layer_cf;
	sc_out< sc_int<32>  >		filter_size_cf;
	sc_out< sc_int<32>  >		ifmap_size_cf;
	sc_out< sc_int<32>  >		ofmap_height_cf;
	sc_out< sc_int<32>  >		ofmap_width_cf;
	sc_out< sc_int<32>  >		ofmap_size_cf;
	sc_out< sc_int<32>  >		folding_cf;
	sc_out< sc_int<32>  >		propass_cf;

	SC_CTOR(CONFIGREG) {
		SC_CTHREAD(config, clk.pos());
			reset_signal_is(rst, true);
	}
	void config(void);
};

