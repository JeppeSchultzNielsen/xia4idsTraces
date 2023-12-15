//
// Created by jeppe on 8/3/23.
//

#include "Trace.hh"
#include "dig_daq_param.hh"

pair<double,double> Trace::getBaselineAndStd(int from, int to) {
    double calcBaseline = 0;
    int added = 0;
    for(int i = from; i < to; i++){
        calcBaseline += data[i];
        added++;
    }
    calcBaseline /= added;
    double calcStd = 0;
    for(int i = from; i < to; i++){
        calcStd += pow(data[i]-calcBaseline,2);
    }
    calcStd = sqrt(calcStd/added);
    return make_pair(calcBaseline,calcStd);
}

pair<double,double> Trace::getRecursiveBaselineAndStd(int from, int to, double threshold, pair<double,double> prevPair) {
    if(to-from < minBaseWidth){
        return prevPair;
    }
    auto basStd = getBaselineAndStd(from, to);
    if(basStd.second < threshold){
        return basStd;
    }
    if(basStd.second > prevPair.second){
        return prevPair;
    }

    else{
        //take the smallest stdev of the right/left interval
        auto basStdLeft = getRecursiveBaselineAndStd(from,to/2,threshold,basStd);
        auto basStdRight = getRecursiveBaselineAndStd(from+to/2,to,threshold,basStd);
        if(basStdLeft.second > basStdRight.second){
            return basStdRight;
        }else{
            return basStdLeft;
        }
    }
}

void Trace::findTraceParams() {
    //find max, maxpos in data->GetTrace()
    maxPos = 0;
    max = 0;
    baseline = 0;
    qdc = 0;
    int minPos = 0;
    min = 10000000;
    for(int i = 0; i < data.size(); i++){
        if(data[i] > max){
            max = data[i];
            maxPos = i;
        }
        if(data[i] < min){
            min = data[i];
            minPos = i;
        }
    }
    val0 = data[0];
    if( abs(min-val0) > abs(max-val0)){
        minGreaterThanMax = true;
    }

    double stdThresh = 50;
    double secondStdThresh = 50;

    int qdcAdded = 0;
    for(int i = 0; i < data.size(); i++){
        qdc += data[i];
        qdcAdded++;
    }

    double std = 0;
    baseline = 0;
    auto basStd = getBaselineAndStd(0,20);
    baseline = basStd.first;
    std = basStd.second;

    qdc = qdc - baseline*qdcAdded;
    max = max - baseline;
}

void Trace::subtractBaseline() {
    for(int i = 0; i < data.size(); i++){
        data[i] -= baseline;
    }
}