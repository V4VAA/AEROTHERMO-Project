#include "stag_properties.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>

#include "EquilFlow.h"

static ShockState readShockState(const EquilFlow& air) {
    ShockState state;
    state.Temperature = air.mix.T;
    state.Pressure = air.mix.P;
    state.Density = air.mix.rho;
    state.WaveAngle = air.mix.Beta;
    state.DownstreamVelocity = air.mix.U2;
    if (!std::isfinite(state.Temperature) || state.Temperature <= 0.0 ||
        !std::isfinite(state.Pressure) || state.Pressure <= 0.0 ||
        !std::isfinite(state.Density) || state.Density <= 0.0 || air.Spc.empty()) {
        throw std::runtime_error("EquilFlow returned an invalid downstream state");
    }
    for (const auto& species : air.Spc) {
        state.Species.push_back(species.name);
        state.MoleFraction.push_back(species.X);
        // EquilFlow gives Cp in J/(mol K) and molecular weight in g/mol
        state.Cpi.push_back(1000.0 * species.CPi / species.MWi);
    }
    return state;
}

static std::tuple<double, double, std::vector<double>, double, double, double, SpreadsheetTable, PhiTable>
compute_transport_from_eqs(const ShockState& shock_state, int eq83_mode);

Results stagProperties(const EquilFlow& postShock, double M_inf, double rho_inf, double theta_deg) {
    Results results;
    results.shock = readShockState(postShock);
    results.T2 = results.shock.Temperature;
    results.P2 = results.shock.Pressure;

    if (theta_deg == 0.0) {
        results.epsilon = rho_inf / results.shock.Density;
        std::tie(results.mu_mix, results.k_f, results.D_im_cm2s,
                 results.D_im_avg_cm2s, results.mu_sutherland, results.k_sutherland,
                 results.speciesTable, results.phiTable) =
            compute_transport_from_eqs(results.shock, M_inf > 15.0 ? 2 : 1);
        results.courseMix = computeCourseTransportMix(results.speciesTable);
    } else {
        results.WaveAngle = results.shock.WaveAngle;
        results.U2 = results.shock.DownstreamVelocity;
        results.Sutherland_OK = results.T2 <= 1500.0;
    }
    return results;
}

