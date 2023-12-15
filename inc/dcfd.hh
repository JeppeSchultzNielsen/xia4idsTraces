//
// Created by jeppe on 12/12/23.
//

#include "Trace.hh"

#ifndef TRACEANALYSIS_DCFD_HH
#define TRACEANALYSIS_DCFD_HH

class Dcfd {
public:
    int FL;
    int FG;
    int D;
    double w;

    Dcfd(int FL, int FG, int D, double w){
        this->FL = FL;
        this->FG = FG;
        this->D = D;
        this->w = w;
    }

    double cfd(Trace *trace, int i);

    double getPhase(Trace *trace);

    double FF(Trace *trace, int i);

    pair<int,int> findValidRange(Trace *trace);
};
#endif //TRACEANALYSIS_DCFD_HH
