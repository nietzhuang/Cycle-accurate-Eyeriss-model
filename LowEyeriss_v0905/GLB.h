#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(GLB) {
	sc_in<   bool	  >		clk;
	sc_in<   bool	  >		rst;
	sc_in< 	 bool	  >		conf_done;

	// Configuration signals
	sc_in<  sc_bv<4>  >		filter_height_cf;
	sc_in<  sc_bv<4>  >		filter_width_cf;
	sc_in<  sc_bv<10> >		ifmap_height_cf;
	sc_in<  sc_bv<10> >		ifmap_width_cf;
	sc_in<  sc_bv<10> >		num_channel_cf;
	sc_in<  sc_bv<10> >		num_filter_cf;
	sc_in<  sc_bv<13> >		units_cf;
	sc_in<  sc_bv<7>  >		stride_cf;
	sc_in<  sc_bv<3>  >		padding_cf;
	sc_in<  sc_uint<8>>		filter_size_cf;
	sc_in< sc_uint<20>>		ifmap_size_cf;
	sc_in< sc_uint<32>>		ofmap_height_cf;
	sc_in< sc_uint<32>>		ofmap_width_cf;
	sc_in< sc_uint<32>>		ofmap_size_cf;
	sc_in<  sc_bv<2>  >		layer_cf;
	sc_in< sc_uint<32>>		folding_cf;
	sc_in< sc_uint<32>>		propass_cf;

	// Request to DRAM
	sc_out<sc_uint<32>>		addr_w_in;
	sc_in<  sc_int<8> >		w_rdata;
	sc_out<sc_uint<32>>		addr_ifmpa_in;
	sc_in<  sc_int<8> >		ifmap_rdata;

	sc_out<  bool	  >		in_vld;
	sc_out< sc_int<8> >		w_wdata[filter_height];
	sc_out< sc_int<8>>		ifmap_wdata[ifmap_height];
	sc_in<   bool	  >		out_vld[PE_length][PE_width];
	sc_in< sc_uint<12>>		ofmap_out[PE_length][PE_width];

	// Send to testbench
	sc_out<  bool     >		layer_done;
	sc_out<sc_uint<12>>		ofmap_result;
	
	sc_signal<   bool    >	w_done;
	sc_signal<   bool    >	iact_done;
	sc_signal<   bool    >	filter_done;
	sc_signal<   bool    >	channel_done;
	sc_signal<sc_uint<10>>	ofmap_channel;
	sc_signal<  sc_int<8>>*	filter			= new sc_signal<sc_int<8>>[(filter_height * num_filter * units) * (filter_width) * (num_channel)];
	sc_signal< sc_int<8> >*	ifmap			= new sc_signal<sc_int<8>>[(ifmap_height) * (ifmap_width) * (num_channel)];
	sc_signal<sc_uint<12>>*	ofmap_buf		= new sc_signal<sc_uint<12>>[(ofmap_height) * (ofmap_width) * (num_filter)];
	sc_signal<sc_uint<12>>*	psum_test_buf	= new sc_signal<sc_uint<12>>[(ofmap_height*num_filter) * (ofmap_width) * (num_channel)];

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
		SC_CTHREAD(psum_check, clk.pos());
	}
	void GLB_weight_address(void);
	void GLB_ifmap_address(void);
	void GLB_read_weight(void);
	void GLB_read_ifmap(void);
	void GLB_send_weight(void);
	void GLB_send_ifmap(void);
	void GLB_ofmap(void);
	void psum_check(void);
};