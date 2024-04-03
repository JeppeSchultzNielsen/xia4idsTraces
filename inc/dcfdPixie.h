//
// Created by jeppe on 12/28/23.
//

#ifndef XIA4IDS_DCFDPIXIE_H
#define XIA4IDS_DCFDPIXIE_H

#include "Trace.hh"
#include "polyCfd.hh"

class DcfdPixie {
public:
    int FL;
    int FG;
    int D;
    double w;
    PolyCfd* polyCfd = new PolyCfd(0.0);

    DcfdPixie(int FL, int FG, int D, double w){
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


#endif //XIA4IDS_DCFDPIXIE_H
