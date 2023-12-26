//
// Created by jeppe on 12/12/23.
//

#include "Trace.hh"
#include "polyCfd.hh"

#ifndef TRACEANALYSIS_DCFD_HH
#define TRACEANALYSIS_DCFD_HH

class Dcfd {
public:
    int FL;
    int FG;
    int D;
    double w;
    PolyCfd* polyCfd = new PolyCfd(0.0);

    Dcfd(int FL, int FG, int D, double w){
        this->FL = FL;
        this->FG = FG;
        this->D = D;
        this->w = w;

    }

    double cfd(const Trace *trace, int i);

    double getPhase(const Trace *trace);

    double FF(const Trace *trace, int i);

    pair<int,int> findValidRange(const Trace *trace);
};
#endif //TRACEANALYSIS_DCFD_HH
