/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_IO_HPP
#define SPATIAL_GRAPH_IO_HPP

#include "spatial_graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <ostream>

namespace SG {

boost::dynamic_properties write_dynamic_properties_sg(GraphType &graph) {
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    dp.property("spatial_node", boost::get(boost::vertex_bundle, graph));
    dp.property("spatial_edge", boost::get(boost::edge_bundle, graph));
    return dp;
}
boost::dynamic_properties read_dynamic_properties_sg(GraphType &graph) {
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(&SpatialNode::id, graph));
    dp.property("spatial_node", boost::get(boost::vertex_bundle, graph));
    dp.property("spatial_edge", boost::get(boost::edge_bundle, graph));
    return dp;
}

void write_graphviz_sg(std::ostream &os, GraphType &graph) {
    auto dp = write_dynamic_properties_sg(graph);
    boost::write_graphviz_dp(os, graph, dp);
}
void write_graphviz_sg(const std::string &output_file, GraphType &graph) {
    auto dp = write_dynamic_properties_sg(graph);
    std::ofstream ofile(output_file);
    boost::write_graphviz_dp(ofile, graph, dp);
}

void read_graphviz_sg(std::istream &is, GraphType &graph) {
    auto dp = read_dynamic_properties_sg(graph);
    boost::read_graphviz(is, graph, dp);
}
void read_graphviz_sg(const std::string &input_file, GraphType &graph) {
    auto dp = read_dynamic_properties_sg(graph);
    std::ifstream ifile(input_file);
    boost::read_graphviz(ifile, graph, dp);
}

} // namespace SG
#endif
