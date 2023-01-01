# A Scalable Eyeriss Model in SystemC
Eyeriss is a well-known DNN accelerator originating from [MIT EEMS group](https://www.rle.mit.edu/eems/) in 2016. Chen Yu-Hsin et al. proposed Row-stationary dataflow along with a spatial architecture to achieve an energy-efficient DNN accelerator. Relative publications can refer to [1] [2] [3] [4], otherwise, Eyeriss v2 is published in 2019 [5].

## Description
This repository designs Eyeriss in cycle-accurate SystemC. Unlike the original architecture, the sizes of on-chip buffers and the number of ports in this variant Eyeriss model remain dynamic when the shape of layer varies. This keeps the scalability when you want to use this design as a part of your implementation or measurement.

## Features
 * Support convolutional layer and fully-connected layer.
 * Support maximum pooling layer.
 * Support padding and strides.
 * Support multiple channels and multiple filters.
 * Support row stationary dataflow.
 * Support intermediate psum check.
 * All submodules are implemented, including configuration register, global buffer, scheduler and PE array.
 * Provide scalable and dynamic buffers for ifmap, weights, ofmaps, and psum.
 * Provide scalable ports for transferring ifmap, weights, ofmaps and psum.
 * Provide simple patterns and a pattern generator in Python.
 * Testbench simulates as an SoC system where CPU configures and triggrs SC_Eyeriss, then SC_Eyeriss reads data from DRAM. 

## Dynamic buffers
### GLB.cpp
GLB module contains two buffers storing weights and ifmaps temporarily. These buffer sizes alter when the shape of processed layer varies. If you want to fix the buffer size, SC_Eyeriss may need to access DRAM more than once when the buffers are too small to compute an entire layer.
```C++
void GLB::GLB_read_weight(void) {
	//...
	if(layer == FC) {
		filter[i
			+ j * filter_height_field * units_field
			+ k * filter_height_field * units_field * filter_width_field].write(0);
	//...
	else {
		filter[i
			+ j * filter_height_field * num_filter_field
			+ k * filter_height_field * num_filter_field * filter_width_field].write(0);
	//...
	// Fetch weight data
	//...
		filter[i
			+ j * filter_height_field * num_filter_field
			+ k * filter_height_field * num_filter_field * filter_width_field].write(w_rdata.read());
	//...
}

void GLB::GLB_read_ifmap(void) {
	//...
	ifmap[i
		+ j * ifmap_height
		+ k * ifmap_height * ifmap_width].write(0);
	//...
	// Fetch iact data
	//...
	ifmap[i
		+ j * ifmap_height
		+ k * ifmap_height * ifmap_width].write(ifmap_rdata.read());
	//...
```
### Scheduler.cpp
Scheduler module also contains weights and ifmap buffers, however, these buffers can be removed since GLB already has enough buffers to store data within a layer. Similarly, you can fix the buffer size. 
```C++
void SCHEDULER::weight_scheduling(void) {
	sc_int<8>* weight_tmp = new sc_int<8>[(filter_height * units) * (filter_width)];
	//...
	for (int i = 0; i < filter_height * units; i++) {
		for (int j = 0; j < filter_width; j++) {
			//weight_tmp[i][j] = 0;
			weight_tmp[i + j * filter_height  * units] = 0;
		}
	}
	//...
}

void SCHEDULER::ifmap_scheduling(void) {
	sc_int<8>*	ifmap_tmp = new sc_int<8> [(ifmap_height+padding*2) * (ifmap_width+padding*2)];
	//...
	for (int i = 0; i < ifmap_height + padding * 2; i++) {
		for (int j = 0; j < ifmap_width + padding * 2; j++) {
			// ifmap_tmp[ifmap_height][ifmap_width];
			ifmap_tmp[i + j * ifmap_height] = 0; 
		}
	}
	//...
}
```
### RegFile.cpp
RegFile module contains scratchpads as the PE computes convolutions in a manner of spatial architecture. The size of scratchpads in this module varies with the width of filter. If you want to fix the size, I would recommend a size as large as the largest filter width in the neural network model.
```C++
void RegFile::RegStream(void) {
	sc_int<8>		w_reg[spad_depth];
	sc_int<8>		ifmap_reg[spad_depth];
	sc_int<8>		psum_reg;
	//...		
}
```



## Usage
### Utility
`LayerCompute.py` is a layer generator, producing the data of iacts, weights, psums, and ofmaps among convolutional, fully-connected and maximum pooling layers. Also, it will generate both a comprehensible format and a memory-like format. First, set the parameters as the shape of layer. You can refer to the existent layer shape in `Pattern_list.txt`, It is suggested that the pattern_name sets as filter_height_width_channel_number_image_height_width_channel, and the layer should set as 'CONV', 'FC' or 'MAX'.
``` Python
# Set parameters
pattern_name                = 'filter3x3x1x3_image5x5x1'
dataflow                    = 'RS'
layer                       = 'CONV'
channels                    = 1
filter_num                  = 3
units                       = 1  # Set 1 if it isn't FC layer 
filter_height, filter_width = (3, 3)
ifmap_height, ifmap_width   = (5, 5)
pool_height, pool_width     = (1, 1)
stride                      = 1
padding                     = 0
```
After generated, a folder named `Patterns` will produce, consisting of the data in layer shape format in folder `Patterns` and memory-like format in folder `Convert2C++`. Next, create a new folder named the pattern_name as filter3x3x1x3_image5x5x1 in this example in `./scEyeriss/Patterns/.`. And move the dat files in `Convert2C++` to this new pattern folder. It is also suggested that rename the dat files as config.dat, iact.dat, ofmap.dat, psum.dat, and weight.dat. Note that the psum.dat is used for psum check.

### Run simulation 
After the pattern data is set appropriately, edit the `Definition.h` in folder `scEyeriss`. Change the pattern_name and corresponding parameters. There are two testing options. `check_conf` will check whether the configuration bits in `config.dat` fit the parameters in `Definition.h`, `check_psum` will check the intermediate partial sums every time the PEs sum up vertically.
``` C++ 
// Set project names
const char* const simulation_name	= "scEyeriss";
const char* const pattern_name		= "filter3x3x1x3_image5x5x1";

// Set Testing options
const bool check_psum = true;  // Set if checking each partial sum is needed. (Set false is recommended if the pattern is large.)
const bool check_conf = true;

// Definition for NN layer 
enum enum_LayerType { CONV = 0, MAX = 1, AVG = 2, FC = 3 };
const int			layer			= CONV;
const int			num_channel		= 1;
const int			num_filter		= 3;
const int			stride			= 1;  // set as filter height when FC layer
const int			padding			= 0;
const int			units			= 1;  // set one when not a FC layer.
const int			filter_height		= 3;
const int			filter_width		= 3;
const int			ifmap_height		= 5;
const int			ifmap_width		= 5;
//...
```
Then, run it or press F5 if you use Visual Studio. You will see the following information in the terminal when you pass both psum and ofmap checks.

```
Info: (I804) /IEEE_Std_1666/deprecated: deprecated function: sc_set_default_time_unit
INFO: Elaborating Low Eyeriss
INFO: Simulating Low Eyeriss with Pattern: filter3x3x1x3_image5x5x1
GLB ifmap memory has been initialised.
GLB filter memory has been initialised.
Scheduler weight has been initialised.
Scheduler ifmap has been initialised.
...
Checking psum buffer...
Checking channel 0...   pass.
...
Layer is done.
Checking 1-th ofmap data...
INFO: Simulation PASSED.

Info: /OSCI/SystemC: Simulation stopped by user.
INFO: Post-processing scEyeriss
INFO: Simulation scEyeriss is Done.
```




## References 
1.  Chen, Yu-Hsin, Joel Emer, and Vivienne Sze. "Eyeriss: A spatial architecture for energy-efficient dataflow for convolutional neural networks." ACM SIGARCH Computer Architecture News 44.3 (2016): 367-379.
2.  Chen, Yu-Hsin, et al. "Eyeriss: An energy-efficient reconfigurable accelerator for deep convolutional neural networks." IEEE journal of solid-state circuits 52.1 (2016): 127-138.
3.  Sze, Vivienne, et al. "Efficient processing of deep neural networks: A tutorial and survey." Proceedings of the IEEE 105.12 (2017): 
4.  Sze, Vivienne, et al. "Efficient processing of deep neural networks." Synthesis Lectures on Computer Architecture 15.2 (2020): 1-341.
5.  Chen, Yu-Hsin, et al. "Eyeriss v2: A flexible accelerator for emerging deep neural networks on mobile devices." IEEE Journal on Emerging and Selected Topics in Circuits and Systems 9.2 (2019): 292-308.
