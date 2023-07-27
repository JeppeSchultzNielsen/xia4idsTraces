//
// Created by jeppe on 7/27/23.
//
#include "dig_daq_param.hh"

using namespace std;
DigDaqParam::DigDaqParam() {
}

DigDaqParamINDiE::DigDaqParamINDiE(int modNo, int chanNo, TString detType, vector<double> &betaParams, vector<double> &gammaParams) {
    module_number = modNo;
    channel_number = chanNo;
    detType = detType;
    if(size(betaParams) != 4 || size(gammaParams) != 5){
        cout << "Wrong number of parameters for INDiE: module number " << modNo << " channel number " << chanNo << endl;
        return;
    }
    else{
        for(int i = 0; i < betaParams.size(); i++){
            this->betaParams[i] = betaParams[i];
        }
        for(int i = 0; i < gammaParams.size(); i++){
            this->gammaParams[i] = gammaParams[i];
        }
    }
    isReady = true;
}

double DigDaqParamINDiE::calculateBeta(double alpha) {
    return betaParams[0] + betaParams[1]*alpha+betaParams[2]*alpha*alpha+betaParams[3]*exp(-alpha*betaParams[4]);
}

double DigDaqParamINDiE::calculateGamma(double alpha) {
    return gammaParams[0] + gammaParams[1]*alpha + gammaParams[2]*alpha*alpha + gammaParams[3]*log(alpha);
}

int PmtFunction(const gsl_vector *x, void *FitData, gsl_vector *f) {
    size_t n = ((struct DigDaqParamINDiE::FitData *) FitData)->n;
    double *y = ((struct DigDaqParamINDiE::FitData *) FitData)->y;
    double beta = ((struct DigDaqParamINDiE::FitData *) FitData)->beta;
    double gamma = ((struct DigDaqParamINDiE::FitData *) FitData)->gamma;

    double phi = gsl_vector_get(x, 0);
    double alpha = gsl_vector_get(x, 1);
    double baseline = gsl_vector_get(x, 2);

    for (size_t i = 0; i < n; i++) {
        double t = i;
        double diff = t - phi;
        double Yi = 0;

        if (t < phi)
            Yi = 0;
        else
            Yi = alpha * exp(-beta * diff) *
                 (1 - exp(-pow(gamma * diff, 4.))) + baseline;

        gsl_vector_set(f, i, (Yi - y[i]) );
    }
    return (GSL_SUCCESS);
}

int CalcPmtJacobian(const gsl_vector *x, void *FitData, gsl_matrix *J){
    size_t n = ((struct DigDaqParamINDiE::FitData *) FitData)->n;
    double beta = ((struct DigDaqParamINDiE::FitData *) FitData)->beta;
    double gamma = ((struct DigDaqParamINDiE::FitData *) FitData)->gamma;

    double phi = gsl_vector_get(x, 0);
    double alpha = gsl_vector_get(x, 1);
    double baseline = gsl_vector_get(x, 2);

    double dphi, dalpha, dbaseline;

    for (size_t i = 0; i < n; i++) {
        double t = i;
        double diff = t - phi;
        double gaussSq = exp(-pow(gamma * diff, 4.));
        if (t < phi) {
            dphi = 0;
            dalpha = 0;
            dbaseline = 1;
        } else {
            dphi = alpha * beta * exp(-beta * diff) * (1 - gaussSq) -
                   4 * alpha * pow(diff, 3.) * exp(-beta * diff) *
                   pow(gamma, 4.) * gaussSq;
            dalpha = exp(-beta * diff) * (1 - gaussSq);
            dbaseline = 1;
        }
        gsl_matrix_set(J, i, 0, dphi);
        gsl_matrix_set(J, i, 1, dalpha);
        gsl_matrix_set(J, i, 2, dbaseline);
    }
    return (GSL_SUCCESS);
}

double DigDaqParamINDiE::calculatePhase(vector<unsigned int> &data, double alpha, double maxPos, double baseline) {
    gsl_multifit_function_fdf f;
    int info;
    const size_t n = data.size();
    size_t p = 3;
    double xInit[3];

    xInit[0] = maxPos;
    xInit[1] = alpha;
    xInit[2] = baseline;

    f.f = &PmtFunction;
    f.df = &CalcPmtJacobian;

    const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmsder;
    gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T, n, p);
    gsl_matrix *jac = gsl_matrix_alloc(n, p);
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    double *y = new double[n];
    double *weights = new double[n];
    struct FitData fitData = {n, y, calculateBeta(alpha), calculateGamma(alpha)};
    gsl_vector_view x = gsl_vector_view_array(xInit, p);
    gsl_vector_view w = gsl_vector_view_array(weights, n);

    static const unsigned int maxIter = 100;
    static const double xtol = 1e-4;
    static const double gtol = 1e-4;
    static const double ftol = 0.0;

    f.n = n;
    f.p = p;
    f.params = &fitData;

    for (unsigned int i = 0; i < n; i++) {
        weights[i] = 1;
        y[i] = data[i];
    }

    gsl_multifit_fdfsolver_wset(s, &f, &x.vector, &w.vector);
    gsl_multifit_fdfsolver_driver(s, maxIter, xtol, gtol, ftol, &info);
    gsl_multifit_fdfsolver_jac(s, jac);
    gsl_multifit_covar(jac, 0.0, covar);

    gsl_vector *res_f = gsl_multifit_fdfsolver_residual(s);

    double phase = gsl_vector_get(s->x, 0);

    gsl_multifit_fdfsolver_free(s);
    gsl_matrix_free(covar);
    gsl_matrix_free(jac);
    delete[] y;
    delete[] weights;

    return phase;
}