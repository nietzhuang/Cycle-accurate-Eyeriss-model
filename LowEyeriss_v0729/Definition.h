#ifndef Definition_H
#define Definition_H
// Definition for NN layer 
const int num_channel	= 1;
const int stride		= 1;
const int filter_height = 5;
const int filter_width  = 5;
const int ifmap_height  = 34;
const int ifmap_width	= 34;
const int ofmap_height  = (ifmap_height - filter_height + stride) / stride;
const int ofmap_width   = (ifmap_width - filter_width + stride) / stride;
const int filter_size   = filter_height * filter_width;
const int ifmap_size    = ifmap_height * ifmap_width;
const int ofmap_size    = ofmap_height * ofmap_width;

// Definition for PE array
enum enum_dataflow { OS = 1, WS = 2, IS = 3, RS = 4 };
const int PE_length		= 12;  // Generally decided by filter height.
const int PE_width		= 14;  // Generally decided by ofmap height   //ofmap width.
const int num_filter	= 1;
const int num_ifmap		= 1;
const int num_ofmap		= 1;

// Definition for dataflow detail
const int folding = PE_length / filter_height;
const int propass = ofmap_height / (PE_width * folding) + 1;

// Definition for processing element.
const int spad_depth	= filter_height;  // Generally depends on filter height.

#endif