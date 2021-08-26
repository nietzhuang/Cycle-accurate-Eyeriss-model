#ifndef Definition_H
#define Definition_H
// Set project names
const char* const simulation_name	= "Low Eyeriss";
const char* const pattern_name		= "filter11x11x3_ifmap163x163x3_stride4";

// Set Testing options
const bool check_psum = true;  // Set if checking each partial sum is needed. (Set false is recommended if pattern is large.)
const bool check_conf = true;

// Set Configuration bitwidth
const int			conf_bit_bw		= 64;

// Definition for NN layer 
const int			num_channel		= 3;
const int			num_filter		= 1;
const int			stride			= 4;
const int			padding			= 0;
const int			filter_height	= 11;
const int			filter_width	= 11;
const int			ifmap_height	= 163;
const int			ifmap_width		= 163;
const int			ofmap_height	= (ifmap_height - filter_height + padding*2 + stride) / stride;  // The division has to be without remainder.
const int			ofmap_width		= (ifmap_width - filter_width + padding*2 + stride) / stride;
const int			filter_size		= filter_height * filter_width;
const long long int ifmap_size		= ifmap_height * ifmap_width;
const int			ofmap_size		= ofmap_height * ofmap_width;

// Definition for PE array
enum enum_dataflow { OS = 1, WS = 2, IS = 3, RS = 4 };
const int			PE_length		= 12;  // Generally decided by filter height.
const int			PE_width		= 14;  // Generally decided by ofmap height.

// Definition for dataflow detail
const int			folding = PE_length / filter_height;
const int			propass = ofmap_height / (PE_width * folding) + 1;

// Definition for processing element.
const int			spad_depth		= filter_height;  // Generally depends on filter height.

#endif