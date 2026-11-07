#include <iostream>
#include <cmath>
#include <vector>
#include "std_atmosphere.h"
#include "shocks.h"

using namespace std;

int main() {
    double z, M, delta_c_1, delta_c_2;
    z = 105000;
    M = 35;
    delta_c_1 = 16;
    delta_c_2 = 40;

    const vector<double> atmosphere = std_atmosphere(z, M);

    cout << "Freestream properties at z = " << z << " m, M = " << M << endl;
    cout << "Pressure: " << atmosphere[0] << " Pa" << endl;
    cout << "Temperature: " << atmosphere[1] << " K" << endl;
    cout << "Enthalpy: " << atmosphere[2] << " kJ/kg" << endl;
    cout << "Density: " << atmosphere[3] << " kg/m^3" << endl;
    cout << "Velocity: " << atmosphere[4] << " m/s" << endl <<endl;

    const vector<double> normal = normal_shock(
        atmosphere[0], atmosphere[1], atmosphere[2], atmosphere[3], atmosphere[4]);

    cout << "Properties after normal shock" << endl;
    cout << "Pressure: " << normal[0] << " Pa" << endl;
    cout << "Temperature: " << normal[1] << " K" << endl;
    cout << "Enthalpy: " << normal[2] << " kJ/kg" << endl;
    cout << "Density: " << normal[3] << " kg/m^3" << endl;
    cout << "Velocity: " << normal[4] << " m/s" << endl << endl;

    const vector<double> oblique = oblique_shock(
        atmosphere[0], atmosphere[1], atmosphere[2], atmosphere[3], atmosphere[4], delta_c_1);

    cout << "Properties after oblique shock with delta_c = " << delta_c_1 << "°" << endl;
    cout << "Pressure: " << oblique[0] << " Pa" << endl;
    cout << "Temperature: " << oblique[1] << " K" << endl;
    cout << "Enthalpy: " << oblique[2] << " kJ/kg" << endl;
    cout << "Density: " << oblique[3] << " kg/m^3" << endl;
    cout << "Velocity: " << oblique[4] << " m/s" << endl;
    cout << "Shock angle: " << oblique[5] << "°" << endl << endl;

    const vector<double> oblique2 = oblique_shock(
        atmosphere[0], atmosphere[1], atmosphere[2], atmosphere[3], atmosphere[4], delta_c_2);

    cout << "Properties after oblique shock with delta_c = " << delta_c_2 << "°" << endl;
    cout << "Pressure: " << oblique2[0] << " Pa" << endl;
    cout << "Temperature: " << oblique2[1] << " K" << endl;
    cout << "Enthalpy: " << oblique2[2] << " kJ/kg" << endl;
    cout << "Density: " << oblique2[3] << " kg/m^3" << endl;
    cout << "Velocity: " << oblique2[4] << " m/s" << endl;
    cout << "Shock angle: " << oblique2[5] << "°" << endl << endl;

    return 0;   
}
