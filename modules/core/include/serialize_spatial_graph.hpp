/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SERIALIZE_SPATIAL_GRAPH_HPP
#define SERIALIZE_SPATIAL_GRAPH_HPP

#include "spatial_graph.hpp"
#include <string>

namespace SG {

[[deprecated("Use write_serialized_sg from spatial_graph_io instead")]] void
write_serialized_graph(GraphType &sg, const std::string &absolute_path);
[[deprecated("Use read_serialized_sg from spatial_graph_io instead")]] GraphType
read_serialized_graph(const std::string &absolute_path);
} // namespace SG
#endif
