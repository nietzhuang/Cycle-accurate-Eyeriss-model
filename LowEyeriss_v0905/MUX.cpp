#include <systemc>
using namespace sc_core;
using namespace sc_dt;

#include "Definition.h"
#include "MUX.h"

void MUX::mux(void) {
	switch (sel.read().to_uint()) {
	case CONV:  
		y.write(a.read());
		break;
	case MAX:
		y.write(b.read());
		break;
	case AVG:
		y.write(b.read()); //!! Not supported yet.
		break;
	case FC:
		y.write(a.read()); 
		break;
	}
}
