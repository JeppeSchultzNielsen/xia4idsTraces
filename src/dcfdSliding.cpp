//
// Created by jeppe on 12/12/23.
//

#include "dcfdSliding.h"

double DcfdSliding::cfd(const Trace *trace, int i) {
    double sum = 0;
    for(int j = i; j < i+L; j++){
        if(j < 0){
            cout << "j is negative" << endl;
        }
        sum += f*trace->data[j]/(1.0*L);
    }
    for(int j = i-D; j < i-D+L2; j++){
        if(j < 0){
            cout << "j is negative" << endl;
        }
        sum -= (1.0*trace->data[j])/(1.0*L2);
    }
    return sum;
}

pair<int,int> DcfdSliding::findValidRange(const Trace *trace) {
    double minVal = 1e9;
    double maxVal = -1e9;
    int maxPos = L+D;
    int minPos = 0;
    //find minPos
    int maxLegal = max(L2+D,L);
    for(int i = D; i < trace-> data.size()-maxLegal; i++){
        double cfdVal = cfd(trace, i);
        //cout << "call with i " << i << " from findValidRange" << endl;
        if(cfdVal < minVal){
            minVal = cfdVal;
            minPos = i;
        }
    }
    //find maxPos left of minPos
    for(int i = minPos; i > D; i--){
        //cout << "call with i " << i << " from findValidRange" << endl;
        double cfdVal = cfd(trace, i);
        if(cfdVal > 0){
            maxVal = cfdVal;
            maxPos = max(i-2,D);
            break;
        }
    }
    return make_pair(maxPos, minPos);
}

double DcfdSliding::getPhase(const Trace *trace) {
    double prevCfd = 0;
    double cfdVal = 0;
    int breakPoint = -1;
    vector<double> cfdVals = {};
    auto fromTo  = findValidRange(trace);
    for(int i = 0; i < trace->data.size(); i++) {
        if(i < fromTo.first or i > fromTo.second) {
            cfdVals.push_back(0);
        }
        else{
            cfdVal = cfd(trace, i);
            cfdVals.push_back(cfdVal);
        }
    }
    for(int i = 0; i < trace->data.size(); i++){
        if(i < fromTo.first or i > fromTo.second) {
            cfdVals.push_back(0);
            continue;
        }
        //cout << "call with i " << i << " from getPhase" << endl;
        cfdVal = cfd(trace, i);
        cfdVals.push_back(cfdVal);
        if(cfdVal <= 0 && prevCfd > 0){
            breakPoint = i;
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
    //cout << phase << endl;
    //cout << endl;
    if(phase < 0){
        return x; //use linear interpolation instead
    }
    else{
        return phase;
    }
}