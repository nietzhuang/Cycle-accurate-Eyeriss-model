#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(RegFile) {
	sc_in<   bool	 >		clk;
	sc_in<   bool	 >		rst;
	sc_in<  sc_bv<4>  >		filter_width_cf;
	sc_in<   bool	 >		in_vld;
	sc_in< sc_int<8> >		w_in;
	sc_in< sc_int<8> >		ifmap_in;
	sc_in<sc_uint<12>>		psum_tmp_MACtoReg;
	sc_out< sc_int<8>>		w;
	sc_out< sc_int<8>>		in;
	sc_out< sc_int<8>>		psum_tmp_RegtoMAC;

	SC_CTOR(RegFile) {
		SC_CTHREAD(RegStream, clk.pos());
		reset_signal_is(rst, true);
	}
	void RegStream(void);
};
