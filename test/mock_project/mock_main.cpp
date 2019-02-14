/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include "spatial_graph.hpp"
#include "compute_graph_properties.hpp"

int main()
{
    SG::GraphType g(2);
    SG::compute_degrees(g);
    std::cout << "Finished!" << std::endl;
}
