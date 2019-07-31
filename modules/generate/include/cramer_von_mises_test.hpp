/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_CRAMER_VON_MISES_TEST_HPP
#define SG_CRAMER_VON_MISES_TEST_HPP

#include <algorithm>
#include <assert.h>
#include <execution>
#include <iterator>
#include <numeric>

namespace SG {
namespace detail {
constexpr double one_over_six = 1.0 / 6.0;
} // namespace detail

/**
 * Compute the cumulative sum of histo_counts with an exclusive_scan. THis means
 * the sum does not include the value of the current index.
 * The first component of the result is equal to the init value (0.0).
 *
 * @tparam TVector
 * @param cumulative_counts_exclusive
 * @param histo_counts
 */
template <typename TVector>
void compute_cumulative_counts(TVector &cumulative_counts_exclusive,
                               const TVector &histo_counts) {
    std::exclusive_scan(std::execution::par_unseq, std::begin(histo_counts),
                        std::end(histo_counts),
                        cumulative_counts_exclusive.begin(), 0.0);
}

template <typename TVector>
TVector compute_cumulative_counts(const TVector &histo_counts) {
    TVector cumulative_counts_exclusive(std::size(histo_counts));
    compute_cumulative_counts(cumulative_counts_exclusive, histo_counts);
    return cumulative_counts_exclusive;
}

/**
 * Intermediate value involving cumulative_counts_exclusive and F.
 *
 * @tparam TVectorFloat
 * @tparam TVectorInt
 * @param S
 * @param cumulative_counts_exclusive
 * @param F
 * @param num_bins
 */
template <typename TVectorFloat, typename TVectorInt>
void compute_S(TVectorFloat &S,
               const TVectorInt &cumulative_counts_exclusive,
               const TVectorFloat &F,
               const size_t &num_bins) {
    assert(std::size(S) == num_bins);
    assert(std::size(S) == std::size(F));
    assert(std::size(cumulative_counts_exclusive) == std::size(F));
    std::transform(
            std::execution::par_unseq, std::begin(cumulative_counts_exclusive),
            std::end(cumulative_counts_exclusive), std::begin(F), std::begin(S),
            [&num_bins](const double &M, const double &f) -> double {
                return M - f * num_bins - 0.5;
            });
}

template <typename TVectorFloat, typename TVectorInt>
TVectorFloat compute_S(const TVectorInt &cumulative_counts_exclusive,
                       const TVectorFloat &F,
                       const size_t &num_bins) {
    TVectorFloat S(num_bins);
    compute_S(S, cumulative_counts_exclusive, F, num_bins);
    return S;
}

/**
 * Intermediate value involving S and histo_counts. Please note that this is
 * missing a division by (num_bins* num_bins) for optimization. The factor is
 * applied in the final
 * @reduce_T.
 *
 * @tparam TVectorFloat
 * @tparam TVectorInt
 * @param T
 * @param S
 * @param histo_counts
 * @param num_bins
 */
template <typename TVectorFloat, typename TVectorInt>
void compute_T(TVectorFloat &T,
               const TVectorFloat &S,
               const TVectorInt &histo_counts,
               const size_t &num_bins) {

    assert(std::size(T) == num_bins);
    assert(std::size(T) == std::size(S));
    assert(std::size(T) == std::size(histo_counts));
    const double inverse_square_num_bins = 1.0 / (num_bins * num_bins);
    std::transform(std::execution::par_unseq, std::begin(S), std::end(S),
                   std::begin(histo_counts), std::begin(T),
                   [&inverse_square_num_bins](const double &s,
                                              const double &m) -> double {
                       return inverse_square_num_bins * m *
                              (detail::one_over_six * (m + 1) *
                                       (6 * s + 2 * m + 1) +
                               s * s);
                   });
}
template <typename TVectorFloat, typename TVectorInt>
TVectorFloat compute_T(const TVectorFloat &S,
                       const TVectorInt &histo_counts,
                       const size_t &num_bins) {
    TVectorFloat T(num_bins);
    compute_T(T, S, histo_counts, num_bins);
    return T;
}

/**
 * Accumulation of T, see @compute_T.
 *
 * @tparam TVector
 * @param T
 * @param num_bins
 *
 * @return
 */
template <typename TVector>
double reduce_T(TVector &T, const size_t &num_bins) {
    return std::reduce(std::execution::par_unseq, std::begin(T), std::end(T));
}

template <typename TVectorInt, typename TVectorFloat>
double cramer_von_mises_test(
        const TVectorInt &histo_counts,
        const TVectorFloat &target_cumulative_distro_at_histogram_bin_centers) {
    assert(std::size(histo_counts) ==
           std::size(target_cumulative_distro_at_histogram_bin_centers));
    const auto &F = target_cumulative_distro_at_histogram_bin_centers;
    const auto cumulative_counts_exclusive =
            compute_cumulative_counts(histo_counts);
    const auto num_bins = std::size(histo_counts);
    const auto S = compute_S(cumulative_counts_exclusive, F, num_bins);
    const auto T = compute_T(S, histo_counts, num_bins);
    return 1.0 / (12 * num_bins) + reduce_T(T, num_bins);
}

} // namespace SG
#endif
