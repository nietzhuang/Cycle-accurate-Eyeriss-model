#pragma once
#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(MUX) {
	sc_in< sc_uint<12>>		a;
	sc_in< sc_uint<12>>		b;
	sc_in< sc_bv<2>   >		sel;
	sc_out<sc_uint<12>>		y;
	
	SC_CTOR(MUX) {
		SC_METHOD(mux);
		sensitive << a
			<< b
			<< sel;
	}
	void mux(void);
};