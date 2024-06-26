#ifndef XIA4IDS_HH // This needs to be unique in each header
#define XIA4IDS_HH

#include "TTree.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <random>

#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TROOT.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <iomanip>

#include "LDFReader.h"
#include "XiaData.h"
#include "Unpacker.h"

#include "dig_daq_param.hh"

#include "globals.h"
#include "BufferInfo.h"
#include <ausa/util/DynamicBranchVector.h>

class Xia4idsRunner{
public:

//  GLOBAL VARIABLES
//everything is initialized 0 by default

 float tot_run_time; //in seconds
 int   tot_sig_read;   //total number of signals read
 int run_good_chunks, run_missing_chunks; // For checking run integrity
 int raw_list_size, good_list_size; // For counting all the raw/processed signals
    DATA_buffer dataForFindSpills;
    int ldf_pos_index_for_findSpills = 0;


 FILE *fp_in;
 FILE *fp_out;
 char filename[1000];
 char outname[10000];
 char runname[1000];
 char outputName[1000];
 char ref_string[10], run_string[10], format_string[100], rate_root_string[1];
 unsigned long long int ref_unit, run_unit;
 int i, j, k, l, m, n, e, z, old_iData,
        iData, iEvt, totEvt,
        runstart, runnumber, runstop, runpart,
        timegate,
        detnum,
        maxnum[MAX_NUM_DETTYPES],
        dettypes,
        modules,
        cs_tac, pair_tac,
        reftype,
        flagtype, flag,
        fold;
 uint32_t blocklen;
 char check_block_size,
        end_of_file;
 uint64_t tref, next_tref;     //tref is global, it starts initialized as zero,
//we change it only when encountering a new reference.
//never reset it to zero !!! NO PROTONS LEFT BEHIND!!!
 double first_ts, last_ts;
 int config_coding[1000][8]; //config_coding[lines][columns]


// ADC identifiers
//         tmc[modnum][chnum] = det type
//  ntmc[type][modnum][chnum] = det number

 int  tmc                  [MAX_NUM_MOD][MAX_NUM_CHN],   // detector type identifier w.r.t. the module and channel number
ntmc[MAX_NUM_DETTYPES][MAX_NUM_MOD][MAX_NUM_CHN],   // detector number identifier
delay[MAX_NUM_MOD][MAX_NUM_CHN],        // timestamp delay to have the signals in sync
link_type[MAX_NUM_DETTYPES],           // link_type[type_to_be_linked]
start[MAX_NUM_DET][MAX_NUM_DETTYPES],   // start for the pair_tac
stop[MAX_NUM_DET][MAX_NUM_DETTYPES],    // stop for the pair_tac
lmc[MAX_NUM_MOD][MAX_NUM_CHN];        // line [mod][chan] -> returns the index of the config line

string typeNames[MAX_NUM_DETTYPES];

 int corr, list, gasp, root, stat, rate, rate_root, savetraces, onlyCoin, ausa;
 int list_evSize, reference_link;
 int corr_hist[MAX_NUM_DET][RANGE];
 int firstMod[MAX_NUM_DET], secondMod[MAX_NUM_DET], firstCh[MAX_NUM_DET], secondCh[MAX_NUM_DET];

 long long int multiplier; //low resolution time multiplier
//multiplier=1000;  // -> 1000*1ns = 1 microsecond

 int stats[3][MAX_NUM_MOD][MAX_NUM_CHN]; // stats[0=out-of-range, 1=pileup, 2=good][modnum][chnum]

 DigDaqParam* dig_daq_params[MAX_NUM_MOD][MAX_NUM_CHN];

 int have_cal;
 double calib[MAX_NUM_MOD][MAX_NUM_CHN][MAX_CAL];

 short int zero[10000];
 double start_clock;
 size_t nbytes;


