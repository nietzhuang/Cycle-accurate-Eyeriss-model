#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h" 
#include "GLB.h"

void GLB::GLB_weight_address(void) {
	// Initialise
	w_done.write(0);
	wait();

	while (true) {
		// Send weight address
		if (conf_done.read() && !w_done.read()) {
			int filter_size_tmp = filter_size_cf.read();
			int num_filter_tmp = num_filter_cf.read().to_uint();
			int num_channel_tmp = num_channel_cf.read().to_uint();

			for (int num_weight = 0; num_weight < filter_size_tmp * num_filter_tmp * num_channel_tmp; num_weight++) {
				addr_w_in.write(num_weight);
				wait();
			}
			w_done.write(1);
			wait();
		}
		else {
			addr_w_in.write(0);
			wait();
		}
	}
}

void GLB::GLB_ifmap_address(void) {
	// Initialise
	iact_done.write(0);
	wait();

	while (true) {
		// Send iact address
		if (conf_done.read() && !iact_done.read()) {
			int ifmap_size_tmp = ifmap_size_cf.read().to_uint();
			int num_channel_tmp = num_channel_cf.read().to_uint();

			for (int num_iact = 0; num_iact < ifmap_size_tmp * num_channel_tmp; num_iact++) {
				addr_ifmpa_in.write(num_iact);
				wait();
			}
			iact_done.write(1);
			wait();
		}
		else {
			addr_ifmpa_in.write(0);
			wait();
		}
	}
}

void GLB::GLB_read_weight(void) {
	// Initialise
	//! have to substitute it into fixed size sram in practical.
	for (int i = 0; i < filter_height*num_filter; i++) {
		for (int j = 0; j < filter_width; j++) {
			for (int k = 0; k < num_channel; k++) {
				filter[i][j][k].write(0);
			}
		}
	}
	cout << "GLB filter memory has been initialised." << endl;
	wait(2);

	// Fetch weight data
	while (true) {  
		if (conf_done.read() && !w_done.read()) {
			wait(2);
			int filter_height_tmp = filter_height_cf.read().to_uint();
			int filter_width_tmp = filter_width_cf.read().to_uint();
			int num_channel_tmp = num_channel_cf.read().to_uint();
			int num_filter_tmp = num_filter_cf.read().to_uint();

			cout << "Fetching filter data into GLB... " << endl;
			for (int k = 0; k < num_channel_tmp; k++) {
				for (int i = 0; i < filter_height_tmp * num_filter_tmp; i++) {  // numerous filters are stored by rows in order
					for (int j = 0; j < filter_width_tmp; j++) {
						filter[i][j][k].write(w_rdata.read());
						wait();
					}
				}
			}
			cout << "Filter data fetching is done. " << endl;
		}
		else 
			wait();
	}
}

void GLB::GLB_read_ifmap(void) {
	// Initialise
	//! have to substitute it into fixed size sram in practical.
	for (int k = 0; k < num_channel; k++) {
		for (int i = 0; i < ifmap_height; i++) {
			for (int j = 0; j < ifmap_width; j++) {
				ifmap[i][j][k].write(0);
			}
		}
	}
	cout << "GLB ifmap memory has been initialised." << endl;
	wait(2);

	// Fetch iact data
	while (true) {
		if (conf_done.read() && !iact_done.read()) {
			wait(2);
			int ifmap_height_tmp = ifmap_height_cf.read().to_uint();
			int ifmap_width_tmp = ifmap_width_cf.read().to_uint();
			int num_channel_tmp = num_channel_cf.read().to_uint();

			cout << "Fetching iact data into GLB... " << endl;
			for (int k = 0; k < num_channel_tmp; k++) {
				for (int i = 0; i < ifmap_height_tmp; i++) {
					for (int j = 0; j < ifmap_width_tmp; j++) {
						ifmap[i][j][k].write(ifmap_rdata.read());
						wait();
					}
				}
			}
			cout << "iact data fetching is done. " << endl;
		}		
		else 
			wait();		
	}
}

