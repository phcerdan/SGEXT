/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_CUMULATIVE_DISTRIBUTION_FUNCTIONS_HPP
#define SG_CUMULATIVE_DISTRIBUTION_FUNCTIONS_HPP

#include <math.h> // erf

namespace SG {

/**
 * The cumulative distribution of the lognormal_distribution.
 * See for example:
 * https://en.wikipedia.org/wiki/Log-normal_distribution#Cumulative_distribution_function
 *
 * @param x
 * @param log_mean
 * @param log_std_deviation
 */
inline double
cumulative_distribution_lognormal(const double &x,
                                  const double &log_mean,
                                  const double &log_std_deviation) {
    return (0.5 + 0.5 * std::erf((std::log(x) - log_mean) /
                                 (sqrt(2.) * log_std_deviation)));
};

inline double cumulative_ditribution_truncated_power_series_3(
        const double &x, const double &b1, const double &b2, const double &b3) {

    // const double b3= -(3/32.) * (-1 + 2*b1 + 4*b2);
    return (-(1 / 12.) * (-3 + x) * (1 + x) *
            (6 * b1 + 3 * b2 * (5 + (-2 + x) * x) +
             2 * b3 * (7 + (-4 + x) * x) * (3 + x * x)));
};

} // namespace SG
#endif
