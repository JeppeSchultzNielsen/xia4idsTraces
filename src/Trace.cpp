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
    for(int i = 0; i < data.size(); i++){
        if(data[i] > max){
            max = data[i];
            maxPos = i;
        }
    }

    //the 10 ticks after max pos and 15 ticks after we use to calculate the qdc
    int lowBound = 10;
    int highBound = 15;
    maxPos = 72;
    double stdThresh = 50;
    double secondStdThresh = 50;

    int qdcAdded = 0;
    for(int i = maxPos - lowBound; i < maxPos + highBound; i++){
        qdc += data[i];
        qdcAdded++;
    }

    auto basStdLeft = getRecursiveBaselineAndStd(0,maxPos-lowBound, stdThresh, make_pair(0,1e9));
    auto basStdRight = getRecursiveBaselineAndStd(maxPos + highBound,data.size(), stdThresh, make_pair(0,1e9));

    double std = 0;
    baseline = 0;


    if(basStdLeft.second > stdThresh and basStdRight.second > stdThresh){
        /*TH1D* traceHist = new TH1D(Form("Max %f Integral %f StdRight %f, StdLeft %f",max,qdc,basStdRight.second,basStdLeft.second),Form("Max %f Integral %f StdRight %f, StdLeft %f",max,qdc,basStdRight.second,basStdLeft.second),data.size(),0,data.size()-1);
        for(int i = 0; i < data.size(); i++){
            traceHist ->SetBinContent(i,data[i]);
        }
        TFile* file = TFile::Open("test.root", "update");
        traceHist -> Write();
        file->Close();*/
        qdc = 0;
        baseline = 0;
        return;
    }

    if(basStdLeft.second < basStdRight.second){
        std = basStdLeft.second;
        baseline = basStdLeft.first;
    }
    else{
        std = basStdRight.second;
        baseline = basStdRight.first;
    }

    /*int added = 0;
    auto basStd = getBaselineAndStd(0,maxPos-lowBound);
    baseline = basStd.first;
    double leftStd = basStd.second;
    if(leftStd > stdThresh){
        basStd = getBaselineAndStd(maxPos + highBound,data.size());
        double rightBaseline = basStd.first;
        double rightStd  = basStd.second;
        if(rightStd < stdThresh){
            //cout << "Choosing right baseline under threshold: " << endl;
           // cout << "calculate left baseline " << baseline << " with std " << std << " right baseline " << rightBaseline << " with std " << rightStd << endl;
            baseline = rightBaseline;
        }
        else{
            if(rightStd > secondStdThresh and leftStd > secondStdThresh){
                cout << "Rejecting event due to too large std: " << endl;
                cout << "calculate left baseline " << baseline << " with std " << leftStd << " right baseline " << rightBaseline << " with std " << rightStd << endl;
                qdc = 0;
                max = max - baseline;
                TH1D* traceHist = new TH1D(Form("Max %f Integral %f Baseline %f",max,qdc,baseline),Form("Max %f Integral %f Baseline %f",max,qdc,baseline),data.size(),0,data.size());
                for(int i = 0; i < data.size(); i++){
                    traceHist ->SetBinContent(i,data[i]);
                }
                TFile* file = TFile::Open("test.root", "update");
                traceHist -> Write();
                file->Close();
                return;
            }
            if(rightStd < leftStd){
                //cout << "Choosing right std under second threshold:" << endl;
                //cout << "calculate left baseline " << baseline << " with std " << std << " right baseline " << rightBaseline << " with std " << rightStd << endl;
                baseline = rightBaseline;
            }
            else{
                //cout << "Choosing left std under second threshold:" << endl;
                //cout << "calculate left baseline " << baseline << " with std " << std << " right baseline " << rightBaseline << " with std " << rightStd << endl;
            }
        }
    }
    else{
        //cout << "Accepting left baseline: " << endl;
        //cout << "calculate left baseline " << baseline << " with std " << std << endl;
    }*/
    //cout << "using baseline " << baseline << endl;
    qdc = qdc - baseline*qdcAdded;
    max = max - baseline;

    /*if(qdc > 150e3){
        TH1D* traceHist = new TH1D(Form("Max %f Integral %f Baseline %f",max,qdc,baseline),Form("Max %f Integral %f Baseline %f",max,qdc,baseline),data.size(),0,data.size());
        for(int i = 0; i < data.size(); i++){
            traceHist ->SetBinContent(i,data[i]);
        }
        if(maxPos > data.size()/2){
            //peak is in left side of trace
            int added = 0;
            for(int i = 0; i < maxPos-20; i++){ //subtract 20 to make sure we are clear of the peak
                baseline += data[i];
                cout << data[i] << endl;
                added++;
            }
            baseline /= added;
            cout << "baseline " << baseline << endl;
        }
        else{
            //peak is in right side of trace
            for(int i = maxPos+20; i < data.size(); i++){ //subtract 20 to make sure we are clear of the peak
                baseline += data[i];
                cout << data[i] << endl;
            }
            baseline /= 20;
            cout << "baseline " << baseline << endl;
        }
        TFile* file = TFile::Open("test.root", "update");
        traceHist -> Write();
        file->Close();
    }*/
}

void Trace::subtractBaseline() {
    for(int i = 0; i < data.size(); i++){
        data[i] -= baseline;
    }
}