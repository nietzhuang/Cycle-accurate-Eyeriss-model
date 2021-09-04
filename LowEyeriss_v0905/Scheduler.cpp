#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "Scheduler.h"

void SCHEDULER::controller(void) {
	sc_uint<10> ifmap_width_field = 0;
	sc_uint<3> padding_field = 0;
	sc_uint<32> propass_field = 0;
	sc_uint<32> ofmap_width_field = 0;
	sc_uint<13> units_field = 0;

	// Initialise
	enable.write(0);
	in_vld_PE.write(0);
	start_sche.write(0);

	while (true) {
		// Wait for scheduler storing temporary data.
		do {
			wait();
		} while (!in_vld.read());
		ifmap_width_field = ifmap_width_cf.read().to_uint();
		padding_field = padding_cf.read().to_uint();
		propass_field = propass_cf.read().to_uint();
		ofmap_width_field = ofmap_width_cf.read().to_uint();
		units_field = units_cf.read().to_uint();

		wait(ifmap_width_field * units_field + padding_field *2);

		enable.write(1);
		in_vld_PE.write(1);
		start_sche.write(1);

		// Stop Scheduling
		wait((spad_depth + 1) + propass_field * spad_depth * ofmap_width_field - 1);
		enable.write(0);
		in_vld_PE.write(0);
		start_sche.write(0);
	}
}

