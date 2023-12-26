#include "polyCfd.hh"


using namespace std;

vector<double> PolyCfd::CalculatePoly2(const vector<double> &data, const unsigned int startBin){
    if (data.size() < 3)
        throw range_error("Polynomial::CalculatePoly2 - The data vector "
        "had the wrong size : " + std::to_string(data.size()));

    double x1[3], x2[3];
    for (size_t i = 0; i < 3; i++) {
    x1[i] = (startBin + i);
    x2[i] = std::pow(startBin + i, 2);
    }

    double denom =
            (x1[1] * x2[2] - x2[1] * x1[2]) - x1[0] * (x2[2] - x2[1] * 1) +
            x2[0] * (x1[2] - x1[1] * 1);

    double p0 = ((data[x1[0]] * (x1[1] * x2[2] - x2[1] * x1[2]) -
                  x1[0] *
                  (data[x1[1]] * x2[2] - x2[1] * data[x1[2]]) +
                  x2[0] *
                  (data[x1[1]] * x1[2] - x1[1] * data[x1[2]])) / denom);
    double p1 = (((data[x1[1]] * x2[2] - x2[1] * data[x1[2]]) -
                  data[x1[0]] * (x2[2] - x2[1] * 1) +
                  x2[0] * (data[x1[2]] - data[x1[1]] * 1)) / denom);
    double p2 = (((x1[1] * data[x1[2]] - data[x1[1]] * x1[2]) -
                  x1[0] * (data[x1[2]] - data[x1[1]] * 1) +
                  data[x1[0]] * (x1[2] - x1[1] * 1)) / denom);
    //Put the coefficients into a vector in ascending power order
    vector<double> coeffs = {p0, p1, p2};
    return coeffs;
}

double PolyCfd::CalcPoly2Phase(const std::vector<double> &data, unsigned int maxPos){
    double pol2Phase_=-9999;
    float multiplier = 1.;

    if(maxPos < 5){
        return -10000; //broken trace
    }
    //maxPos is the maximum point of the raw trace, traceMax is the maximum interpolated from 4 points surrounding the max.
    double traceMax = Get3PolyInterpMax(data, maxPos);

    vector<double> result;
    for (unsigned int cfdIndex = maxPos; cfdIndex > 0; cfdIndex--) {
        if (data[cfdIndex - 1] < threshold*traceMax && data[cfdIndex] >= threshold*traceMax) {
            // Fit the rise of the trace to a 2nd order polynomial.
            result = CalculatePoly2(data, cfdIndex - 1);

            // Calculate the phase of the trace.
            if (result[2] > 1)
                multiplier = -1.;

            pol2Phase_ = (-result[1] + multiplier * sqrt(result[1] * result[1] - 4 * result[2] * (result[0] - threshold*traceMax))) / (2 * result[2]);

            break;
        }
    }
    return pol2Phase_;
}

double PolyCfd::CalcPoly3Phase(const std::vector<double> &data, unsigned int maxPos){
    double pol3Phase_=-9999;
    float multiplier = 1.;

    if(maxPos < 5){
        return -10000; //broken trace
    }
    //maxPos is the maximum point of the raw trace, traceMax is the maximum interpolated from 4 points surrounding the max.
    double traceMax = Get3PolyInterpMax(data, maxPos);

    vector<double> result;
    for (unsigned int cfdIndex = maxPos; cfdIndex > 0; cfdIndex--) {
        if (data[cfdIndex - 1] < threshold*traceMax && data[cfdIndex] >= threshold*traceMax) {
            // Fit the rise of the trace to a 2nd order polynomial.
            //cout << "cfdThresh is " << threshold*traceMax << endl;

            if(cfdIndex < 2){
                return -10000; //broken trace
            }

            result = CalculatePoly3(data, cfdIndex - 2);

            result[0] = result[0]-threshold*traceMax;
            // Calculate the phase of the trace.
            //cout << "zero crossing between" << cfdIndex-1 << " " <<  cfdIndex << endl;
            pol3Phase_ = getPol3Zero(result,cfdIndex-1,cfdIndex);

            //cout << pol3Phase_ << endl;
            break;
        }
    }
    return pol3Phase_;
}

