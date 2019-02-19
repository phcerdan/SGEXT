/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SERIALIZE_SPATIAL_GRAPH_HPP
#define SERIALIZE_SPATIAL_GRAPH_HPP

#include <string>
#include "spatial_graph.hpp"

namespace SG {

void write_serialized_graph(GraphType & sg, const std::string & absolute_path);
GraphType read_serialized_graph(const std::string & absolute_path);
} // ns SG
#endif
