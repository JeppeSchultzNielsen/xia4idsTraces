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
//adapted from PAASS



using namespace std;

class polyCfd {
public:
    vector<double> CalculatePoly2(vector<double> &data, const unsigned int &startBin);

    vector<double> CalculatePoly3(vector<double> &data, const unsigned int &startBin);

};

#endif //XIA4IDS_POLYCFD_HH