void GLB::GLB_send_weight(void) {
	// Initialise
	wait(2);

	bool w_in_done = false;  // to control data in different channels. 
	int cnt_filter = 0;
	int channel = 0;
	while (true) {
		int filter_height_tmp = filter_height_cf.read().to_uint();
		int filter_width_tmp = filter_width_cf.read().to_uint();
		int num_channel_tmp = num_channel_cf.read().to_uint();
		int num_filter_tmp = num_filter_cf.read().to_uint();

		if (conf_done.read() && !layer_done.read()) {
			if (!w_in_done && w_done.read() && iact_done.read()) {  // transfer data after all data is stored in GLB.
				// transfer weight data to scheduler
				for (int num_column = 0; num_column < filter_width_tmp; num_column++) {
					for (int num_height = 0; num_height < filter_height_tmp; num_height++) {
						w_wdata[num_height].write(filter[num_height + cnt_filter*filter_height][num_column][channel].read());
					}
					wait();
				}
				w_in_done = true;
			}
			else if (channel_done.read()) {
				if (channel == num_channel_tmp - 1)
					channel = 0;
				else
					channel++;
				
				if (filter_done.read()) {
					if (cnt_filter == num_filter_tmp - 1)
						cnt_filter = 0;
					else
						cnt_filter++;
				}

				w_in_done = false;
				wait();
			}
			else {
				for (int num_height = 0; num_height < filter_height_tmp; num_height++) {
					w_wdata[num_height].write(0);
				}
				wait();
			}
		}
		else {
			for (int num_height = 0; num_height < filter_height_tmp; num_height++) {
				w_wdata[num_height].write(0);
			}
			channel = 0;

			wait();
		}
	}
}

void GLB::GLB_send_ifmap(void) {
	// Initialise
	in_vld.write(0);
	wait(2);

	bool ifmap_in_done = false;
	int channel = 0;
	while (true) {
		int ifmap_height_tmp = ifmap_height_cf.read().to_uint();
		int ifmap_width_tmp = ifmap_width_cf.read().to_uint();
		int num_channel_tmp = num_channel_cf.read().to_uint();
		int num_filter_tmp = num_filter_cf.read().to_uint();
		int padding_tmp = padding_cf.read().to_uint();

		if (conf_done.read() && !layer_done.read()) {
			if (!ifmap_in_done && w_done.read() && iact_done.read()) {
				in_vld.write(1);

				// transfer iact data to scheduler
				if (padding)
					wait(padding);  //!!  let scheduler store padding zero
				for (int num_column = 0; num_column < ifmap_width_tmp; num_column++) {
					for (int num_height = 0; num_height < ifmap_height_tmp; num_height++) {
						ifmap_wdata[num_height].write(ifmap[num_height][num_column][channel].read());
					}
					wait();
				}
				if (padding_tmp)
					wait(padding_tmp);  //!!  let scheduler store padding zero
				ifmap_in_done = true;
			}
			
			else if (channel_done.read()) {
				if (channel == num_channel_tmp - 1)
					channel = 0;
				else
					channel++;
				ifmap_in_done = false;
				wait();
			}
			
			else {
				for (int num_height = 0; num_height < ifmap_height_tmp; num_height++) {
					ifmap_wdata[num_height].write(0);
				}

				in_vld.write(0);
				wait();
			}
		}
		else {
			for (int num_height = 0; num_height < ifmap_height_tmp; num_height++) {
				ifmap_wdata[num_height].write(0);
			}

			channel = 0;
			in_vld.write(0);
			wait();
		}
	}
}

