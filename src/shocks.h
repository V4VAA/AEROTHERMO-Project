#pragma once

#include <vector>

std::vector<double> normal_shock(double P1, double T1, double h1, double rho1, double u1);
std::vector<double> oblique_shock(double P1, double T1, double h1, double rho1, double u1, double delta_c);
