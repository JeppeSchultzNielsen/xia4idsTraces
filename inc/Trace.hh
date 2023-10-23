//
// Created by jeppe on 8/3/23.
//

#ifndef XIA4IDS_TRACE_HH
#define XIA4IDS_TRACE_HH

#include "vector"
#include "iostream"

using namespace std;
class Trace {
public:
    vector<double> data;
    double baseline;
    double max;
    double maxPos;
    double phase;
    double qdc;
    int minBaseWidth;
    Trace(vector<unsigned int> data){
        for(int i = 0; i < data.size(); i++){
            this->data.push_back(data[i]);
        }
        minBaseWidth = 20;
    }
    void clear(){
        std::vector<double>().swap(data);
        baseline = 0;
        max = 0;
        maxPos = 0;
        phase = 0;
        qdc = 0;
    };

    void findTraceParams(); //sets baseline, max, maxPos, qdc
    void subtractBaseline();

    pair<double,double>  getBaselineAndStd(int from, int to);

    pair<double,double>  getRecursiveBaselineAndStd(int from, int to, double threshold, pair<double,double> prevPair);

};

#endif //XIA4IDS_TRACE_HH
