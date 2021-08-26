#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "RegFile.h"
#include "MAC.h"

SC_MODULE(PE) {
	sc_in<	bool		>		clk;
	sc_in<	bool		>		rst;
	sc_in<  sc_bv<4>	>		filter_width_cf;
	sc_in<	bool		>		enable;
	sc_in<	bool		>		in_vld;
	sc_out<	bool		>		out_vld;
	sc_in<	sc_int<8>	>		w_in;
	sc_in<	sc_int<8>	>		ifmap_in;
	sc_out< sc_uint<12> >		ofmap_out;  // input from MAC to PE, and output ouside.  

	sc_signal<sc_int<8>>		weight;
	sc_signal<sc_int<8>>		ifmap;
	sc_signal<sc_int<8>>		psum;

	RegFile*	regfile_i;
	MAC*		mac_i;

	SC_CTOR(PE) {
		// Connect ports
		regfile_i = new RegFile("regfile_i");
		regfile_i->clk(clk);
		regfile_i->rst(rst);
		regfile_i->filter_width_cf(filter_width_cf);
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
		mac_i->psum_tmp_MACtoReg(ofmap_out);

		SC_CTHREAD(pe_ctrl, clk.pos());
	}
	void pe_ctrl(void);
};