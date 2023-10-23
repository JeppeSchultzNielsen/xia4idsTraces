#include "polyCfd.hh"


using namespace std;

vector<double> polyCfd::CalculatePoly2(vector<double> &data, const unsigned int &startBin){
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
}


/*vector<double> polyCfd::CalculatePoly3(vector<double> &data, const unsigned int &startBin) {
        if (data.size() < 4)
            throw range_error("Polynomial::CalculatePoly3 - The data vector "
                              "had the wrong size : " + std::to_string(data.size()));

        double x1[4], x2[4], x3[4];
        for (size_t i = 0; i < 4; i++) {
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

        return make_pair(p0 + p1 * xmax + p2 * xmax * xmax +
                         p3 * xmax * xmax * xmax, coeffs);
    }
}//Polynomial namespace*/