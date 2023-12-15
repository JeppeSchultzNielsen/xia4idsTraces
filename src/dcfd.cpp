//
// Created by jeppe on 12/12/23.
//

#include "dcfd.hh"

double Dcfd::cfd(Trace *trace, int i) {
    return FF(trace, i)*(1-w) - FF(trace, i-D);
}

double Dcfd::FF(Trace *trace, int i) {
    double ff = 0;
    for(int j = i-(FL-1); j < i; j++){
        ff += trace->data[j];
    }
    for(int j = i-2*(FL+FG-1); j < i-(FL+FG); j++){
        if(j < 0){
            cout << "FL: " << FL << " FG: " << FG << " D: " << D << " i: " << i << " j: " << j << " upda " <<endl;
        }
        ff -= trace->data[j];
    }
    return ff;
}

pair<int,int> Dcfd::findValidRange(Trace *trace) {
    double minVal = 1e9;
    double maxVal = -1e9;
    int maxPos = 2*(FL+FG-1)+D;
    int minPos = 0;
    //find minPos
    for(int i = 2*(FL+FG-1)+D; i < trace-> data.size(); i++){
        double cfdVal = cfd(trace, i);
        if(cfdVal < minVal){
            minVal = cfdVal;
            minPos = i;
        }
    }
    //find maxPos left of minPos
    for(int i = minPos; i > 2*(FL+FG-1)+D; i--){
        double cfdVal = cfd(trace, i);
        if(cfdVal > 0){
            maxVal = cfdVal;
            maxPos = max(i-5,2*(FL+FG-1)+D);
            break;
        }
    }
    return make_pair(maxPos, minPos);
}

double Dcfd::getPhase(Trace *trace) {
    double prevCfd = 0;
    double cfdVal = 0;
    int breakPoint = 0;
    auto fromTo = findValidRange(trace);
    for(int i = fromTo.first; i < fromTo.second; i++){
        cfdVal = cfd(trace, i);
        if(cfdVal < 0 && prevCfd > 0){
            breakPoint = i;
            break;
        }
        prevCfd = cfdVal;
    }
    if(breakPoint == 0){
        return -10000;
    }
    double f = (prevCfd)/(prevCfd - cfdVal);
    double x = breakPoint - ((breakPoint-1)-breakPoint)*cfdVal/(prevCfd - cfdVal);
    return x;
}