void SCHEDULER::weight_scheduling(void) {
	sc_int<8>* weight_tmp = new sc_int<8>[(filter_height * units) * (filter_width)];
	sc_int<8>	cnt_weightrow;
	sc_int<16>	cnt_ofmap;

	sc_uint<4> filter_height_field = 0;
	sc_uint<4> filter_width_field = 0;
	sc_uint<2> dataflow_field = 0;
	sc_uint<32> ofmap_height_field = 0;
	sc_uint<32> ofmap_width_field = 0;
	sc_uint<32> folding_field = 0;
	sc_uint<32> propass_field = 0;
	sc_uint<2> layer_field = 0;
	sc_uint<13> units_field = 0;

	int cnt_pass;
	int cnt_units;
	bool w_done;
	int cur_fold;
	int PE_width_act;

	// Initialise
	//! have to substitute it into fixed size sram in practical.
	cout << "Scheduler weight has been initialised." << endl;
	for (int i = 0; i < filter_height * units; i++) {
		for (int j = 0; j < filter_width; j++) {
			//weight_tmp[i][j] = 0;
			weight_tmp[i + j * filter_height  * units] = 0;
		}
	}
	w_done = false;
	cur_fold = 0;
	PE_width_act = 0;
	cnt_weightrow = 0;
	cnt_ofmap = 0;
	cnt_pass = 0;
	cnt_units = 0;
	wait();


	while (true) {
		filter_height_field = filter_height_cf.read().to_uint();
		filter_width_field = filter_width_cf.read().to_uint();
		dataflow_field = dataflow_cf.read().to_uint();
		ofmap_height_field = ofmap_height_cf.read();
		ofmap_width_field = ofmap_width_cf.read();
		folding_field = folding_cf.read();
		propass_field = propass_cf.read();
		layer_field = layer_cf.read().to_uint();
		units_field = units_cf.read().to_int();

		//!!! It has potential to reduce the temporary buffer.
		if (in_vld.read() && !w_done && !start_sche.read()) {
			cout << "Fetching filter data into scheduler... " << endl;
			for (int num_units = 0; num_units < units; num_units++) {
				for (int cnt_w_tmp = 0; cnt_w_tmp < filter_width_field; cnt_w_tmp++) {
					for (int num_height = 0; num_height < filter_height_field; num_height++) {
						//weight_tmp[num_height + num_units*filter_height][cnt_w_tmp] = weight[num_height].read();
						weight_tmp[num_height + num_units * filter_height_field
						+ cnt_w_tmp * filter_height_field * units_field] = weight[num_height].read();

					}
					wait();
				}
			}
			cout << "Filter data fetching is done. " << endl;		
			w_done = true;
			cnt_weightrow = 0;
			cnt_ofmap = 0;
			cnt_pass = 0;
			wait();
		}
		else if (start_sche.read()) {
			switch (dataflow_field) {
			case OS:
				cerr << "Output Stationary is not supported." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			case RS:
				//!!!  Consider whether the Normal and lack of PEs conditions can combine together.
				// Clear all PEs before weight data propagates
				for (int num_width = 0; num_width < PE_width; num_width++) {
					for (int num_height = 0; num_height < PE_length; num_height++) {
						weight_out_PE[num_height][num_width].write(0);
					}
				}
				// Normal RS condition
				if (ofmap_height_field <= PE_width && filter_height_field <= PE_length) {
					for (int num_units = 0; num_units < units_field; num_units++) {
						for (int num_width = 0; num_width < ofmap_width_field; num_width++) {
							for (int num_height = 0; num_height < filter_height_field; num_height++) {
								weight_out_PE[num_height][num_width + num_units].write(
									weight_tmp[num_height + num_units * filter_height_field
									+ cnt_weightrow * filter_height_field *units_field]);
							}
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
				else if (ofmap_height_field > PE_width && filter_height_field <= PE_length) {
					int remain_ofmap = ofmap_height_field - (propass_field - 1) * folding_field * PE_width;

					while (cnt_pass < propass) {
						count_pass.write(cnt_pass);
						// Clear all PEs before weight data propagates
						for (int num_length = 0; num_length < PE_length; num_length++) {
							for (int num_width = 0; num_width < PE_width; num_width++) {
								weight_out_PE[num_length][num_width].write(0);
							}
						}
						// Estimate folding number at current processing pass
						if (cnt_pass == propass_field - 1)
							cur_fold = remain_ofmap / PE_width + 1;
						else
							cur_fold = folding_field;

						// Estimate active PEs in last fold at last processing pass 
						for (int num_fold = 0; num_fold < cur_fold; num_fold++) {
							if ((cnt_pass == propass_field - 1) && (num_fold == cur_fold - 1))
								PE_width_act = remain_ofmap - (cur_fold - 1) * PE_width;
							else
								PE_width_act = PE_width;

							for (int num_width = 0; num_width < PE_width_act; num_width++) {
								for (int num_height = num_fold * filter_height_field; num_height < (num_fold + 1) * filter_height_field; num_height++) {
									cnt_units = num_width + num_fold * PE_width + cnt_pass * folding_field * PE_width;

									if (layer_field == FC) {
										/*weight_out_PE[num_height][num_width].write(
											weight_tmp[num_height - num_fold*filter_height + cnt_units*filter_height][cnt_weightrow]);*/
										weight_out_PE[num_height][num_width].write(
											weight_tmp[num_height - num_fold * filter_height_field + cnt_units * filter_height_field
											+ cnt_weightrow * filter_height_field  * units_field]);

									}
									else {
										weight_out_PE[num_height][num_width].write(
											weight_tmp[num_height - num_fold * filter_height_field
											+ cnt_weightrow * filter_height_field * units_field]);

									}
								}
							}
						}

						if (cnt_weightrow < spad_depth - 1)
							cnt_weightrow++;
						else {
							cnt_weightrow = 0;
							cnt_ofmap++;
						}

						if (cnt_ofmap == ofmap_width_field)
							cnt_pass++;
						else if ((cnt_ofmap == ofmap_width_field) && (cnt_pass == propass_field - 1))
							cnt_pass = 0;

						if (cnt_ofmap == ofmap_width_field)
							cnt_ofmap = 0;

						w_done = false;
						wait();
					}
					w_done = false;
					wait();
				}
				// Unsupported RS condition
				else {
					cerr << "Error: RS dataflow is supported, "
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

	sc_uint<4> filter_height_field = 0;
	sc_uint<10> ifmap_height_field = 0;
	sc_uint<10> ifmap_width_field = 0;
	sc_uint<7> stride_field = 0;
	sc_uint<2> dataflow_field = 0;
	sc_uint<32> ofmap_height_field = 0;
	sc_uint<32> ofmap_width_field = 0;
	sc_uint<3> padding_field = 0;
	sc_uint<32> folding_field = 0;
	sc_uint<32> propass_field = 0;
	sc_uint<13> units_field = 0;

	int cnt_pass;
	int cnt_units;
	bool ifmap_done;
	int cur_fold;
	int PE_width_act;

	// Initialise
	cout << "Scheduler ifmap has been initialised." << endl;
	for (int i = 0; i < ifmap_height + padding * 2; i++) {
		for (int j = 0; j < ifmap_width + padding * 2; j++) {
			// ifmap_tmp[ifmap_height][ifmap_width];
			ifmap_tmp[i + j * ifmap_height] = 0; 
		}
	}

	ifmap_done = false;
	cnt_ifmap_in = 0;
	cnt_ofmap = 0;
	cnt_pass = 0;
	cnt_units = 0;
	cur_fold = folding;
	PE_width_act = 0;
	wait();


	//enum_Dataflow Dataflow = RS;
	while (true) {
		filter_height_field = filter_height_cf.read().to_uint();
		ifmap_height_field = ifmap_height_cf.read().to_uint();
		ifmap_width_field = ifmap_width_cf.read().to_uint();
		stride_field = stride_cf.read().to_uint();
		dataflow_field = dataflow_cf.read().to_uint();
		ofmap_height_field = ofmap_height_cf.read();
		ofmap_width_field = ofmap_width_cf.read();
		padding_field = padding_cf.read().to_uint();
		folding_field = folding_cf.read();
		propass_field = propass_cf.read();
		units_field = units_cf.read().to_uint();

		if (in_vld.read() && !ifmap_done && !start_sche.read()) {
			cout << "Fetching iact data into scheduler... " << endl;
			for (int cnt_ifmap_tmp = 0; cnt_ifmap_tmp < ifmap_width_field + padding_field*2; cnt_ifmap_tmp++) {
				for (int num_height = 0; num_height < ifmap_height_field + padding_field * 2; num_height++) {
					//! Expanding ifmap ports is not practical.
					//! Reduce the bandwidth when design in RTL.
					if ((cnt_ifmap_tmp < padding_field) || (cnt_ifmap_tmp >= ifmap_width_field + padding_field))
						ifmap_tmp[num_height + cnt_ifmap_tmp * (ifmap_height_field + padding_field * 2)] = 0;
					else if ((num_height < padding_field) || (num_height >= ifmap_height_field + padding_field))
						ifmap_tmp[num_height + cnt_ifmap_tmp * (ifmap_height_field + padding_field * 2)] = 0;
					else
						ifmap_tmp[num_height + cnt_ifmap_tmp * (ifmap_height_field + padding_field * 2)] = ifmap[num_height - padding_field].read();
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
			switch (dataflow_field) {
			case OS:
				cerr << "Output Stationary is not supported." << endl;
				cerr << "Warning: Simulation has been shut down." << endl;
				sc_stop();
				return;
			case RS:
				// Clear all PEs before ifmap data propagates
				for (int num_width = 0; num_width < PE_width; num_width++) {
					for (int num_height = 0; num_height < PE_length; num_height++) {
						ifmap_out_PE[num_height][num_width].write(0);
					}
				}

				// Normal RS condition
				if (ofmap_height_field <= PE_width && filter_height_field <= PE_length) {
					for (int num_units = 0; num_units < units_field; num_units++) {
						for (int num_width = 0; num_width < ofmap_width_field; num_width++) {
							for (int num_height = 0; num_height < filter_height_field; num_height++) {
								//ifmap_out_PE[num_height][num_width].write(ifmap_tmp[num_height + num_width * stride][cnt_ifmap_in + cnt_ofmap * stride]);
								ifmap_out_PE[num_height][num_width + num_units].write(
									ifmap_tmp[num_height + num_width * stride_field
									+ (cnt_ifmap_in + cnt_ofmap * stride_field) * (ifmap_height_field + padding_field * 2)]);
							}
						}
					}

					if (cnt_ifmap_in < spad_depth - 1)
						cnt_ifmap_in++;
					else if (cnt_ifmap_in == spad_depth - 1) {
						cnt_ifmap_in = 0;
						cnt_ofmap++;
					}
					if (cnt_ofmap == ofmap_width_field)
						cnt_ofmap = 0;

					ifmap_done = false;
					wait();
				}
				// Lack of PE_width RS condition
				else if (ofmap_height_field > PE_width && filter_height_field <= PE_length) {
					int remain_ofmap = ofmap_height_field - (propass_field - 1) * folding_field * PE_width;

					while (cnt_pass < propass_field) {
						// Clear all PEs before ifmap data propagates
						for (int num_length = 0; num_length < PE_length; num_length++) {
							for (int num_width = 0; num_width < PE_width; num_width++) {
								ifmap_out_PE[num_length][num_width].write(0);
							}
						}
						// Estimate folding number at current processing pass
						if (cnt_pass == propass_field - 1)
							cur_fold = remain_ofmap / PE_width + 1;
						else
							cur_fold = folding_field;

						// Estimate active PEs in last fold at last processing pass 
						for (int num_fold = 0; num_fold < cur_fold; num_fold++) {
							if ((cnt_pass == propass_field - 1) && (num_fold == cur_fold - 1))
								PE_width_act = remain_ofmap - (cur_fold - 1) * PE_width;
							else
								PE_width_act = PE_width;

							for (int num_width = 0; num_width < PE_width_act; num_width++) {
								for (int num_height = num_fold * filter_height_field; num_height < (num_fold + 1) * filter_height; num_height++) {
									if (layer == FC) {
										ifmap_out_PE[num_height][num_width].write(
											ifmap_tmp[num_height - num_fold * filter_height_field + 
											+ cnt_ifmap_in * ifmap_height_field]);
									}
									else {
										int tmp = num_height + num_fold * (PE_width * stride_field - filter_height) + cnt_pass * PE_width * stride_field * folding_field + num_width * stride_field;
										ifmap_out_PE[num_height][num_width].write(
											ifmap_tmp[tmp
											+ (cnt_ifmap_in + cnt_ofmap * stride_field) * (ifmap_height_field + padding_field * 2)]);
									}
								}
							}
						}

						if (cnt_ifmap_in < spad_depth - 1)
							cnt_ifmap_in++;
						else if (cnt_ifmap_in == spad_depth - 1) {
							cnt_ifmap_in = 0;
							cnt_ofmap++;
						}
						if (cnt_ofmap == ofmap_width_field)
							cnt_pass++;
						else if ((cnt_ofmap == ofmap_width_field) && (cnt_pass == propass_field - 1))
							cnt_pass = 0;

						if (cnt_ofmap == ofmap_width_field)
							cnt_ofmap = 0;

						ifmap_done = false;
						wait();
					}
					ifmap_done = false;
					wait();
				}
				
				// Unsupported RS condition
				else {
					cerr << "Error: RS dataflow is supported, "
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
				break;
			}		
		}
		else 
			wait();		
	}
}