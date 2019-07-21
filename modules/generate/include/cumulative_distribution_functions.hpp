/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_CUMULATIVE_DISTRIBUTION_FUNCTIONS_HPP
#define SG_CUMULATIVE_DISTRIBUTION_FUNCTIONS_HPP

#include <algorithm>
#include <execution>  // std::execution::par_unseq
#include <functional> // std::function
#include <math.h>     // erf

namespace SG {

/**
 * The cumulative distribution of the lognormal_distribution.
 * See for example:
 * https://en.wikipedia.org/wiki/Log-normal_distribution#Cumulative_distribution_function
 *
 * From: https://reference.wolfram.com/language/ref/LogNormalDistribution.html
 * mu and std_deviation refer to mean and std_deviation of the normal
 * distribution the lognormal_distribution is derived from.
 *
 * Mean of lognormal_distribution(mu, std_deviation) =  exp(mu +
 * std_deviation^2/2.0)
 *
 * Variance (i.e sigma^2) of lognormal_distribution(mu, std_deviation) =
 * exp(2*mu + std_deviation^2) ( -1.0 + exp(std_deviation^2))
 *
 * @param x input, if working with histograms: the center of the bin.
 * @param normal_mean mean of the normal distribution (not log)
 * @param normal_std_deviation std_deviation of the normal_distribution (not
 * log)
 */
inline double
cumulative_distribution_lognormal(const double &x,
                                  const double &normal_mean,
                                  const double &normal_std_deviation) {
    return (0.5 + 0.5 * std::erf((std::log(x) - normal_mean) /
                                 (sqrt(2.) * normal_std_deviation)));
};

inline double cumulative_ditribution_truncated_power_series_3(
        const double &x, const double &b1, const double &b2, const double &b3) {

    // const double b3= -(3/32.) * (-1 + 2*b1 + 4*b2);
    return (-(1 / 12.) * (-3 + x) * (1 + x) *
            (6 * b1 + 3 * b2 * (5 + (-2 + x) * x) +
             2 * b3 * (7 + (-4 + x) * x) * (3 + x * x)));
};

template <typename TArrayType1, typename TArrayType2>
TArrayType2
apply_distro(TArrayType1 &X,
             std::function<typename TArrayType2::value_type(
                     const typename TArrayType1::value_type &)> func) {
    auto policy = std::execution::par_unseq;
    TArrayType2 F(std::size(X));
    std::copy(policy, std::begin(X), std::end(X), std::begin(F));
    std::transform(policy, std::begin(X), std::end(X), std::begin(F), func);
    return F;
}

} // namespace SG
#endif
