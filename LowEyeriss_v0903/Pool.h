#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(POOL) {
	//sc_in<	 bool	 >		clk;
	//sc_in<	 bool	 >		rst;
	//sc_in<  sc_bv<4> >		layer_cf;
	sc_in<  sc_int<8>>		ifmap;
	sc_in<  sc_int<8>>		psum_tmp_RegtoPool;
	sc_out<sc_uint<12>>		psum_tmp_PooltoReg;

	SC_CTOR(POOL) {
		SC_METHOD(max);
		sensitive << ifmap
			<< psum_tmp_RegtoPool;
	}
	void max(void);
	//void avg(void);
};