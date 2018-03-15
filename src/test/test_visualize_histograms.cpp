/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * See LICENSE on top of this repository. */

#include "catch_header.h"
#include "visualize_histo.hpp"

TEST_CASE("VisualizeHisto", "[withJustData]")
{
    std::vector<double> data{0.0, 1.0, 1.0,1.0, 2.0, 3.0, 5.0, 5.0, 8.0, 8.0,  12.0};
    histo::Histo<double> h(data, histo::GenerateBreaksFromRangeAndBins<double>(0.0,15.0, 5));
    h.PrintBreaksAndCounts(std::cout);
    h.name = "withJustData";
    visualize_histo(h);
    visualize_histo(h, vtkChart::BAR);
}
