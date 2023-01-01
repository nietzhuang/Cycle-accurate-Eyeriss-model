#pragma once
#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;

#include "Definition.h"
#include "testbench.h"
#include "LowEyeriss.h"

SC_MODULE(SYSTEM) {
	sc_clock						clk;
	sc_signal<    bool	  >			rst;
	sc_signal<    bool	  >			cpu_trigger;
	sc_signal< sc_bv<32>  >			conf_bits;
	sc_signal<    bool    >			conf_done;
	sc_signal< sc_uint<32>>			addr_w_in;
	sc_signal< sc_int<8>  >			w_rdata;
	sc_signal< sc_uint<32>>			addr_ifmpa_in;
	sc_signal< sc_int<8> >			ifmap_rdata;
	sc_signal<    bool	  >			layer_done;
	sc_signal< sc_uint<12>>			ofmap_result;

	TESTBENCH*			tb_i;
	SC_EYERISS*		    sc_eyeriss_i;
	sc_trace_file*		tracefile;

	SC_CTOR(SYSTEM) : clk("clk", 10, SC_NS) {
		//Connect ports
		tb_i = new TESTBENCH("tb_i");
		tb_i->clk(clk);
		tb_i->rst(rst);
		tb_i->cpu_trigger(cpu_trigger);
		tb_i->conf_bits(conf_bits);
		tb_i->conf_done(conf_done);
		tb_i->addr_w_in(addr_w_in);
		tb_i->w_rdata(w_rdata);
		tb_i->addr_ifmpa_in(addr_ifmpa_in);
		tb_i->ifmap_rdata(ifmap_rdata);
		`tb_i->layer_done(layer_done);
		tb_i->ofmap_result(ofmap_result);

		sc_eyeriss_i = new SC_EYERISS("sc_eyeriss_i");
		sc_eyeriss_i->clk(clk);
		sc_eyeriss_i->rst(rst);
		sc_eyeriss_i->cpu_trigger(cpu_trigger);
		sc_eyeriss_i->conf_bits(conf_bits);
		sc_eyeriss_i->conf_done(conf_done);
		sc_eyeriss_i->addr_w_in(addr_w_in);
		sc_eyeriss_i->w_rdata(w_rdata); 
		sc_eyeriss_i->addr_ifmpa_in(addr_ifmpa_in);
		sc_eyeriss_i->ifmap_rdata(ifmap_rdata);
		sc_eyeriss_i->layer_done(layer_done);
		sc_eyeriss_i->ofmap_result(ofmap_result);


		tracefile = sc_create_vcd_trace_file("Low_Eyeriss");
		sc_trace(tracefile, tb_i->clk, "tb_i.clk");
		sc_trace(tracefile, tb_i->rst, "tb_i.rst");
		sc_trace(tracefile, tb_i->cpu_trigger, "tb_i.cpu_trigger");
		sc_trace(tracefile, tb_i->conf_bits, "tb_i.conf_bits");
		sc_trace(tracefile, tb_i->conf_done, "tb_i.conf_done");
		sc_trace(tracefile, tb_i->addr_w_in, "tb_i.addr_w_in");
		sc_trace(tracefile, tb_i->w_rdata, "tb_i.w_rdata");
		sc_trace(tracefile, tb_i->addr_ifmpa_in, "tb_i.addr_ifmpa_in");
		sc_trace(tracefile, tb_i->ifmap_rdata, "tb_i.ifmap_rdata");
		sc_trace(tracefile, tb_i->layer_done, "tb_i.layer_done");
		sc_trace(tracefile, tb_i->ofmap_result, "tb_i.ofmap_result");

		sc_trace(tracefile, sc_eyeriss_i->in_vld, "sc_eyeriss_i.in_vld");	
		sc_trace(tracefile, sc_eyeriss_i->w_wdata[0], "sc_eyeriss_i.w_wdata[0]");
		sc_trace(tracefile, sc_eyeriss_i->w_wdata[1], "sc_eyeriss_i.w_wdata[1]");
		sc_trace(tracefile, sc_eyeriss_i->w_wdata[2], "sc_eyeriss_i.w_wdata[2]");
		sc_trace(tracefile, sc_eyeriss_i->ifmap_wdata[0], "sc_eyeriss_i.ifmap_wdata[0]");
		sc_trace(tracefile, sc_eyeriss_i->ifmap_wdata[1], "sc_eyeriss_i.ifmap_wdata[1]");
		sc_trace(tracefile, sc_eyeriss_i->ifmap_wdata[2], "sc_eyeriss_i.ifmap_wdata[2]");
		//sc_trace(tracefile, sc_eyeriss_i->ifmap_wdata[3], "sc_eyeriss_i.ifmap_wdata[3]");
		//sc_trace(tracefile, sc_eyeriss_i->ifmap_wdata[4], "sc_eyeriss_i.ifmap_wdata[4]");
		sc_trace(tracefile, sc_eyeriss_i->enable, "sc_eyeriss_i.enable");
		sc_trace(tracefile, sc_eyeriss_i->in_vld_PE, "sc_eyeriss_i.in_vld_PE");
		sc_trace(tracefile, sc_eyeriss_i->weight_out_PE[0][0], "sc_eyeriss_i.weight_out_PE[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->ifmap_out_PE[0][0], "sc_eyeriss_i.ifmap_out_PE[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->layer_done, "sc_eyeriss_i.layer_done");
		sc_trace(tracefile, sc_eyeriss_i->ofmap_out[0][0], "sc_eyeriss_i.ofmap_out[0][0]");

		sc_trace(tracefile, sc_eyeriss_i->config_i->cpu_trigger, "sc_eyeriss_i.config_i.cpu_trigger");
		sc_trace(tracefile, sc_eyeriss_i->config_i->conf_bits, "sc_eyeriss_i.config_i.conf_bits");
		sc_trace(tracefile, sc_eyeriss_i->config_i->conf_done, "sc_eyeriss_i.config_i.conf_done");
		sc_trace(tracefile, sc_eyeriss_i->config_i->filter_height_cf, "sc_eyeriss_i.config_i.filter_height_cf");
		sc_trace(tracefile, sc_eyeriss_i->config_i->filter_width_cf, "sc_eyeriss_i.config_i.filter_width_cf");

		// Test trace
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->count_pass, "sc_eyeriss_i.scheduler_i.count_pass");
		//
		sc_trace(tracefile, sc_eyeriss_i->glb_i->w_done, "sc_eyeriss_i.glb_i.w_done");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->addr_w_in, "sc_eyeriss_i.glb_i.addr_w_in");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->w_rdata, "sc_eyeriss_i.glb_i.w_rdata");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->addr_ifmpa_in, "sc_eyeriss_i.glb_i.addr_ifmpa_in");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ifmap_rdata, "sc_eyeriss_i.glb_i.ifmap_rdata");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->in_vld, "sc_eyeriss_i.glb_i.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->w_wdata[0], "sc_eyeriss_i.glb_i.w_wdata[0]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->w_wdata[1], "sc_eyeriss_i.glb_i.w_wdata[1]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->w_wdata[2], "sc_eyeriss_i.glb_i.w_wdata[2]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ifmap_wdata[0], "sc_eyeriss_i.glb_i.ifmap_wdata[0]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ifmap_wdata[1], "sc_eyeriss_i.glb_i.ifmap_wdata[1]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ifmap_wdata[2], "sc_eyeriss_i.glb_i.ifmap_wdata[2]");
		//sc_trace(tracefile, sc_eyeriss_i->glb_i->ifmap_wdata[3], "sc_eyeriss_i.glb_i.ifmap_wdata[3]");
		//sc_trace(tracefile, sc_eyeriss_i->glb_i->ifmap_wdata[4], "sc_eyeriss_i.glb_i.ifmap_wdata[4]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->out_vld[0][0], "sc_eyeriss_i.glb_i.out_vld[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ofmap_out[0][0], "sc_eyeriss_i.glb_i.ofmap_out[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ofmap_out[1][0], "sc_eyeriss_i.glb_i.ofmap_out[1][0]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ofmap_out[2][0], "sc_eyeriss_i.glb_i.ofmap_out[2][0]");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->layer_done, "sc_eyeriss_i.glb_i.layer_done");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ofmap_result, "sc_eyeriss_i.glb_i.ofmap_result");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->channel_done, "sc_eyeriss_i.glb_i.channel_done");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->filter_done, "sc_eyeriss_i.glb_i.filter_done");
		sc_trace(tracefile, sc_eyeriss_i->glb_i->ofmap_channel, "sc_eyeriss_i.glb_i.ofmap_channel");

		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->in_vld, "sc_eyeriss_i.scheduler_i.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->weight[0], "sc_eyeriss_i.scheduler_i.weight[0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->weight[1], "sc_eyeriss_i.scheduler_i.weight[1]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->weight[2], "sc_eyeriss_i.scheduler_i.weight[2]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap[0], "sc_eyeriss_i.scheduler_i.ifmap[0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap[1], "sc_eyeriss_i.scheduler_i.ifmap[1]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap[2], "sc_eyeriss_i.scheduler_i.ifmap[2]");
		//sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap[3], "sc_eyeriss_i.scheduler_i.ifmap[3]");
		//sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap[4], "sc_eyeriss_i.scheduler_i.ifmap[4]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->start_sche, "sc_eyeriss_i.scheduler_i.start_sche");		
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->enable, "sc_eyeriss_i.scheduler_i.enable");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->in_vld_PE, "sc_eyeriss_i.scheduler_i.in_vld_PE");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->weight_out_PE[0][0], "sc_eyeriss_i.scheduler_i.weight_out_PE[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->weight_out_PE[1][0], "sc_eyeriss_i.scheduler_i.weight_out_PE[1][0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->weight_out_PE[2][0], "sc_eyeriss_i.scheduler_i.weight_out_PE[2][0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap_out_PE[0][0], "sc_eyeriss_i.scheduler_i.ifmap_out_PE[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap_out_PE[1][0], "sc_eyeriss_i.scheduler_i.ifmap_out_PE[1][0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap_out_PE[2][0], "sc_eyeriss_i.scheduler_i.ifmap_out_PE[2][0]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap_out_PE[0][1], "sc_eyeriss_i.scheduler_i.ifmap_out_PE[0][1]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap_out_PE[1][1], "sc_eyeriss_i.scheduler_i.ifmap_out_PE[1][1]");
		sc_trace(tracefile, sc_eyeriss_i->scheduler_i->ifmap_out_PE[2][1], "sc_eyeriss_i.scheduler_i.ifmap_out_PE[2][1]");

		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->enable, "sc_eyeriss_i.pe_array_i.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->in_vld_PE, "sc_eyeriss_i.pe_array_i.in_vld_PE");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->weight_out_PE[0][0], "sc_eyeriss_i.pe_array_i.weight_out_PE[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ifmap_out_PE[0][0], "sc_eyeriss_i.pe_array_i.ifmap_out_PE[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ifmap_out_PE[1][0], "sc_eyeriss_i.pe_array_i.ifmap_out_PE[1][0]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ifmap_out_PE[2][0], "sc_eyeriss_i.pe_array_i.ifmap_out_PE[2][0]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ifmap_out_PE[0][1], "sc_eyeriss_i.pe_array_i.ifmap_out_PE[0][1]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ifmap_out_PE[1][1], "sc_eyeriss_i.pe_array_i.ifmap_out_PE[1][1]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ifmap_out_PE[2][1], "sc_eyeriss_i.pe_array_i.ifmap_out_PE[2][1]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->out_vld[0][0], "sc_eyeriss_i.pe_array_i.out_vld[0][0]");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->ofmap_out[0][0], "sc_eyeriss_i.pe_array_i.ofmap_out[0][0]");

		//
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_0->enable, "sc_eyeriss_i.pe_array_i.PE_0_0.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_0->in_vld, "sc_eyeriss_i.pe_array_i.PE_0_0.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_0->out_vld, "sc_eyeriss_i.pe_array_i.PE_0_0.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_0->w_in, "sc_eyeriss_i.pe_array_i.PE_0_0.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_0->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_0_0.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_0->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_0_0.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->enable, "sc_eyeriss_i.pe_array_i.PE_1_0.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->in_vld, "sc_eyeriss_i.pe_array_i.PE_1_0.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->out_vld, "sc_eyeriss_i.pe_array_i.PE_1_0.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->w_in, "sc_eyeriss_i.pe_array_i.PE_1_0.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_1_0.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_1_0.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_2_0->enable, "sc_eyeriss_i.pe_array_i.PE_2_0.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_2_0->in_vld, "sc_eyeriss_i.pe_array_i.PE_2_0.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_2_0->out_vld, "sc_eyeriss_i.pe_array_i.PE_2_0.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_2_0->w_in, "sc_eyeriss_i.pe_array_i.PE_2_0.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_2_0->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_2_0.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_2_0->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_2_0.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_3_0->enable, "sc_eyeriss_i.pe_array_i.PE_3_0.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_3_0->in_vld, "sc_eyeriss_i.pe_array_i.PE_3_0.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_3_0->out_vld, "sc_eyeriss_i.pe_array_i.PE_3_0.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_3_0->w_in, "sc_eyeriss_i.pe_array_i.PE_3_0.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_3_0->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_3_0.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_0->enable, "sc_eyeriss_i.pe_array_i.PE_5_0.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_0->in_vld, "sc_eyeriss_i.pe_array_i.PE_5_0.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_0->out_vld, "sc_eyeriss_i.pe_array_i.PE_5_0.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_0->w_in, "sc_eyeriss_i.pe_array_i.PE_5_0.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_0->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_5_0.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_0->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_5_0.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_1->enable, "sc_eyeriss_i.pe_array_i.PE_5_1.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_1->in_vld, "sc_eyeriss_i.pe_array_i.PE_5_1.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_1->out_vld, "sc_eyeriss_i.pe_array_i.PE_5_1.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_1->w_in, "sc_eyeriss_i.pe_array_i.PE_5_1.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_1->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_5_1.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_5_1->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_5_1.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_6_0->enable, "sc_eyeriss_i.pe_array_i.PE_6_0.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_6_0->in_vld, "sc_eyeriss_i.pe_array_i.PE_6_0.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_6_0->out_vld, "sc_eyeriss_i.pe_array_i.PE_6_0.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_6_0->w_in, "sc_eyeriss_i.pe_array_i.PE_6_0.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_6_0->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_6_0.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_6_0->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_6_0.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_1->enable, "sc_eyeriss_i.pe_array_i.PE_0_1.enable");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_1->in_vld, "sc_eyeriss_i.pe_array_i.PE_0_1.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_1->out_vld, "sc_eyeriss_i.pe_array_i.PE_0_1.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_1->w_in, "sc_eyeriss_i.pe_array_i.PE_0_1.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_1->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_0_1.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_1->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_0_1.ofmap_out");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_2->in_vld, "sc_eyeriss_i.pe_array_i.PE_0_2.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_2->out_vld, "sc_eyeriss_i.pe_array_i.PE_0_2.out_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_2->w_in, "sc_eyeriss_i.pe_array_i.PE_0_2.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_2->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_0_2.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_0_2->ofmap_out, "sc_eyeriss_i.pe_array_i.PE_0_2.ofmap_out");

		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->in_vld, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.in_vld");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->w_in, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.w_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->ifmap_in, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.ifmap_in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->psum_tmp_MACtoReg, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.psum_tmp_MACtoReg");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->w, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.w");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->in, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.in");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->regfile_i->psum_tmp_RegtoMAC, "sc_eyeriss_i.pe_array_i.PE_1_0.regfile_i.psum_tmp_RegtoMAC");

		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->mac_i->weight, "sc_eyeriss_i.pe_array_i.PE_1_0.mac_i.weight");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->mac_i->ifmap, "sc_eyeriss_i.pe_array_i.PE_1_0.mac_i.ifmap");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->mac_i->psum_tmp_RegtoMAC, "sc_eyeriss_i.pe_array_i.PE_1_0.mac_i.psum_tmp_RegtoMAC");
		sc_trace(tracefile, sc_eyeriss_i->pe_array_i->PE_1_0->mac_i->psum_tmp_MACtoReg, "sc_eyeriss_i.pe_array_i.PE_1_0.mac_i.psum_tmp_MACtoReg");
	}
	
	~SYSTEM () {
		delete tb_i;
		delete sc_eyeriss_i;

		sc_close_vcd_trace_file(tracefile);
	}
};


SYSTEM* top = NULL;

int sc_main(int argc, char* argv[]) {
	sc_set_time_resolution(1, SC_PS);
	sc_set_default_time_unit(1, SC_NS);

	//ELABORATION PHASE
	cout << "INFO: Elaborating " << simulation_name << endl;
	top = new SYSTEM("top");

	//SIMULATION PHASE
	cout << "INFO: Simulating " << simulation_name << " with Pattern: " << pattern_name << endl;
	sc_start();

	// Post-proccesing
	cout << "INFO: Post-processing " << simulation_name << endl;

	cout << "INFO: Simulation " << simulation_name
		<< " is Done."
		<< endl;

	return 0;
}
