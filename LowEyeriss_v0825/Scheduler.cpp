#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "Scheduler.h"

void SCHEDULER::controller(void) {
	// Initialise
	enable.write(0);
	in_vld_PE.write(0);
	start_sche.write(0);


	while (true) {
		// Wait for scheduler storing temporary data.
		do {
			wait();
		} while (!in_vld.read());
		wait(ifmap_width + padding*2);

		enable.write(1);
		in_vld_PE.write(1);
		start_sche.write(1);

		// Stop Scheduling
		wait((spad_depth + 1) + propass * spad_depth * ofmap_width - 1);
		enable.write(0);
		in_vld_PE.write(0);
		start_sche.write(0);
	}
}

void SCHEDULER::weight_scheduling(void) {
	sc_int<8>	weight_tmp[filter_height][filter_width];
	sc_int<8>	cnt_weightrow;
	sc_int<16>	cnt_ofmap;
	sc_int<8>	cnt_pass;
	bool w_done;
	int cur_fold;
	int PE_width_act;

	// Initialise
	cout << "Scheduler weight has been initialised." << endl;
	for (int i = 0; i < filter_height; i++) {
		for (int j = 0; j < filter_width; j++) {
			weight_tmp[i][j] = 0;
		}
	}
	w_done = false;
	cur_fold = 0;
	PE_width_act = 0;
	cnt_weightrow = 0;
	cnt_ofmap = 0;
	cnt_pass = 0;
	wait();


	enum_dataflow Dataflow = RS;
	while (true) {
		//!!! It has potential to reduce the temporary buffer.
		if (in_vld.read() && !w_done && !start_sche.read()) {
			cout << "Fetching filter data into scheduler... " << endl;
			for (int cnt_w_tmp = 0; cnt_w_tmp < filter_width; cnt_w_tmp++) {
				for (int num_height = 0; num_height < filter_height; num_height++) {
					weight_tmp[num_height][cnt_w_tmp] = weight[num_height].read();
				}
				wait();			
			}
			cout << "Filter data fetching is done. " << endl;		
			w_done = true;
			cnt_weightrow = 0;
			cnt_ofmap = 0;
			cnt_pass = 0;
			wait();
		}
		else if (start_sche.read()) {
			switch (Dataflow) {
			case OS:
				cerr << "Output Stationary is not supported." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			case RS:
				//!!!  Consider whether the Normal and lack of PEs conditions can combine together.

				// Normal RS condition
				if (ofmap_height <= PE_width && filter_height <= PE_length) {
					for (int num_width = 0; num_width < ofmap_width; num_width++) {
						for (int num_height = 0; num_height < filter_height; num_height++) {
							weight_out_PE[num_height][num_width].write(weight_tmp[num_height][cnt_weightrow]);
						}
					}
					// Otherwise give zero for gating power.
					for (int num_width = ofmap_width; num_width < PE_width; num_width++) {
						for (int num_height = filter_height; num_height < PE_length; num_height++) {
							weight_out_PE[num_height][num_width].write(0);
						}
					}

					if (cnt_weightrow < spad_depth - 1)
						cnt_weightrow++;
					else
						cnt_weightrow = 0;

					w_done = false;
					wait();
				}
				// Lack of PE_width RS condition
				else if (ofmap_height > PE_width && filter_height <= PE_length) {
					int remain_ofmap = ofmap_height - (propass - 1) * folding * PE_width;

					while(cnt_pass < propass) {
						count_pass.write(cnt_pass);
						// Clear all PEs before weight data propagates
						for (int num_length = 0; num_length < PE_length; num_length++) {
							for (int num_width = 0; num_width < PE_width; num_width++) {
								weight_out_PE[num_length][num_width].write(0);
							}
						}
						// Estimate folding number at current processing pass
						if (cnt_pass == propass - 1)
							cur_fold = remain_ofmap / PE_width + 1;
						else
							cur_fold = folding;

						// Estimate active PEs in last fold at last processing pass 
						for (int num_fold = 0; num_fold < cur_fold; num_fold++) {
							if ((cnt_pass == propass - 1) && (num_fold == cur_fold - 1)) 
								PE_width_act = remain_ofmap - (cur_fold - 1) * PE_width;
							else
								PE_width_act = PE_width;

							for (int num_width = 0; num_width < PE_width_act; num_width++) {
								for (int num_height = num_fold * filter_height; num_height < (num_fold + 1) * filter_height; num_height++) {
									weight_out_PE[num_height][num_width].write(weight_tmp[num_height - num_fold * filter_height][cnt_weightrow]);
								}
							}
						}
						
						if (cnt_weightrow < spad_depth - 1)
							cnt_weightrow++;
						else {
							cnt_weightrow = 0;
							cnt_ofmap++;
						}

						if (cnt_ofmap == ofmap_width)
							cnt_pass++;
						else if ((cnt_ofmap == ofmap_width) && (cnt_pass == propass - 1))
							cnt_pass = 0;
						
						if (cnt_ofmap == ofmap_width)
							cnt_ofmap = 0;
						
						w_done = false;
						wait();
					}
					w_done = false;
					wait();
				}
				// Unsupported RS condition
				else {
					cerr << "Error: RS dataflow is supported,"
						<< "but size of PE array can't support." << endl;
					sc_stop();
					return;
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
			wait();
		}
	}
}

void SCHEDULER::ifmap_scheduling(void) {
	sc_int<8>*	ifmap_tmp = new sc_int<8> [(ifmap_height+padding*2) * (ifmap_width+padding*2)];
	sc_int<8>	cnt_ifmap_in;
	sc_int<16>	cnt_ofmap;
	sc_int<8>	cnt_pass;
	bool ifmap_done;
	int cur_fold;
	int PE_width_act;

	// Initialise
	cout << "Scheduler ifmap has been initialised." << endl;
	/*for (int i = 0; i < ifmap_height; i++) {
		for (int j = 0; j < ifmap_width; j++) {
			ifmap_tmp[i + j * ifmap_height] = 0;  // ifmap_tmp[ifmap_height][ifmap_width];
		}
	}*/
	for (int i = 0; i < ifmap_height + padding * 2; i++) {
		for (int j = 0; j < ifmap_width + padding * 2; j++) {
			ifmap_tmp[i + j * ifmap_height] = 0;  // ifmap_tmp[ifmap_height][ifmap_width];
		}
	}

	ifmap_done = false;
	cnt_ifmap_in = 0;
	cnt_ofmap = 0;
	cnt_pass = 0;
	cur_fold = folding;
	PE_width_act = 0;
	wait();


	enum_dataflow Dataflow = RS;
	while (true) {
		if (in_vld.read() && !ifmap_done && !start_sche.read()) {
			cout << "Fetching iact data into scheduler... " << endl;
			for (int cnt_ifmap_tmp = 0; cnt_ifmap_tmp < ifmap_width+padding*2; cnt_ifmap_tmp++) {
				for (int num_height = 0; num_height < ifmap_height + padding * 2; num_height++) {
					//! Expanding ifmap ports is not practical.
					//! Reduce the bandwidth when design in RTL.
					if ((cnt_ifmap_tmp < padding) || (cnt_ifmap_tmp >= ifmap_width+padding))
						ifmap_tmp[num_height + cnt_ifmap_tmp * (ifmap_height + padding * 2)] = 0;
					else if ((num_height < padding) || (num_height >= ifmap_height+padding))
						ifmap_tmp[num_height + cnt_ifmap_tmp * (ifmap_height + padding * 2)] = 0;
					else
						ifmap_tmp[num_height + cnt_ifmap_tmp * (ifmap_height + padding * 2)] = ifmap[num_height - padding].read();
				}
				wait();
			}
			cout << "iact data fetching is done. " << endl;
			ifmap_done = true;
			cnt_ifmap_in = 0;
			cnt_ofmap = 0;
			cnt_pass = 0;
			wait();
		}
		else if (start_sche.read()) {
			switch (Dataflow) {
			case OS:
				cerr << "Output Stationary is not supported." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			case RS:
				// Normal RS condition
				// Clear all PEs before ifmap data propagates
				for (int num_width = 0; num_width < PE_width; num_width++) {
					for (int num_height = 0; num_height < PE_length; num_height++) {
						ifmap_out_PE[num_height][num_width].write(0);
					}
				}

				if (ofmap_width <= PE_width && filter_height <= PE_length) {
					for (int num_width = 0; num_width < ofmap_width; num_width++) {
						for (int num_height = 0; num_height < filter_height; num_height++) {
							//ifmap_out_PE[num_height][num_width].write(ifmap_tmp[num_height + num_width * stride][cnt_ifmap_in + cnt_ofmap * stride]);
							ifmap_out_PE[num_height][num_width].write(
								ifmap_tmp[num_height + num_width * stride 
								+ (cnt_ifmap_in + cnt_ofmap * stride)*(ifmap_height+padding*2)]);
						}
					}

					if (cnt_ifmap_in < spad_depth - 1)
						cnt_ifmap_in++;
					else if (cnt_ifmap_in == spad_depth - 1) {
						cnt_ifmap_in = 0;
						cnt_ofmap++;
					}
					if (cnt_ofmap == ofmap_width)
						cnt_ofmap = 0;

					ifmap_done = false;
					wait();
				}
				// Lack of PE_width RS condition
				else if (ofmap_height > PE_width && filter_height <= PE_length) {
					int remain_ofmap = ofmap_height - (propass - 1) * folding * PE_width;

					while (cnt_pass < propass) {
						// Clear all PEs before ifmap data propagates
						for (int num_length = 0; num_length < PE_length; num_length++) {
							for (int num_width = 0; num_width < PE_width; num_width++) {
								ifmap_out_PE[num_length][num_width].write(0);
							}
						}
						// Estimate folding number at current processing pass
						if (cnt_pass == propass - 1)
							cur_fold = remain_ofmap / PE_width + 1;
						else
							cur_fold = folding;

						// Estimate active PEs in last fold at last processing pass 
						for (int num_fold = 0; num_fold < cur_fold; num_fold++) {
							if ((cnt_pass == propass - 1) && (num_fold == cur_fold - 1))
								PE_width_act = remain_ofmap - (cur_fold - 1) * PE_width;
							else
								PE_width_act = PE_width;

							for (int num_width = 0; num_width < PE_width_act; num_width++) {
								for (int num_height = num_fold * filter_height; num_height < (num_fold + 1) * filter_height; num_height++) {
									int tmp = num_height + num_fold * (PE_width*stride - filter_height) + cnt_pass * PE_width * stride * folding + num_width*stride;
									ifmap_out_PE[num_height][num_width].write(
										ifmap_tmp[tmp 
										+ (cnt_ifmap_in + cnt_ofmap * stride) * (ifmap_height+padding*2)]);
								}
							}
						}

						if (cnt_ifmap_in < spad_depth - 1)
							cnt_ifmap_in++;
						else if (cnt_ifmap_in == spad_depth - 1) {
							cnt_ifmap_in = 0;
							cnt_ofmap++;
						}
						if (cnt_ofmap == ofmap_width)
							cnt_pass++;
						else if ((cnt_ofmap == ofmap_width) && (cnt_pass == propass - 1))
							cnt_pass = 0;

						if (cnt_ofmap == ofmap_width)
							cnt_ofmap = 0;

						ifmap_done = false;
						wait();
					}
					ifmap_done = false;
					wait();
				}
				
				// Unsupported RS condition
				else {
					cerr << "Error: RS dataflow is supported,"
						<< "but size of PE array can't support." << endl;
					sc_stop();
					return;
				}
				break;			
			default:
				cerr << " Error: Dataflow has not configured." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			}		
		}
		else 
			wait();		
	}
}