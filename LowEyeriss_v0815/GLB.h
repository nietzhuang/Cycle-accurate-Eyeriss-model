#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(GLB) {
	sc_in<  bool	>		clk;
	sc_in<  bool	>		rst;

	// Request to DRAM
	sc_out<sc_uint<16>>		addr_w_in;
	sc_in<  sc_int<8> >		w_rdata;
	sc_out<sc_uint<16>>		addr_ifmpa_in;
	sc_in<  sc_int<8> >		ifmap_rdata;

	sc_out<  bool	  >		in_vld;
	sc_out< sc_int<8> >		w_wdata[filter_height];
	sc_out< sc_int<8> >		ifmap_wdata[ifmap_height];
	sc_in<   bool	  >		out_vld[PE_length][PE_width];
	sc_in<sc_uint<12> >		ofmap_out[PE_length][PE_width];

	// Send to testbench
	sc_out<    bool   >		layer_done;
	sc_out<sc_uint<12>>		ofmap_result;
	
	sc_signal<   bool    >	w_done;
	sc_signal<   bool    >	iact_done;
	sc_signal<   bool    >	channel_done;
	sc_signal< sc_int<8> >	ofmap_channel;
	sc_signal< sc_int<8> >	filter[filter_height][filter_width][num_channel];
	sc_signal< sc_int<8> >	ifmap[ifmap_height][ifmap_width][num_channel];
	sc_signal<sc_uint<12>>	ofmap_buf[ofmap_height][ofmap_width];
	sc_signal<sc_uint<12>>	psum_test_buf[ofmap_height][ofmap_width][num_channel];

	SC_CTOR(GLB) {
		SC_CTHREAD(GLB_weight_address, clk.pos());
		SC_CTHREAD(GLB_ifmap_address, clk.pos());
		SC_CTHREAD(GLB_read_weight, clk.pos());
		reset_signal_is(rst, true);
		SC_CTHREAD(GLB_read_ifmap, clk.pos());
		reset_signal_is(rst, true);
		SC_CTHREAD(GLB_send_weight, clk.pos());
		SC_CTHREAD(GLB_send_ifmap, clk.pos());
		SC_CTHREAD(GLB_ofmap, clk.pos());
		SC_CTHREAD(ofmap_check, clk.pos());
	}
	void GLB_weight_address(void);
	void GLB_ifmap_address(void);
	void GLB_read_weight(void);
	void GLB_read_ifmap(void);
	void GLB_send_weight(void);
	void GLB_send_ifmap(void);
	void GLB_ofmap(void);
	void ofmap_check(void);
};