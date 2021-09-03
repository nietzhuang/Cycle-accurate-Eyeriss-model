#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "RegFile.h"

void RegFile::RegStream(void) {
	sc_int<8>		w_reg[spad_depth];
	sc_int<8>		ifmap_reg[spad_depth];
	sc_int<8>		psum_reg;

	// Reset
	if (rst.read()) {
		w.write(0);
		in.write(0);
		psum_tmp_RegtoMAC.write(0);
		for (int i = 0; i < spad_depth; i++) {
			w_reg[i] = 0;
			ifmap_reg[i] = 0;
		}
		psum_reg = 0;
		wait();
	}

	int cnt_weight = -(spad_depth);  // store data at first

	//enum_Dataflow Dataflow = RS;
	while (true) {
		// Store data
		if (in_vld.read() == 0) {
			for (int i = 0; i < spad_depth; i++) {
				w_reg[i] = 0;
				ifmap_reg[i] = 0;
			}
			cnt_weight = 0;
			wait();
		}
		else {
			int filter_width_tmp = filter_width_cf.read().to_uint();
			int dataflow_tmp = dataflow_cf.read().to_uint();

			switch (dataflow_tmp) {
			case OS:
				cerr << "Output Stationary is not supported." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			case RS:
				w_reg[0] = w_in.read();
				ifmap_reg[0] = ifmap_in.read();
				psum_reg = psum_tmp_MACtoReg.read();

				// Counter
				if (cnt_weight == filter_width_tmp - 1) {
					psum_reg = 0;
					cnt_weight = 0;
				}
				else if (cnt_weight == 0) {
					cnt_weight += 1;
				}
				else {
					psum_reg = psum_tmp_MACtoReg.read();
					cnt_weight += 1;
				}

				// Shift the data in Reg
				for (int i = spad_depth - 2; i >= 0; i--) {
					w_reg[i + 1] = w_reg[i];
					ifmap_reg[i + 1] = ifmap_reg[i];
				}
				// Write the newest data		
				w.write(w_reg[spad_depth - 1]);
				in.write(ifmap_reg[spad_depth - 1]);
				psum_tmp_RegtoMAC.write(psum_reg);

				wait();
				break;
			default:
				cerr << "Error: Dataflow has not configured." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			}
		}		
	}
}