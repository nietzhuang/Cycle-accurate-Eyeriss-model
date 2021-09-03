#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h" 
#include "PE.h"

void PE::pe_ctrl(void) {
	int cnt_weight = -(spad_depth-1);

	//enum_Dataflow Dataflow = RS;
	while (true) {
		if (enable.read()) {
			int filter_width_tmp = filter_width_cf.read().to_uint();
			int dataflow_tmp = dataflow_cf.read().to_uint();

			switch (dataflow_tmp) {
			case OS:
				cerr << "Output Stationary is not supported." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			case RS:
				if (cnt_weight == filter_width_tmp - 1) {
					cnt_weight = 0;
					out_vld.write(1);
				}
				else {
					cnt_weight += 1;
					out_vld.write(0);
				}
				break;
			default:
				cerr << "Error: Dataflow has not configured." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			}
		}
		else {
			cnt_weight = -(spad_depth - 1);
			out_vld.write(0);
		}

		wait();
	}
}