 struct           Event *EventArray;
 struct   GaspRecHeader *GHeader;
 struct      dataStruct *DataArray;
 struct      dataStruct *TempArray;


// ROOT Variables and objects
 TFile *rootfile;
 TTree *tree;
 TH1F *hStats, *h[MAX_NUM_DET];

 double E_branch[MAX_NUM_DETTYPES][MAX_NUM_DET];
 double TI_branch[MAX_NUM_DETTYPES][MAX_NUM_DET];
 double T_branch[MAX_NUM_DETTYPES][MAX_NUM_DET] ;
 int F_branch[MAX_NUM_DETTYPES][MAX_NUM_DET] ;
 int   //E_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
//T_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
TRACELEN_branch[MAX_NUM_DETTYPES][MAX_NUM_DET],
        M_branch[MAX_NUM_DETTYPES],
        MULT_branch;
 unsigned int TRACE_branch[MAX_NUM_DETTYPES][MAX_NUM_DET][130]; //seems that in IS659 at least, traces always have length 130. Since it does not seem root allows multidimensional branches of variable size, this number has to be hard coded?
 ULong64_t  TIME_REF_branch;
 ULong64_t  TIME_RUN_branch;
 char root_string[MAX_NUM_DET][100];

 char *comment_line;
 char comment_string[MAX_NUM_DET][100];

 //ausa output stuff
 UInt_t detectorMuls[MAX_NUM_DETTYPES];
 UInt_t detectorMulsCheck[MAX_NUM_DETTYPES][MAX_NUM_DET];
 unique_ptr<AUSA::DynamicBranchVector<UInt_t>> DET_E[MAX_NUM_DETTYPES];
    unique_ptr<AUSA::DynamicBranchVector<UInt_t>> DET_T[MAX_NUM_DETTYPES];
    unique_ptr<AUSA::DynamicBranchVector<UInt_t>> DET_I[MAX_NUM_DETTYPES];

    // variables for ldf_read_spill
    // variables for reading dir buffer
    char* x = new char[2];
    unsigned int check_bufftype_dir, check_buffsize_dir, unknown[2], run_num;

    // variables for reading head buffer
    unsigned int check_bufftype_head, check_buffsize_head, run_num_2;
    char facility[9];
    char format[9];
    char type[17];
    char date[17];
    char run_title[81];

    // variables for reading data buffer
    bool full_spill;
    bool bad_spill;
    unsigned int nBytes;
    std::stringstream status;

    // variable that stores data spill
    unsigned int* data_ = new unsigned int[memoryuse];

    Xia4idsRunner(){};

    Xia4idsRunner(int argc, char **argv);

    int readSpill(int runNumber, int fileNumber, BufferInfo bufferInfo, string fileDestinationStem);

    pair<vector<BufferInfo>,int> findSpills(int runNumber, int fileNumber, BufferInfo bufferInfo, int nThreads, int spillsPerRead);

    void prepareFile(int runNumber, int fileNumber, BufferInfo bufferInfo, string fileDestinationStem);

    int xia4ids(int argc, char **argv, int lastRead);

    double calibrate(int module, int channel, int energy);

    void correlations();

    void event_builder();

    void define_root();

    void define_ausa();

    void event_builder_tree();

    void event_builder_ausa();

    void event_builder_list();

    void Merge(struct dataStruct *array, struct dataStruct *tempArray, int left, int mid, int right);

    void MergeSort(struct dataStruct *array, struct dataStruct *tempArray, int left, int right);

    void read_cal(int argc, char **argv);

    void read_config(int argc, char **argv, bool manualRun);

    void read_dig_daq_params(int argc, char **argv);

    int read_ldf(LDF_file& ldf, DATA_buffer& data, int& pos_index, int max_spills = -1);

    int read_ldf_spill(LDF_file& ldf, DATA_buffer& data, int& pos_index, BufferInfo bufferInfo, int max_spills = -1);

    void write_correlations();

    void write_gasp();

    void write_list();

    void write_stats();

    void printProgress(double percentage);

    void write_time(int current_block, int full_size);

};

#endif