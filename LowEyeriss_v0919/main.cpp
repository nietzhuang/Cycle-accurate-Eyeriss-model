#pragma once
#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;

#include "Definition.h"
#include "testbench.h"
#include "LowEyeriss.h"

const char* simulation_name = "Low Eyeriss";

SC_MODULE(SYSTEM) {
	sc_clock						clk;
	sc_signal<    bool	  >			rst;
	sc_signal< sc_uint<12>>			addr_w_in;
	sc_signal< sc_int<8>  >			w_rdata;
	sc_signal< sc_uint<12>>			addr_ifmpa_in;
	sc_signal< sc_int<8>  >			ifmap_rdata;
	sc_signal<    bool	  >			layer_done;
	sc_signal< sc_uint<12>>			ofmap_result;

	TESTBENCH*			tb_i;
	LOW_EYERISS*		low_eyeriss_i;
	sc_trace_file*		tracefile;

	SC_CTOR(SYSTEM) : clk("clk", 10, SC_NS) {
		//Connect ports
		tb_i = new TESTBENCH("tb_i");
		tb_i->clk(clk);
		tb_i->rst(rst);
		tb_i->addr_w_in(addr_w_in);
		tb_i->w_rdata(w_rdata);
		tb_i->addr_ifmpa_in(addr_ifmpa_in);
		tb_i->ifmap_rdata(ifmap_rdata);
		tb_i->layer_done(layer_done);
		tb_i->ofmap_result(ofmap_result);

		low_eyeriss_i = new LOW_EYERISS("low_eyeriss_i");
		low_eyeriss_i->clk(clk);
		low_eyeriss_i->rst(rst);
		low_eyeriss_i->addr_w_in(addr_w_in);
		low_eyeriss_i->w_rdata(w_rdata); 
		low_eyeriss_i->addr_ifmpa_in(addr_ifmpa_in);
		low_eyeriss_i->ifmap_rdata(ifmap_rdata);
		low_eyeriss_i->layer_done(layer_done);
		low_eyeriss_i->ofmap_result(ofmap_result);


		tracefile = sc_create_vcd_trace_file("Low_Eyeriss");
		sc_trace(tracefile, tb_i->clk, "tb_i.clk");
		sc_trace(tracefile, tb_i->rst, "tb_i.rst");
		sc_trace(tracefile, tb_i->addr_w_in, "tb_i.addr_w_in");
		sc_trace(tracefile, tb_i->w_rdata, "tb_i.w_rdata");
		sc_trace(tracefile, tb_i->addr_ifmpa_in, "tb_i.addr_ifmpa_in");
		sc_trace(tracefile, tb_i->ifmap_rdata, "tb_i.ifmap_rdata");
		sc_trace(tracefile, tb_i->layer_done, "tb_i.layer_done");
		sc_trace(tracefile, tb_i->ofmap_result, "tb_i.ofmap_result");

		sc_trace(tracefile, low_eyeriss_i->in_vld, "low_eyeriss_i.in_vld");	
		sc_trace(tracefile, low_eyeriss_i->w_wdata[0], "low_eyeriss_i.w_wdata[0]");
		sc_trace(tracefile, low_eyeriss_i->w_wdata[1], "low_eyeriss_i.w_wdata[1]");
		sc_trace(tracefile, low_eyeriss_i->w_wdata[2], "low_eyeriss_i.w_wdata[2]");
		//sc_trace(tracefile, low_eyeriss_i->w_wdata[3], "low_eyeriss_i.w_wdata[3]");
		//sc_trace(tracefile, low_eyeriss_i->w_wdata[4], "low_eyeriss_i.w_wdata[4]");
		sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[0], "low_eyeriss_i.ifmap_wdata[0]");
		sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[1], "low_eyeriss_i.ifmap_wdata[1]");
		sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[2], "low_eyeriss_i.ifmap_wdata[2]");
		sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[3], "low_eyeriss_i.ifmap_wdata[3]");
		sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[4], "low_eyeriss_i.ifmap_wdata[4]");
		//sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[5], "low_eyeriss_i.ifmap_wdata[5]");
		//sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[6], "low_eyeriss_i.ifmap_wdata[6]");
		//sc_trace(tracefile, low_eyeriss_i->ifmap_wdata[7], "low_eyeriss_i.ifmap_wdata[7]");
		sc_trace(tracefile, low_eyeriss_i->enable, "low_eyeriss_i.enable");
		sc_trace(tracefile, low_eyeriss_i->in_vld_PE, "low_eyeriss_i.in_vld_PE");
		sc_trace(tracefile, low_eyeriss_i->weight_out_PE[0][0], "low_eyeriss_i.weight_out_PE[0][0]");
		sc_trace(tracefile, low_eyeriss_i->ifmap_out_PE[0][0], "low_eyeriss_i.ifmap_out_PE[0][0]");
		sc_trace(tracefile, low_eyeriss_i->layer_done, "low_eyeriss_i.layer_done");
		sc_trace(tracefile, low_eyeriss_i->ofmap_out[0][0], "low_eyeriss_i.ofmap_out[0][0]");

		sc_trace(tracefile, low_eyeriss_i->glb_i->addr_w_in, "low_eyeriss_i.glb_i.addr_w_in");
		sc_trace(tracefile, low_eyeriss_i->glb_i->w_rdata, "low_eyeriss_i.glb_i.w_rdata");
		sc_trace(tracefile, low_eyeriss_i->glb_i->addr_ifmpa_in, "low_eyeriss_i.glb_i.addr_ifmpa_in");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ifmap_rdata, "low_eyeriss_i.glb_i.ifmap_rdata");
		sc_trace(tracefile, low_eyeriss_i->glb_i->in_vld, "low_eyeriss_i.glb_i.in_vld");
		sc_trace(tracefile, low_eyeriss_i->glb_i->w_wdata[0], "low_eyeriss_i.glb_i.w_wdata[0]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->w_wdata[1], "low_eyeriss_i.glb_i.w_wdata[1]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->w_wdata[2], "low_eyeriss_i.glb_i.w_wdata[2]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ifmap_wdata[0], "low_eyeriss_i.glb_i.ifmap_wdata[0]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ifmap_wdata[1], "low_eyeriss_i.glb_i.ifmap_wdata[1]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ifmap_wdata[2], "low_eyeriss_i.glb_i.ifmap_wdata[2]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ifmap_wdata[3], "low_eyeriss_i.glb_i.ifmap_wdata[3]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ifmap_wdata[4], "low_eyeriss_i.glb_i.ifmap_wdata[4]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->out_vld[0][0], "low_eyeriss_i.glb_i.out_vld[0][0]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ofmap_out[0][0], "low_eyeriss_i.glb_i.ofmap_out[0][0]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ofmap_out[1][0], "low_eyeriss_i.glb_i.ofmap_out[1][0]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ofmap_out[2][0], "low_eyeriss_i.glb_i.ofmap_out[2][0]");
		sc_trace(tracefile, low_eyeriss_i->glb_i->layer_done, "low_eyeriss_i.glb_i.layer_done");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ofmap_result, "low_eyeriss_i.glb_i.ofmap_result");
		sc_trace(tracefile, low_eyeriss_i->glb_i->channel_done, "low_eyeriss_i.glb_i.channel_done");
		sc_trace(tracefile, low_eyeriss_i->glb_i->ofmap_channel, "low_eyeriss_i.glb_i.ofmap_channel");

		sc_trace(tracefile, low_eyeriss_i->scheduler_i->in_vld, "low_eyeriss_i.scheduler_i.in_vld");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->weight[0], "low_eyeriss_i.scheduler_i.weight[0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->weight[1], "low_eyeriss_i.scheduler_i.weight[1]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->weight[2], "low_eyeriss_i.scheduler_i.weight[2]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap[0], "low_eyeriss_i.scheduler_i.ifmap[0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap[1], "low_eyeriss_i.scheduler_i.ifmap[1]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap[2], "low_eyeriss_i.scheduler_i.ifmap[2]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap[3], "low_eyeriss_i.scheduler_i.ifmap[3]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap[4], "low_eyeriss_i.scheduler_i.ifmap[4]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->start_sche, "low_eyeriss_i.scheduler_i.start_sche");		
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->enable, "low_eyeriss_i.scheduler_i.enable");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->in_vld_PE, "low_eyeriss_i.scheduler_i.in_vld_PE");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->weight_out_PE[0][0], "low_eyeriss_i.scheduler_i.weight_out_PE[0][0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->weight_out_PE[1][0], "low_eyeriss_i.scheduler_i.weight_out_PE[1][0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->weight_out_PE[2][0], "low_eyeriss_i.scheduler_i.weight_out_PE[2][0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap_out_PE[0][0], "low_eyeriss_i.scheduler_i.ifmap_out_PE[0][0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap_out_PE[1][0], "low_eyeriss_i.scheduler_i.ifmap_out_PE[1][0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap_out_PE[2][0], "low_eyeriss_i.scheduler_i.ifmap_out_PE[2][0]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap_out_PE[0][1], "low_eyeriss_i.scheduler_i.ifmap_out_PE[0][1]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap_out_PE[1][1], "low_eyeriss_i.scheduler_i.ifmap_out_PE[1][1]");
		sc_trace(tracefile, low_eyeriss_i->scheduler_i->ifmap_out_PE[2][1], "low_eyeriss_i.scheduler_i.ifmap_out_PE[2][1]");

		sc_trace(tracefile, low_eyeriss_i->pe_array_i->enable, "low_eyeriss_i.pe_array_i.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->in_vld_PE, "low_eyeriss_i.pe_array_i.in_vld_PE");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->weight_out_PE[0][0], "low_eyeriss_i.pe_array_i.weight_out_PE[0][0]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ifmap_out_PE[0][0], "low_eyeriss_i.pe_array_i.ifmap_out_PE[0][0]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ifmap_out_PE[1][0], "low_eyeriss_i.pe_array_i.ifmap_out_PE[1][0]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ifmap_out_PE[2][0], "low_eyeriss_i.pe_array_i.ifmap_out_PE[2][0]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ifmap_out_PE[0][1], "low_eyeriss_i.pe_array_i.ifmap_out_PE[0][1]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ifmap_out_PE[1][1], "low_eyeriss_i.pe_array_i.ifmap_out_PE[1][1]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ifmap_out_PE[2][1], "low_eyeriss_i.pe_array_i.ifmap_out_PE[2][1]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->out_vld[0][0], "low_eyeriss_i.pe_array_i.out_vld[0][0]");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->ofmap_out[0][0], "low_eyeriss_i.pe_array_i.ofmap_out[0][0]");

		//
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_5_0->enable, "low_eyeriss_i.pe_array_i.PE_5_0.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_5_0->in_vld, "low_eyeriss_i.pe_array_i.PE_5_0.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_5_0->out_vld, "low_eyeriss_i.pe_array_i.PE_5_0.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_5_0->w_in, "low_eyeriss_i.pe_array_i.PE_5_0.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_5_0->ifmap_in, "low_eyeriss_i.pe_array_i.PE_5_0.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_5_0->ofmap_out, "low_eyeriss_i.pe_array_i.PE_5_0.ofmap_out");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_6_0->enable, "low_eyeriss_i.pe_array_i.PE_6_0.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_6_0->in_vld, "low_eyeriss_i.pe_array_i.PE_6_0.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_6_0->out_vld, "low_eyeriss_i.pe_array_i.PE_6_0.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_6_0->w_in, "low_eyeriss_i.pe_array_i.PE_6_0.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_6_0->ifmap_in, "low_eyeriss_i.pe_array_i.PE_6_0.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_6_0->ofmap_out, "low_eyeriss_i.pe_array_i.PE_6_0.ofmap_out");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_9_0->enable, "low_eyeriss_i.pe_array_i.PE_9_0.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_9_0->in_vld, "low_eyeriss_i.pe_array_i.PE_9_0.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_9_0->out_vld, "low_eyeriss_i.pe_array_i.PE_9_0.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_9_0->w_in, "low_eyeriss_i.pe_array_i.PE_9_0.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_9_0->ifmap_in, "low_eyeriss_i.pe_array_i.PE_9_0.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_9_0->ofmap_out, "low_eyeriss_i.pe_array_i.PE_9_0.ofmap_out");
		//
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->enable, "low_eyeriss_i.pe_array_i.PE_1_0.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->in_vld, "low_eyeriss_i.pe_array_i.PE_1_0.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->out_vld, "low_eyeriss_i.pe_array_i.PE_1_0.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->w_in, "low_eyeriss_i.pe_array_i.PE_1_0.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->ifmap_in, "low_eyeriss_i.pe_array_i.PE_1_0.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->ofmap_out, "low_eyeriss_i.pe_array_i.PE_1_0.ofmap_out");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_0->enable, "low_eyeriss_i.pe_array_i.PE_2_0.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_0->in_vld, "low_eyeriss_i.pe_array_i.PE_2_0.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_0->out_vld, "low_eyeriss_i.pe_array_i.PE_2_0.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_0->w_in, "low_eyeriss_i.pe_array_i.PE_2_0.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_0->ifmap_in, "low_eyeriss_i.pe_array_i.PE_2_0.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_0->ofmap_out, "low_eyeriss_i.pe_array_i.PE_2_0.ofmap_out");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_0_1->enable, "low_eyeriss_i.pe_array_i.PE_0_1.enable");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_0_1->in_vld, "low_eyeriss_i.pe_array_i.PE_0_1.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_0_1->out_vld, "low_eyeriss_i.pe_array_i.PE_0_1.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_0_1->w_in, "low_eyeriss_i.pe_array_i.PE_0_1.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_0_1->ifmap_in, "low_eyeriss_i.pe_array_i.PE_0_1.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_0_1->ofmap_out, "low_eyeriss_i.pe_array_i.PE_0_1.ofmap_out");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_1->out_vld, "low_eyeriss_i.pe_array_i.PE_1_1.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_1->w_in, "low_eyeriss_i.pe_array_i.PE_1_1.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_1->ifmap_in, "low_eyeriss_i.pe_array_i.PE_1_1.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_1->ofmap_out, "low_eyeriss_i.pe_array_i.PE_1_1.ofmap_out");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_1->out_vld, "low_eyeriss_i.pe_array_i.PE_2_1.out_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_1->w_in, "low_eyeriss_i.pe_array_i.PE_2_1.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_1->ifmap_in, "low_eyeriss_i.pe_array_i.PE_2_1.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_2_1->ofmap_out, "low_eyeriss_i.pe_array_i.PE_2_1.ofmap_out");

		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->in_vld, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.in_vld");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->w_in, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.w_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->ifmap_in, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.ifmap_in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->psum_tmp_MACtoReg, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.psum_tmp_MACtoReg");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->w, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.w");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->in, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.in");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->regfile_i->psum_tmp_RegtoMAC, "low_eyeriss_i.pe_array_i.PE_1_0.regfile_i.psum_tmp_RegtoMAC");

		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->mac_i->weight, "low_eyeriss_i.pe_array_i.PE_1_0.mac_i.weight");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->mac_i->ifmap, "low_eyeriss_i.pe_array_i.PE_1_0.mac_i.ifmap");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->mac_i->psum_tmp_RegtoMAC, "low_eyeriss_i.pe_array_i.PE_1_0.mac_i.psum_tmp_RegtoMAC");
		sc_trace(tracefile, low_eyeriss_i->pe_array_i->PE_1_0->mac_i->psum_tmp_MACtoReg, "low_eyeriss_i.pe_array_i.PE_1_0.mac_i.psum_tmp_MACtoReg");
	}
	
	~SYSTEM () {
		delete tb_i;
		delete low_eyeriss_i;

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
	cout << "INFO: Simulating " << simulation_name << endl;
	sc_start();

	// Post-proccesing
	cout << "INFO: Post-processing " << simulation_name << endl;

	cout << "INFO: Simulation " << simulation_name
		<< " is Done."
		<< endl;

	return 0;
}