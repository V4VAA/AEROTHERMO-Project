#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

const double pi = 3.14159265358979323846;

vector<double> aerodynamicCoefficients(double a, double b, double delta_c1, double r_n, double Xc, double alpha, double Cpmax) {
    const double e = sqrt(a * a - b * b) / a;
    const double q = e * e * cos(delta_c1) * cos(delta_c1);
    const double Aref = a * b * pi;

    const double X1 = r_n * cos(delta_c1) / tan(delta_c1);

    const double dOmega = pi / 400;

    // Ca calculation
    double caIntegral = 0.0;

    double inner_integral = 0.0;
    for (double omega = -pi/2; omega <= pi/2; omega += dOmega) {
        double top = sin(delta_c1) * cos(alpha) - cos(delta_c1)*sin(alpha)*sin(omega);
        double integrand = pow(top,2) / (1 - q * q * pow(cos(omega),2));
        inner_integral += integrand * dOmega;
    }
    caIntegral += inner_integral * (Xc*Xc - X1*X1) / 2;

    double noseCa = 0.5 * Cpmax * (1 - pow(sin(delta_c1),4));
    double Ca = -noseCa * pi * r_n * r_n / Aref - 2 * Cpmax / (pi * Xc * Xc) * caIntegral;

    // Cn calculation
    double cnIntegral = 0.0;
    inner_integral = 0.0;
    for (double omega = -pi/2; omega <= pi/2; omega += dOmega) {
        double top = sin(delta_c1) * cos(alpha) - cos(delta_c1)*sin(alpha)*sin(omega);
        double integrand = pow(top,2) / (1 - q * q * pow(cos(omega),2)) * sin(omega);
        inner_integral += integrand * dOmega;
    }
    cnIntegral += inner_integral * (Xc*Xc - X1*X1) / 2;

    double Cn = -2 * Cpmax / (pi * b * Xc) * cnIntegral;


    double Cl = Cn * cos(alpha) - Ca * sin(alpha);
    double Cd = Cn * sin(alpha) + Ca * cos(alpha);
    double LD = Cl / Cd;

    return {Cl, Cd, LD};
}