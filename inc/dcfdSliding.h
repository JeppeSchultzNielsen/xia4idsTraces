//
// Created by jeppe on 12/28/23.
//

#ifndef XIA4IDS_DCFDSLIDING_H
#define XIA4IDS_DCFDSLIDING_H

#include "Trace.hh"
#include "polyCfd.hh"

class DcfdSliding {
public:
    int L;
    int L2;
    int D;
    double f;
    PolyCfd* polyCfd = new PolyCfd(0.0);

    DcfdSliding(int L, int L2, int D, double f){
        this->L = L;
        this->L2 = L2;
        this->D = D;
        this->f = f;

    }

    double cfd(const Trace *trace, int i);

    double getPhase(const Trace *trace);

    double FF(const Trace *trace, int i);

    pair<int,int> findValidRange(const Trace *trace);
};

#endif //XIA4IDS_DCFDSLIDING_H
