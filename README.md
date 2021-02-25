# 3D-Mem-Therm-I

With the growing power density in both processors and memories (esp. 3D), thermal issues significantly impact application performance. Thus, increasingly researchers have become interested in understanding the performance, power, and thermal effects of the proposed changes in hardware and software.

3D-Mem-Therm-I is an architectural simulation tool for processors and memory. It supports various main memory types and its integration to processors (2D DDR, 3D memory, 2.5D integration, and 3D integration of core and memory). It integrates Sniper (performance simulator for x86), McPAT (power model for processors), CACTI3DD (power model for memory), and HotSpot (thermal simulator) to periodically provide designers - performance, power, and thermal information for processor and memory.  

# Part 1
- We have extended the source code of Sniper multicore simulator developed by the Performance Lab research group at Ghent University, Belgium. This code extension provides us with DRAM access information per memory bank at a periodic interval (unlike Sniper which gives a total count of DRAM accesses). It emits the access count for read and write separately, which can be useful for memories having asymmetric read/write energy and delay (e.g., NVM).
- We also integrated it to periodically invoke HotSpot thermal simulator (leakage aware and supports 2D/3D memory as well). The access trace of memory is passed to HotSpot, which generates the temperature trace of the 3D memory. The core is also simulated for temperature.
- The tool also generates a video showing the thermal pattern for various time steps.

# Getting Started

- Installation
	- sudo dpkg --add-architecture i386
	- sudo apt-get install binutils build-essential curl git libboost-dev libbz2-dev libc6:i386 libncurses5:i386 libsqlite3-dev libstdc++6:i386 python wget zlib1g-dev
	- sudo apt-get install ffmpeg python-matplotlib (optional step - if thermal trace video generation is required)

- Compile
	- In the main folder, make # or use 'make -j N' where N is the number of cores in your machine to use parallel make
	- Go to the hotspot\_tool folder and run 'make' to compile the hotspot tool for memory temperature estimation
	- Go to the hotspot\_c\_tool folder and run 'make' to compile the hotspot tool for core temperature estimation
	- Configure the path of the hotspot tool and config directory in the config/gainstown\_my3D.cfg file (search for tool\_path and config\_path variables)

- Running an application 
	- cd test/app\_name
	- make run

- To see the DRAM accesses per memory bank, please use the application my\_test\_case inside test folder
	- To use this feature the application should be long enough to run for atleast 1 ms.
	- cd test/my\_test\_case
	- make run
- Sample output: Apart from Sniper messages and commandline, we see a detailed bank level trace for DRAM accesses. Please note the output like shown below in the terminal output.

```
   	Time	#READs	#WRITEs	#Access	Address		#BANK	Bank Counters

@& 	19000	0	0	0	7ffe33fd5140	80	Current:19021
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

@& 	20000	7943	0	7943	7ffe34052440	16	Current:20021
994,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,992,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,993,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,992,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,992,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,993,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,993,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,994,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

@& 	21000	8019	0	8019	7ffe340cf700	64	Current:21021
1002,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1003,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1003,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1003,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1002,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1002,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1002,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1002,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

@& 	22000	433	0	433	000000400c40	16	54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,


```
	- Multiple files containing simulation outputs will be generated (sim.cfg, sim.out etc)
	- If we sum the #Access in last three epochs (20000, 21000 and 22000 as all other epochs have no DRAM access), the number is same as "num dram accesses" under "DRAM summary" in sim.out file
	- If you are able to verify this, then you have successfully setup the system.


- To run the application on 32/48 cores with 16 channels
	- Use gainestown\_16channel\_32cores.cfg and gainestown\_16channel\_48cores.cfg respectively
	- For 32 cores, we are required to launch n=32 application instances so we collect and play back traces simultaneously on 32 different cores
	- To run on 32 cores with 16 DRAM channels architecture
		- cd test/multiple\_core
		- make run
		- The total number of DRAM accesses in detailed trace should match with number of DRAM accesses in sim.out file

- Understanding the output
	- To see the output corresponding to number of DRAM read/write accesses per bank, the application should run for atleast 1 ms. This is due to length of epoch that we use for counting the DRAM accesses and some other delays.
	- The output of `make run` displays the time interval in which a DRAM access was made, #reads and #writes and also reports the number of DRAM accesses directed to a particular bank.


- An example testcase which calls the thermal simulation periodically using hotspot is also created in the thermal\_example folder. The simulation can be configured for various memory and core configurations
	- cd test/thermal\_example
	- open Makefile and use appropriate config file (pre-designed) as per the following descriptions. The parameter type\_of\_stack in the config file controls the architecture type.
		- gainestown_DDR - 4x4 core and an external 4x4 bank DDR main memory (2D memory). It invokes two different hotspot runs to estimate temperatures for core and memory separately.
		- gainestown_3Dmem - 4x4 core and an external 4x4x8 banks 3D main memory. It invokes two different hotspot runs to estimate temperatures for core and memory separately.
		- gainestown_2_5D - 4x4 core and a 4x4x8 banks 3D main memory integrated on the same die (2.5D architecture). It invokes a single hotspot run and simulates core and memory together.
		- gainestown_3D - 4x4 core on top of a 4x4x8 banks 3D main memory. It invokes a single hotspot run and simulates core and memory together.
	- `make run > logfile`
	
    - Multiple files would be generated, but the useful ones are described below (these files would have \_mem and\_core suffix to indicate if they are for memory or core temperature simulation):
	- full\_temperature.trace - the temperature trace at periodic intervals for various banks and logic cores in the 3D memory. core trace is not generated in case of a 2.5D and 3D architecture.
	- logfile - the simulation output from the terminal. bank\_access\_counter lists the access counts for different banks
