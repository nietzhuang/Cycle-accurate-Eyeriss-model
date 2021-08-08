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
		if (!w_done.read()) {
			for (int num_weight = 0; num_weight < filter_size * num_channel; num_weight++) {
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
		if (!iact_done.read()) {
			for (int num_iact = 0; num_iact < ifmap_size * num_channel; num_iact++) {
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
	for (int i = 0; i < filter_height; i++) {
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
		if (!w_done.read()) {
			cout << "Fetching filter data into GLB... " << endl;
			for (int k = 0; k < num_channel; k++) {
				for (int i = 0; i < filter_height; i++) {
					for (int j = 0; j < filter_width; j++) {
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
		if (!iact_done.read()) {
			cout << "Fetching iact data into GLB... " << endl;
			for (int k = 0; k < num_channel; k++) {
				for (int i = 0; i < ifmap_height; i++) {
					for (int j = 0; j < ifmap_width; j++) {
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
	int channel = 0;
	while (true) {	
		if (!layer_done.read()) {
			if (!w_in_done && w_done.read() && iact_done.read()) {  // transfer data after all data is stored in GLB.
				// transfer weight data to scheduler
				for (int num_column = 0; num_column < filter_width; num_column++) {
					for (int num_height = 0; num_height < filter_height; num_height++) {
						w_wdata[num_height].write(filter[num_height][num_column][channel].read());
					}
					wait();
				}
				w_in_done = true;
			}
			else if (channel_done.read()) {
				channel++;
				w_in_done = false;
				wait();
			}
			else {
				for (int num_height = 0; num_height < filter_height; num_height++) {
					w_wdata[num_height].write(0);
				}
				wait();
			}
		}
		else {
			for (int num_height = 0; num_height < filter_height; num_height++) {
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
		if (!layer_done.read()) {
			if (!ifmap_in_done && w_done.read() && iact_done.read()) {
				in_vld.write(1);

				// transfer iact data to scheduler
				for (int num_column = 0; num_column < ifmap_width; num_column++) {
					for (int num_height = 0; num_height < ifmap_height; num_height++) {
						ifmap_wdata[num_height].write(ifmap[num_height][num_column][channel].read());
					}
					wait();
				}
				ifmap_in_done = true;
			}
			else if (channel_done.read()) {
				channel++;
				ifmap_in_done = false;
				wait();
			}
			else {
				for (int num_height = 0; num_height < ifmap_height; num_height++) {
					ifmap_wdata[num_height].write(0);
				}
				
				in_vld.write(0);
				wait();
			}
		}
		else {
			for (int num_height = 0; num_height < ifmap_height; num_height++) {
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
	int ofmap_ch = 0;
	bool out_valid = false;
	ofmap_channel.write(0);
	layer_done.write(0);

	int tmp = 0;
	while (true) {
		if (layer_done.read()) {
			// Write final results to DRAM
			for (int i = 0; i < ofmap_height; i++) {
				for (int j = 0; j < ofmap_width; j++) {
					ofmap_result.write(ofmap_buf[i][j]);
					wait();
				}
			}
		}
		else {
			// Normal RS condition
			if (ofmap_width <= PE_width && filter_height <= PE_length) {
				//!!! Can the code reduce by bitwise or operator??
				/*for (int i = 0; i < PE_width; i++) {
					for (int j = 0; j < PE_length; j++) {
						out_valid |= out_vld[j][i].read();
					}
				}*/
				for (int i = 0; i < ofmap_width; i++) {
					for (int j = 0; j < filter_height; j++) {
						if (out_vld[j][i].read())
							out_valid = true;
						else
							out_valid = false;
					}
				}

				if (out_valid) {
					ofmap_ch = ofmap_channel.read();
					for (int i = 0; i < ofmap_height; i++) {
						for (int j = 0; j < filter_height; j++) {
							if (out_vld[j][i].read())
								ofmap_tmp[i] += ofmap_out[j][i].read();
						}
						tmp = ofmap_buf[i][ofmap_column].read();
						ofmap_buf[i][ofmap_column].write(tmp + ofmap_tmp[i]);
						ofmap_test_buf[i][ofmap_column][ofmap_ch].write(ofmap_tmp[i]);	// Only for check intermediate ofmap		

						ofmap_tmp[i] = 0;  // clear after store in buffer
					}
					ofmap_column++;

					if (ofmap_column == ofmap_width) {
						ofmap_column = 0;
						ofmap_channel.write(ofmap_ch + 1);
						channel_done.write(1);

						if (ofmap_ch == num_channel - 1) {
							layer_done.write(1);
							wait();
						}
						wait();
					}
					else
						wait();

				}
				else {
					channel_done.write(0);

					wait();
				}
			}
			// Lack of PE_width RS condition
			else if (ofmap_width > PE_width && filter_height <= PE_length) {
				for (int i = 0; i < ofmap_width; i++) {
					for (int j = 0; j < filter_height; j++) {
						if (out_vld[j][i].read())
							out_valid = true;
						else
							out_valid = false;
					}
				}
				
				if (out_valid) {
					ofmap_ch = ofmap_channel.read();
					for (int i = 0; i < ofmap_height; i++) {
						if (i < PE_width) {
							for (int j = 0; j < filter_height; j++) {
								if (out_vld[j][i].read())
									ofmap_tmp[i] += ofmap_out[j][i].read();
							}
							tmp = ofmap_buf[i][ofmap_column].read();
							ofmap_buf[i][ofmap_column].write(tmp + ofmap_tmp[i]);
							ofmap_test_buf[i][ofmap_column][ofmap_ch].write(ofmap_tmp[i]);	// Only for check intermediate ofmap		

							ofmap_tmp[i] = 0;  // clear after store in buffer
						}
						else if (i < 2 * PE_width) {
							for (int j = filter_height; j < 2 * filter_height; j++) {
								if (out_vld[j][i-PE_width].read())
									ofmap_tmp[i] += ofmap_out[j][i-PE_width].read();
							}
							tmp = ofmap_buf[i][ofmap_column].read();
							ofmap_buf[i][ofmap_column].write(tmp + ofmap_tmp[i]);
							ofmap_test_buf[i][ofmap_column][ofmap_ch].write(ofmap_tmp[i]);	// Only for check intermediate ofmap

							ofmap_tmp[i] = 0;  // clear after store in buffer
						}
						else
							cout << "Warning: " << endl;
					}
					ofmap_column++;

					if (ofmap_column == ofmap_width) {
						ofmap_column = 0;
						ofmap_channel.write(ofmap_ch + 1);
						channel_done.write(1);

						if (ofmap_ch == num_channel - 1) {
							layer_done.write(1);
							wait();
						}
						wait();
					}
					else
						wait();

				}
				else {
					channel_done.write(0);

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
	}
}

void GLB::ofmap_check(void) {
	bool check = true;
	int psum_tmp[num_channel];
	int errors = 0;

	//ifstream psum_file("psum.dat", ios::in);
	//ifstream psum_file("psum2.dat", ios::in);
	//ifstream psum_file("psum3.dat", ios::in);
	ifstream psum_file("psum4.dat", ios::in);

	if (!psum_file) {
		cerr << "Warning: Could not open psum.dat file." << endl;
		cerr << "Warning: Simulation has been shut down." << endl;
		sc_stop();
		return;
	}

	while (true) {
		if (check && layer_done.read()) {
			wait();

			cout << endl;
			cout << "Checking ofmap test buffer... " << endl;
			for (int i = 0; i < num_channel; i++) {				
				cout << "Checking channel " << i << "...";
				for (int j = 0; j < ofmap_height; j++) {
					for (int k = 0; k < ofmap_width; k++) {

						//!!! The function of loader has to generalise afterwards.
						//psum_file >> psum_tmp[0] >> psum_tmp[1] >> psum_tmp[2];
						psum_file >> psum_tmp[0];
						if (ofmap_test_buf[j][k][i].read() != psum_tmp[i]) {
							cout << endl;
							cout << setw(30) << "ofmap[" << j << "][" << k << "] return: "
								<< ofmap_test_buf[j][k][i].read()
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
			
			check = false;
			wait();
		}
		else
			wait();
	}
}