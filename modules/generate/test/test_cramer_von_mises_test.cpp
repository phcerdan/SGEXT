/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cramer_von_mises_test.hpp"
#include "cumulative_distribution_functions.hpp"
#include "generate_common.hpp" // For Histogram
#include "gmock/gmock.h"

using namespace ::testing;

class CramerVonMisesTestFixture : public ::testing::Test {
  public:
    using TVectorInt = std::vector<size_t>;
    using TVectorFloat = std::vector<double>;
    void SetUp() override {
        this->CreateHistograms();
        this->CreateFLognormal();
    };
    void CreateFLognormal() {
        const double normal_mean = -0.5;
        const double normal_std_deviation = 0.4;
        histogram_centers = histogram.ComputeBinCenters();
        F_lognormal = SG::apply_distro<TVectorFloat, TVectorFloat>(
                histogram_centers,
                [&normal_mean,
                 &normal_std_deviation](const double &x) -> double {
                    return SG::cumulative_distribution_lognormal(
                            x, normal_mean, normal_std_deviation);
                });
    }
    void CreateHistograms() {
        std::vector<double> dummy_histo_data;
        // Create histograms
        histogram = SG::Histogram(
                dummy_histo_data,
                histo::GenerateBreaksFromRangeAndBins(
                        0.0, sqrt(2.0) + std::numeric_limits<double>::epsilon(),
                        5));
        histo_counts = TVectorInt{2, 4, 6, 4, 2};
        histo_bins = std::size(histo_counts);
        histogram.counts = histo_counts;
        // histogram_normalized = histo::NormalizeByArea(histogram);
    }
    SG::Histogram histogram;
    // histo::Histo<double, double> histogram_normalized;
    size_t histo_bins = 5;
    TVectorInt histo_counts;
    TVectorFloat histogram_centers;
    TVectorFloat F_lognormal;
    const double eps_big = 5.0e-9;
};

TEST_F(CramerVonMisesTestFixture, compute_cumulative_counts) {

    TVectorInt expected_M = {0, 2, 6, 12, 16};
    auto M = SG::compute_cumulative_counts(histo_counts);
    EXPECT_THAT(M, ElementsAreArray(expected_M));
}
TEST_F(CramerVonMisesTestFixture, F_lognormal) {
    std::cout << "histogram:" << std::endl;
    histogram.PrintBreaksAndCounts(std::cout);
    std::cout << "histogram_centers: " << std::endl;
    for (auto i : histogram_centers) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
    std::cout << "F_logormal_at_centers: " << std::endl;
    for (auto &f : F_lognormal) {
        std::cout << f << ", ";
    }
    std::cout << std::endl;
    // might be problematic to check all these doubles. Turn it off if needed.
    TVectorFloat F_lognormal_expected = {0.00013630399999999999, 0.185795262,
                                         0.649349937, 0.889664648, 0.968061163};
    EXPECT_THAT(F_lognormal,
                ElementsAre(DoubleNear(F_lognormal_expected[0], eps_big),
                            DoubleNear(F_lognormal_expected[1], eps_big),
                            DoubleNear(F_lognormal_expected[2], eps_big),
                            DoubleNear(F_lognormal_expected[3], eps_big),
                            DoubleNear(F_lognormal_expected[4], eps_big)));
}

TEST_F(CramerVonMisesTestFixture, cramer_von_mises_test) {
    auto cost = SG::cramer_von_mises_test(histo_counts, F_lognormal);
    std::cout << "cramer_von_mises_test result: " << cost << std::endl;
    EXPECT_GT(cost, 0.0);
}

class CramerVonMisesUniformDistributionTestFixture : public ::testing::Test {
  public:
    using TVectorInt = std::vector<size_t>;
    using TVectorFloat = std::vector<double>;
    void SetUp() override {
        this->CreateHistograms();
        this->CreateUniformDistribution();
    };
    void CreateHistograms() {
        std::vector<double> dummy_histo_data;
        // Create histograms
        histogram = SG::Histogram(
                dummy_histo_data,
                histo::GenerateBreaksFromRangeAndBins(a_, b_, histo_bins));
        auto uniform_counts_value = 2;
        for (auto &c : histogram.counts) {
            c = uniform_counts_value;
        }
        total_counts = std::accumulate(std::begin(histogram.counts),
                                       std::end(histogram.counts), 0);
    }
    void CreateUniformDistribution() {
        auto histogram_centers = histogram.ComputeBinCenters();
        F_uniform = this->apply_uniform_distro_cdf(a_, b_, histogram_centers);
        const auto total_counts = std::accumulate(
                std::begin(histogram.counts), std::end(histogram.counts), 0);
        LUT_uniform = this->LUT(F_uniform, total_counts);
    }
    double
    uniform_distro_cdf(const double &x, const double &a, const double &b) {
        if (x < a)
            return 0;
        else if (x >= b)
            return 1;
        else {
            return (x - a) / (b - a);
        }
    }

