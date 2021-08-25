#pragma once
#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(CONFIGREG) {
	sc_in <   bool		>		clk;
	sc_in <   bool		>		rst;
	sc_in <	  bool		>		cpu_trigger;
	sc_in<  sc_bv<32>  >		conf_bits;
	sc_out<	  bool		>		conf_done;
	
	//sc_signal<sc_uint<64>>		configuration_reg;

	SC_CTOR(CONFIGREG) {
		SC_CTHREAD(config, clk.pos());
			reset_signal_is(rst, true);
	}
	void config(void);
};

