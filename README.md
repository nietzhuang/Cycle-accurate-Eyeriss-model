# Eyeriss-unfinished

Introduction:
As an IC designer and DNN accelerator researcher, I found that there exist several sophisticated designs of accelerators, however, Eyeriss project is the one inspiring me the most.
The efficiency and performance were both impressive at the time, even now Eyeriss still is. Moreover, the tutorial articles are incredibly in detail.
In fact, because many variant DNN models booming throughout the world are much difficult to design a corresponding accelerator such as MobileNet, SqueezeNet, old design accelerator might be lack optimisation. Eyeriss project has designed its better and far more sophisticated variant, Eyeriss v2.

This repository involves a mimic design referred to MIT Eyeriss project using SystemC. Now, it is near finished, however, there're several modifications that are worthy to note. 
	- At first, I intended to design a variant version to compute AlexNet via only one CPU event, in other words, this version is capable of inferencing entire data in the 	  on-chip caches and reuses not only the data but memory spaces. The current version is in the progress of this purpose.

	- Due to this aggressive purpose, it must have a very large amount of memory cells which is not practical both in industry and comparing benchmarks. For this reason, 	  	  I'll modify it to compute layer by layer.
	
	- There're many input/output signals not practical, for instance the ifmap_wdata, because it must be a fixed amount of signals. In order to reduce design period, I 		  determined them in advance.

	- Apart from TLM method of SystemC, I prefer to programme the entire circuit in the HDL manner. Afterwards, if there's enough time, I'll transport it into verilog HDL 	  	  to complete it as ASIC circuit. 



Current Progress:
Now, LowEyeriss is re-configurable and is capable of computing convolutional layer, maximal pooing, fully-connected layer separately including the multiple filters, channels and plenty of different parameters.


Next Progress:
Because of the impractical memory size, it has to resize the cache size and design it to separate layer computation into channel and filter chunks.



References:
1.  Chen, Yu-Hsin, Joel Emer, and Vivienne Sze. "Eyeriss: A spatial architecture for energy-efficient dataflow for convolutional neural networks." ACM SIGARCH Computer Architecture News 44.3 (2016): 367-379.
2.  Chen, Yu-Hsin, et al. "Eyeriss: An energy-efficient reconfigurable accelerator for deep convolutional neural networks." IEEE journal of solid-state circuits 52.1 (2016): 127-138.
3.  Chen, Yu-Hsin, et al. "Eyeriss v2: A flexible accelerator for emerging deep neural networks on mobile devices." IEEE Journal on Emerging and Selected Topics in Circuits and Systems 9.2 (2019): 292-308.
4.  Sze, Vivienne, et al. "Efficient processing of deep neural networks: A tutorial and survey." Proceedings of the IEEE 105.12 (2017): 2295-2329.
5.  Sze, Vivienne, et al. "Hardware for machine learning: Challenges and opportunities." 2017 IEEE Custom Integrated Circuits Conference (CICC). IEEE, 2017.
6.  Sze, Vivienne, et al. "Efficient processing of deep neural networks." Synthesis Lectures on Computer Architecture 15.2 (2020): 1-341.
