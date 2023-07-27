//
// Created by jeppe on 7/27/23. Adapted from GslFitter in PAASS
//

#ifndef XIA4IDS_DIG_DAQ_PARAM_HH
#define XIA4IDS_DIG_DAQ_PARAM_HH

#include <cmath>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_vector.h>

#include <TString.h>
#include "iostream"

using namespace std;
class DigDaqParam {
public:
    int module_number;
    int channel_number;
    TString detType;
    bool isReady = false;
    DigDaqParam();

    double calculatePhase(vector<unsigned int> &data, double alpha, double maxpos, double baseline);
};

class DigDaqParamINDiE : public DigDaqParam {
public:
    int module_number;
    int channel_number;
    TString detType;
    double betaParams[5];
    double gammaParams[5];
    DigDaqParamINDiE(int modNo, int chanNo, TString detType, vector<double> &betaParams, vector<double> &gammaParams);

    double calculatePhase(vector<unsigned int> &data, double alpha, double maxpos, double baseline);

    double calculateBeta(double alpha);
    double calculateGamma(double alpha);

    /// @brief Structure necessary for the GSL fitting routines
    struct FitData {
        size_t n;//!< size of the fitting parameters
        double *y;//!< ydata to fit
        double beta; //!< the beta parameter for the fit
        double gamma; //!< the gamma parameter for the fit
    };
};

#endif //XIA4IDS_DIG_DAQ_PARAM_HH