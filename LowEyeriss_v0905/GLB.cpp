#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h" 
#include "GLB.h"

void GLB::GLB_weight_address(void) {
	sc_uint<8> filter_size_field = 0;
	sc_uint<10> num_channel_field = 0;
	sc_uint<10> num_filter_field = 0;
	sc_uint<13> units_field = 0;
	sc_uint<2> layer_field = 0;

	// Initialise
	w_done.write(0);
	wait();

	while (true) {
		// Send weight address
		if (conf_done.read() && !w_done.read()) {
			filter_size_field = filter_size_cf.read();
			num_channel_field = num_channel_cf.read().to_uint();
			num_filter_field = num_filter_cf.read().to_uint();
			units_field = units_cf.read().to_uint();
			layer_field = layer_cf.read().to_uint();

			if (layer_field == FC)
				num_filter_field = units_field;

			if ((layer_field != MAX) && (layer_field != AVG)) {
				for (int num_weight = 0; num_weight < filter_size_field * num_filter_field * num_channel_field; num_weight++) {
					addr_w_in.write(num_weight);
					wait();
				}
			}
			else
				addr_w_in.write(0);

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
	sc_uint<20> ifmap_size_field = 0;
	sc_uint<10> num_channel_field = 0;

	// Initialise
	iact_done.write(0);
	wait();

	while (true) {
		// Send iact address
		if (conf_done.read() && !iact_done.read()) {
			ifmap_size_field = ifmap_size_cf.read().to_uint();
			num_channel_field = num_channel_cf.read().to_uint();

			for (int num_iact = 0; num_iact < ifmap_size_field * num_channel_field; num_iact++) {
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
	sc_uint<4> filter_height_field = 0;
	sc_uint<4> filter_width_field = 0;
	sc_uint<10> num_channel_field = 0;
	sc_uint<10> num_filter_field = 0;
	sc_uint<13> units_field = 0;
	sc_uint<2> layer_field = 0;

	// Initialise
	//! have to substitute it into fixed size sram in practical.
	if (layer == FC) {
		for (int i = 0; i < filter_height_field * units_field; i++) {
			for (int j = 0; j < filter_width_field; j++) {
				for (int k = 0; k < num_channel_field; k++) {
					//filter[i][j][k].write(0);
					filter[i
						+ j * filter_height_field * units_field
						+ k * filter_height_field * units_field * filter_width_field].write(0);
				}
			}
		}
	}
	else {
		for (int i = 0; i < filter_height_field * num_filter_field; i++) {
			for (int j = 0; j < filter_width_field; j++) {
				for (int k = 0; k < num_channel_field; k++) {
					//filter[i][j][k].write(0);
					filter[i
					+ j * filter_height_field * num_filter_field
					+ k * filter_height_field * num_filter_field * filter_width_field].write(0);
				}
			}
		}
	}
	cout << "GLB filter memory has been initialised." << endl;
	wait(2);

	// Fetch weight data
	while (true) {  
		if (conf_done.read() && !w_done.read()) {
			wait(2);
			filter_height_field = filter_height_cf.read().to_uint();
			filter_width_field = filter_width_cf.read().to_uint();
			num_channel_field = num_channel_cf.read().to_uint();
			num_filter_field = num_filter_cf.read().to_uint();
			units_field = units_cf.read().to_uint();
			layer_field = layer_cf.read().to_uint();

			if (layer == FC)
				num_filter_field = units;

			cout << "Fetching filter data into GLB... " << endl;
			for (int k = 0; k < num_channel_field; k++) {
				for (int i = 0; i < filter_height_field * num_filter_field; i++) {  // numerous filters are stored by rows in order
					for (int j = 0; j < filter_width_field; j++) {
						//filter[i][j][k].write(w_rdata.read());
						filter[i
						+ j * filter_height_field * num_filter_field
						+ k * filter_height_field * num_filter_field * filter_width_field].write(w_rdata.read());
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
	sc_uint<10> ifmap_height_field = 0;
	sc_uint<10> ifmap_width_field = 0;
	sc_uint<10> num_channel_field = 0;

	// Initialise
	//! have to substitute it into fixed size sram in practical.
	for (int k = 0; k < num_channel; k++) {
		for (int i = 0; i < ifmap_height; i++) {
			for (int j = 0; j < ifmap_width; j++) {
				ifmap[i
				+ j * ifmap_height
				+ k * ifmap_height * ifmap_width].write(0);
			}
		}
	}
	cout << "GLB ifmap memory has been initialised." << endl;
	wait(2);

	// Fetch iact data
	while (true) {
		if (conf_done.read() && !iact_done.read()) {
			wait(2);
			ifmap_height_field = ifmap_height_cf.read().to_uint();
			ifmap_width_field = ifmap_width_cf.read().to_uint();
			num_channel_field = num_channel_cf.read().to_uint();

			cout << "Fetching iact data into GLB... " << endl;
			for (int k = 0; k < num_channel_field; k++) {
				for (int i = 0; i < ifmap_height_field; i++) {
					for (int j = 0; j < ifmap_width_field; j++) {
						ifmap[i
						+ j * ifmap_height
						+ k * ifmap_height * ifmap_width].write(ifmap_rdata.read());
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
	sc_uint<4> filter_height_field = 0;
	sc_uint<4> filter_width_field = 0;
	sc_uint<10> num_channel_field = 0;
	sc_uint<10> num_filter_field = 0;
	sc_uint<13> units_field = 0;
	sc_uint<2> layer_field = 0;

	bool w_in_done = false;  // to control data in different channels. 
	int cnt_filter = 0;
	int channel = 0;

	// Initialise
	wait(2);

	while (true) {
		filter_height_field = filter_height_cf.read().to_uint();
		filter_width_field = filter_width_cf.read().to_uint();
		num_channel_field = num_channel_cf.read().to_uint();
		num_filter_field = num_filter_cf.read().to_uint();
		units_field = units_cf.read().to_uint();
		layer_field = layer_cf.read().to_uint();

		if (conf_done.read() && !layer_done.read()) {
			if (!w_in_done && w_done.read() && iact_done.read()) {  // transfer data after all data is stored in GLB.
				// transfer weight data to scheduler
				//!! Expanding write ports is not practical.
				//!! It has to increase the number of transfer clocks in practical.
				for (int num_units = 0; num_units < units_field; num_units++) {
					for (int num_column = 0; num_column < filter_width_field; num_column++) {
						for (int num_height = 0; num_height < filter_height_field; num_height++) {
							//w_wdata[num_height].write(filter[num_height + cnt_filter * filter_height + num_units * filter_height][num_column][channel].read());
							w_wdata[num_height].write(
								filter[num_height + cnt_filter * filter_height_field + num_units * filter_height_field
								+ num_column * filter_height_field * num_filter_field * units_field
								+ channel * filter_height_field * num_filter_field * units_field * filter_width_field].read());
						}
						wait();
					}
				}
				w_in_done = true;
			}
			else if (channel_done.read()) {
				if (channel == num_channel_field - 1)
					channel = 0;
				else
					channel++;
				
				if (filter_done.read()) {
					if (cnt_filter == num_filter_field - 1)
						cnt_filter = 0;
					else
						cnt_filter++;
				}

				w_in_done = false;
				wait();
			}
			else {
				for (int num_height = 0; num_height < filter_height_field; num_height++) {
					w_wdata[num_height].write(0);
				}
				wait();
			}
		}
		else {
			for (int num_height = 0; num_height < filter_height_field; num_height++) {
				w_wdata[num_height].write(0);
			}
			channel = 0;

			wait();
		}
	}
}

void GLB::GLB_send_ifmap(void) {
	sc_uint<10> ifmap_height_field = 0;
	sc_uint<10> ifmap_width_field = 0;
	sc_uint<10> num_channel_field = 0;
	sc_uint<10> num_filter_field = 0;
	sc_uint<3> padding_field = 0;
	sc_uint<13> units_field = 0;
	sc_uint<2> layer_field = 0;

	bool ifmap_in_done = false;
	int channel = 0;

	// Initialise
	in_vld.write(0);
	wait(2);

	while (true) {
		ifmap_height_field = ifmap_height_cf.read().to_uint();
		ifmap_width_field = ifmap_width_cf.read().to_uint();
		num_channel_field = num_channel_cf.read().to_uint();
		num_filter_field = num_filter_cf.read().to_uint();
		padding_field = padding_cf.read().to_uint();
		layer_field = layer_cf.read().to_uint();
		units_field = units_cf.read().to_uint();

		if (conf_done.read() && !layer_done.read()) {
			if (!ifmap_in_done && w_done.read() && iact_done.read()) {
				in_vld.write(1);

				// transfer iact data to scheduler
				if (padding_field)
					wait(padding_field);  // Let scheduler store padding zeros.
				for (int num_column = 0; num_column < ifmap_width_field; num_column++) {
					for (int num_height = 0; num_height < ifmap_height_field; num_height++) {
						ifmap_wdata[num_height].write(
							ifmap[num_height
							+ num_column * ifmap_height_field
							+ channel * ifmap_height_field * ifmap_width_field].read());
					}
					wait();
				}
				if (padding_field)
					wait(padding_field);
				if (layer_field == FC)
					wait(ifmap_width_field * (units_field - 1));

				ifmap_in_done = true;
			}
			
			else if (channel_done.read()) {
				if (channel == num_channel_field - 1)
					channel = 0;
				else
					channel++;
				ifmap_in_done = false;
				wait();
			}
			
			else {
				for (int num_height = 0; num_height < ifmap_height_field; num_height++) {
					ifmap_wdata[num_height].write(0);
				}

				in_vld.write(0);
				wait();
			}
		}
		else {
			for (int num_height = 0; num_height < ifmap_height_field; num_height++) {
				ifmap_wdata[num_height].write(0);
			}

			channel = 0;
			in_vld.write(0);
			wait();
		}
	}
}

void GLB::GLB_ofmap(void) {
	sc_uint<4> filter_height_field = 0;
	sc_uint<32> ofmap_height_field = 0;
	sc_uint<32> ofmap_width_field = 0;
	sc_uint<10> num_channel_field = 0;
	sc_uint<10> num_filter_field = 0;
	sc_uint<3> padding_field = 0;
	sc_uint<32> folding_field = 0;
	sc_uint<32> propass_field = 0;
	sc_uint<2> layer_field = 0;
	sc_uint<13> units_field = 0;

	int ofmap_tmp[ofmap_height] = {0};
	int ofmap_column = 0;
	int PE_width_act = 0;	
	int cnt_filter = 0;
	int ofmap_ch = 0;
	int cur_fold = 0;
	int cnt_pass = 0;
	int units_offset = 0;
	bool out_valid = false;

	// Initialise
	ofmap_channel.write(0);
	filter_done.write(0);
	layer_done.write(0);

	int tmp = 0;  // use to store intermediate ofmap
	while (true) {
		filter_height_field = filter_height_cf.read().to_uint();
		ofmap_height_field = ofmap_height_cf.read();
		ofmap_width_field = ofmap_width_cf.read();
		num_channel_field = num_channel_cf.read().to_uint();
		num_filter_field = num_filter_cf.read().to_uint();
		padding_field = padding_cf.read().to_uint();
		folding_field = folding_cf.read();
		propass_field = propass_cf.read();
		layer_field = layer_cf.read().to_uint();
		units_field = units_cf.read().to_uint();

		if (conf_done.read() && layer_done.read()) {
			// Clear all control signals   //!! and all counters ?
			channel_done.write(0);
			filter_done.write(0);

			// Write final results to DRAM
			for (int k = 0; k < num_filter_field; k++) {  // set one in FC layer.
				for (int i = 0; i < ofmap_height_field; i++) {  // equal to units in FC layer.
					for (int j = 0; j < ofmap_width_field; j++) {
						ofmap_result.write(
							ofmap_buf[i 
							+ j * ofmap_height_field
							+ k * ofmap_height_field * ofmap_width_field].read());
						wait();
					}
				}
			}
		}
		else if (conf_done.read()) {
			// Normal RS condition
			if (ofmap_height_field <= PE_width && filter_height_field <= PE_length) {
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
					for (int i = 0; i < ofmap_height_field; i++) {
						for (int j = 0; j < filter_height_field; j++) {
							if (out_vld[j][i].read())
								switch (layer_field) {
								case CONV:
									ofmap_tmp[i] += ofmap_out[j][i].read();
									break;
								case MAX:
									tmp = ofmap_out[j][i].read();
									ofmap_tmp[i] = std::max(ofmap_tmp[i], tmp);
									break;
								case AVG:
									cerr << "Error: Average pooling is not supported." << endl;
									cerr << "Warning: Simulation has been shut down." << endl;
									sc_stop();
									return;
								case FC:
									ofmap_tmp[i] += ofmap_out[j][i].read();
									break;
								default:
									cerr << "Error: Layer type can't be computed." << endl;
									cerr << "Warning: Simulation has been shut down." << endl;
									sc_stop();
									return;
								}
						}
						if ((layer_field == CONV)) {
							tmp = ofmap_buf[i 
								+ ofmap_column * ofmap_height_field
								+ cnt_filter * ofmap_height_field * ofmap_width_field].read();
							ofmap_buf[i
							+ ofmap_column * ofmap_height_field
							+ cnt_filter * ofmap_height_field * ofmap_width_field].write(tmp + ofmap_tmp[i]);

							psum_test_buf[i + cnt_filter * ofmap_height_field
							+ ofmap_column * ofmap_height_field * num_filter_field
							+ ofmap_ch * ofmap_height_field * num_filter_field * ofmap_width_field].write(ofmap_tmp[i]);	// Only for check intermediate ofmap		
						}
						else if (layer_field == FC) {
							tmp = ofmap_buf[i
							+ ofmap_column * ofmap_height_field
							+ 0].read();
							ofmap_buf[i
							+ ofmap_column * ofmap_height_field
							+ 0].write(tmp + ofmap_tmp[i]);

							psum_test_buf[i
							+ ofmap_column * ofmap_height_field * num_filter_field
							+ ofmap_ch * ofmap_height_field * num_filter_field * ofmap_width_field].write(ofmap_tmp[i]);
						}
						// do not accumulate in pooling layer
						else {
							ofmap_buf[i
							+ ofmap_column * ofmap_height_field
							+ cnt_filter * ofmap_height_field * ofmap_width_field].write(ofmap_tmp[i]);

							psum_test_buf[i + cnt_filter * ofmap_height_field
								+ ofmap_column * ofmap_height_field * num_filter_field
								+ ofmap_ch * ofmap_height_field * num_filter_field * ofmap_width_field].write(ofmap_tmp[i]);	// Only for check intermediate ofmap		

						}

						ofmap_tmp[i] = 0;  // clear after store in buffer
					}
					ofmap_column++;
					if (ofmap_column == ofmap_width_field) {
						ofmap_column = 0;
						ofmap_channel.write(ofmap_ch + 1);
						channel_done.write(1);

						if (ofmap_ch == num_channel_field - 1) {
							ofmap_channel.write(0);
							filter_done.write(1);
							cnt_filter++;

							if (cnt_filter == num_filter_field)
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
			else if (ofmap_height_field > PE_width && filter_height_field <= PE_length) {
				int remain_ofmap = ofmap_height_field - (propass_field - 1) * folding_field * PE_width;

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
					if (cnt_pass == propass_field - 1)
						cur_fold = remain_ofmap / PE_width + 1;
					else
						cur_fold = folding_field;

					for (int num_fold = 0; num_fold < cur_fold; num_fold++) {
						if ((cnt_pass == propass_field - 1) && (num_fold == cur_fold - 1))
							PE_width_act = remain_ofmap - (cur_fold-1) * PE_width;
						else
							PE_width_act = PE_width;

						for (int i = 0; i < PE_width_act; i++) {
							for (int j = 0; j < filter_height_field; j++) {
								if (out_vld[j + num_fold * filter_height_field][i].read())
									switch (layer_field) {
									case CONV:
										ofmap_tmp[i] += ofmap_out[j + num_fold * filter_height_field][i].read();
										break;
									case MAX:
										tmp = ofmap_out[j + num_fold * filter_height_field][i].read();
										ofmap_tmp[i] = std::max(ofmap_tmp[i], tmp);
										break;
									case AVG:
										cerr << "Error: Average pooling is not supported." << endl;
										cerr << "Warning: Simulation has been shut down." << endl;
										sc_stop();
										return;
									case FC:
										ofmap_tmp[i] += ofmap_out[j + num_fold * filter_height_field][i].read();

										break;
									default:
										cerr << "Error: Layer type can't be computed." << endl;
										cerr << "Warning: Simulation has been shut down." << endl;
										sc_stop();
										return;
									}
							}
							if (layer_field == CONV) {
								tmp = ofmap_buf[i + num_fold * PE_width + cnt_pass * folding_field * PE_width
								+ ofmap_column * ofmap_height_field
								+ cnt_filter * ofmap_height_field * ofmap_width_field].read();  // Accumulate mutiple channels
								ofmap_buf[i + num_fold * PE_width + cnt_pass * folding_field * PE_width
								+ ofmap_column * ofmap_height_field
								+ cnt_filter * ofmap_height_field * ofmap_width_field].write(tmp + ofmap_tmp[i]);

								psum_test_buf[i + cnt_filter * ofmap_height_field + num_fold * PE_width + cnt_pass * folding_field * PE_width
								+ ofmap_column * ofmap_height_field * num_filter_field
								+ ofmap_ch * ofmap_height_field * num_filter_field * ofmap_width_field].write(ofmap_tmp[i]);  // Only for check intermediate ofmap		
							}
							else if (layer_field == FC) {
								tmp = ofmap_buf[i + units_offset*PE_width
								+ ofmap_column * ofmap_height_field
								+ 0].read();
								ofmap_buf[i + units_offset * PE_width
								+ ofmap_column * ofmap_height_field
								+ 0].write(tmp + ofmap_tmp[i]);

								psum_test_buf[i + units_offset * PE_width
								+ ofmap_column * ofmap_height_field * num_filter_field
								+ ofmap_ch * ofmap_height_field * num_filter_field * ofmap_width_field].write(ofmap_tmp[i]);
							}
							// do not accumulate in pooling layer
							else {
								ofmap_buf[i + num_fold * PE_width + cnt_pass * folding_field * PE_width
								+ ofmap_column * ofmap_height_field
								+ cnt_filter * ofmap_height_field * ofmap_width_field].write(ofmap_tmp[i]);

								psum_test_buf[i + cnt_filter * ofmap_height_field + num_fold * PE_width + cnt_pass * folding_field * PE_width
									+ ofmap_column * ofmap_height_field * num_filter_field
									+ ofmap_ch * ofmap_height_field * num_filter_field * ofmap_width_field].write(ofmap_tmp[i]);  // Only for check intermediate ofmap		
							}
							ofmap_tmp[i] = 0;  // clear after store in buffer
						}

						units_offset++;
					}
					ofmap_column++;

					if (propass_field > 1) {
						if ((cnt_pass == propass_field - 1) && (ofmap_column == ofmap_width_field)) {
							ofmap_column = 0;
							cnt_pass = 0;
							units_offset = 0;
							ofmap_channel.write(ofmap_ch + 1);
							channel_done.write(1);

							if (ofmap_ch == num_channel_field - 1) {
								ofmap_channel.write(0);
								filter_done.write(1);
								cnt_filter++;

								if (cnt_filter == num_filter_field) {
									cnt_filter = 0;
									layer_done.write(1);
								}
							}
							wait();
						}
						else if (ofmap_column == ofmap_width_field) {
							ofmap_column = 0;
							cnt_pass++;
							wait();
						}
						else
							wait();
					}
					else {
						if (ofmap_column == ofmap_width_field) {
							ofmap_column = 0;
							ofmap_channel.write(ofmap_ch + 1);
							channel_done.write(1);

							if (ofmap_ch == num_channel_field - 1) {
								ofmap_channel.write(0);
								filter_done.write(1);
								cnt_filter++;

								if (cnt_filter == num_filter_field) {
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
				cerr << "Error: RS dataflow is supported, "
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

						if (psum_test_buf[j
						+ k * ofmap_height * num_filter 
						+ i * ofmap_height * num_filter * ofmap_width].read() != psum_tmp[i]) {
							cout << endl;
							cout << setw(30) << "psum[" << j << "][" << k << "] "
								<<"\treturn: " 
								<< psum_test_buf[j
								+ k * ofmap_height * num_filter
								+ i * ofmap_height * num_filter * ofmap_width].read()
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
			wait();
		}
		else
			wait();
	}
}