vector<double> PolyCfd::CalculatePoly3(vector<double> data, const unsigned int &startBin) {
    if (data.size() < 4)
        throw range_error("Polynomial::CalculatePoly3 - The data vector "
                          "had the wrong size : " + std::to_string(data.size()));

    double x1[4], x2[4], x3[4];
    for (size_t i = 0; i < 4; i++) {
        //cout << "using the points " << startBin + i << ", " << data[startBin+i] << endl;
        x1[i] = (startBin + i);
        x2[i] = std::pow(startBin + i, 2);
        x3[i] = std::pow(startBin + i, 3);
    }

    double denom = (x1[1] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                    x1[2] * (x2[1] * x3[3] - x2[3] * x3[1]) +
                    x1[3] * (x2[1] * x3[2] - x2[2] * x3[1])) -
                   (x1[0] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                    x1[2] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                    x1[3] * (x2[0] * x3[2] - x2[2] * x3[0])) +
                   (x1[0] * (x2[1] * x3[3] - x2[3] * x3[1]) -
                    x1[1] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                    x1[3] * (x2[0] * x3[1] - x2[1] * x3[0])) -
                   (x1[0] * (x2[1] * x3[2] - x2[2] * x3[1]) -
                    x1[1] * (x2[0] * x3[2] - x2[2] * x3[0]) +
                    x1[2] * (x2[0] * x3[1] - x2[1] * x3[0]));

    double p0 = (double) (
            (data[x1[0]] * (x1[1] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            x1[2] * (x2[1] * x3[3] - x2[3] * x3[1]) +
                            x1[3] * (x2[1] * x3[2] - x2[2] * x3[1])) -
             data[x1[1]] * (x1[0] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            x1[2] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            x1[3] * (x2[0] * x3[2] - x2[2] * x3[0])) +
             data[x1[2]] * (x1[0] * (x2[1] * x3[3] - x2[3] * x3[1]) -
                            x1[1] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            x1[3] * (x2[0] * x3[1] - x2[1] * x3[0])) -
             data[x1[3]] * (x1[0] * (x2[1] * x3[2] - x2[2] * x3[1]) -
                            x1[1] * (x2[0] * x3[2] - x2[2] * x3[0]) +
                            x1[2] * (x2[0] * x3[1] - x2[1] * x3[0]))) /
            denom);

    double p1 = (double) (((data[x1[1]] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            data[x1[2]] * (x2[1] * x3[3] - x2[3] * x3[1]) +
                            data[x1[3]] * (x2[1] * x3[2] - x2[2] * x3[1])) -
                           (data[x1[0]] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            data[x1[2]] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            data[x1[3]] * (x2[0] * x3[2] - x2[2] * x3[0])) +
                           (data[x1[0]] * (x2[1] * x3[3] - x2[3] * x3[1]) -
                            data[x1[1]] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            data[x1[3]] * (x2[0] * x3[1] - x2[1] * x3[0])) -
                           (data[x1[0]] * (x2[1] * x3[2] - x2[2] * x3[1]) -
                            data[x1[1]] * (x2[0] * x3[2] - x2[2] * x3[0]) +
                            data[x1[2]] *
                            (x2[0] * x3[1] - x2[1] * x3[0]))) /
                          denom);

    double p2 = (double) (
            ((x1[1] * (data[x1[2]] * x3[3] - data[x1[3]] * x3[2]) -
              x1[2] * (data[x1[1]] * x3[3] - data[x1[3]] * x3[1]) +
              x1[3] * (data[x1[1]] * x3[2] - data[x1[2]] * x3[1])) -
             (x1[0] * (data[x1[2]] * x3[3] - data[x1[3]] * x3[2]) -
              x1[2] * (data[x1[0]] * x3[3] - data[x1[3]] * x3[0]) +
              x1[3] * (data[x1[0]] * x3[2] - data[x1[2]] * x3[0])) +
             (x1[0] * (data[x1[1]] * x3[3] - data[x1[3]] * x3[1]) -
              x1[1] * (data[x1[0]] * x3[3] - data[x1[3]] * x3[0]) +
              x1[3] * (data[x1[0]] * x3[1] - data[x1[1]] * x3[0])) -
             (x1[0] * (data[x1[1]] * x3[2] - data[x1[2]] * x3[1]) -
              x1[1] * (data[x1[0]] * x3[2] - data[x1[2]] * x3[0]) +
              x1[2] * (data[x1[0]] * x3[1] - data[x1[1]] * x3[0]))) /
            denom);

    double p3 = (double) (
            ((x1[1] * (x2[2] * data[x1[3]] - x2[3] * data[x1[2]]) -
              x1[2] * (x2[1] * data[x1[3]] - x2[3] * data[x1[1]]) +
              x1[3] * (x2[1] * data[x1[2]] - x2[2] * data[x1[1]])) -
             (x1[0] * (x2[2] * data[x1[3]] - x2[3] * data[x1[2]]) -
              x1[2] * (x2[0] * data[x1[3]] - x2[3] * data[x1[0]]) +
              x1[3] * (x2[0] * data[x1[2]] - x2[2] * data[x1[0]])) +
             (x1[0] * (x2[1] * data[x1[3]] - x2[3] * data[x1[1]]) -
              x1[1] * (x2[0] * data[x1[3]] - x2[3] * data[x1[0]]) +
              x1[3] * (x2[0] * data[x1[1]] - x2[1] * data[x1[0]])) -
             (x1[0] * (x2[1] * data[x1[2]] - x2[2] * data[x1[1]]) -
              x1[1] * (x2[0] * data[x1[2]] - x2[2] * data[x1[0]]) +
              x1[2] * (x2[0] * data[x1[1]] - x2[1] * data[x1[0]]))) /
            denom);

    //Put the coefficients into a vector in ascending power order
    vector<double> coeffs = {p0, p1, p2, p3};

    // Calculate the maximum of the polynomial.
    double xmax;
    double node1 =
            (-2 * p2 + std::sqrt(4 * p2 * p2 - 12 * p3 * p1)) / (6 * p3);
    double node2 =
            (-2 * p2 - std::sqrt(4 * p2 * p2 - 12 * p3 * p1)) / (6 * p3);

    //Check if the curvature at node1 is positive or negative. If it is
    // negative then we have the maximum. If not then node2 is the
    // maximum.
    if ((2 * p2 + 6 * p3 * node1) < 0)
        xmax = node1;
    else
        xmax = node2;

    vector<double>().swap(data);

    return coeffs;
}

