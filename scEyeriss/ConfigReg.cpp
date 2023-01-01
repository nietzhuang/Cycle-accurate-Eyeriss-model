#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "ConfigReg.h"

void CONFIGREG::config(void) {
	sc_bv<conf_bit_bw>	configuration_reg;

	// Declare configuration fields.
	sc_uint<13>		    units_field = 0;
	sc_uint<7>		    stride_field = 0;
	sc_uint<3>		    padding_field = 0;
	sc_uint<4>		    filter_height_field = 0;
	sc_uint<4>		    filter_width_field = 0;
	sc_uint<10>		    ifmap_height_field = 0;
	sc_uint<10>		    ifmap_width_field = 0;
	sc_uint<32>		    ofmap_height_field = 0;;
	sc_uint<32>		    ofmap_width_field = 0;;
	sc_uint<8>		    filter_size_field = 0;
	sc_uint<20>		    ifmap_size_field = 0;
	sc_uint<20>		    ofmap_size_field = 0;
	sc_uint<32>		    folding_field = 0;
	sc_uint<32>		    propass_field = 0;

	bool check = check_conf;
	int errors = 0;

	// Reset
	if (rst.read()) {
		configuration_reg = 0;
		conf_done.write(0);
		wait();
	}

	while (true) {
		if (cpu_trigger.read()) {
			wait();  // To avoid loading zero configuration bits.
			for (int i = 0; i < conf_phase; i++) {
				configuration_reg.range((i + 1) * 32 - 1, i * 32) = conf_bits.read();
				wait();
			}
			if (check) {
				if (configuration_reg.range(3, 0) != filter_height) {
					cerr << "Warning: filter height is configured wrong: " << configuration_reg.range(3, 0) << endl;
					errors++;
				}
				if (configuration_reg.range(7, 4) != filter_width) {
					cerr << "Warning: filter width is configured wrong: " << configuration_reg.range(7, 4) << endl;
					errors++;
				}
				if (configuration_reg.range(17, 8) != ifmap_height) {
					cerr << "Warning: ifmap height is configured wrong: " << configuration_reg.range(17, 8) << endl;
					errors++;
				}
				if (configuration_reg.range(27, 18) != ifmap_width) {
					cerr << "Warning: ifmap width is configured wrong: " << configuration_reg.range(27, 18) << endl;
					errors++;
				}
				if (configuration_reg.range(37, 28) != num_channel) {
					cerr << "Warning: number of channels is configured wrong: " << configuration_reg.range(37, 28) << endl;
					errors++;
				}
				if (configuration_reg.range(47, 38) != num_filter) {
					cerr << "Warning: number of filters is configured wrong: " << configuration_reg.range(47, 38) << endl;
					errors++;
				}
				if (configuration_reg.range(60, 48) != units) {
					cerr << "Warning: units number is configured wrong: " << configuration_reg.range(60, 48) << endl;
					errors++;
				}
				if (configuration_reg.range(67, 61) != stride) {
					cerr << "Warning: stride is configured wrong: " << configuration_reg.range(67, 61) << endl;
					errors++;
				}
				if (configuration_reg.range(70, 68) != padding) {
					cerr << "Warning: padding is configured wrong: " << configuration_reg.range(70, 68) << endl;
					errors++;
				}
				if (configuration_reg.range(72, 71) != dataflow) {
					cerr << "Warning: dataflow scheme is configured wrong: " << configuration_reg.range(72, 71) << endl;
					errors++;
				}
				if (configuration_reg.range(74, 73) != layer) {
					cerr << "Warning: shape of layer is configured wrong: " << configuration_reg.range(74, 73) << endl;
					errors++;
				}

				if (errors) {
					cerr << "Error: Configuration is wrong, "
						<< "computation is incapable in this layer." << endl;
					sc_stop();
					return;
				}
			}
			units_field = configuration_reg.range(60, 48).to_uint();
			stride_field = configuration_reg.range(67, 61).to_uint();
			padding_field = configuration_reg.range(70, 68).to_uint();
			filter_height_field = configuration_reg.range(3, 0).to_uint();
			filter_width_field = configuration_reg.range(7, 4).to_uint();
			ifmap_height_field = configuration_reg.range(17, 8).to_uint();
			ifmap_width_field = configuration_reg.range(27, 18).to_uint();

			if (layer == FC) {
				ofmap_height_field = units_field;
				ofmap_width_field = 1;
			}
			else {
				ofmap_height_field = (ifmap_height_field - filter_height_field + padding_field * 2 + stride_field) / stride_field;
				ofmap_width_field = (ifmap_width_field - filter_width_field + padding_field * 2 + +stride_field) / stride_field;
			}
			filter_size_field = filter_height_field * filter_width_field;
			ifmap_size_field = ifmap_height_field * ifmap_width_field;
			ofmap_size_field = ofmap_height_field * ofmap_width_field;
			folding_field = PE_length / filter_height_field;
			propass_field = ofmap_height_field / (PE_width * folding_field) + (ofmap_height_field % (PE_width * folding_field) > 0);

			// Write configuration of layer shape
			filter_height_cf.write(configuration_reg.range(3, 0));
			filter_width_cf.write(configuration_reg.range(7, 4));
			ifmap_height_cf.write(configuration_reg.range(17, 8));
			ifmap_width_cf.write(configuration_reg.range(27, 18));
			num_channel_cf.write(configuration_reg.range(37, 28));
			num_filter_cf.write(configuration_reg.range(47, 38));
			units_cf.write(configuration_reg.range(60, 48));
			stride_cf.write(configuration_reg.range(67, 61));
			padding_cf.write(configuration_reg.range(70, 68));
			filter_size_cf.write(filter_size_field);
			ifmap_size_cf.write(ifmap_size_field);
			ofmap_height_cf.write(ofmap_height_field);
			ofmap_width_cf.write(ofmap_width_field);
			ofmap_size_cf.write(ofmap_size_field);

			// Write configuration of computation
			dataflow_cf.write(configuration_reg.range(72, 71));
			layer_cf.write(configuration_reg.range(74, 73));
			folding_cf.write(folding_field);
			propass_cf.write(propass_field);

			conf_done.write(1);
		}
		else if (layer_done.read()) {
			conf_done.write(0);
			wait();
		}
		else {
			wait();
		}

	}
}
