//
// Created by jeppe on 4/17/24.
//

#ifndef XIA4IDS_GO4PIXIEUNPACKER_H
#define XIA4IDS_GO4PIXIEUNPACKER_H

#include "xia4ids.hh"
#include "thread"

class Go4PixieUnpacker {
public:
    Go4PixieUnpacker(int argc, char **argv);

    int threadCount = 1;
    int startRunNumber = 0;
    char *fileDestinationStem;
    thread threads[48];
    vector<Xia4idsRunner*> runners;
    Xia4idsRunner* spillFinder;
    char runname[1000];
    int argc;
    char **argv;
    int spillsPerRead = 10;

    void run();

    void multiReadSpills(vector<BufferInfo> bufferInfos, int runNumber, int fileNumber);

    pair <int,int> newFileExists(int runNumber, int fileNumber);

};

#endif //XIA4IDS_GO4PIXIEUNPACKER_H
