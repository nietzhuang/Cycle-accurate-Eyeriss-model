#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "RegFile.h"
#include "MAC.h"
#include "Pool.h"
#include "MUX.h"

SC_MODULE(PE) {
	sc_in<	  bool		 >		clk;
	sc_in<	  bool		 >		rst;
	sc_in<    sc_bv<4>	 >		filter_width_cf;
	sc_in<    sc_bv<2>	 >		dataflow_cf;
	sc_in<    sc_bv<2>	 >		layer_cf;
	sc_in<	  bool		 >		enable;
	sc_in<	  bool		 >		in_vld;
	sc_out<	  bool		 >		out_vld;
	sc_in<	  sc_int<8>	 >		w_in;
	sc_in<	  sc_int<8>	 >		ifmap_in;
	sc_out<   sc_uint<12>>		ofmap_out;  // input from MAC to PE, and output ouside.  

	sc_signal< sc_int<8> >		weight;
	sc_signal< sc_int<8> >		ifmap;
	sc_signal< sc_int<8> >		psum;
	sc_signal<sc_uint<12>>		ofmap_mac;
	sc_signal<sc_uint<12>>		ofmap_pool;

	RegFile*		regfile_i;
	MAC*			mac_i;
	POOL*			pool_i;
	MUX*			mux_i;

	SC_CTOR(PE) {
		// Connect ports
		regfile_i = new RegFile("regfile_i");
		regfile_i->clk(clk);
		regfile_i->rst(rst);
		regfile_i->filter_width_cf(filter_width_cf);
		regfile_i->dataflow_cf(dataflow_cf);
		regfile_i->layer_cf(layer_cf);
		regfile_i->in_vld(in_vld);
		regfile_i->w_in(w_in);
		regfile_i->ifmap_in(ifmap_in);
		regfile_i->psum_tmp_MACtoReg(ofmap_out);
		regfile_i->w(weight);
		regfile_i->in(ifmap);
		regfile_i->psum_tmp_RegtoMAC(psum);

		mac_i = new MAC("mac_i");
		mac_i->clk(clk);
		mac_i->rst(rst);
		mac_i->weight(weight);
		mac_i->ifmap(ifmap);
		mac_i->psum_tmp_RegtoMAC(psum);
		mac_i->psum_tmp_MACtoReg(ofmap_mac);

		pool_i = new POOL("pool_i");
		pool_i->ifmap(ifmap);
		pool_i->psum_tmp_RegtoPool(psum);
		pool_i->psum_tmp_PooltoReg(ofmap_pool);

		mux_i = new MUX("mux_i");
		mux_i->a(ofmap_mac);
		mux_i->b(ofmap_pool);
		mux_i->sel(layer_cf);
		mux_i->y(ofmap_out);

		SC_CTHREAD(pe_ctrl, clk.pos());
	}
	void pe_ctrl(void);
};