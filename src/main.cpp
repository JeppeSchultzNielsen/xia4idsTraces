//
// Created by jeppe on 10/24/23.
//

#include "main.h"

int main(int argc, char **argv){

    int lastRead = 0;
    for(int i = 0; i < 100; i++){
        auto x4ir = new xia4idsRunner(2);
        lastRead = x4ir -> xia4ids(argc, argv, lastRead);
        cout << "done " << endl;
        cout << lastRead << endl;
        cout << i << endl;
    }
}