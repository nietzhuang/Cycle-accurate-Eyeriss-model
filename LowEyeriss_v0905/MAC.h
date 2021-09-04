#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(MAC) {
	sc_in<    bool	  >		clk;
	sc_in<    bool	  >		rst;
	sc_in<  sc_int<8> >		weight;
	sc_in<  sc_int<8> >		ifmap;
	sc_in<  sc_int<8> >		psum_tmp_RegtoMAC;
	sc_out<sc_uint<12>>		psum_tmp_MACtoReg;

	SC_CTOR(MAC) {
		SC_METHOD(mac);
		sensitive << weight
			<< ifmap
			<< psum_tmp_RegtoMAC;
	}
	void mac(void);
};