void GLB::GLB_ofmap(void) {
	int ofmap_tmp[ofmap_height] = {0};
	int ofmap_column = 0;
	int PE_width_act = 0;	
	int cnt_filter = 0;
	int ofmap_ch = 0;
	int cur_fold = 0;
	int cnt_pass = 0;
	bool out_valid = false;
	ofmap_channel.write(0);
	filter_done.write(0);
	layer_done.write(0);

	int tmp = 0;  // use to store intermediate ofmap
	while (true) {
		int filter_height_tmp = filter_height_cf.read().to_uint();
		int ofmap_height_tmp = ofmap_height_cf.read();
		int ofmap_width_tmp = ofmap_width_cf.read();
		int num_channel_tmp = num_channel_cf.read().to_uint();
		int num_filter_tmp = num_filter_cf.read().to_uint();
		int padding_tmp = padding_cf.read().to_uint();
		int folding_tmp = folding_cf.read();
		int propass_tmp = propass_cf.read();

		if (conf_done.read() && layer_done.read()) {
			// Clear all control signals   //!! and all counters ?
			channel_done.write(0);
			filter_done.write(0);

			// Write final results to DRAM
			for(int k=0; k < num_filter_tmp; k++){
				for (int i = 0; i < ofmap_height_tmp; i++) {
					for (int j = 0; j < ofmap_width_tmp; j++) {
						ofmap_result.write(ofmap_buf[i][j][k]);
						wait();
					}
				}
			}
		}
		else if (conf_done.read()) {
			// Normal RS condition
			if (ofmap_width_tmp <= PE_width && filter_height_tmp <= PE_length) {
				//!!! Can the code reduce by bitwise or operator??
				/*for (int i = 0; i < PE_width; i++) {
					for (int j = 0; j < PE_length; j++) {
						out_valid |= out_vld[j][i].read();
					}
				}*/
				for (int i = 0; i < PE_width; i++) {
					for (int j = 0; j < PE_length; j++) {
						if (out_vld[j][i].read())
							out_valid = true;
						else
							out_valid = false;
					}
				}

				if (out_valid) {
					ofmap_ch = ofmap_channel.read();
					for (int i = 0; i < ofmap_height_tmp; i++) {
						for (int j = 0; j < filter_height_tmp; j++) {
							if (out_vld[j][i].read())
								ofmap_tmp[i] += ofmap_out[j][i].read();
						}
						tmp = ofmap_buf[i][ofmap_column][cnt_filter].read();
						ofmap_buf[i][ofmap_column][cnt_filter].write(tmp + ofmap_tmp[i]);
						psum_test_buf[i+cnt_filter*ofmap_height_tmp][ofmap_column][ofmap_ch].write(ofmap_tmp[i]);	// Only for check intermediate ofmap		

						ofmap_tmp[i] = 0;  // clear after store in buffer
					}
					ofmap_column++;

					if (ofmap_column == ofmap_width_tmp) {
						ofmap_column = 0;
						ofmap_channel.write(ofmap_ch + 1);
						channel_done.write(1);

						if (ofmap_ch == num_channel_tmp - 1) {
							ofmap_channel.write(0);
							filter_done.write(1);
							cnt_filter++;

							if (cnt_filter == num_filter_tmp)
								layer_done.write(1);
						}
						wait();
					}
					else
						wait();
				}
				else {
					channel_done.write(0);
					filter_done.write(0);

					wait();
				}
			}			
			// Lack of PE_width RS condition
			else if (ofmap_width_tmp > PE_width && filter_height_tmp <= PE_length) {
				int remain_ofmap = ofmap_height_tmp - (propass_tmp - 1) * folding_tmp * PE_width;

				for (int i = 0; i < PE_width; i++) {
					for (int j = 0; j < PE_length; j++) {
						if (out_vld[j][i].read())
							out_valid = true;
						else
							out_valid = false;
					}
				}

				if (out_valid) {
					ofmap_ch = ofmap_channel.read();

					// Align the active PEs with ofmap buff
					if (cnt_pass == propass_tmp - 1)
						cur_fold = remain_ofmap / PE_width + 1;
					else
						cur_fold = folding;

					for (int num_fold = 0; num_fold < cur_fold; num_fold++) {
						if ((cnt_pass == propass_tmp - 1) && (num_fold == cur_fold - 1))
							PE_width_act = remain_ofmap - (cur_fold-1) * PE_width;
						else
							PE_width_act = PE_width;

						for (int i = 0; i < PE_width_act; i++) {
							for (int j = 0; j < filter_height_tmp; j++) {
								if (out_vld[j + num_fold * filter_height_tmp][i].read())
									ofmap_tmp[i] += ofmap_out[j + num_fold * filter_height_tmp][i].read();
							}

							tmp = ofmap_buf[i + num_fold * PE_width + cnt_pass * folding_tmp * PE_width][ofmap_column][cnt_filter].read();  // Accumulate mutiple channels
							ofmap_buf[i + num_fold * PE_width + cnt_pass * folding_tmp * PE_width][ofmap_column][cnt_filter].write(tmp + ofmap_tmp[i]);
							psum_test_buf[i + cnt_filter * ofmap_height_tmp + num_fold * PE_width + cnt_pass * folding_tmp * PE_width][ofmap_column][ofmap_ch].write(ofmap_tmp[i]);  // Only for check intermediate ofmap		

							ofmap_tmp[i] = 0;  // clear after store in buffer
						}
					}
					ofmap_column++;

					if (propass_tmp > 1) {
						if ((cnt_pass == propass_tmp - 1) && (ofmap_column == ofmap_width_tmp)) {
							ofmap_column = 0;
							cnt_pass = 0;
							ofmap_channel.write(ofmap_ch + 1);
							channel_done.write(1);

							if (ofmap_ch == num_channel_tmp - 1) {
								ofmap_channel.write(0);
								filter_done.write(1);
								cnt_filter++;

								if (cnt_filter == num_filter_tmp) {
									cnt_filter = 0;
									layer_done.write(1);
								}
							}
							wait();
						}
						else if (ofmap_column == ofmap_width_tmp) {
							ofmap_column = 0;
							cnt_pass++;
							wait();
						}
						else
							wait();
					}
					else {
						if (ofmap_column == ofmap_width_tmp) {
							ofmap_column = 0;
							ofmap_channel.write(ofmap_ch + 1);
							channel_done.write(1);

							if (ofmap_ch == num_channel - 1) {
								ofmap_channel.write(0);
								filter_done.write(1);
								cnt_filter++;

								if (cnt_filter == num_filter) {
									cnt_filter = 0;
									layer_done.write(1);
								}
							}
							wait();
						}
						else
							wait();
					}
				}
				else {
					channel_done.write(0);
					filter_done.write(0);
					wait();
				}
			}
			// Unsupported RS condition
			else {
				cerr << "Error: RS dataflow is supported,"
					<< "but size of PE array can't support." << endl;
				sc_stop();
				return;
			}
		}
		else
			wait();
	}
}