double PolyCfd::getPol3Zero(vector<double> &coeffs, double low, double high) {
    double a = coeffs[3];
    double b = coeffs[2];
    double c = coeffs[1];
    double d = coeffs[0];

    complex<double> discriminant = 18 * a * b * c * d - 4 * b * b * b * d + b * b * c * c - 4 * a * c * c * c - 27 * a * a * d * d;
    complex<double> discriminant0 = b * b - 3 * a * c;
    complex<double> discriminant1 = 2 * b * b * b - 9 * a * b * c + 27 * a * a * d;

    complex<double> C = std::pow((discriminant1 + std::sqrt(discriminant1 * discriminant1 - 4.0 * discriminant0 * discriminant0 * discriminant0)) / 2.0, 1.0 / 3.0);
    if(discriminant0.real() == 0){
        C = std::pow((discriminant1 - std::sqrt(discriminant1 * discriminant1 - 4.0 * discriminant0 * discriminant0 * discriminant0)) / 2.0, 1.0 / 3.0);
    }

    complex<double> m3 = -3;
    complex<double> xi = (-1.0 + sqrt(m3))/2.0;

    for(int i = 0; i < 3; i++){
        complex<double> root = -(1.0/(3*a))*(b + pow(xi,i)*C + discriminant0/(pow(xi,i)*C));
        if(root.real() > low && root.real() < high){
            //cout << "low is " << low << " high is " << high << " root is " << root.real() << endl;
            return root.real();
        }
    }
    return -1;
}

