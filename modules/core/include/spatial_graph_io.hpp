/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#ifndef SPATIAL_GRAPH_IO_HPP
#define SPATIAL_GRAPH_IO_HPP

#include "hash_edge_descriptor.hpp"
#include "spatial_graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace SG {

using vertex_to_label_map_t =
        std::unordered_map<GraphType::vertex_descriptor, size_t>;

using edge_to_label_map_t = std::unordered_map<GraphType::edge_descriptor,
                                               size_t,
                                               SG::edge_hash<GraphType>>;

/* ************* Graphviz *************/
boost::dynamic_properties get_write_dynamic_properties_sg(GraphType &graph);
boost::dynamic_properties get_read_dynamic_properties_sg(GraphType &graph);
void write_graphviz_sg(std::ostream &os, GraphType &graph);
void write_graphviz_sg(const std::string &output_file, GraphType &graph);
void read_graphviz_sg(std::istream &is, GraphType &graph);
void read_graphviz_sg(const std::string &input_file, GraphType &graph);
GraphType read_graphviz_sg(const std::string &input_file);

/* ************* Serialize *************/
void write_serialized_sg(std::ostream &os, const GraphType &graph);
void write_serialized_sg(const std::string &output_file,
                         const GraphType &graph);
void read_serialized_sg(std::istream &is, GraphType &graph);
void read_serialized_sg(const std::string &input_file, GraphType &graph);
GraphType read_serialized_sg(const std::string &input_file);

/* ************ vertex and edge label maps ************/

/**
 * Read/Write a CSV-like file containing a one-line header and two
 * comma-separated values with vertex_id and label.
 *
 * Example:
 * # vertex_id, label
 * 4, 0
 * 5, 1
 * 12, 2
 *
 * This map can be used in @ref voxelize_graph.
 *
 * @param input_fixed_generation_map_file csv file
 *
 * @return vertex_to_label_map
 */
vertex_to_label_map_t
read_vertex_to_label_map(const std::string &vertex_to_label_map_file);
void write_vertex_to_label_map(const vertex_to_label_map_t &vertex_to_label_map,
                               const std::string &output_filename);

/**
 * Read/Write a CSV-like file containing a one-line header and two
 * comma-separated values with edge_id and label.
 *
 * The column holding the edge has a '-' separating source and target.
 *
 * Example:
 * # edge_source-target, label
 * 4-12, 0
 * 23-43, 1
 * 12-2, 2
 *
 * @param edge_to_label_map_file
 *
 * @return
 */
edge_to_label_map_t
read_edge_to_label_map(const std::string &edge_to_label_map_file);
void write_edge_to_label_map(const edge_to_label_map_t &edge_to_label_map,
                             const std::string &output_filename);

} // namespace SG
#endif
