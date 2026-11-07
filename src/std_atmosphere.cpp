#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include "EquilFlow.h"

using namespace std;

// Constants
const double R = 8.314;          // J/(mol K)
const double g = 9.80665;         // m/s^2
const double r_0 = 6356766.0;     // m
const double gamma = 1.4;         // Specific heat ratio for air
const double MW_air = 0.0289644;  // kg/mol
const double b = 3.31e-7;         // 1/m

double Temperature(double Tm_i, double Lr, double baseAltitude, double altitude)
{
    return Tm_i + Lr * (altitude - baseAltitude);
}

double Pressure(double Pi, double Tm_i, double Tm, double Lr,
    double baseAltitude, double altitude, double molecularWeight = MW_air)
{
    if (Lr == 0.0)
    {
        return Pi * exp(-g * molecularWeight * (altitude - baseAltitude) / (R * Tm_i));
    }

    return Pi * pow(Tm / Tm_i, -g * molecularWeight / (Lr * R));
}

double Density(double P, double T)
{
    return P * MW_air / (R * T);
}

double UpperPressure(double Pi, double Tm_i, double Tm, double Li,
    double zi, double z, double molecularWeight)
{
    const double Rair = R / molecularWeight;
    const double powerP = -(g / (Rair * Li)) * (1.0 + b * (Tm_i / Li - zi));
    const double pressureExponential = exp((g * b / (Rair * Li)) * (z - zi));

    return Pi * pow(Tm / Tm_i, powerP) * pressureExponential;
}

double UpperDensity(double Pi, double T_i, double Tm_i, double Tm, double Li, 
    double zi, double z, double molecularWeight)
{
    const double Rair = R / molecularWeight;
    const double rho_i = Pi / (Rair * T_i);
    const double powerRho = -(g / (Rair * Li)) * (Rair * Li / g + 1.0 + b * (Tm_i / Li - zi));
    const double densityExponential = exp((g * b / (Rair * Li)) * (z - zi));

    return rho_i * pow(Tm / Tm_i, powerRho) * densityExponential;
}

vector<double> std_atmosphere(double z, double M)
{
    // z is geometric altitude in metres; M is Mach number.
    if (!isfinite(z) || z < 0.0 || z > 150000.0)
    {
        throw out_of_range("altitude must be between 0 and 150000 m");
    }
    if (!isfinite(M))
    {
        throw invalid_argument("Mach number must be finite");
    }

    const double h_geo = r_0 * z / (r_0 + z);

    EquilFlow air6("E6");

    double Tm = 0.0;
    double T = 0.0;
    double h = 0.0;
    double P = 0.0;
    double rho = 0.0;
    double molecularWeight = MW_air;

    if (h_geo < 11000.0)
    {
        const double Pi = 101325.0;
        const double Tm_i = 288.15;
        const double Li = -0.0065;
        const double hi_geo = 0.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;

    }
    else if (h_geo < 20000.0)
    {
        const double Pi = 22631.95;
        const double Tm_i = 216.65;
        const double Li = 0.0;
        const double hi_geo = 11000.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (h_geo < 32000.0)
    {
        const double Pi = 5474.79;
        const double Tm_i = 216.65;
        const double Li = 0.0010;
        const double hi_geo = 20000.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (h_geo < 47000.0)
    {
        const double Pi = 868.01;
        const double Tm_i = 228.65;
        const double Li = 0.0028;
        const double hi_geo = 32000.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (h_geo < 51000.0)
    {
        const double Pi = 110.9;
        const double Tm_i = 270.65;
        const double Li = 0.0;
        const double hi_geo = 47000.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (h_geo < 71000.0)
    {
        const double Pi = 66.94;
        const double Tm_i = 270.65;
        const double Li = -0.0028;
        const double hi_geo = 51000.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (z <= 86000.0)
    {
        const double Pi = 3.956;
        const double Tm_i = 214.65;
        const double Li = -0.0020;
        const double hi_geo = 71000.0;

        Tm = Temperature(Tm_i, Li, hi_geo, h_geo);
        T = Tm;
        P = Pressure(Pi, Tm_i, Tm, Li, hi_geo, h_geo);
        rho = Density(P, T);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (z < 100000.0)
    {
        const double zi = 86000.0;
        const double zf = 100000.0;
        const double Tm_i = 186.945;
        const double T_i = 186.946;
        const double Pi = 0.344180;
        const double Li = 1.6481 / 1000.0;
        const double MW_i = 28.9644 / 1000.0;
        const double MW_f = 28.88 / 1000.0;

        Tm = Temperature(Tm_i, Li, zi, z);
        molecularWeight = MW_i + (MW_f - MW_i) * (z - zi) / (zf - zi);
        T = Tm * molecularWeight / MW_air;
        P = UpperPressure(Pi, Tm_i, Tm, Li, zi, z, molecularWeight);
        rho = UpperDensity(Pi, T_i, Tm_i, Tm, Li, zi, z, molecularWeight);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (z < 110000.0)
    {
        const double zi = 100000.0;
        const double zf = 110000.0;
        const double Tm_i = 210.65;
        const double T_i = 210.02;
        const double Pi = 0.029073;
        const double Li = 5.0 / 1000.0;
        const double MW_i = 28.88 / 1000.0;
        const double MW_f = 28.56 / 1000.0;

        Tm = Temperature(Tm_i, Li, zi, z);
        molecularWeight = MW_i + (MW_f - MW_i) * (z - zi) / (zf - zi);
        T = Tm * molecularWeight / MW_air;
        P = UpperPressure(Pi, Tm_i, Tm, Li, zi, z, molecularWeight);
        rho = UpperDensity(Pi, T_i, Tm_i, Tm, Li, zi, z, molecularWeight);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else if (z < 120000.0)
    {
        const double zi = 110000.0;
        const double zf = 120000.0;
        const double Tm_i = 260.65;
        const double T_i = 257.0;
        const double Pi = 0.006801;
        const double Li = 10.0 / 1000.0;
        const double MW_i = 28.56 / 1000.0;
        const double MW_f = 28.08 / 1000.0;

        Tm = Temperature(Tm_i, Li, zi, z);
        molecularWeight = MW_i + (MW_f - MW_i) * (z - zi) / (zf - zi);
        T = Tm * molecularWeight / MW_air;
        P = UpperPressure(Pi, Tm_i, Tm, Li, zi, z, molecularWeight);
        rho = UpperDensity(Pi, T_i, Tm_i, Tm, Li, zi, z, molecularWeight);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }
    else
    {
        const double zi = 120000.0;
        const double zf = 150000.0;
        const double Tm_i = 360.65;
        const double T_i = 349.49;
        const double Pi = 0.002247;
        const double Li = 20.0 / 1000.0;
        const double MW_i = 28.08 / 1000.0;
        const double MW_f = 26.92 / 1000.0;

        Tm = Temperature(Tm_i, Li, zi, z);
        molecularWeight = MW_i + (MW_f - MW_i) * (z - zi) / (zf - zi);
        T = Tm * molecularWeight / MW_air;
        P = UpperPressure(Pi, Tm_i, Tm, Li, zi, z, molecularWeight);
        rho = UpperDensity(Pi, T_i, Tm_i, Tm, Li, zi, z, molecularWeight);

        air6.Composition(1, T, P);
        h = air6.mix.h;
    }

    const double a = sqrt(gamma * R * T / molecularWeight);
    const double u = a * M;

    vector<double> output = {P, T, h, rho, u};

    return output;
}