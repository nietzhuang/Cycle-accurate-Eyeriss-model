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
	int conf_phase = 2;
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
				configuration_reg.range( (i+1)*32-1, i*32 ) = conf_bits.read();
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
				if (configuration_reg.range(50, 48) != stride) {
					cerr << "Warning: stride is configured wrong: " << configuration_reg.range(50, 48) << endl;
					errors++;
				}
				if (configuration_reg.range(53, 51) != padding) {
					cerr << "Warning: padding is configured wrong: " << configuration_reg.range(53, 51) << endl;
					errors++;
				}
				//!!  Not designed yet.
				/*if (configuration_reg.range(55, 54) != dataflow) {
					cerr << "Warning: dataflow scheme is configured wrong: " << configuration_reg.range(55, 54) << endl;
					errors++;
				}
				if (configuration_reg.range(57, 56) != layer) {
					cerr << "Warning: shape of layer is configured wrong: " << configuration_reg.range(57, 56) << endl;
					errors++;
				}*/

				if (errors) {
					cerr << "Error: Configuration is wrong, "
						<< "computation is incapable in this layer." << endl;
					sc_stop();
					return;
				}
			}

			conf_done.write(1);
		}
		else
			wait();
	}
}