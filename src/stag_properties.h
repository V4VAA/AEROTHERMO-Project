#pragma once

#include <string>
#include <vector>

#include "ArmSutMixing.h"

struct ShockState {
    double Temperature = 0.0;
    double Pressure = 0.0;
    double Density = 0.0;
    double WaveAngle = 0.0;
    double DownstreamVelocity = 0.0;
    std::vector<std::string> Species;
    std::vector<double> MoleFraction;
    std::vector<double> Cpi;  // J/(kg K)
};

struct SpreadsheetTable {
    std::vector<std::string> Species;
    std::vector<double> Mole_Fraction;
    std::vector<double> Cp_J_molK;
    std::vector<double> MW;
    std::vector<double> Sigma;
    std::vector<double> eps_kB;
    std::vector<double> T_star;
    std::vector<double> Omega_kf;
    std::vector<double> Mu_i;
    std::vector<double> k_i;
    std::vector<double> n_e;
    std::vector<double> Lambda;
};

struct PhiTable {
    std::vector<std::vector<double>> data;
    std::vector<std::string> VariableNames;
    std::vector<std::string> RowNames;
};

struct Results {
    ShockState shock;
    double T2 = 0.0;
    double P2 = 0.0;
    double epsilon = 0.0;
    SpreadsheetTable speciesTable;
    PhiTable phiTable;
    double mu_mix = 0.0;
    double k_f = 0.0;
    CourseTransportMix courseMix;
    std::vector<double> D_im_cm2s;
    double D_im_avg_cm2s = 0.0;
    double mu_sutherland = 0.0;
    double k_sutherland = 0.0;
    double WaveAngle = 0.0;
    double U2 = 0.0;
    bool Sutherland_OK = false;
};

class EquilFlow;

// Pass an already solved shock. Zero deflection selects the mixture calculations.
Results stagProperties(const EquilFlow& postShock, double M_inf, double rho_inf, double theta_deg);
