#ifndef Definition_H
#define Definition_H
// Set project names
const char* const simulation_name	= "scEyeriss";
const char* const pattern_name		= "fc5x5x5_image5x5x5_units100";

// Set Testing options
const bool check_psum = true;  // Set if checking each partial sum is needed. (Set false is recommended if pattern is large.)
const bool check_conf = true;

// Set Configuration bitwidth
const int			conf_bit_bw		= 96;
const int			conf_phase		= conf_bit_bw / 32;

// Definition for NN layer 
enum enum_LayerType { CONV = 0, MAX = 1, AVG = 2, FC = 3 };
const int			layer			= FC;
const int			num_channel		= 5;
const int			num_filter		= 1;
const int			stride			= 5;  // set as filter height when FC layer
const int			padding			= 0;
const int			units			= 100;  // set one when not FC layer.
const int			filter_height	= 5;
const int			filter_width	= 5;
const int			ifmap_height	= 5;
const int			ifmap_width		= 5;
const int			ofmap_height	= (ifmap_height - filter_height + padding*2 + stride) * units / stride;  // units is used for FC layer.
const int			ofmap_width		= (ifmap_width - filter_width + padding*2 + stride) / stride;
const int			filter_size		= filter_height * filter_width;
const long long int ifmap_size		= ifmap_height * ifmap_width;
const int			ofmap_size		= ofmap_height * ofmap_width;

// Definition for PE array
enum enum_Dataflow  { OS = 0, WS = 1, IS = 2, RS = 3 };
const int			PE_length		= 12;  // Generally decided by filter height.
const int			PE_width		= 14;  // Generally decided by ofmap height.

// Definition for dataflow detail
const int			dataflow		= RS;
const int			folding			= PE_length / filter_height;
const int			propass			= ofmap_height / (PE_width * folding) + (ofmap_height % (PE_width * folding) > 0);

// Definition for processing element.
const int			spad_depth		= filter_height;  // Generally depends on filter height.

#endif
