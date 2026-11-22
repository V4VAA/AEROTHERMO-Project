#include "ArmSutMixing.h"

#include <algorithm>
#include <cmath>
#include <string>

#include "stag_properties.h"

namespace {

double interactionFactor(const std::string& nameI, const std::string& nameJ,
                         double massRatio, double viscosityRatio, bool armalySutton) {
    double a = 1.25;
    double b = 1.0;
    if (armalySutton) {
        const bool ionI = nameI.ends_with('+');
        const bool ionJ = nameJ.ends_with('+');
        if (ionI != ionJ) {
            b = 0.15;
            const auto& ion = ionI ? nameI : nameJ;
            const auto& neutral = ionI ? nameJ : nameI;
            if (neutral.size() + 1 == ion.size() && ion.starts_with(neutral)) {
                // Use the course coefficient for a species paired with its own ion.
                a = 1.1;
            }
        } else if (!ionI) {
            b = 0.78;
        }
    }

    const double term = 1.0 + b * std::sqrt(viscosityRatio) * std::pow(massRatio, 0.25);
    const double factor = term * term / std::sqrt(8.0 * (1.0 + 1.0 / massRatio));
    if (!armalySutton) {
        return factor;
    }
    return factor * (5.0 / (3.0 * a) + massRatio) / (1.0 + massRatio);
}

}  // namespace

CourseTransportMix computeCourseTransportMix(const SpreadsheetTable& species) {
    const auto& names = species.Species;
    const auto& x = species.Mole_Fraction;
    const auto& mu = species.Mu_i;
    const auto& mw = species.MW;

    CourseTransportMix mix;
    mix.usesArmalySutton = std::ranges::find(names, "e-") != names.end();
    const auto contributes = [&](std::size_t i) {
        return names[i] != "e-" && x[i] > 0.0 && mu[i] > 0.0 && std::isfinite(mu[i]);
    };

    for (std::size_t i = 0; i < names.size(); ++i) {
        if (!contributes(i)) {
            continue;
        }

        // Both rules include the self term once. Electron transport is omitted.
        double denominator = x[i];
        for (std::size_t j = 0; j < names.size(); ++j) {
            if (j != i && contributes(j)) {
                denominator += x[j] * interactionFactor(names[i], names[j], mw[j] / mw[i],
                                                       mu[i] / mu[j], mix.usesArmalySutton);
            }
        }
        mix.viscosity += x[i] * mu[i] / denominator;
        mix.conductivity += x[i] * species.k_i[i] / denominator;
    }
    return mix;
}
