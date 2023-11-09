//
// Created by jeppe on 10/24/23.
//

#ifndef XIA4IDS_GLOBALS_H
#define XIA4IDS_GLOBALS_H

#define MAX_NUM_DET      300
#define MAX_NUM_DETTYPES  60
#define MAX_NUM_CHN       17
#define MAX_NUM_MOD       15
#define MAX_CAL            5
#define memoryuse   10000000 // Size of the data array
#define max_num_spill     20 // Limit of number of spills to read into the data array.
#define reject_pileup      1 // Reject events marked as pileup (set to 0 to treat them as good data)
#define reject_out		   1 // Reject events marked as out-of-range (set to 0 to treat them as good data)
#define DEBUG		    false // Debug information from LDFReader.cpp
#define VERBOSE			true // Display information from Unpacker.cpp
#define RATE_EOF_MB		  10 // Size in MBytes to read from the end of file in ratemeter mode

#define HEAD 1145128264 /// Run begin buffer
#define DATA 1096040772 /// Physics data buffer
#define DIR 542263620   /// "DIR "
#define LDF_DIR_LENGTH 8192 /// Size of DIR buffer
#define LDF_DATA_LENGTH 8193 /// Maximum length of an ldf style DATA buffer.
#define ENDFILE 541478725 /// End of file buffer
#define ENDBUFF 0xFFFFFFFF /// End of buffer marker
#define ACTUAL_BUFF_SIZE 8194

////if digitizer unit time = 10ns
//#define US 100
//#define MS 100000
//#define  S 100000000

//XIA DGF Pixie-16 250MHz - digitizer unit time = 4ns
#define US 250
#define MS 250000
#define  S 250000000

//correlation prompt delay and histogram range
#define CORR_DELAY 30000
#define RANGE      60000

//progress bar specs
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 50
#define ACTUAL_BUFF_SIZE 8194


typedef struct dataStruct {
    double  time;
    double cfdtime;
    double traceIntegral;
    //short int energy;
    double energy;
    short int chnum;
    short int modnum;
    std::vector<unsigned int> trace;
} dataStruct;

typedef struct Event {
    short int elem[MAX_NUM_DET];
    short int evSize;
} Event;


typedef struct GaspRecHeader {
    short int head[16];
} GaspRecHeader;

#endif