#pragma once
#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TESTBENCH) {
	sc_in<    bool     >		clk;
	sc_out<   bool	   >		rst;
	sc_in<  sc_uint<12>>		addr_w_in;
	sc_out< sc_int<8>  >		w_rdata;
	sc_in<  sc_uint<12>>		addr_ifmpa_in;
	sc_out< sc_int<8>  >		ifmap_rdata;
	sc_in<    bool	   >		layer_done;
	sc_in<  sc_uint<12>>		ofmap_result;

	SC_CTOR(TESTBENCH) {
		SC_CTHREAD(dram_weight, clk.pos());
		SC_CTHREAD(dram_ifmap, clk.pos());
		SC_CTHREAD(results, clk.pos());
	}
	void dram_weight(void);
	void dram_ifmap(void);
	void results(void);
};
