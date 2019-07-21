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
