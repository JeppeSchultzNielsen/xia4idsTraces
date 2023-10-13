//
// Created by jeppe on 8/3/23.
//

#include "Trace.hh"
#include "dig_daq_param.hh"

void Trace::findTraceParams() {
    //find max, maxpos in data->GetTrace()
    maxPos = 0;
    max = 0;
    baseline = 0;
    qdc = 0;
    for(int i = 0; i < data.size(); i++){
        if(data[i] > max){
            max = data[i];
            maxPos = i;
        }
        qdc += data[i];
    }
    for(int i = 0; i < maxPos-20; i++){ //subtract 20 to make sure we are clear of the peak
        baseline += data[i];
    }
    baseline /= (maxPos-20);
    qdc = qdc - data.size()*baseline;
    max = max - baseline;
}

void Trace::subtractBaseline() {
    for(int i = 0; i < data.size(); i++){
        data[i] -= baseline;
    }
}