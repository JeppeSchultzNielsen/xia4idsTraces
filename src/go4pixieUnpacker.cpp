//
// Created by jeppe on 4/17/24.
//

#include "go4pixieUnpacker.h"

Go4PixieUnpacker::Go4PixieUnpacker(int argc, char **argv) {
    //4th argument is thread count, 5th is start run number
    if (argc > 4) {
        threadCount = atoi(argv[4]);
    }
    if (argc > 5) {
        startRunNumber = atoi(argv[5]);
    }
    if (argc > 6) {
        fileDestinationStem = argv[6];
    }
    if (argc < 6) {
        printf("Usage: xia4ids [config_file_name] [input_file] [cal_file] [dig_daq_param_file] [thread_count] [start_run_number] [file_destination_stem]\n");
        return;
    }
    ROOT::EnableThreadSafety();

    spillFinder = new Xia4idsRunner(argc, argv);

    this->argc = argc;
    this->argv = argv;

    for (int i = 0; i < threadCount; i++) {
        Xia4idsRunner *runner = new Xia4idsRunner(argc, argv);
        runners.push_back(runner);
    }

    strcpy(runname,spillFinder -> runname);

}

void Go4PixieUnpacker::multiReadSpills(vector<BufferInfo> bufferInfos, int runNumber, int fileNumber){
    int runningThreads = 0;
    for(int i = 0; i < bufferInfos.size(); i++){
        runners[i]-> prepareFile(runNumber, fileNumber, bufferInfos[i], fileDestinationStem);
    }

    for(int i = 0; i < bufferInfos.size(); i++){
        threads[i] = thread(&Xia4idsRunner::readSpill, runners[i], runNumber, fileNumber, bufferInfos[i], fileDestinationStem);
        runningThreads++;
    }

    for(int i = 0; i < runningThreads; i++){
        threads[i].join();
    }
}

void Go4PixieUnpacker::run(){
    bool notDone = true;

    BufferInfo bufferInfoStart;
    bufferInfoStart.bcount = 0;
    bufferInfoStart.spillEnd = 0;//274761208;
    //bufferInfoStart.spillEnd = 0;
    int currentRunNumber = startRunNumber;
    int currentFileNumber = 0;
    int sleepCount = 0;
    while(notDone){
        //start of spills. last index is either exit code or index of next spill.
        auto spillsRetval = spillFinder -> findSpills(currentRunNumber, currentFileNumber, bufferInfoStart, threadCount, spillsPerRead);
        vector<BufferInfo> bufferInfos = spillsRetval.first;
        int retval = spillsRetval.second;

        /*for(int i = 0; i < bufferInfos.size(); i++){
            cout << i << endl;
            cout << bufferInfos[i].spillStart << endl;
            cout << bufferInfos[i].spillEnd << endl;
            cout << endl;
        }*/

        if(bufferInfos.size() == 1 && bufferInfos[0].spillEnd == -1){

        }else {
            multiReadSpills(bufferInfos, currentRunNumber, currentFileNumber);
            sleepCount = 0; //reset sleep count
        }

        if(retval < 10 && retval > 0){
            if(retval == 1 || retval == 2){
                //end of run, look for new run
                pair<int,int> newFile = newFileExists(currentRunNumber, currentFileNumber);
                if(newFile.first == -1){
                    cout << "No new file found, wait 1s" << endl;
                    if(!bufferInfos.size() == 0){
                        bufferInfoStart = bufferInfos[bufferInfos.size()-1];
                    }
                    this_thread::sleep_for(chrono::seconds(1));
                    continue;
                }
                else{
                    cout << "Starting with next file " << endl;
                    currentRunNumber = newFile.first;
                    currentFileNumber = newFile.second;
                    delete spillFinder;
                    bufferInfoStart.spillEnd = 0;
                    spillFinder = new Xia4idsRunner(argc, argv);
                    for(int i = 0; i < runners.size(); i++){
                        runners[i] -> ldf_pos_index_for_findSpills = 0;
                        runners[i] -> dataForFindSpills.Reset();
                    }
                    continue;
                }
            }
            else{
                if(sleepCount > 5){
                    cout << "No new file found after 5 tries, attempt to find new file" << endl;
                    pair<int,int> newFile = newFileExists(currentRunNumber, currentFileNumber);
                    if(newFile.first == -1){
                        cout << "No new file found, wait another 5 tries" << endl;
                        sleepCount = 0;
                        if(!bufferInfos.empty()){
                            //last spill not read correctly. start from there
                            if(bufferInfos[bufferInfos.size()-1].spillStart != -1) {
                                bufferInfoStart.spillEnd = bufferInfos[bufferInfos.size() - 1].spillStart;
                            }
                        }
                        continue;
                    }
                    else{
                        cout << "Starting with next file " << endl;
                        currentRunNumber = newFile.first;
                        currentFileNumber = newFile.second;
                        delete spillFinder;
                        bufferInfoStart.spillEnd = 0;
                        spillFinder = new Xia4idsRunner(argc, argv);
                        for(int i = 0; i < runners.size(); i++){
                            runners[i] -> ldf_pos_index_for_findSpills = 0;
                            runners[i] -> dataForFindSpills.Reset();
                        }
                        continue;
                    }
                    break;
                }
                //not eof or end of run, but no more completed spills. Wait for 1s and try again
                cout << "No more spills found, waiting for 5s" << endl;
                sleepCount++;
                this_thread::sleep_for(chrono::seconds(5));
                if(!bufferInfos.empty()){
                    //last spill not read correctly. start from there
                    bufferInfoStart.spillEnd = bufferInfos[bufferInfos.size()-1].spillStart;
                }
                continue;
            }
            break;
        }
        if(!bufferInfos.empty()){
            bufferInfoStart = bufferInfos[bufferInfos.size()-1];
        }
    }
    cout << "Finished reading all spills" << endl;
}

pair<int,int> Go4PixieUnpacker::newFileExists(int runNumber, int fileNumber){
    //first check if filenumber+1 exists
    char filename[100];
    sprintf(filename, "%s%03d-%d.ldf", runname, runNumber, fileNumber + 1);
    auto fp_in = fopen(filename, "rb");
    if (!fp_in) {
        cout << "File " << filename << " does not exist" << endl;
    }
    else{
        cout << "File " << filename << " exists " << endl;
        fclose(fp_in);
        return make_pair(runNumber, fileNumber + 1);
    }
    //if not, check if runnumber+1 exists
    sprintf(filename, "%s%03d.ldf", runname, runNumber+1);
    fp_in = fopen(filename, "rb");
    if (!fp_in) {
        cout << "File " << filename << " does not exist" << endl;
    }
    else{
        cout << "File " << filename << " exists " << endl;
        fclose(fp_in);
        return make_pair(runNumber+1, 0);
    }
    //there is no new file
    return make_pair(-1, -1);
}