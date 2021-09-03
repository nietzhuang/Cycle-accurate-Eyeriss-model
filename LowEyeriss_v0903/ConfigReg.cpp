#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "ConfigReg.h"

void CONFIGREG::config(void) {
	sc_bv<conf_bit_bw>	configuration_reg;
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
			int units_tmp = configuration_reg.range(60, 48).to_uint();
			int	stride_tmp = configuration_reg.range(67, 61).to_uint();
			int	padding_tmp = configuration_reg.range(70, 68).to_uint();
			int filter_height_tmp = configuration_reg.range(3, 0).to_uint();
			int filter_width_tmp = configuration_reg.range(7, 4).to_uint();
			int ifmap_height_tmp = configuration_reg.range(17, 8).to_uint();
			int ifmap_width_tmp = configuration_reg.range(27, 18).to_uint();
			int ofmap_height_tmp;
			int ofmap_width_tmp;
			if (layer == FC) {
				ofmap_height_tmp = units_tmp;
				ofmap_width_tmp = 1;
			}
			else {
				ofmap_height_tmp = (ifmap_height_tmp - filter_height_tmp + padding_tmp * 2 + stride_tmp) / stride_tmp;
				ofmap_width_tmp = (ifmap_width_tmp - filter_width_tmp + padding_tmp * 2 + +stride_tmp) / stride_tmp;
			}
			int filter_size_tmp = filter_height_tmp * filter_width_tmp;
			int ifmap_size_tmp = ifmap_height_tmp * ifmap_width_tmp;
			int ofmap_size_tmp = ofmap_height_tmp * ofmap_width_tmp;
			int folding_tmp = PE_length / filter_height_tmp;
			int	propass_tmp = ofmap_height_tmp / (PE_width * folding_tmp) + (ofmap_height_tmp % (PE_width * folding_tmp) > 0);

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
			filter_size_cf.write(filter_size_tmp);
			ifmap_size_cf.write(ifmap_size_tmp);
			ofmap_height_cf.write(ofmap_height_tmp);
			ofmap_width_cf.write(ofmap_width_tmp);
			ofmap_size_cf.write(ofmap_size_tmp);

			// Write configuration of computation
			dataflow_cf.write(configuration_reg.range(72, 71));
			layer_cf.write(configuration_reg.range(74, 73));
			folding_cf.write(folding_tmp);
			propass_cf.write(propass_tmp);

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