double PolyCfd::Get3PolyInterpMax(const std::vector<double> &data, unsigned int traceMax) {
    double max = 0;

    if(traceMax < 3){
        return -10000; //broken trace
    }

    double maxVal = data[traceMax];
    unsigned int startBin;
    if(data[traceMax-1] > data[traceMax+1]){
        //cout << data[traceMax-1] << " is greater than " << data[traceMax+1] << endl;
        startBin = traceMax-2;
    }
    else{
        //cout << data[traceMax-1] << " is less than " << data[traceMax+1] << endl;
        startBin = traceMax-1;
    }

    double x1[4], x2[4], x3[4];
    //cout << "using points" << endl;
    for (size_t i = 0; i < 4; i++) {
        //cout << startBin + i << ", " << data[startBin+i] << endl;
        x1[i] = (startBin + i);
        x2[i] = std::pow(startBin + i, 2);
        x3[i] = std::pow(startBin + i, 3);
    }

    double denom = (x1[1] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                    x1[2] * (x2[1] * x3[3] - x2[3] * x3[1]) +
                    x1[3] * (x2[1] * x3[2] - x2[2] * x3[1])) -
                   (x1[0] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                    x1[2] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                    x1[3] * (x2[0] * x3[2] - x2[2] * x3[0])) +
                   (x1[0] * (x2[1] * x3[3] - x2[3] * x3[1]) -
                    x1[1] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                    x1[3] * (x2[0] * x3[1] - x2[1] * x3[0])) -
                   (x1[0] * (x2[1] * x3[2] - x2[2] * x3[1]) -
                    x1[1] * (x2[0] * x3[2] - x2[2] * x3[0]) +
                    x1[2] * (x2[0] * x3[1] - x2[1] * x3[0]));

    double p0 = (double) (
            (data[x1[0]] * (x1[1] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            x1[2] * (x2[1] * x3[3] - x2[3] * x3[1]) +
                            x1[3] * (x2[1] * x3[2] - x2[2] * x3[1])) -
             data[x1[1]] * (x1[0] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            x1[2] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            x1[3] * (x2[0] * x3[2] - x2[2] * x3[0])) +
             data[x1[2]] * (x1[0] * (x2[1] * x3[3] - x2[3] * x3[1]) -
                            x1[1] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            x1[3] * (x2[0] * x3[1] - x2[1] * x3[0])) -
             data[x1[3]] * (x1[0] * (x2[1] * x3[2] - x2[2] * x3[1]) -
                            x1[1] * (x2[0] * x3[2] - x2[2] * x3[0]) +
                            x1[2] * (x2[0] * x3[1] - x2[1] * x3[0]))) /
            denom);

    double p1 = (double) (((data[x1[1]] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            data[x1[2]] * (x2[1] * x3[3] - x2[3] * x3[1]) +
                            data[x1[3]] * (x2[1] * x3[2] - x2[2] * x3[1])) -
                           (data[x1[0]] * (x2[2] * x3[3] - x2[3] * x3[2]) -
                            data[x1[2]] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            data[x1[3]] * (x2[0] * x3[2] - x2[2] * x3[0])) +
                           (data[x1[0]] * (x2[1] * x3[3] - x2[3] * x3[1]) -
                            data[x1[1]] * (x2[0] * x3[3] - x2[3] * x3[0]) +
                            data[x1[3]] * (x2[0] * x3[1] - x2[1] * x3[0])) -
                           (data[x1[0]] * (x2[1] * x3[2] - x2[2] * x3[1]) -
                            data[x1[1]] * (x2[0] * x3[2] - x2[2] * x3[0]) +
                            data[x1[2]] *
                            (x2[0] * x3[1] - x2[1] * x3[0]))) /
                          denom);

    double p2 = (double) (
            ((x1[1] * (data[x1[2]] * x3[3] - data[x1[3]] * x3[2]) -
              x1[2] * (data[x1[1]] * x3[3] - data[x1[3]] * x3[1]) +
              x1[3] * (data[x1[1]] * x3[2] - data[x1[2]] * x3[1])) -
             (x1[0] * (data[x1[2]] * x3[3] - data[x1[3]] * x3[2]) -
              x1[2] * (data[x1[0]] * x3[3] - data[x1[3]] * x3[0]) +
              x1[3] * (data[x1[0]] * x3[2] - data[x1[2]] * x3[0])) +
             (x1[0] * (data[x1[1]] * x3[3] - data[x1[3]] * x3[1]) -
              x1[1] * (data[x1[0]] * x3[3] - data[x1[3]] * x3[0]) +
              x1[3] * (data[x1[0]] * x3[1] - data[x1[1]] * x3[0])) -
             (x1[0] * (data[x1[1]] * x3[2] - data[x1[2]] * x3[1]) -
              x1[1] * (data[x1[0]] * x3[2] - data[x1[2]] * x3[0]) +
              x1[2] * (data[x1[0]] * x3[1] - data[x1[1]] * x3[0]))) /
            denom);

    double p3 = (double) (
            ((x1[1] * (x2[2] * data[x1[3]] - x2[3] * data[x1[2]]) -
              x1[2] * (x2[1] * data[x1[3]] - x2[3] * data[x1[1]]) +
              x1[3] * (x2[1] * data[x1[2]] - x2[2] * data[x1[1]])) -
             (x1[0] * (x2[2] * data[x1[3]] - x2[3] * data[x1[2]]) -
              x1[2] * (x2[0] * data[x1[3]] - x2[3] * data[x1[0]]) +
              x1[3] * (x2[0] * data[x1[2]] - x2[2] * data[x1[0]])) +
             (x1[0] * (x2[1] * data[x1[3]] - x2[3] * data[x1[1]]) -
              x1[1] * (x2[0] * data[x1[3]] - x2[3] * data[x1[0]]) +
              x1[3] * (x2[0] * data[x1[1]] - x2[1] * data[x1[0]])) -
             (x1[0] * (x2[1] * data[x1[2]] - x2[2] * data[x1[1]]) -
              x1[1] * (x2[0] * data[x1[2]] - x2[2] * data[x1[0]]) +
              x1[2] * (x2[0] * data[x1[1]] - x2[1] * data[x1[0]]))) /
            denom);

    // Calculate the maximum of the polynomial.
    double xmax;
    double node1 =
            (-2 * p2 + std::sqrt(4 * p2 * p2 - 12 * p3 * p1)) / (6 * p3);
    double node2 =
            (-2 * p2 - std::sqrt(4 * p2 * p2 - 12 * p3 * p1)) / (6 * p3);

    //Check if the curvature at node1 is positive or negative. If it is
    // negative then we have the maximum. If not then node2 is the
    // maximum.
    if ((2 * p2 + 6 * p3 * node1) < 0)
        xmax = node1;
    else
        xmax = node2;

    //cout << p0 + p1 * xmax + p2 * xmax * xmax +
    //        p3 * xmax * xmax * xmax << endl;
    return p0 + p1 * xmax + p2 * xmax * xmax +
                     p3 * xmax * xmax * xmax;
}