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
		wait(ifmap_width);  //!!! Need to establish a delay function more sophisticated.

		enable.write(1);
		in_vld_PE.write(1);
		start_sche.write(1);

		// Stop Scheduling
		wait((spad_depth+1)*ofmap_width-1);  //!!! Need to establish a delay function more sophisticated.
		enable.write(0);
		in_vld_PE.write(0);
		start_sche.write(0);
	}
}

void SCHEDULER::weight_scheduling(void) {
	sc_int<8>	weight_tmp[filter_height][filter_width];
	sc_int<8>	cnt_weightrow;
	//sc_int<8>	weight_tmp[PE_length][spad_depth];

	bool w_done;

	// Initialise
	for (int i = 0; i < filter_height; i++) {
		for (int j = 0; j < filter_width; j++) {
			weight_tmp[i][j] = 0;
		}
	}
	w_done = false;
	cnt_weightrow = 0;
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
				if (ofmap_width <= PE_width && filter_height <= PE_length) {
					for (int num_width = 0; num_width < ofmap_width; num_width++) {
						for (int num_height = 0; num_height < filter_height; num_height++) {
							weight_out_PE[num_height][num_width].write(weight_tmp[num_height][cnt_weightrow]);
						}
					}
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
				else if (ofmap_width > PE_width && filter_height <= PE_length) {
					//!!  write all PE zeros at first
					for (int num_width = 0; num_width < PE_width; num_width++) {
						for (int num_height = 0; num_height < PE_length; num_height++) {
							weight_out_PE[num_height][num_width].write(0);
						}
					}

					//!!  overwrite correct data
					for (int num_width = 0; num_width < PE_width; num_width++) {
						for (int num_height = 0; num_height < filter_height; num_height++) {
							weight_out_PE[num_height][num_width].write(weight_tmp[num_height][cnt_weightrow]);
						}
					}
					for (int num_width = PE_width; num_width < ofmap_width; num_width++) {
						for (int num_height = filter_height; num_height < 2 * filter_height; num_height++) {
							weight_out_PE[num_height][num_width-PE_width].write(weight_tmp[num_height - filter_height][cnt_weightrow]);
						}
					}

					if (cnt_weightrow < spad_depth - 1)
						cnt_weightrow++;
					else
						cnt_weightrow = 0;

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
	sc_int<8>	ifmap_tmp[ifmap_height][ifmap_width];
	sc_int<8>	cnt_ifmap_in;
	sc_int<8>	cnt_ofmap;
	bool ifmap_done;

	// Initialise
	cout << "Scheduler ifmap has been initialised." << endl;
	for (int i = 0; i < ifmap_height; i++) {
		for (int j = 0; j < ifmap_width; j++) {
			ifmap_tmp[i][j] = 0;
		}
	}
	ifmap_done = false;
	cnt_ifmap_in = 0;
	cnt_ofmap = 0;
	wait();


	enum_dataflow Dataflow = RS;
	while (true) {
		if (in_vld.read() && !ifmap_done && !start_sche.read()) {
			cout << "Fetching iact data into scheduler... " << endl;
			for (int cnt_ifmap_tmp = 0; cnt_ifmap_tmp < ifmap_width; cnt_ifmap_tmp++) {
				for (int num_height = 0; num_height < ifmap_height; num_height++) {
					ifmap_tmp[num_height][cnt_ifmap_tmp] = ifmap[num_height].read();

				}
				wait();
			}
			cout << "iact data fetching is done. " << endl;
			ifmap_done = true;
			cnt_ifmap_in = 0;
			cnt_ofmap = 0;
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
				if (ofmap_width <= PE_width && filter_height <= PE_length) {
					for (int num_width = 0; num_width < ofmap_width; num_width++) {
						for (int num_height = 0; num_height < filter_height; num_height++) {
							ifmap_out_PE[num_height][num_width].write(ifmap_tmp[num_height + num_width][cnt_ifmap_in + cnt_ofmap]);
						}
					}
					for (int num_width = ofmap_width; num_width < PE_width; num_width++) {
						for (int num_height = filter_height; num_height < PE_length; num_height++) {
							ifmap_out_PE[num_height][num_width].write(0);
						}
					}


					/*
					for (int num_width = 0; num_width < PE_width; num_width++) {
						for (int num_height = 0; num_height < PE_length; num_height++) {
							ifmap_out_PE[num_height][num_width].write(ifmap_tmp[num_height + num_width][cnt_ifmap_in + cnt_ofmap]);
						}
					}*/

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
				else if (ofmap_width > PE_width && filter_height <= PE_length) {
					//!! Write all PEs zeros at first
					for (int num_width = 0; num_width < PE_width; num_width++) {
						for (int num_height = 0; num_height < PE_length; num_height++) {
							ifmap_out_PE[num_height][num_width].write(0);
						}
					}
					//!! overwrite the correct data
					for (int num_width = 0; num_width < PE_width; num_width++) {
						for (int num_height = 0; num_height < filter_height; num_height++) {
							ifmap_out_PE[num_height][num_width].write(ifmap_tmp[num_height + num_width][cnt_ifmap_in + cnt_ofmap]);
						}
					}
					for (int num_width = PE_width; num_width < ofmap_width; num_width++) {
						for (int num_height = filter_height; num_height < 2 * filter_height; num_height++) {
							ifmap_out_PE[num_height][num_width-PE_width].write(ifmap_tmp[num_height + num_width - filter_height][cnt_ifmap_in + cnt_ofmap]);

							//weight_out_PE[num_height][num_width - PE_width].write(weight_tmp[num_height - filter_height][cnt_weightrow]);
						}
					}
					//
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


// 1. For RS dataflow, scheduler start scheduling after registers store information of a layer.
//