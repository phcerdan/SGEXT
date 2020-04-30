/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_CUMULATIVE_DISTRIBUTION_FUNCTIONS_HPP
#define SG_CUMULATIVE_DISTRIBUTION_FUNCTIONS_HPP

#include <algorithm>
#include <boost/math/constants/constants.hpp> // for pi...
#include <functional>                         // std::function
#include <math.h>                             // erf

#ifdef WITH_PARALLEL_STL
#include <execution>                          // std::execution::par_unseq
#elif WITH_OPENMP
#include <omp.h>
#endif

namespace SG {

/**
 * Integrate function in the interval [a,b]
 * func takes one argument. If more are needed, use a lambda:
 * const double a = -1.0;
 * const double &b = x;
 * const auto func = [&b1, &b2, &b3](const double &z) -> double {
 *     return distribution_truncated_power_series_3(z, b1, b2, b3);
 * };
 * const double tolerance = 0.00001;
 * return integral(a, b, tolerance, func);
 *
 * Taken from:
 * https://www.boost.org/doc/libs/1_63_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/fp_eg/gi.html
 *
 * This function is not used because there are analytical forms for the
 * Cumulative Function Distribution of our particular functions. But that might
 * not be the case for other distros.
 *
 * @tparam value_type
 * @tparam function_type
 * @param a
 * @param b
 * @param tol
 * @param func
 *
 * @return
 */
template <typename value_type, typename function_type>
inline value_type integral(const value_type a,
                           const value_type b,
                           const value_type tol,
                           function_type func) {
    unsigned n = 1U;
    value_type h = (b - a);
    value_type I = (func(a) + func(b)) * (h / 2);

    for (unsigned k = 0U; k < 8U; k++) {
        h /= 2;

        value_type sum(0);
        for (unsigned j = 1U; j <= n; j++) {
            sum += func(a + (value_type((j * 2) - 1) * h));
        }

        const value_type I0 = I;
        I = (I / 2) + (h * sum);

        const value_type ratio = I0 / I;
        const value_type delta = ratio - 1;
        const value_type delta_abs = ((delta < 0) ? -delta : delta);

        if ((k > 1U) && (delta_abs < tol)) {
            break;
        }

        n *= 2U;
    }
    return I;
}

inline double distribution_lognormal(const double &x,
                                     const double &log_mean,
                                     const double &log_std_deviation) {
    if (x == 0) {
        return 0;
    }
    const auto &mu = log_mean;
    const auto &sigma = log_std_deviation;
    const auto pi = boost::math::constants::pi<double>();

    double exponent = log(x) - mu;
    exponent *= -exponent;
    exponent /= 2 * sigma * sigma;

    auto result = exp(exponent);
    result /= sigma * sqrt(2 * pi) * x;

    return result;
}
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
                                  const double &log_mean,
                                  const double &log_std_deviation) {
    return 0.5 * std::erfc(-(std::log(x) - log_mean) /
                           (sqrt(2.) * log_std_deviation));
};

inline double distribution_truncated_power_series_3(const double &x,
                                                    const double &b1,
                                                    const double &b2,
                                                    const double &b3) {
    // const double b3= -(3/32.) * (-1 + 2*b1 + 4*b2);
    const double z = 1 - x;
    const double z3 = z * z * z;
    const double z5 = z3 * z * z;
    return b1 * z + b2 * z3 + b3 * z5;
};
inline double cumulative_distribution_truncated_power_series_3(
        const double &x, const double &b1, const double &b2, const double &b3) {
    // const double b3= -(3/32.) * (-1 + 2*b1 + 4*b2);
    return (-(1 / 12.) * (-3 + x) * (1 + x) *
            (6 * b1 + 3 * b2 * (5 + (-2 + x) * x) +
             2 * b3 * (7 + (-4 + x) * x) * (3 + x * x)));
};

template <typename TArrayType1, typename TArrayType2>
void apply_distro(const TArrayType1 &X,
                  TArrayType2 &F /* output*/,
                  std::function<typename TArrayType2::value_type(
                          const typename TArrayType1::value_type &)> func) {
    assert(std::size(X) == std::size(F));
#ifdef WITH_PARALLEL_STL
    auto policy = std::execution::par_unseq;
    std::transform(policy, std::begin(X), std::end(X), std::begin(F), func);
#elif WITH_OPENMP
    const auto nelems = std::size(X);
#pragma omp parallel for schedule(dynamic)
    for(unsigned int i = 0; i < nelems ; ++i) {
        F[i] = func(i);
    }
#else
    std::transform(std::begin(X), std::end(X), std::begin(F), func);
#endif
}

template <typename TArrayType1, typename TArrayType2>
TArrayType2
apply_distro(const TArrayType1 &X,
             std::function<typename TArrayType2::value_type(
                     const typename TArrayType1::value_type &)> func) {
    TArrayType2 F(std::size(X));
    apply_distro<TArrayType1, TArrayType2>(X, F, func);
    return F;
}


} // namespace SG
#endif
