#pragma once
#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h" 
#include "ConfigReg.h"
#include "GLB.h"
#include "Scheduler.h"
#include "PE_array.h"

SC_MODULE(LOW_EYERISS) {
	sc_in<      bool 	>		clk;
	sc_in<      bool    >		rst;
	sc_in<      bool    >		cpu_trigger;
	sc_in<   sc_bv<32>  >		conf_bits;
	sc_out<		bool    >		conf_done;
	sc_out<  sc_uint<32>>		addr_w_in;
	sc_in<   sc_int<8>  >		w_rdata;
	sc_out<  sc_uint<32>>		addr_ifmpa_in;
	sc_in<   sc_int<8>  >		ifmap_rdata;
	sc_out<     bool 	>		layer_done;
	sc_out<  sc_uint<12>>		ofmap_result;

	sc_signal< sc_bv<4> >		filter_height_cf;
	sc_signal< sc_bv<4> >		filter_width_cf;
	sc_signal< sc_bv<10>>		ifmap_height_cf;
	sc_signal< sc_bv<10>>		ifmap_width_cf;
	sc_signal< sc_bv<10>>		num_channel_cf;
	sc_signal< sc_bv<10>>		num_filter_cf;
	sc_signal< sc_bv<13>>		units_cf;
	sc_signal< sc_bv<7> >		stride_cf;
	sc_signal< sc_bv<3> >		padding_cf;
	sc_signal< sc_bv<2> >		dataflow_cf;
	sc_signal< sc_bv<2> >		layer_cf;
	sc_signal<sc_int<32>>		filter_size_cf;
	sc_signal<sc_int<32>>		ifmap_size_cf;
	sc_signal<sc_int<32>>		ofmap_height_cf;
	sc_signal<sc_int<32>>		ofmap_width_cf;
	sc_signal<sc_int<32>>		ofmap_size_cf;
	sc_signal<sc_int<32>>		folding_cf;
	sc_signal<sc_int<32>>		propass_cf;
	sc_signal<  bool    >		in_vld;
	sc_signal< sc_int<8>>		w_wdata[filter_height];
	sc_signal< sc_int<8>>		ifmap_wdata[ifmap_height];
	sc_signal<  bool    >		enable;
	sc_signal<  bool    >		in_vld_PE;
	sc_signal< sc_int<8>>		weight_out_PE[PE_length][PE_width];
	sc_signal< sc_int<8>>		ifmap_out_PE[PE_length][PE_width];
	sc_signal<  bool    >		out_vld[PE_length][PE_width];
	sc_signal<sc_uint<12>>		ofmap_out[PE_length][PE_width];	

	CONFIGREG*			config_i;
	GLB*				glb_i;
	SCHEDULER*			scheduler_i;
	PE_ARRAY*			pe_array_i;

	SC_CTOR(LOW_EYERISS) {
		//Connect ports
		config_i = new CONFIGREG("config_i");
		config_i->clk(clk);
		config_i->rst(rst);
		config_i->cpu_trigger(cpu_trigger);
		config_i->conf_bits(conf_bits);
		config_i->conf_done(conf_done);
		config_i->layer_done(layer_done);
		config_i->filter_height_cf(filter_height_cf);
		config_i->filter_width_cf(filter_width_cf);
		config_i->ifmap_height_cf(ifmap_height_cf);
		config_i->ifmap_width_cf(ifmap_width_cf);
		config_i->num_channel_cf(num_channel_cf);
		config_i->num_filter_cf(num_filter_cf);
		config_i->units_cf(units_cf);
		config_i->stride_cf(stride_cf);
		config_i->padding_cf(padding_cf);
		config_i->dataflow_cf(dataflow_cf);
		config_i->layer_cf(layer_cf);
		config_i->filter_size_cf(filter_size_cf);
		config_i->ifmap_size_cf(ifmap_size_cf);
		config_i->ofmap_height_cf(ofmap_height_cf);
		config_i->ofmap_width_cf(ofmap_width_cf);
		config_i->ofmap_size_cf(ofmap_size_cf);
		config_i->folding_cf(folding_cf);
		config_i->propass_cf(propass_cf);

		glb_i = new GLB("glb_i");
		glb_i->clk(clk);
		glb_i->rst(rst);
		glb_i->conf_done(conf_done);
		glb_i->filter_height_cf(filter_height_cf);
		glb_i->filter_width_cf(filter_width_cf);
		glb_i->ifmap_height_cf(ifmap_height_cf);
		glb_i->ifmap_width_cf(ifmap_width_cf);
		glb_i->num_channel_cf(num_channel_cf);
		glb_i->num_filter_cf(num_filter_cf);
		glb_i->units_cf(units_cf);
		glb_i->stride_cf(stride_cf);
		glb_i->padding_cf(padding_cf);
		glb_i->filter_size_cf(filter_size_cf);
		glb_i->ifmap_size_cf(ifmap_size_cf);
		glb_i->ofmap_height_cf(ofmap_height_cf);
		glb_i->ofmap_width_cf(ofmap_width_cf);
		glb_i->ofmap_size_cf(ofmap_size_cf);
		glb_i->layer_cf(layer_cf);
		glb_i->folding_cf(folding_cf);
		glb_i->propass_cf(propass_cf);
		glb_i->addr_w_in(addr_w_in);
		glb_i->w_rdata(w_rdata);
		glb_i->addr_ifmpa_in(addr_ifmpa_in);
		glb_i->ifmap_rdata(ifmap_rdata);
		glb_i->in_vld(in_vld);
		for (int i = 0; i < filter_height; i++) {
			glb_i->w_wdata[i](w_wdata[i]);
		}
		for (int i = 0; i < ifmap_height; i++) {
			glb_i->ifmap_wdata[i](ifmap_wdata[i]);
		}
		for (int i = 0; i < PE_width; i++) {
			for (int j = 0; j < PE_length; j++) {
				glb_i->out_vld[j][i](out_vld[j][i]);
				glb_i->ofmap_out[j][i](ofmap_out[j][i]);
			}
		}
		glb_i->layer_done(layer_done);
		glb_i->ofmap_result(ofmap_result);

		scheduler_i = new SCHEDULER("scheduler_i");
		scheduler_i->clk(clk);
		scheduler_i->rst(rst);
		scheduler_i->filter_height_cf(filter_height_cf);
		scheduler_i->filter_width_cf(filter_width_cf);
		scheduler_i->ifmap_height_cf(ifmap_height_cf);
		scheduler_i->ifmap_width_cf(ifmap_width_cf);
		//scheduler_i->num_channel_cf(num_channel_cf);
		//scheduler_i->num_filter_cf(num_filter_cf);
		scheduler_i->units_cf(units_cf);
		scheduler_i->stride_cf(stride_cf);
		scheduler_i->padding_cf(padding_cf);
		//scheduler_i->filter_size_cf(filter_size_cf);
		//scheduler_i->ifmap_size_cf(ifmap_size_cf);
		scheduler_i->ofmap_height_cf(ofmap_height_cf);
		scheduler_i->ofmap_width_cf(ofmap_width_cf);
		//scheduler_i->ofmap_size_cf(ofmap_size_cf);
		scheduler_i->dataflow_cf(dataflow_cf);
		scheduler_i->layer_cf(layer_cf);
		scheduler_i->folding_cf(folding_cf);
		scheduler_i->propass_cf(propass_cf);
		scheduler_i->in_vld(in_vld);
		for (int i = 0; i < filter_height; i++) {
			scheduler_i->weight[i](w_wdata[i]);
		}
		for (int i = 0; i < ifmap_height; i++) {
			scheduler_i->ifmap[i](ifmap_wdata[i]);
		}
		scheduler_i->enable(enable);
		scheduler_i->in_vld_PE(in_vld_PE);
		for (int i = 0; i < PE_width; i++) {
			for (int j = 0; j < PE_length; j++) {
				scheduler_i->weight_out_PE[j][i](weight_out_PE[j][i]);
				scheduler_i->ifmap_out_PE[j][i](ifmap_out_PE[j][i]);
			}
		}

		pe_array_i = new PE_ARRAY("pe_array_i");
		pe_array_i->clk(clk);
		pe_array_i->rst(rst);
		pe_array_i->filter_width_cf(filter_width_cf);
		pe_array_i->dataflow_cf(dataflow_cf);
		pe_array_i->layer_cf(layer_cf);
		pe_array_i->enable(enable);
		pe_array_i->in_vld_PE(in_vld_PE);
		for (int i = 0; i < PE_width; i++) {
			for (int j = 0; j < PE_length; j++) {
				pe_array_i->weight_out_PE[j][i](weight_out_PE[j][i]);
				pe_array_i->ifmap_out_PE[j][i](ifmap_out_PE[j][i]);
				pe_array_i->out_vld[j][i](out_vld[j][i]);
				pe_array_i->ofmap_out[j][i](ofmap_out[j][i]);
			}
		}
	}
};