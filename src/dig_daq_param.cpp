//
// Created by jeppe on 7/27/23.
//
#include "dig_daq_param.hh"

using namespace std;
DigDaqParam::DigDaqParam() {
}

DigDaqParamINDiE::DigDaqParamINDiE(int modNo, int chanNo, TString detType, vector<double> &betaParams, vector<double> &gammaParams, bool freeBetaGamma) {
    this->module_number = modNo;
    this->channel_number = chanNo;
    this->detType = detType;
    this->freeBetaGamma = freeBetaGamma;
    if(betaParams.size() == 1 && gammaParams.size() == 1){
        this->singleBeta = betaParams[0];
        this->singleGamma = gammaParams[0];
        isSingleParam = true;
    }
    else if(betaParams.size() != 4 || gammaParams.size() != 5){
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

double DigDaqParamINDiE::calculateGamma(double alpha) {
    return gammaParams[0] + gammaParams[1]*alpha+gammaParams[2]*alpha*alpha+gammaParams[3]*exp(-alpha*gammaParams[4]);
}

double DigDaqParamINDiE::calculateBeta(double alpha) {
    return betaParams[0] + betaParams[1]*alpha + betaParams[2]*alpha*alpha + betaParams[3]*log(alpha);
}

double DigDaqParamINDiE::fitFun(double x, double phi, double alpha, double beta, double gamma){
    double t = x;
    double diff = t - phi;
    double Yi = 0;

    if (t < phi)
        Yi = 0;
    else
        Yi = alpha * exp(-beta * diff) *
             (1 - exp(-pow(gamma * diff, 4.)));
    return Yi;
}

int PmtFunction(const gsl_vector *x, void *FitData, gsl_vector *f) {
    size_t n = ((struct DigDaqParamINDiE::FitData *) FitData)->n;
    double *y = ((struct DigDaqParamINDiE::FitData *) FitData)->y;

    double phi = gsl_vector_get(x, 0);
    double alpha = gsl_vector_get(x, 1);
    double beta;
    double gamma;
    if(!((struct DigDaqParamINDiE::FitData *) FitData)->freeBetaGamma){
        beta = ((struct DigDaqParamINDiE::FitData *) FitData)->beta;
        gamma = ((struct DigDaqParamINDiE::FitData *) FitData)->gamma;
    }
    else{
        beta = gsl_vector_get(x, 2);
        gamma = gsl_vector_get(x, 3);
    }

    for (size_t i = 0; i < n; i++) {
        double t = i;
        double diff = t - phi;
        double Yi = 0;

        if (t < phi)
            Yi = 0;
        else
            Yi = alpha * exp(-beta * diff) *
                 (1 - exp(-pow(gamma * diff, 4.)));

        gsl_vector_set(f, i, Yi - y[i]);
    }
    return (GSL_SUCCESS);
}

int CalcPmtJacobian(const gsl_vector *x, void *FitData, gsl_matrix *J) {
    size_t n = ((struct DigDaqParamINDiE::FitData *) FitData)->n;

    double phi = gsl_vector_get(x, 0);
    double alpha = gsl_vector_get(x, 1);
    double beta;
    double gamma;
    if(!((struct DigDaqParamINDiE::FitData *) FitData)->freeBetaGamma){
        beta = ((struct DigDaqParamINDiE::FitData *) FitData)->beta;
        gamma = ((struct DigDaqParamINDiE::FitData *) FitData)->gamma;
    }
    else{
        beta = gsl_vector_get(x, 2);
        gamma = gsl_vector_get(x, 3);
    }

    double dphi, dalpha, dbeta, dgamma;

    for (size_t i = 0; i < n; i++) {
        /* Jacobian matrix J(i,j) = dfi / dxj, */
        double t = i;
        double diff = t - phi;
        double gaussSq = exp(-pow(gamma * diff, 4.));
        //derivatives of alpha * exp(-beta * diff) *
        //            (1 - exp(-pow(gamma * diff, 4.)))+baseline;
        if (t < phi) {
            dphi = 0;
            dalpha = 0;
            dbeta = 0;
            dgamma = 0;
        } else {
            dphi = alpha * beta * exp(-beta * diff) * (1 - gaussSq) -
                   4 * alpha  * pow(diff, 3.) * exp(-beta * diff) *
                   pow(gamma, 4.) * gaussSq;
            dalpha = exp(-beta * diff) * (1 - gaussSq);
            dbeta = -alpha * diff * exp(-beta * diff) * (1 - gaussSq);
            dgamma = 4 * alpha * pow(diff, 4.) * exp(-beta * diff) *
                     pow(gamma, 3.) * gaussSq;
        }
        gsl_matrix_set(J, i, 0, dphi);
        gsl_matrix_set(J, i, 1, dalpha);
        if(((struct DigDaqParamINDiE::FitData *) FitData)->freeBetaGamma) {
            gsl_matrix_set(J, i, 2, dbeta);
            gsl_matrix_set(J, i, 3, dgamma);
        }
    }
    return (GSL_SUCCESS);
}

pair <double,double> DigDaqParamINDiE::calculatePhase(const Trace* trace){
    gsl_multifit_function_fdf f;
    int info;
    const size_t n = trace->data.size();
    size_t p;
    double xInit[5];
    double beta;
    double gamma;
    if(isSingleParam){
        beta = singleBeta;
        gamma = singleGamma;
    }
    else{
        beta = calculateBeta(trace->max);
        gamma = calculateGamma(trace->max);
    }

    if(!freeBetaGamma){
        p = 2;
        xInit[0] = trace->maxPos;
        xInit[1] = trace->max;
    }
    else{
        p = 4;
        xInit[0] = trace->maxPos;
        xInit[1] = trace->max;
        xInit[2] = beta;
        xInit[3] = gamma;
    }

    f.f = &PmtFunction;
    f.df = &CalcPmtJacobian;

    const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmsder;
    gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T, n, p);
    gsl_matrix *jac = gsl_matrix_alloc(n, p);
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    double *y = new double[n];
    double *weights = new double[n];
    struct FitData fitData = {n, y, beta, gamma, freeBetaGamma};
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
        y[i] = trace -> data[i];
    }

    gsl_multifit_fdfsolver_wset(s, &f, &x.vector, &w.vector);
    gsl_multifit_fdfsolver_driver(s, maxIter, xtol, gtol, ftol, &info);
    gsl_multifit_fdfsolver_jac(s, jac);
    gsl_multifit_covar(jac, 0.0, covar);

    gsl_vector *res_f = gsl_multifit_fdfsolver_residual(s);

    double phase = gsl_vector_get(s->x, 0);
    double alphaFit = gsl_vector_get(s->x, 1);
    double betaFit;
    double gammaFit;
    if(freeBetaGamma) {
        betaFit = gsl_vector_get(s->x, 2);
        gammaFit = gsl_vector_get(s->x, 3);
    }
    else{
        betaFit = beta;
        gammaFit = gamma;
    }

    //cout << "From digdaq" <<  trace -> qdc << endl;

    //to test the fit visually: print histogram to root file -- extremely slow, do only to check
    /*TGraph* newGraph = new TGraph();
    TH1D* traceHist = new TH1D(Form("Max %f Integral %f",trace->max,trace->qdc),Form("Max %f Integral %f",trace->max,trace->qdc),trace->data.size(),0,trace->data.size());
    for(int i = 0; i < trace->data.size(); i++){
        newGraph->SetPoint(i, i, trace->data[i]);
        traceHist ->SetBinContent(i,trace->data[i]);
    }
    TGraph* newGraph2 = new TGraph();
    for(int i = 0; i < 1000; i++){
        double x = i/1000.0*130;
        double y = fitFun(x, phase, alphaFit, betaFit, gammaFit);
        newGraph2->SetPoint(i, x, y);
    }
    TCanvas *myCanv = new TCanvas(Form("test%f",phase),Form("test%f",phase),1000,1000);
    myCanv ->SetName(Form("Max %f Integral %f",trace->max,trace->qdc));
    newGraph2 ->SetLineColor(kBlack);
    //draw graph as line
    newGraph ->SetLineStyle(2);
    newGraph -> Draw("AL");
    newGraph2 ->SetLineColor(kRed);
    newGraph2 -> Draw("same");
    TLine* line = new TLine(phase, 0, phase, 0+alphaFit);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    line->Draw();
    TFile* file = TFile::Open("test.root", "update");
    myCanv -> Write();
    traceHist -> Write();
    file->Close();*/

    gsl_multifit_fdfsolver_free(s);
    gsl_matrix_free(covar);
    gsl_matrix_free(jac);
    delete[] y;
    delete[] weights;


    return make_pair(phase,alphaFit);
}

