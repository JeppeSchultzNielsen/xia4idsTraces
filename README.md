# xia4idsTraces

xia4idsTraces is a modification on the xia4ids code, which in turn is based on PAASS. It allows reading traces from binary .ldf files and performing various timing algorithms for neutron TOF with INDiE. It also allows an output format mimicking the output format of the UCESB unpacker. It also allows reading .ldf files piecewise using multithreading for use in the go4pixie project. 

## Build steps for xia4idsTraces

Prerequisites:
* CMake version >= 3.15
* ROOT version >= 6.xx
* AUSAlib
* GSL

Then, from the root of the xia4ids project run
```shell
mkdir build
cd build
cmake ../
make -j8
```

The executables "xia4idsTraces" and "go4pixie" will be built in the build directory.

## Authors and contributors
**Original xia4ids code:**
 * Razvan Lica, CERN / IFIN-HH, razvan.lica@cern.ch
 * Khai Phan, Tampere University - CERN Summer Student Project 2020, khai.phan@tuni.fi
 * James Cubiss, University of York, james.cubiss@york.ac.uk
 * Chris Page, University of York, chris.page@york.ac.uk

**xia4idsTraces code:**
 * Jeppe Schultz Nielsen, Aarhus University, jsnielsen@phys.au.dk


## Installation and running
There are two executables in xia4idsTraces.

**Xia4idsTraces** functions exactly like the old xia4ids code, but with the added functionality of reading traces from binary .ldf files. 
Additionally, the config file has been slightly altered. To get timing information from the traces, a "digital DAQ parameter file" must also be given, so the correct usage is:

```./xia4idsTraces [config_file] [calibration_file] [digital_daq_file]```

The digital DAQ file is a text file with the following format:
```
0	0	0	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
1	0	1	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
2	0	2	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
3	0	3	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
4	0	4	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
5	0	5	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
6	0	6	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
7	0	7	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
8	0	8	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
9	0	9	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
10	0	10	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
11	0	11	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
12	0	12	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
13	0	13	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
14	0	14	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
15	0	15	Beta	-usecfd=true	-FL=8	-FG=0	-D=4	-w=5.00000e-01
0	1	0	INDiE	-usePolyCfd=true	-w=0.540000
1	1	1	INDiE	-usePolyCfd=true	-w=0.540000
2	1	2	INDiE	-usePolyCfd=true	-w=0.540000
3	1	3	INDiE	-usePolyCfd=true	-w=0.540000
...
```
Where the first column is detector index, second is module, third is channel, fourth is detector type, and the rest are parameters for the digital DAQ.

**Go4pixie** will unpack .ldf files piecewise using multithreading. It will wait for a new file to appear after the previous file is no longer being written to, and it will only read up to the latest written spill in a file, and then wait for more spills. It then spits files out containing unpacked data, with each file containing ten spills. The usage is:

```./go4pixie [config_file] [calibration_file] [digital_daq_file] [noThreads] [startRunNo] [outputStem]```

The first three parameters are the same as for xia4idsTraces. The fourth parameter is the number of threads to use, the fifth parameter is the run number to start from (this file must exist when the program starts, but not necessarily the following runs. Runs are always expected to be subsequent, so if the run after 188 is 190, go4pixie will hang), and the sixth parameter is the stem of the output files. 
The output files will be named [outputStem][runNo] _ [partitionNo] _ [spillNo].root, with spillNo being the index in the .ldf file, where the last spill was read. A directory declared in outputstem will then continuously be filled with root files as go4pixie reads the .ldf files.

## Config file format
**This part is largely adopted from xia4ids.**
 * runName  = location of raw data including the name of the runs, not including the numbers
 * timegate = coincidence timegate
 * cs-tac, pair-tac, flagtype = specific detector types (only for GASPware - for ROOT set them to zero)
 * reftype  = the reference type (proton pulse)
 * ref_unit  = the unit for the event time versus the reference (Low Resolution Time)
 * run_unit  = the unit for the timestamp
 * Format = the format in which to convert the data 
      * gasp = GASPware
      * root = ROOT
      * list = binary event lists
      * stat = print statistics for the entire run, the event builder will be skipped
      * rate = only print statistics for the last buffer, the event builder will be skipped
      * ausa = ROOT output format mimicking the UCESB unpacker.
 * Fold = the number of coincidences that will trigger an event 
 * Traces = 1 if save traces, 0 if not. Only works in ROOT format.
 * OnlyCoin = Will only save INDiE/beta events if they are coincident.

Then follows the detector configuration. An example is: 

```
runName    /media/jeppe/2TB/RAW/run_
timegate   500
cs-tac     0
pair-tac   0
reftype    38
flagtype   0
ref_unit   1 us
run_unit   1 s
Format     ausa
Fold       1
Traces     0
OnlyCoin   0
outputName /media/jeppe/2TB/unpackedAUSA/run_

|Index		|Type		|Mod		|Chan	|Delay		|Link		|Start		|Stop		|Comment
1	1	1	0	0	0	0	0	I1
2	1	1	1	0	0	0	0	I1
1	2	1	2	0	0	0	0	I2
2	2	1	3	0	0	0	0	I2
1	3	1	4	0	0	0	0	I3
2	3	1	5	0	0	0	0	I3
1	4	1	6	0	0	0	0	I4
2	4	1	7	0	0	0	0	I4
...
```