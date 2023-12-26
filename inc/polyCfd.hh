//
// Created by jeppe on 10/23/23.
//

#ifndef XIA4IDS_POLYCFD_HH
#define XIA4IDS_POLYCFD_HH

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
//adapted from PAASS



using namespace std;

class PolyCfd {
public:
    double threshold;
    PolyCfd(double threshold){
        this->threshold = threshold;
    }
    vector<double> CalculatePoly2(const vector<double> &data, const unsigned int startBin);

    vector<double> CalculatePoly3(vector<double> data, const unsigned int &startBin);

    double CalcPoly2Phase(const std::vector<double> &data, unsigned int traceMax);

    double Get3PolyInterpMax(const std::vector<double> &data, unsigned int traceMax);

    double getPol3Zero(vector<double> &coeffs, double low, double high);

    vector<double> getPol3Coeffs(vector<double> &data, unsigned int traceMax);

    double CalcPoly3Phase(const std::vector<double> &data, unsigned int maxPos);
};

#endif //XIA4IDS_POLYCFD_HH
