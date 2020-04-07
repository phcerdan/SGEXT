/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_CRAMER_VON_MISES_TEST_HPP
#define SG_CRAMER_VON_MISES_TEST_HPP

#include <algorithm>
#include <assert.h>
#ifdef WITH_PARALLEL_STL
#include <execution>
#endif
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
#ifdef WITH_PARALLEL_STL
    std::exclusive_scan(
            std::execution::par_unseq,
            std::begin(histo_counts),
            std::end(histo_counts),
            cumulative_counts_exclusive.begin(), 0.0);
#else
    std::partial_sum(
            std::begin(histo_counts),
            std::end(histo_counts),
            cumulative_counts_exclusive.begin());
    // Transform the partial sum into a exclusive_scan.
    // Shift container elements to the right, and assign 0 to the first element.
    std::rotate(
        cumulative_counts_exclusive.rbegin(),
        cumulative_counts_exclusive.rbegin() + 1,
        cumulative_counts_exclusive.rend());
    cumulative_counts_exclusive[0] = 0.0;
#endif
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
 * @param F target_cumulative_distro_at_histogram_bin_centers;
 * @param total_counts
 */
template <typename TVectorFloat, typename TVectorInt>
void compute_S(TVectorFloat &S,
               const TVectorInt &cumulative_counts_exclusive,
               const TVectorFloat &F,
               const size_t &total_counts) {
    assert(std::size(S) == std::size(F));
    assert(std::size(cumulative_counts_exclusive) == std::size(F));
    std::transform(
#ifdef WITH_PARALLEL_STL
            std::execution::par_unseq,
#endif
            std::begin(cumulative_counts_exclusive),
            std::end(cumulative_counts_exclusive),
            std::begin(F), std::begin(S),
            [&total_counts](const double &M, const double &f) -> double {
                return M - f * total_counts - 0.5;
            });
}

template <typename TVectorFloat, typename TVectorInt>
TVectorFloat compute_S(const TVectorInt &cumulative_counts_exclusive,
                       const TVectorFloat &F,
                       const size_t &total_counts) {
    TVectorFloat S(std::size(F));
    compute_S(S, cumulative_counts_exclusive, F, total_counts);
    return S;
}

/**
 * Intermediate value involving cumulative_counts_exclusive and F.
 * But F is the target_cumulative_distro_at_bin_center multiplied by the
 * total_counts of the histogram and minus 0.5.
 *
 * This is a micro optimization, if you are computing F,
 * you can also compute F_optimized and use this function.
 *
 * @tparam TVectorFloat
 * @tparam TVectorInt
 * @param S
 * @param cumulative_counts_exclusive
 * @param F target_cumulative_distro_at_histogram_bin_centers * total_counts -
 * 0.5
 * @param total_counts
 */
template <typename TVectorFloat, typename TVectorInt>
void compute_S_optimized(TVectorFloat &S,
                         const TVectorInt &cumulative_counts_exclusive,
                         const TVectorFloat &F_optimized,
                         const size_t &) {
    assert(std::size(S) == std::size(F_optimized));
    assert(std::size(cumulative_counts_exclusive) == std::size(F_optimized));
    std::transform(
#ifdef WITH_PARALLEL_STL
            std::execution::par_unseq,
#endif
            std::begin(cumulative_counts_exclusive),
            std::end(cumulative_counts_exclusive),
            std::begin(F_optimized), std::begin(S),
            [](const double &M, const double &f) -> double { return M - f; });
}

template <typename TVectorFloat, typename TVectorInt>
TVectorFloat compute_S_optimized(const TVectorInt &cumulative_counts_exclusive,
                                 const TVectorFloat &F_optimized,
                                 const size_t &total_counts) {
    TVectorFloat S(std::size(F_optimized));
    compute_S_optimized(S, cumulative_counts_exclusive, F_optimized,
                        total_counts);
    return S;
}

/**
 * Intermediate value involving S and histo_counts. Please note that this is
 * missing a division by (total_counts* total_counts) for optimization. The
 * factor is applied in the final
 * @reduce_T.
 *
 * @tparam TVectorFloat
 * @tparam TVectorInt
 * @param T
 * @param S
 * @param histo_counts
 * @param total_counts
 */
template <typename TVectorFloat, typename TVectorInt>
void compute_T(TVectorFloat &T,
               const TVectorFloat &S,
               const TVectorInt &histo_counts) {

    assert(std::size(T) == std::size(S));
    assert(std::size(T) == std::size(histo_counts));
    std::transform(
#ifdef WITH_PARALLEL_STL
            std::execution::par_unseq,
#endif
            std::begin(S),
            std::end(S),
            std::begin(histo_counts), std::begin(T),
            [](const double &s, const double &m) -> double {
            return m * (detail::one_over_six * (m + 1) *
                    (6 * s + 2 * m + 1) +
                    s * s);
            });
}
template <typename TVectorFloat, typename TVectorInt>
TVectorFloat compute_T(const TVectorFloat &S, const TVectorInt &histo_counts) {
    TVectorFloat T(std::size(S));
    compute_T(T, S, histo_counts);
    return T;
}

/**
 * Accumulation of T, see @compute_T.
 *
 * @tparam TVector
 * @param T
 * @param total_counts
 *
 * @return
 */
template <typename TVector>
double reduce_T(const TVector &T, const size_t &total_counts) {
    const double inverse_square_total_counts =
            1.0 / (total_counts * total_counts);
    return inverse_square_total_counts *
#ifdef WITH_PARALLEL_STL
           std::reduce(
                   std::execution::par_unseq,
#else
           std::accumulate(
#endif
                   std::begin(T), std::end(T)
#ifndef WITH_PARALLEL_STL
                   , 0.
#endif
                   );
}

template <typename TVectorInt, typename TVectorFloat>
double cramer_von_mises_test(
        const TVectorInt &histo_counts,
        const TVectorFloat &target_cumulative_distro_at_histogram_bin_centers) {
    const auto exclusive_cumulative_counts =
            compute_cumulative_counts(histo_counts);
    const auto total_counts =
            exclusive_cumulative_counts.back() + histo_counts.back();
    assert(std::accumulate(std::begin(histo_counts), std::end(histo_counts),
                           0) == total_counts);
    const auto &F = target_cumulative_distro_at_histogram_bin_centers;
    assert(std::size(histo_counts) == std::size(F));
    return 1.0 / (12 * total_counts) +
           reduce_T(compute_T(compute_S(exclusive_cumulative_counts, F,
                                        total_counts),
                              histo_counts),
                    total_counts);
}

/**
 * F_optimized is target_cumulative_distro_at_histogram_bin_centers *
 * total_counts - 0.5
 *
 * @tparam TVectorInt
 * @tparam TVectorFloat
 * @param histo_counts
 * @param F_optimized
 * @param total_counts
 *
 * @return
 */
template <typename TVectorInt, typename TVectorFloat>
double cramer_von_mises_test_optimized(const TVectorInt &histo_counts,
                                       const TVectorFloat &F_optimized,
                                       const size_t &total_counts) {
    assert(std::size(histo_counts) == std::size(F_optimized));
    assert(std::accumulate(std::begin(histo_counts), std::end(histo_counts),
                           0) == total_counts);
    return 1.0 / (12 * total_counts) +
           reduce_T(compute_T(compute_S_optimized(
                                      compute_cumulative_counts(histo_counts),
                                      F_optimized, total_counts),
                              histo_counts),
                    total_counts);
}

} // namespace SG
#endif
