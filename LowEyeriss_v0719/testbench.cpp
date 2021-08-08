#include<iostream>
#include<fstream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "testbench.h"

void TESTBENCH::dram_weight(void) {
	int weight_tmp[filter_size][num_channel];
	
	//ifstream weight_file("weight.dat", ios::in);
	//ifstream weight_file("weight2.dat", ios::in);
	//ifstream weight_file("weight3.dat", ios::in);
	ifstream weight_file("weight4.dat", ios::in);
	if (!weight_file) {
		cerr << "Warning: Could not open weight.dat file." << endl;
		cerr << "Warning: Simulation has been shut down." << endl;		
		sc_stop();
		return;
	}

	for (int i = 0; i < filter_size; i++) {
		for (int j = 0; j < num_channel; j++) {
			weight_file >> weight_tmp[i][j];
		}	
	}

	// Initialise
	rst.write(1);
	wait();
	rst.write(0);
	wait();


	// Input weight data
	int addr_w;
	int w_ch;
	while (true) {
		addr_w = addr_w_in.read();
		
		w_ch = addr_w / filter_size;
		addr_w = addr_w % filter_size;
		w_rdata.write(weight_tmp[addr_w][w_ch]);
		wait();
	}
}

void TESTBENCH::dram_ifmap(void) {
	int ifmap_tmp[ifmap_size][num_channel];

	//ifstream ifmap_file("iact.dat", ios::in);
	//ifstream ifmap_file("iact2.dat", ios::in);
	//ifstream ifmap_file("iact3.dat", ios::in);
	ifstream ifmap_file("iact4.dat", ios::in);
	
	if (!ifmap_file) {
		cerr << "Warning: Could not open iact.dat file." << endl;
		cerr << "Warning: Simulation has been shut down." << endl;
		sc_stop();
		return;
	}

	for (int i = 0; i < ifmap_size; i++) {
		for (int j = 0; j < num_channel; j++) {
			ifmap_file >> ifmap_tmp[i][j];
		}
	}


	// Input input activation data
	int	addr_iact;
	int ifmap_ch;
	while (true) {
		addr_iact = addr_ifmpa_in.read();

		ifmap_ch = addr_iact / ifmap_size;
		addr_iact = addr_iact % ifmap_size;
		ifmap_rdata.write(ifmap_tmp[addr_iact][ifmap_ch]);

		wait();
	}
}

void TESTBENCH::results(void) {
	int ofmap_tmp;
	int cnt_out = 0;
	int errors = 0;

	//ifstream ofmap_file("ofmap.dat", ios::in);
	//ifstream ofmap_file("ofmap2.dat", ios::in);
	//ifstream ofmap_file("ofmap3.dat", ios::in);
	ifstream ofmap_file("ofmap4.dat", ios::in);

	if (!ofmap_file) {
		cerr << "Warning: Could not open ofmap.dat file." << endl;
		cerr << "Warning: Simulation has been shut down." << endl;
		sc_stop();
		return;
	}
	wait(2);


	int addr_ofmap = 0;
	while (true) {
		if (layer_done.read()) {
			wait(2);  // Delay two clock to avoid reading zero.

			cout << endl;
			cout << "1st Layer is done." << endl;
			cout << "Checking ofmap data..." << endl;
			cout << endl;

			for (int i = 0; i < ofmap_height; i++) {
				for (int j = 0; j < ofmap_width; j++) {
					ofmap_file >> ofmap_tmp;
					if (ofmap_result.read() != ofmap_tmp) {
						cout << "ofmap[" << i << "][" << j << "] return: "
							<< ofmap_result.read()
							<< "\tGolden value:" << ofmap_tmp
							<< endl;
						errors++;
					}
					wait();
				}
			}
			sc_stop();

			if (errors == 0)
				cout << "INFO: Simulation PASSED. " << endl;
			else
				cout << "INFO: Simulation FAILED with total error: " << errors << endl;
			return;
		}
		else
			wait();
	}
}