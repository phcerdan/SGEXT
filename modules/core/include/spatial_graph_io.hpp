/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_IO_HPP
#define SPATIAL_GRAPH_IO_HPP

#include "spatial_graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace SG {

// Graphviz
boost::dynamic_properties get_write_dynamic_properties_sg(GraphType &graph);
boost::dynamic_properties get_read_dynamic_properties_sg(GraphType &graph);
void write_graphviz_sg(std::ostream &os, GraphType &graph);
void write_graphviz_sg(const std::string &output_file, GraphType &graph);
void read_graphviz_sg(std::istream &is, GraphType &graph);
void read_graphviz_sg(const std::string &input_file, GraphType &graph);
GraphType read_graphviz_sg(const std::string &input_file);

// Serialize
void write_serialized_sg(std::ostream &os, const GraphType &graph);
void write_serialized_sg(const std::string &output_file, const GraphType &graph);
void read_serialized_sg(std::istream &is, GraphType &graph);
void read_serialized_sg(const std::string &input_file, GraphType &graph);
GraphType read_serialized_sg(const std::string &input_file);

} // namespace SG
#endif
