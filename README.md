# A Scalable Eyeriss Model in SystemC
Eyeriss is a well-known DNN accelerator originating from [MIT EEMS group](https://www.rle.mit.edu/eems/) in 2016. Chen Yu-Hsin et al. proposed Row-stationary dataflow along with a spatial architecture to achieve an energy efficent DNN accelerator. Relative publications can refer to [1] [2] [3] [4], otherwise, Eyeriss v2 is published in 2019 [5].

## Description
This repository designs Eyeriss in cycle-accurate SystemC. Unlike the original architecture, the sizes of on-chip buffers and the number of ports in this variant Eyeriss model remain dynamic when the shape of layer varies. This keeps the scalability when you want to use this design as a part of your implementation or measurement.

## Features
 * Support convolutional layer and fully-connected layer.
 * Support maximum pooling layer.
 * Support padding and strides.
 * Support mutiple channels and multiple filters.
 * Support row stationary dataflow.
 * Support intermediate psum check.
 * All submodules are implemented, including configuration register, global buffer, scheduler and PE array.
 * Provide scalable and dynamic buffers for ifmap, weights, ofmaps, and psum.
 * Provide scalable ports for trasferring ifmap, weights, ofmaps and psum.
 * Provide simple patterns and pattern generator in Python.
 * Testbench simulates as a SoC system where CPU configures and triggrs SC_Eyeriss, then SC_Eyeriss reads data from DRAM. 

## Dynamic buffers
### GLB.cpp
GLB module contains two buffers storing weights and ifmap stemporarily. These buffer sizes alter when the shape of processed layer varies. IF you want to fix the buffer size, SC_Eyeriss may need to access DRAM more than once when the buffers are too small to compute an entire layer.
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
RegFile module contains scratchpads as the PE computes convolutions in a manner of spatial architecture. The size of scratchpads in this module varies as the width of filter. If you want to fix the size, I would recommend the size as large as the largest filter width among the neural network model.
```C++
void RegFile::RegStream(void) {
	sc_int<8>		w_reg[spad_depth];
	sc_int<8>		ifmap_reg[spad_depth];
	sc_int<8>		psum_reg;
	//...		
}
```



## Usage

## References 
1.  Chen, Yu-Hsin, Joel Emer, and Vivienne Sze. "Eyeriss: A spatial architecture for energy-efficient dataflow for convolutional neural networks." ACM SIGARCH Computer Architecture News 44.3 (2016): 367-379.
2.  Chen, Yu-Hsin, et al. "Eyeriss: An energy-efficient reconfigurable accelerator for deep convolutional neural networks." IEEE journal of solid-state circuits 52.1 (2016): 127-138.
3.  Sze, Vivienne, et al. "Efficient processing of deep neural networks: A tutorial and survey." Proceedings of the IEEE 105.12 (2017): 
4.  Sze, Vivienne, et al. "Efficient processing of deep neural networks." Synthesis Lectures on Computer Architecture 15.2 (2020): 1-341.
5.  Chen, Yu-Hsin, et al. "Eyeriss v2: A flexible accelerator for emerging deep neural networks on mobile devices." IEEE Journal on Emerging and Selected Topics in Circuits and Systems 9.2 (2019): 292-308.