    std::vector<double>
    apply_uniform_distro_cdf(const double &a,
                             const double &b,
                             const std::vector<double> &centers) {
        std::vector<double> result(std::size(centers));
        auto distro = [&a, &b, this](const double &x) -> double {
            return this->uniform_distro_cdf(x, a, b);
        };
        std::transform(std::begin(centers), std::end(centers),
                       std::begin(result), distro);
        return result;
    }
    std::vector<double> LUT(const std::vector<double> &cdf_distro,
                            const size_t &input_total_counts) {
        std::vector<double> result(std::size(cdf_distro));
        auto lut_func = [&input_total_counts](const double &x) -> double {
            return input_total_counts * x + 0.5;
        };
        std::transform(std::begin(cdf_distro), std::end(cdf_distro),
                       std::begin(result), lut_func);
        return result;
    }

    SG::Histogram histogram;
    TVectorFloat F_uniform;
    TVectorFloat LUT_uniform;
    double a_ = 0.0;
    double b_ = 30.0;
    size_t histo_bins = 30;
    size_t total_counts;
};

TEST_F(CramerVonMisesUniformDistributionTestFixture,
       is_zero_when_distro_and_cdf_are_aligned) {

    auto test_value = SG::cramer_von_mises_test(histogram.counts, F_uniform);
    std::cout << "Test value for uniform distro: " << test_value << std::endl;
    EXPECT_LT(test_value, 0.008);
    auto test_value_optimized = SG::cramer_von_mises_test_optimized(
            histogram.counts, LUT_uniform, total_counts);
    std::cout << "Test value for uniform distro: " << test_value_optimized
              << std::endl;
    EXPECT_DOUBLE_EQ(test_value_optimized, test_value);
}

TEST_F(CramerVonMisesUniformDistributionTestFixture,
       high_value_when_all_modified) {

    auto modified_counts = histogram.counts;
    auto modified_total_counts = 0;
    for (auto &c : modified_counts) {
        c += 1;
        modified_total_counts += c;
    }
    auto modified_LUT_uniform = this->LUT(F_uniform, modified_total_counts);
    auto test_value = SG::cramer_von_mises_test(modified_counts, F_uniform);
    std::cout << "Test value for uniform distro MODIFIED: " << test_value
              << std::endl;
    EXPECT_GT(test_value, 0.008);
    auto test_value_optimized = SG::cramer_von_mises_test_optimized(
            modified_counts, modified_LUT_uniform, modified_total_counts);
    std::cout << "Test value for uniform distro: " << test_value_optimized
              << std::endl;
    EXPECT_DOUBLE_EQ(test_value_optimized, test_value);
}

TEST_F(CramerVonMisesUniformDistributionTestFixture,
       slight_increase_when_only_one_value_is_modified) {
    auto modified_counts = histogram.counts;
    EXPECT_GT(modified_counts.size(), 0);
    auto modified_total_counts = total_counts;
    modified_counts[0] += 1;
    modified_total_counts += 1;
    auto modified_LUT_uniform = this->LUT(F_uniform, modified_total_counts);
    auto test_value = SG::cramer_von_mises_test(modified_counts, F_uniform);
    std::cout << "Test value for uniform distro MODIFIED: " << test_value
              << std::endl;
    EXPECT_GT(test_value, 0.008);
    EXPECT_LT(test_value, 1e6);

    auto test_value_optimized = SG::cramer_von_mises_test_optimized(
            modified_counts, modified_LUT_uniform, modified_total_counts);
    std::cout << "Test value for uniform distro: " << test_value_optimized
              << std::endl;
    EXPECT_DOUBLE_EQ(test_value_optimized, test_value);
}

TEST_F(CramerVonMisesUniformDistributionTestFixture, compute_T) {
    EXPECT_GT(histogram.counts.size(), 0);
    auto M = SG::compute_cumulative_counts(histogram.counts);
    const auto total_counts = M.back() + histogram.counts.back();
    auto S = SG::compute_S(M, F_uniform, total_counts);
    auto T = SG::compute_T(S, histogram.counts);
    std::cout << "histogram.counts:" << std::endl;
    for (const auto &t : histogram.counts) {
        std::cout << t << ", ";
    }
    std::cout << std::endl;
    std::cout << "M:" << std::endl;
    for (const auto &t : M) {
        std::cout << t << ", ";
    }
    std::cout << std::endl;
    std::cout << "S:" << std::endl;
    for (const auto &t : S) {
        std::cout << t << ", ";
    }
    std::cout << std::endl;
    std::cout << "T:" << std::endl;
    for (const auto &t : T) {
        std::cout << t << ", ";
    }
    std::cout << std::endl;
}
