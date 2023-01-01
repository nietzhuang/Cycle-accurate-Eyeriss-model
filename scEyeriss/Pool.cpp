//#include <iostream>
#include <algorithm>
using namespace std;

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "Pool.h"

void POOL::max(void) {
	int			out;

	out = std::max(ifmap.read(), psum_tmp_RegtoPool.read());
	psum_tmp_PooltoReg.write(out);
}

/*
void POOL::avg(void) {


}*/