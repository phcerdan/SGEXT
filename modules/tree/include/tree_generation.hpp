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

#ifndef SG_TREE_GENERATION_HPP
#define SG_TREE_GENERATION_HPP

#include "image_types.hpp" // for FloatImageType

#include "spatial_graph.hpp"
#include <unordered_map>

namespace SG {

/**
 * Associate to each node of the graph a generation based on the branching of
 * the tree. Generation = 0 is associated to the root node. An end node of the
 * first branching will have generation 1.
 *
 * Sometimes is not clear in a Y branching if both ends will get its generation
 * increased, or only one. The parameter
 * decrease_radius_ratio_to_increase_generation allows to fine tune this
 * behaviour. Those nodes that have a smaller radius would increase its
 * generation.
 *
 * @sa tree_generation_visitor
 *
 * @param graph input spatial graph
 *
 * @param distance_map_image input distance map image @ref create_distance_map
 *
 * @param spatial_nodes_position_are_in_physical_space true if the positions
 * are in physical space.
 *
 * @param decrease_radius_ratio_to_increase_generation controls when the
 *  generation of a node will be increased, based on the radius decrease.
 *
 * @param keep_generation_if_angle_less_than Do not increase generation for
 *  angles smaller than this (in degrees). Used in the first pass using radius.
 *
 * @param increase_generation_if_angle_greater_than Increase generation if
 *  the angle is greater than this value. Angle analysis is used in a second
 * pass analysis, and it only applies to the cases where the first pass (based
 * on radius) generates at leat two branches with the same generation. It fails
 * to increase the target branch generation, when one of the sibling branches
 *  (with same source) got its generation increased by the radius first pass.
 *
 * @param num_of_edge_points_to_compute_angle
 *  When computing angles between edges with same source,
 *  use the edge point number m_num_of_edge_points_to_compute_angle closer to
 *  source.  Warning: This is not an index, but a count.
 *  If = 0, it uses the angle between source and target nodes (no edge points
 * involved)
 *
 * @param input_roots
 *  Vector holding user specified root nodes. If empty, the algorithm takes
 *  as root the vertex with greatest radius (from the distance map), and
 *  runs only in the largest connected component of the graph.
 *  If multiple roots are provided, the algorithm will start visiting from them.
 *  Also, it will assume there are disconnected components in the graph, one for
 *  each root.
 *
 * @param input_fixed_generation_map
 *  Map provided by the user to fix the generation for certain nodes.
 *  The algorithm won't modify the generation on those vertex.
 *  Empty by default.
 *
 * @param verbose extra infro to std::cout
 *
 * @return A map vertex to generation
 */
using VertexGenerationMap =
        std::unordered_map<GraphType::vertex_descriptor, size_t>;
VertexGenerationMap tree_generation(
        const GraphType &graph,
        const typename FloatImageType::Pointer &distance_map_image,
        const bool spatial_nodes_position_are_in_physical_space = false,
        const double &decrease_radius_ratio_to_increase_generation = 0.1,
        const double &keep_generation_if_angle_less_than = 10,
        const double &increase_generation_if_angle_greater_than = 40,
        const size_t &num_of_edge_points_to_compute_angle = 5,
        const std::vector<GraphType::vertex_descriptor> &input_roots =
        std::vector<GraphType::vertex_descriptor>(),
        const VertexGenerationMap &input_fixed_generation_map =
                VertexGenerationMap(),
        const bool verbose = false);

/**
 * Read/Write a CSV-like file containing a one-line header and two
 * comma-separated values with vertex_id and generation.
 *
 * Example:
 * # vertex_id, generation
 * 4, 0
 * 5, 1
 * 12, 2
 *
 * This map can be used in @ref tree_generation to fix those nodes, avoiding
 * that tree_generation modifies them.
 *
 * @param input_fixed_generation_map_file csv file
 *
 * @return VertexGenerationMap: vertex_to_generation_map
 */
VertexGenerationMap read_vertex_to_generation_map(
        const std::string &input_fixed_generation_map_file);
void write_vertex_to_generation_map(
        const VertexGenerationMap &vertex_to_generation_map,
        const std::string &output_filename);

} // end namespace SG
#endif
