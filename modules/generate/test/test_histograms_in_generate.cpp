#include "generate_common.hpp"
#include "gmock/gmock.h"

struct HistoFixture : public ::testing::Test {
    void SetUp() override {
        breaks = histo::GenerateBreaksFromRangeAndBins<double>(
                init_histo_break, final_histo_break, histo_bins);
        histogram = SG::Histogram(histogram_data, breaks);
        histogram.counts = histogram_counts;
    }
    std::vector<double> histogram_data;
    double init_histo_break = -0.5;
    double final_histo_break = 0.5;
    size_t histo_bins = 7;
    double step_size = 1.0 / histo_bins;
    std::vector<size_t> histogram_counts = {{1, 2, 4, 6, 4, 2, 1}};
    std::vector<double> breaks;
    SG::Histogram histogram;
};

TEST_F(HistoFixture, works) {
    EXPECT_GT(histogram.breaks.size(), 0);
    EXPECT_GT(histogram.counts.size(), 0);
    // histogram.PrintBreaksAndCounts(std::cout);

    double value0 = init_histo_break;
    auto bin0 = histogram.IndexFromValue(value0);
    EXPECT_EQ(bin0, 0);
    double value1 = init_histo_break + step_size;
    auto bin1 = histogram.IndexFromValue(value1);
    EXPECT_EQ(bin1, 1);
    double value2 = init_histo_break + 2 * step_size;
    auto bin2 = histogram.IndexFromValue(value2);
    EXPECT_EQ(bin2, 2);
    double value3 = init_histo_break + 3 * step_size;
    auto bin3 = histogram.IndexFromValue(value3);
    EXPECT_EQ(bin3, 3);
    double value4 = init_histo_break + 4 * step_size;
    auto bin4 = histogram.IndexFromValue(value4);
    EXPECT_EQ(bin4, 4);
    double value5 = init_histo_break + 5 * step_size;
    auto bin5 = histogram.IndexFromValue(value5);
    EXPECT_EQ(bin5, 5);
    double value6 = init_histo_break + 6 * step_size;
    auto bin6 = histogram.IndexFromValue(value6);
    EXPECT_EQ(bin6, 6);

    auto bin_final = histogram.IndexFromValue(final_histo_break);
    EXPECT_EQ(bin_final, 6);

    EXPECT_ANY_THROW(histogram.IndexFromValue(init_histo_break - 10));
}