static std::tuple<double, double, std::vector<double>, double, double, double, SpreadsheetTable, PhiTable>
compute_transport_from_eqs(const ShockState& shock_state, int eq83_mode) {
    double T = shock_state.Temperature;
    double P = shock_state.Pressure;
    double P_atm = P / 101325.0;
    const auto& species = shock_state.Species;
    const auto& X = shock_state.MoleFraction;
    const auto& Cpi = shock_state.Cpi;
    int N = static_cast<int>(species.size());
    double Ru = 8314.5;
    double kB = 1.38065e-23;
    std::vector<double> MW(N, 0.0);
    std::vector<double> sigma(N, 0.0);
    std::vector<double> eps_kB(N, 0.0);
    std::vector<int> type(N, 1); // 1 = Neutral, 2 = Ion, 3 = Electron

    for (int i = 0; i < N; ++i) {
        const std::string& sp = species[i];
        // Each ion uses the same parameters as its neutral counterpart.
        if (sp == "N2") {
            MW[i] = 28.014; sigma[i] = 3.798; eps_kB[i] = 71.4;
        } else if (sp == "O2") {
            MW[i] = 31.998; sigma[i] = 3.467; eps_kB[i] = 106.7;
        } else if (sp == "N" || sp == "N+") {
            MW[i] = 14.007; sigma[i] = 3.298; eps_kB[i] = 71.4;
        } else if (sp == "O" || sp == "O+") {
            MW[i] = 15.999; sigma[i] = 3.050; eps_kB[i] = 106.7;
        } else if (sp == "NO" || sp == "NO+") {
            MW[i] = 30.006; sigma[i] = 3.492; eps_kB[i] = 116.7;
        } else if (sp == "Ar") {
            MW[i] = 39.948; sigma[i] = 3.542; eps_kB[i] = 93.3;
        } else if (sp == "e-") {
            MW[i] = 5.4858e-4; sigma[i] = 1.000; eps_kB[i] = 100.0;
            type[i] = 3;
        } else {
            throw std::runtime_error("Unrecognized species: " + sp);
        }

        if (sp == "N+" || sp == "O+" || sp == "NO+") {
            type[i] = 2;
        }
    }

    const double unavailable = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> T_star(N, unavailable);
    std::vector<double> Omega_kf(N, unavailable);
    std::vector<double> mu_i(N, 0.0);
    std::vector<double> k_i(N, 0.0);
    std::vector<double> n_e_col(N, unavailable);
    std::vector<double> Lambda(N, unavailable);

    int idx_e = -1;
    for (int i = 0; i < N; ++i) {
        if (type[i] == 3) {
            idx_e = i;
            break;
        }
    }

    double X_e = 0.0;
    if (idx_e != -1 && X[idx_e] > 0.0) {
        X_e = X[idx_e];
    } else {
        for (int i = 0; i < N; ++i) {
            if (type[i] == 2) {
                X_e += X[i];
            }
        }
    }

    for (int i = 0; i < N; ++i) {
        if (type[i] == 1) {
            T_star[i]   = T / eps_kB[i];
            Omega_kf[i] = (1.16145 / std::pow(T_star[i], 0.14874)) + (0.52478 / std::exp(0.7732 * T_star[i])) + (2.16178 / std::exp(2.43787 * T_star[i]));

            mu_i[i] = 2.6693e-6 * std::sqrt(MW[i] * T) / (std::pow(sigma[i], 2) * Omega_kf[i]);

            if (species[i] == "N" || species[i] == "O" || species[i] == "Ar") {
                k_i[i] = 0.00019891 * std::sqrt(T / MW[i]) / (std::pow(sigma[i], 2) * Omega_kf[i]) * 4.186 * 100.0;
            } else {
                k_i[i] = (Cpi[i] + 1.25 * (Ru / MW[i])) * mu_i[i];
            }

        } else if (type[i] == 2) {
            if (X[i] > 0.0 && X_e > 0.0) {
                n_e_col[i] = (X_e * P / (kB * T)) / 1e6;
                Lambda[i]  = 12143.0 * std::pow(T, 1.5) / std::sqrt(n_e_col[i]);
                mu_i[i]    = 2.207e-15 * std::sqrt(MW[i]) * std::pow(T, 2.5) / std::log(Lambda[i]) * 0.1;
                k_i[i]     = (Cpi[i] + 1.25 * (Ru / MW[i])) * mu_i[i];
            } else {
                mu_i[i] = 0.0;
                k_i[i]  = 0.0;
            }
        } else if (type[i] == 3) {
            if (X[i] > 0.0) {
                n_e_col[i] = (X[i] * P / (kB * T)) / 1e6;
                Lambda[i]  = 12143.0 * std::pow(T, 1.5) / std::sqrt(n_e_col[i]);
            }
            mu_i[i] = 0.0;
            k_i[i]  = 0.0;
        }
    }

    std::vector<double> Cp_molar(N, 0.0);
    for (int i = 0; i < N; ++i) {
        Cp_molar[i] = Cpi[i] * MW[i] / 1000.0;
    }

    SpreadsheetTable sp_table = {species, X, Cp_molar, MW, sigma, eps_kB, T_star, Omega_kf, mu_i, k_i, n_e_col, Lambda};

    std::vector<std::vector<double>> B_ij(N, std::vector<double>(N, 1.0));
    const double A_star = 1.25;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                B_ij[i][j] = 1.0;
            } else if (type[i] == 1 && type[j] == 1) {
                B_ij[i][j] = 0.78;
            } else if ((type[i] == 1 && type[j] == 2) || (type[i] == 2 && type[j] == 1)) {
                B_ij[i][j] = 0.15;
            } else if ((type[i] == 1 && type[j] == 3) || (type[i] == 3 && type[j] == 1)) {
                B_ij[i][j] = 0.20;
            } else {
                B_ij[i][j] = 1.00;
            }
        }
    }

    std::vector<std::vector<double>> phi_ij(N, std::vector<double>(N, 0.0));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (mu_i[i] == 0.0 || mu_i[j] == 0.0) {
                phi_ij[i][j] = static_cast<double>(i == j);
                continue;
            }

            if (eq83_mode == 1) {
                phi_ij[i][j] = std::pow(1.0 + B_ij[i][j] * std::sqrt(mu_i[i] / mu_i[j]) * std::pow(MW[j] / MW[i], 0.25), 2) / std::sqrt(8.0 * (1.0 + MW[i] / MW[j]));
            } else {
                double term1 = std::pow(1.0 + B_ij[i][j] * std::sqrt(mu_i[i] / mu_i[j]) * std::pow(MW[j] / MW[i], 0.25), 2);
                double term2 = std::sqrt(8.0 * (1.0 + MW[i] / MW[j]));
                double term3 = (5.0 / (3.0 * A_star) + MW[i] / MW[j]) / (1.0 + 5.0 / (3.0 * A_star));
                phi_ij[i][j] = (term1 / term2) * term3;
            }
        }
    }

    PhiTable phi_table = {phi_ij, species, species};

    std::vector<bool> active(N, false);
    for (int i = 0; i < N; ++i) {
        active[i] = (type[i] != 3) && (X[i] > 0.0) && (mu_i[i] > 0.0) && !std::isnan(mu_i[i]);
    }

    double mu_mix = 0.0;
    double k_f = 0.0;
    for (int i = 0; i < N; ++i) {
        if (active[i]) {
            double denom = 0.0;
            for (int j = 0; j < N; ++j) {
                if (active[j]) {
                    denom += X[j] * phi_ij[i][j];
                }
            }
            mu_mix = mu_mix + (X[i] * mu_i[i]) / denom;
            k_f = k_f + (X[i] * k_i[i]) / denom;
        }
    }

    std::vector<int> diff_idx;
    for (int i = 0; i < N; ++i) {
        if (type[i] != 3 && X[i] > 0.0 && !std::isnan(sigma[i]) && !std::isnan(eps_kB[i])) {
            diff_idx.push_back(i);
        }
    }
    int N_diff = static_cast<int>(diff_idx.size());
    std::vector<std::vector<double>> D_ij(N_diff, std::vector<double>(N_diff, 0.0));

    for (int a = 0; a < N_diff; ++a) {
        for (int b = 0; b < N_diff; ++b) {
            int i = diff_idx[a];
            int j = diff_idx[b];

            double sigma_ij = 0.5 * (sigma[i] + sigma[j]);
            double eps_ij = std::sqrt(eps_kB[i] * eps_kB[j]);
            double T_star_ij = T / eps_ij;
            double Omega_D = 1.06036 / std::pow(T_star_ij, 0.15610);
            D_ij[a][b] = 1.8583e-7 * std::pow(T, 1.5) * std::sqrt(1.0 / MW[i] + 1.0 / MW[j]) / (P_atm * std::pow(sigma_ij, 2) * Omega_D);
        }
    }

    std::vector<double> D_im_m2s(N, 0.0);
    std::vector<double> X_diff(N_diff, 0.0);
    for (int a = 0; a < N_diff; ++a) {
        X_diff[a] = X[diff_idx[a]];
    }

    for (int a = 0; a < N_diff; ++a) {
        int i = diff_idx[a];
        double denom_diff = 0.0;
        for (int b = 0; b < N_diff; ++b) {
            if (b != a) {
                denom_diff = denom_diff + X_diff[b] / D_ij[a][b];
            }
        }
        if (denom_diff > 0.0) {
            D_im_m2s[i] = (1.0 - X[i]) / denom_diff;
        }
    }

    std::vector<double> D_im_cm2s(N, 0.0);
    for (int i = 0; i < N; ++i) {
        D_im_cm2s[i] = D_im_m2s[i] * 1e4;
    }

    double sum_X_Dim = 0.0;
    double sum_X = 0.0;
    for (int a = 0; a < N_diff; ++a) {
        int i = diff_idx[a];
        sum_X_Dim += X[i] * D_im_cm2s[i];
        sum_X     += X[i];
    }
    double D_im_avg_cm2s = sum_X_Dim / sum_X;

    double T_ref  = 273.15;
    double S_mu   = 110.4;
    double S_k    = 194.0;

    double mu_sutherland = 1.716e-5 * std::pow(T / T_ref, 1.5) * (T_ref + S_mu) / (T + S_mu);
    double k_sutherland  = 0.0241   * std::pow(T / T_ref, 1.5) * (T_ref + S_k)  / (T + S_k);

    return {mu_mix, k_f, D_im_cm2s, D_im_avg_cm2s, mu_sutherland, k_sutherland, sp_table, phi_table};
}
