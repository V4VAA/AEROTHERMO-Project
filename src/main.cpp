#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <exception>
#include <vector>

#include "std_atmosphere.h"
#include "cone.h"
#include "EquilFlow.h"
#include "stag_properties.h"

using namespace std;

const double pi = 3.14159265358979323846;

int main() {
    const double a = 1.5;
    const double b = 1.0;
    const double delta_c1 = 29.7448 * pi / 180.0;
    const double r_n = 0.1;
    const double Xc = 1.75;

    struct FlightCondition {
        double M;
        double z;
    };
    const FlightCondition conditions[] = {{35.0, 105.0}, {22.0, 60.0}, {10.5, 45.0}};
    const double R_air = 287.05;
    const double gamma_air = 1.4;

    ofstream csv("aero_results.csv");
    csv << "mach,altitude_km,epsilon,alpha_deg,CA,CN,CL,CD,LD\n";
    csv << setprecision(12);

    for (const FlightCondition& condition : conditions) {
        const double M = condition.M;
        const double z = condition.z;
        const vector<double> atmosphere = std_atmosphere(z * 1000.0, M);
        const double P_inf = atmosphere[0];
        const double T_inf = atmosphere[1];
        const double rho_inf = atmosphere[3];
        const double U_inf = atmosphere[4];
        // For some reason the oblique shock function stalls at M= 35 with the U_inf value but works with this one
        // we have no idea why it happens...
        const double U_oblique = M * sqrt(gamma_air * R_air * T_inf);


        EquilFlow normalShock("E10");
        normalShock.NormalShock(P_inf, T_inf, U_inf);
        const Results normal = stagProperties(normalShock, M, rho_inf, 0.0);

        const double epsilon = normal.epsilon;
        const double Cpmax = 2.0 - epsilon;
        for (int alpha = -40; alpha <= 40; ++alpha) {
            const double alpha_rad = alpha * pi / 180.0;
            const vector<double> coeffs = aerodynamicCoefficients(a, b, delta_c1, r_n, Xc, alpha_rad, Cpmax);
            csv << M << ',' << z << ',' << epsilon << ',' << alpha << ','
                << coeffs[0] << ',' << coeffs[1] << ',' << coeffs[2] << ','
                << coeffs[3] << ',' << coeffs[4] << '\n';
        }

        cout << "\nMach " << M << " at " << z << " km\n";
        cout << "  Normal shock: T2 = " << normal.T2 << " K, P2 = " << normal.P2
                << " Pa, rho1/rho2 = " << epsilon << '\n'
                << "    Mixture viscosity: " << normal.mu_mix << " Pa s\n"
                << "    Mixture conductivity: " << normal.k_f << " W/(m K)\n"
                << "    " << (normal.courseMix.usesArmalySutton ? "Armaly-Sutton" : "Wilke")
                << " comparison: mu = " << normal.courseMix.viscosity << " Pa s, k = "
                << normal.courseMix.conductivity << " W/(m K)\n"
                << "    Average D_im: " << normal.D_im_avg_cm2s << " cm^2/s\n"
                << "    Sutherland comparison: mu = " << normal.mu_sutherland
                << " Pa s, k = " << normal.k_sutherland << " W/(m K)\n";

        for (const double theta : {20.0, 30.0}) {
            EquilFlow obliqueShock("E10");
            obliqueShock.ObliqueShock(P_inf, T_inf, U_oblique, theta, 0.0);
            const Results oblique = stagProperties(obliqueShock, M, rho_inf, theta);
            cout << "  " << theta << " deg oblique shock: T2 = "
                    << oblique.T2 << " K, shock angle = " << oblique.WaveAngle
                    << " deg, U2 = " << oblique.U2
                    << " m/s, Sutherland OK: " << (oblique.Sutherland_OK ? "Yes" : "No") << '\n';
        }
    }

    csv.close();
    return 0;
}
