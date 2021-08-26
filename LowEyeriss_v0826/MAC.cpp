#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "MAC.h"

void MAC::mac(void) {
	sc_int<8>		w;
	sc_int<8>		in;
	sc_int<8>		psum_tmp;
	sc_uint<8>		psum;

	w = weight.read();
	in = ifmap.read();
	psum_tmp = psum_tmp_RegtoMAC.read();

	psum = w * in + psum_tmp;
	psum_tmp_MACtoReg.write(psum);
}