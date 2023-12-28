//
// Created by jeppe on 12/12/23.
//

#include "dcfd.hh"

double Dcfd::cfd(const Trace *trace, int i) {
    return FF(trace, i)*(1-w) - FF(trace, i-D);
}

double Dcfd::FF(const Trace *trace, int i) {
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

pair<int,int> Dcfd::findValidRange(const Trace *trace) {
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
    /*for(int i = minPos; i > 2*(FL+FG-1)+D; i--){
        double cfdVal = cfd(trace, i);
        if(cfdVal > 0){
            maxVal = cfdVal;
            maxPos = max(i-2,2*(FL+FG-1)+D);
            break;
        }
    }*/
    //find maxPos right of minPos
    for(int i = minPos; i < FL+FG; i--){
        double cfdVal = cfd(trace, i);
        if(cfdVal > 0){
            maxVal = cfdVal;
            maxPos = max(i+2,2*(FL+FG));
            break;
        }
    }
    return make_pair(maxPos, minPos);
}

double Dcfd::getPhase(const Trace *trace) {
    double prevCfd = -1;
    double cfdVal = 0;
    int breakPoint = -1;
    //cout << "FL: " << FL << " FG: " << FG << " D: " << D << endl;
    vector<double> cfdVals = {};
    auto fromTo  = findValidRange(trace);
    for(int i = 0; i < trace->data.size(); i++) {
        if(i < 2*(FL+FG-1)+D) {
            cfdVals.push_back(0);
        }
        else{
            cfdVal = cfd(trace, i);
            cfdVals.push_back(cfdVal);
        }
    }
    //cout << endl;
    for(int i = fromTo.second; i < trace->data.size(); i++){
        cfdVal = cfd(trace, i);
        cfdVals.push_back(cfdVal);
        /*cout << cfdVal << endl;
        cout << i << endl;
        cout << trace->data.size() << endl;*/
        if(cfdVal > 0 && prevCfd <= 0){
            breakPoint = i;
            //cout << "break" << endl;
            break;
        }
        prevCfd = cfdVal;
    }
    if(breakPoint == -1){
        return -10000;
    }
    double f = (prevCfd)/(prevCfd - cfdVal);
    double x = breakPoint - ((breakPoint-1)-breakPoint)*cfdVal/(prevCfd - cfdVal);
    vector<double> params = polyCfd->CalculatePoly3(cfdVals,breakPoint-2);
    double phase = polyCfd->getPol3Zero(params,breakPoint-1,breakPoint);
    vector<double>().swap(cfdVals);
    vector<double>().swap(params);
    if(phase < 0){
        return x; //use linear interpolation instead
    }
    else{
        return phase;
    }
}