void GLB::psum_check(void) {
	bool check = check_psum;
	int psum_tmp[num_channel];
	int errors = 0;

	ifstream psum_file("./Patterns/" + string(pattern_name) + "/psum.dat", ios::in);
	if (!psum_file) {
		cerr << "Warning: Could not open psum.dat file." << endl;
		cerr << "Warning: Simulation has been shut down." << endl;
		sc_stop();
		return;
	}

	while (true) {
		//if (check && layer_done.read()) {
		if (check && conf_done.read() && filter_done.read()) {  // !! It may modify to check after a channel done instead of filter done.
			cout << "Checking psum buffer... " << endl;
			for (int i = 0; i < num_channel; i++) {				
				cout << "Checking channel " << i << "...";
				for (int j = 0; j < ofmap_height; j++) {
					for (int k = 0; k < ofmap_width; k++) {
						// Read psum data aligned channel number 
						for (int l = 0; l < num_channel; l++) {
							psum_file >> psum_tmp[l];
						
						}
						if (psum_test_buf[j][k][i].read() != psum_tmp[i]) {
							cout << endl;
							cout << setw(30) << "psum[" << j << "][" << k << "] "
								<<"\treturn: " 
								<< psum_test_buf[j][k][i].read()
								<< "\tGolden value:" << psum_tmp[i];
							errors++;
						}
					}
				}
				if (errors > 0) {
					cerr << endl;
					cerr << setw(50)
						<< "Warning: ofmap in channel " << i
						<< " has " << errors << " wrong data" << endl;
				}
				else
					cout << "\tpass." << endl;
					
				psum_file.seekg(0, ios::beg);
				errors = 0;
			}
			cout << endl
				<< endl;
			//check = false;
			wait();
		}
		else
			wait();
	}
}