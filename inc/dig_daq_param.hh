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
#include "polyCfd.hh"

#include <TString.h>
#include "iostream"
#include "TGraph.h"
#include "TFile.h"
#include "TLine.h"
#include "TCanvas.h"
#include "Trace.hh"
#include "TH1D.h"

using namespace std;
class DigDaqParam {
public:
    int module_number;
    int channel_number;
    TString detType = "NOT INIT";
    bool isReady = false;
    bool isSingleParam = false;
    bool freeBetaGamma = false;
    DigDaqParam();

    pair <double,double> calculatePhase(Trace &trace);
};

class DigDaqParamINDiE : public DigDaqParam {
public:
    double betaParams[5];
    double gammaParams[5];
    double singleBeta;
    double singleGamma;
    DigDaqParamINDiE(int modNo, int chanNo, TString detType, vector<double> &betaParams, vector<double> &gammaParams, bool freeBetaGamma);

    pair <double,double> calculatePhase(const Trace* trace);

    double calculateBeta(double alpha);
    double calculateGamma(double alpha);
    static double fitFun(double x, double phi, double alpha, double beta, double gamma);

    /// @brief Structure necessary for the GSL fitting routines
    struct FitData {
        size_t n;//!< size of the fitting parameters
        double *y;//!< ydata to fit
        double beta; //!< the beta parameter for the fit
        double gamma; //!< the gamma parameter for the fit
        bool freeBetaGamma;
    };


};

class DigDaqParamBeta : public DigDaqParam {
public:
    double betaParams[5];
    double gammaParams[5];
    double singleBeta;
    double singleGamma;
    DigDaqParamBeta(int modNo, int chanNo, TString detType, vector<double> &betaParams, vector<double> &gammaParams, bool freeBetaGamma);

    pair <double,double> calculatePhase(const Trace* trace);

    double calculateBeta(double alpha);
    double calculateGamma(double alpha);
    static double fitFun(double x, double phi, double alpha, double beta, double gamma);

    /// @brief Structure necessary for the GSL fitting routines
    struct FitData {
        size_t n;//!< size of the fitting parameters
        double *y;//!< ydata to fit
        double beta; //!< the beta parameter for the fit
        double gamma; //!< the gamma parameter for the fit
        bool freeBetaGamma;
    };
};

#endif //XIA4IDS_DIG_DAQ_PARAM_HH