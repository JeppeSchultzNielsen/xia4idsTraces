//
// Created by jeppe on 4/14/24.
//

#include "go4pixieUnpacker.h"

int main(int argc, char **argv){
    Go4PixieUnpacker *unpacker = new Go4PixieUnpacker(argc, argv);
    unpacker->run();
    return 0;
}