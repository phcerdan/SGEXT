/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "graph_data.hpp"
#include "gmock/gmock.h"
#include <algorithm>
#include <sstream>

TEST(IO, print_and_read_graph_data) {
    // Setup data
    std::vector<double> degrees({1, 2, 3, 4});
    std::string header = "degrees";
    std::stringstream buffer;
    SG::print_graph_data(header, degrees, buffer);
    // Read data
    auto head_data = SG::read_graph_data(buffer);
    EXPECT_EQ(head_data.first, header);
    EXPECT_EQ(head_data.second, degrees);
}
