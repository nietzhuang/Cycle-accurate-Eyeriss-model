# Eyeriss-unfinished

Introduction:
As an IC designer and DNN accelerator researcher, I found that there exists several sophisticated designs of accelerators, however, Eyeriss project is the one inspiring me the most.
The efficiency and performance were both impressive at the time, even now Eyeriss still is. Moreover, the tutorial articals are incredibly in detail.
In fact, because many variant DNN models booming throughout the world are much difficult to design a corresponding accelerator such as MobileNet, SqueezeNet, old design accelerator might be lack of optimisation. Eyeriss project has design its better and far more sophisticated variant, Eyeriss v2.

This repository involves a mimic design referred to MIT Eyeriss projecte using SystemC. Now, it is unfinished, however I intend to finish it as soon as possible. 
Afterwards, if there's enough time, I'll transpote it into verilog HDL to complete it as ASIC circuit. 


Current Progress:
Solved the problem of stack size overflow. Now, support computation when stride larger than 1 and add several related patterns.

Next Progress:
When the data of a layer is large amount, data in local buffer is missing. 



References:
1.  Chen, Yu-Hsin, Joel Emer, and Vivienne Sze. "Eyeriss: A spatial architecture for energy-efficient dataflow for convolutional neural networks." ACM SIGARCH Computer Architecture News 44.3 (2016): 367-379.
2.  Chen, Yu-Hsin, et al. "Eyeriss: An energy-efficient reconfigurable accelerator for deep convolutional neural networks." IEEE journal of solid-state circuits 52.1 (2016): 127-138.
3.  Chen, Yu-Hsin, et al. "Eyeriss v2: A flexible accelerator for emerging deep neural networks on mobile devices." IEEE Journal on Emerging and Selected Topics in Circuits and Systems 9.2 (2019): 292-308.
4.  Sze, Vivienne, et al. "Efficient processing of deep neural networks: A tutorial and survey." Proceedings of the IEEE 105.12 (2017): 2295-2329.
5.  Sze, Vivienne, et al. "Hardware for machine learning: Challenges and opportunities." 2017 IEEE Custom Integrated Circuits Conference (CICC). IEEE, 2017.
6.  Sze, Vivienne, et al. "Efficient processing of deep neural networks." Synthesis Lectures on Computer Architecture 15.2 (2020): 1-341.