DigDaqParamBeta::DigDaqParamBeta(int modNo, int chanNo, TString detType, vector<double> &betaParams, vector<double> &gammaParams, bool freeBetaGamma) {
    this->module_number = modNo;
    this->channel_number = chanNo;
    this->detType = detType;
    this->freeBetaGamma = freeBetaGamma;
    if(betaParams.size() == 1 && gammaParams.size() == 1){
        this->singleBeta = betaParams[0];
        this->singleGamma = gammaParams[0];
        isSingleParam = true;
    }
    else if(betaParams.size() != 5 || gammaParams.size() != 5){
        cout << "Wrong number of parameters for Beta: module number " << modNo << " channel number " << chanNo  << endl;
        cout << "There are " << betaParams.size() << " beta params, there should be 5, and " << gammaParams.size() << "there should be 5." << endl;
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

double DigDaqParamBeta::calculateGamma(double alpha) {
    return gammaParams[0] + gammaParams[1]*alpha+gammaParams[2]*alpha*alpha+gammaParams[3]*exp(-alpha*gammaParams[4]);
}

double DigDaqParamBeta::calculateBeta(double alpha) {
    return gammaParams[0] + gammaParams[1]*alpha+gammaParams[2]*alpha*alpha+gammaParams[3]*exp(-alpha*gammaParams[4]);
}

double DigDaqParamBeta::fitFun(double x, double phi, double alpha, double beta, double gamma){
    double t = x;
    double diff = t - phi;
    double Yi = 0;

    if (t < phi)
        Yi = 0;
    else
    Yi = alpha * exp(-beta * diff) *
         (1 - exp(-pow(gamma * diff, 4.)));
    return Yi;
}

pair <double,double> DigDaqParamBeta::calculatePhase(const Trace* trace){
    gsl_multifit_function_fdf f;
    int info;
    const size_t n = trace -> data.size();
    size_t p;
    double xInit[5];
    double beta;
    double gamma;
    if(isSingleParam){
        beta = singleBeta;
        gamma = singleGamma;
    }
    else{
        beta = calculateBeta(trace -> max);
        gamma = calculateGamma(trace -> max);
    }

    if(!freeBetaGamma){
        p = 2;
        xInit[0] = trace -> maxPos;
        xInit[1] = trace -> max;
    }
    else{
        p = 4;
        xInit[0] = trace -> maxPos;
        xInit[1] = trace -> max;
        xInit[2] = 0.3;//beta;
        xInit[3] = 0.35;//gamma;
    }

    f.f = &PmtFunction;
    f.df = &CalcPmtJacobian;

    const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmsder;
    gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T, n, p);
    gsl_matrix *jac = gsl_matrix_alloc(n, p);
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    double *y = new double[n];
    double *weights = new double[n];
    struct FitData fitData = {n, y, beta, gamma, freeBetaGamma};
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
        y[i] = trace -> data[i];
    }

    gsl_multifit_fdfsolver_wset(s, &f, &x.vector, &w.vector);
    gsl_multifit_fdfsolver_driver(s, maxIter, xtol, gtol, ftol, &info);
    gsl_multifit_fdfsolver_jac(s, jac);
    gsl_multifit_covar(jac, 0.0, covar);

    gsl_vector *res_f = gsl_multifit_fdfsolver_residual(s);

    double phase = gsl_vector_get(s->x, 0);
    double alphaFit = gsl_vector_get(s->x, 1);
    double betaFit;
    double gammaFit;
    if(freeBetaGamma) {
        betaFit = gsl_vector_get(s->x, 2);
        gammaFit = gsl_vector_get(s->x, 3);
    }
    else{
        betaFit = beta;
        gammaFit = gamma;
    }

    //to test the fit visually: print histogram to root file -- extremely slow, do only to check
    /*TGraph* newGraph = new TGraph();
    for(int i = 0; i < trace->data.size(); i++){
        newGraph->SetPoint(i, i, trace->data[i]);
    }
    TGraph* newGraph2 = new TGraph();
    for(int i = 0; i < 1000; i++){
        double x = i/1000.0*130;
        double y = fitFun(x, phase, alphaFit, betaFit, gammaFit);
        newGraph2->SetPoint(i, x, y);
    }
    TCanvas *myCanv = new TCanvas(Form("test%f",phase),Form("test%f",phase),1000,1000);
    myCanv ->SetName(Form("Max %f Integral %f",trace->max,trace->qdc));
    newGraph2 ->SetLineColor(kBlack);
    //draw graph as line
    newGraph ->SetLineStyle(2);
    newGraph -> Draw("AL");
    newGraph2 ->SetLineColor(kRed);
    newGraph2 -> Draw("same");
    TLine* line = new TLine(phase, 0, phase, 0+alphaFit);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    line->Draw();
    TFile* file = TFile::Open("test.root", "update");
    myCanv -> Write();
    file->Close();*/

    gsl_multifit_fdfsolver_free(s);
    gsl_matrix_free(covar);
    gsl_matrix_free(jac);
    delete[] y;
    delete[] weights;


    return make_pair(phase,alphaFit);
}