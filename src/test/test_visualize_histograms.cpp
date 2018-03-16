/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * See LICENSE on top of this repository. */

#include "catch_header.h"
#include "visualize_histo.hpp" // External, from histo.
#include "visualize_histograms.hpp" // Visualize using Qt and histo.

// Unit testing is done in the external project, this just checks that works here.
TEST_CASE("ExternalProject Histo works", "[external][visualize][histo]")
{
    std::vector<double> data{0.0, 1.0, 1.0,1.0, 2.0, 3.0, 5.0, 5.0, 8.0, 8.0,  12.0};
    histo::Histo<double> h(data, histo::GenerateBreaksFromRangeAndBins<double>(0.0,15.0, 5));
    h.PrintBreaksAndCounts(std::cout);
    h.name = "withJustData";
    histo::visualize_histo(h);
    histo::visualize_histo(h, vtkChart::BAR);
}

TEST_CASE("visualize_histograms", "[visualize][histo]")
{
    std::vector<double> data{0.0, 1.0, 1.0,1.0, 2.0, 3.0, 5.0, 5.0, 8.0, 8.0,  12.0};
    std::vector<histo::Histo<double>> histograms;
    histograms.emplace_back(
            histo::Histo<double>(data, histo::GenerateBreaksFromRangeAndBins<double>(0.0,15.0, 5))
            );
    histograms.back().name = "h1";
    histograms.emplace_back(
            histo::Histo<double>(data, histo::GenerateBreaksFromRangeAndBins<double>(0.0,15.0, 5))
            );
    histograms.back().name = "h2";
    histograms.emplace_back(
            histo::Histo<double>(data, histo::GenerateBreaksFromRangeAndBins<double>(0.0,15.0, 5))
            );
    histograms.back().name = "h3";
    histograms.emplace_back(
            histo::Histo<double>(data, histo::GenerateBreaksFromRangeAndBins<double>(0.0,15.0, 5))
            );
    histograms.back().name = "h4";

    SG::visualize_histograms(histograms);
}
