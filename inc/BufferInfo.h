//
// Created by jeppe on 4/15/24.
//

#ifndef XIA4IDS_BUFFERINFO_H
#define XIA4IDS_BUFFERINFO_H

struct BufferInfo {
    unsigned int bcount; /// The total number of ldf buffers read from file.
    int spillEnd;
    int spillStart;
    unsigned int bpos;
};

#endif //XIA4IDS_BUFFERINFO_H
