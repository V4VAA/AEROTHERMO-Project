#include <iostream>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <vector>
#include "EquilFlow.h"

using namespace std;

double Beta(double theta, double epsilon){
    double A = (1/epsilon - 1) / tan(theta);
    return atan((A - pow(A * A - 4/epsilon, 0.5))/2);
}

vector<double> normal_shock(double P1, double T1, double h1, double rho1, double u1)
{
    EquilFlow air6("E6");
    
    double P2, T2, h2, rho2, u2 = 0;
    
    double epsilon = 0.1;
    double delta_epsilon = 1;

    while (delta_epsilon > 1.0e-6){
        P2 = P1 + rho1 * u1 * u1 * (1 - epsilon);
        h2 = h1 + 0.5 * u1 * u1 * (1 - epsilon * epsilon) / 1000;

        air6.Composition(2, P2, h2);

        T2 = air6.mix.T;
        rho2 = air6.mix.rho;

        double new_epsilon = rho1 / rho2;
        delta_epsilon = abs(new_epsilon - epsilon)/epsilon;
        epsilon = new_epsilon;
    }

    u2 = u1 * epsilon;
    rho2 = rho1 / epsilon;
    P2 = P1 + rho1 * u1 * u1 * (1 - epsilon);
    h2 = h1 + 0.5 * u1 * u1 * (1 - epsilon * epsilon) / 1000;

    air6.Composition(2, P2, h2);
    T2 = air6.mix.T;
    rho2 = air6.mix.rho;

    vector<double> output = {P2, T2, h2, rho2, u2};

    return output;
}

vector<double> oblique_shock(double P1, double T1, double h1, double rho1, double u1, double delta_c)
{
    const double theta = delta_c * std::numbers::pi_v<double> / 180.0;
    EquilFlow air6("E6");

    double P2, T2, h2, rho2, u2, beta, u1n;

    double epsilon = 0.1;
    double delta_epsilon = 1.0;
    int epsilonIteration = 0;
    while (delta_epsilon > 1.0e-6 && epsilonIteration++ < 100)
    {
        beta = Beta(theta, epsilon);
        u1n = u1 * sin(beta);
        P2 = P1 + rho1 * u1n * u1n * (1.0 - epsilon);
        h2 = h1 + 0.5 * u1n * u1n * (1.0 - epsilon * epsilon) / 1000.0;

        air6.Composition(2, P2, h2);
        double new_epsilon = rho1 / air6.mix.rho;
        delta_epsilon = abs(new_epsilon - epsilon) / epsilon;
        epsilon = new_epsilon;
    }
    if (delta_epsilon > 1.0e-6) throw runtime_error("Oblique-shock iteration did not converge");

    beta = Beta(theta, epsilon);
    u1n = u1 * sin(beta);
    P2 = P1 + rho1 * u1n * u1n * (1.0 - epsilon);
    h2 = h1 + 0.5 * u1n * u1n * (1.0 - epsilon * epsilon) / 1000.0;

    air6.Composition(2, P2, h2);
    rho2 = air6.mix.rho;
    T2 = air6.mix.T;
    u2 = epsilon * u1n / sin(beta - theta);

    return {P2, T2, h2, rho2, u2, beta * 180.0 / std::numbers::pi_v<double>};
}
