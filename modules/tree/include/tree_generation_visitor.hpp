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

#ifndef SG_TREE_GENERATION_VISITOR_HPP
#define SG_TREE_GENERATION_VISITOR_HPP

#include "array_utilities.hpp"
#include "rng.hpp"           // for RNG::pi
#include "image_types.hpp" // for FloatImageType

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/logic/tribool_io.hpp>

#include <iostream>
#include <unordered_map>

namespace SG {

/**
 * Use BFS (Breadth first search/visitor) to compute generations of a tree-like
 * spatial graph.
 *
 * @sa tree_generation for a ready to use function using this visitor.
 *
 * Extra from:
https://www.boost.org/doc/libs/1_65_1/libs/graph/doc/breadth_first_search.html

- vis.initialize_vertex(v, g) is invoked on every vertex before the start of the
search.
- vis.examine_vertex(u, g)r is invoked in each vertex as it is removed from the
queue.
- vis.examine_edge(e, g) is invoked on every out-edge of each vertex immediately
after the vertex is removed from the queue.
- vis.tree_edge(e, g) is invoked (in addition to examine_edge()) if the edge is
a tree edge. The target vertex of edge e is discovered at this time.

- vis.discover_vertex(u, g) is invoked the first time the algorithm encounters
vertex u. All vertices closer to the source vertex have been discovered, and
vertices further from the source have not yet been discovered.

- vis.non_tree_edge(e, g) is invoked (in addition to examine_edge()) if the edge
is not a tree edge.
- vis.gray_target(e, g) is invoked (in addition to non_tree_edge()) if the
target vertex is colored gray at the time of examination. The color gray
indicates that the vertex is currently in the queue.
- vis.black_target(e, g) is invoked (in addition to non_tree_edge()) if the
target vertex is colored black at the time of examination. The color black
indicates that the vertex is no longer in the queue.
- vis.finish_vertex(u, g) is invoked after all of the out edges of u have been
examined and all of the adjacent vertices have been discovered.
 */
template <typename SpatialGraph>
struct TreeGenerationVisitor : public boost::default_bfs_visitor {
    using SpatialGraphVertexBundle =
            typename boost::vertex_bundle_type<SpatialGraph>::type;
    using SpatialVertex = SpatialGraphVertexBundle;
    using SpatialGraphEdgeBundle =
            typename boost::edge_bundle_type<SpatialGraph>::type;
    using SpatialEdge = SpatialGraphEdgeBundle;
    using vertex_descriptor =
            typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using edge_descriptor =
            typename boost::graph_traits<SpatialGraph>::edge_descriptor;
    using VertexToGenerationMap = std::unordered_map<vertex_descriptor, size_t>;
    using VertexToLocalRadiusMap =
            std::unordered_map<vertex_descriptor, double>;
    using VertexToDistanceFromRootMap =
            std::unordered_map<vertex_descriptor, size_t>;
    using VertexAnomalies = std::unordered_map<vertex_descriptor, double>;
    using VertexAlreadyIncreasedMap =
            std::unordered_map<vertex_descriptor, bool>;

    TreeGenerationVisitor(
            VertexToGenerationMap &vertex_to_generation_map,
            const typename SG::FloatImageType::Pointer &distance_map_image,
            const VertexToLocalRadiusMap &vertex_to_local_radius_map,
            VertexToDistanceFromRootMap &vertex_to_distance_from_root_map,
            const double &decrease_radius_ratio = 0.1,             // 10%
            const double &keep_generation_if_angle_less_than = 10, // degrees
            const double &increase_generation_if_angle_greater_than = 40,
            const size_t &num_of_edge_points_to_compute_angle = 5,
            const bool &spatial_nodes_position_are_in_physical_space = false,
            VertexAnomalies &vertex_anomalies = VertexAnomalies(),
            const bool &verbose = false)
            : m_vertex_to_generation_map(vertex_to_generation_map),
              m_distance_map_image(distance_map_image),
              m_vertex_to_local_radius_map(vertex_to_local_radius_map),
              m_vertex_to_distance_from_root_map(
                      vertex_to_distance_from_root_map),
              m_decrease_radius_ratio_to_increase_generation(
                      decrease_radius_ratio),
              m_keep_generation_if_angle_less_than(
                      keep_generation_if_angle_less_than),
              m_increase_generation_if_angle_greater_than(
                      increase_generation_if_angle_greater_than),
              m_num_of_edge_points_to_compute_angle(
                      num_of_edge_points_to_compute_angle),
              m_spatial_nodes_position_are_in_physical_space(
                      spatial_nodes_position_are_in_physical_space),
              m_vertex_anomalies(vertex_anomalies), m_verbose(verbose) {}

    // Copy constructor. The breadth_first_search takes a copy of the input
    // visitor.
    TreeGenerationVisitor(const TreeGenerationVisitor<SpatialGraph> &other)
            : m_vertex_to_generation_map(other.m_vertex_to_generation_map),
              m_distance_map_image(other.m_distance_map_image),
              m_vertex_to_local_radius_map(other.m_vertex_to_local_radius_map),
              m_vertex_to_distance_from_root_map(
                      other.m_vertex_to_distance_from_root_map),
              m_decrease_radius_ratio_to_increase_generation(
                      other.m_decrease_radius_ratio_to_increase_generation),
              m_keep_generation_if_angle_less_than(
                      other.m_keep_generation_if_angle_less_than),
              m_increase_generation_if_angle_greater_than(
                      other.m_increase_generation_if_angle_greater_than),
              m_num_of_edge_points_to_compute_angle(
                      other.m_num_of_edge_points_to_compute_angle),
              m_spatial_nodes_position_are_in_physical_space(
                      other.m_spatial_nodes_position_are_in_physical_space),
              m_vertex_anomalies(other.m_vertex_anomalies),
              m_vertex_already_increased(other.m_vertex_already_increased),
              m_verbose(other.m_verbose){};

    /**
     * A map from vertex to a generation number.
     * This is the output of the visitor.
     */
    VertexToGenerationMap &m_vertex_to_generation_map;
    const typename SG::FloatImageType::Pointer &m_distance_map_image;
    /**
     * A map from vertex to the value of the local radius, obtained from
     * a distance map.
     * A distance map can be computed from any binary image, it assigns to each
     * voxel the value of the distance from that voxel to the nearest border of
     * the foreground object. Here it works as an approximation to the local
     * radius of a filament.
     */
    const VertexToLocalRadiusMap &m_vertex_to_local_radius_map;
    /**
     * distance recorder map. Computes the distance of each node to the root.
     * It grows inside tree_edge.
     */
    VertexToDistanceFromRootMap &m_vertex_to_distance_from_root_map;
    /**
     * Generation of a connected node only will be increase +1 if the radius is
     * relatively smaller than the source node by this value.
     */
    const double &m_decrease_radius_ratio_to_increase_generation;
    /**
     * Keep the same generation if the angle (in degrees) is less than this
     * value.
     */
    const double &m_keep_generation_if_angle_less_than;
    /**
     * Increase generation if the angle is greater than this value. Angle
     * analysis is used in a second pass analysis, and it only applies to the
     * cases where the first pass (based on radius) generates at leat two
     * branches with the same generation. It fails to increase the target branch
     * generation, when one of the sibling branches (with same source) got its
     * generation increased by the radius first pass.
     */
    const double &m_increase_generation_if_angle_greater_than;
    /**
     * When computing angles between edges with same source,
     * use the edge point number m_num_of_edge_points_to_compute_angle closer to
     * source.
     * Warning: This is not an index, but a count.
     */
    const size_t &m_num_of_edge_points_to_compute_angle;
    /**
     * Positions in the graph are in physical space (using image metadata)
     */
    const bool &m_spatial_nodes_position_are_in_physical_space;
    /**
     * Populated with the ratio target_radius/source_radius only when target
     * radius is greater than the source radius.
     */
    VertexAnomalies &m_vertex_anomalies;

    /**
     * Internal map to check if a vertex has been already increased. Used
     * when studying angles.
     */
    VertexAlreadyIncreasedMap m_vertex_already_increased =
            VertexAlreadyIncreasedMap();

    /**
     * Extra information to std::cout
     */
    const bool &m_verbose;

  public:
    /**
     * invoked when a vertex is encountered for the first time.
     *
     * @param u
     * @param input_sg
     */
    // void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
    //     if (m_verbose) {
    //         std::cout << "discover_vertex: " << u << " | pos : ("
    //                   << ArrayUtilities::to_string(input_sg[u].pos)
    //                   << ") | radius: " << m_vertex_to_local_radius_map.at(u)
    //                   << std::endl;
    //     }
    // }

    /**
     * Use source and target node and the middle point of the edge points.
     * The middle point of the spatial edge points will be used to differentiate
     * an edge with two regimes versus a smooth edge with a constant decrease of
     * radius.
     *
     * Radius diference between source and target:
     * |---------------------|
     * s                     t
     * If using middle ~ 2 options:
     * - Two vessels of different radius when the two drops are quite different.
     * |----------------|   +   |------|
     * s                m       m      t
     * - Both drop are similar indicates that the vessel is smoothly decreasing
     * the ratio which indicates that there is no jump. This would correspond to
     * the main vessel.
     * |-----------|   +   |---------|
     * s           m       m         t
     *
     * @param input_edge
     * @param decrease_radius_ratio_to_increase_generation
     * @param input_sg
     *
     * @return
     */
    boost::tribool increase_generation_based_on_radius_of_edge(
            edge_descriptor input_edge,
            const SpatialGraph &input_sg,
            const size_t minimum_size_edge_points = 5,
            const double differences_ratio = 2.0) const {
        const auto source = boost::source(input_edge, input_sg);
        const auto target = boost::target(input_edge, input_sg);
        const auto &source_radius = m_vertex_to_local_radius_map.at(source);
        const auto &target_radius = m_vertex_to_local_radius_map.at(target);
        const auto &edge_points = input_sg[input_edge].edge_points;
        const auto edge_points_size = std::size(edge_points);
        // There must be at least some edge points for this to make sense
        if (edge_points_size < minimum_size_edge_points) {
            return boost::logic::indeterminate;
        }
        // Get the middle edge point radius (using distance_map_image)
        const size_t middle_index = std::size(edge_points) / 2;
        const auto &middle_edge_point = edge_points[middle_index];
        typename FloatImageType::IndexType itk_index;
        if (m_spatial_nodes_position_are_in_physical_space) {
            typename FloatImageType::PointType itk_point;
            for (size_t i = 0; i < FloatImageType::ImageDimension; i++) {
                itk_point[i] = middle_edge_point[i];
            }
            m_distance_map_image->TransformPhysicalPointToIndex(itk_point,
                                                                itk_index);
        } else {
            for (size_t i = 0; i < FloatImageType::ImageDimension; i++) {
                itk_index[i] = middle_edge_point[i];
            }
        }
        const auto middle_radius = m_distance_map_image->GetPixel(itk_index);

        // compare radius between source, target and middle.
        const double target_middle_abs_diff =
                std::abs(target_radius - middle_radius);
        const double source_middle_abs_diff =
                std::abs(source_radius - middle_radius);

        const auto min_max_diff =
                std::minmax(target_middle_abs_diff, source_middle_abs_diff);
        const double max_min_ratio = min_max_diff.second / min_max_diff.first;
        // increase the generation if there is a significative difference
        // between radius differences from end points to the middle point
        if (max_min_ratio > differences_ratio) {
            return true;
        }
        return false;
    }

    bool increase_generation_based_on_radius_of_nodes(
            const double &decrease_ratio,
            const double &decrease_radius_ratio_to_increase_generation) const {
        // decrease percentage
        const bool increase_generation_because_radius_of_nodes =
                decrease_ratio >= decrease_radius_ratio_to_increase_generation
                        ? true
                        : false;
        return increase_generation_because_radius_of_nodes;
    }

    std::vector<edge_descriptor>
    get_edges_with_same_source_than_input_edge(edge_descriptor input_edge,
                                               const SpatialGraph &input_sg) {
        const auto source = boost::source(input_edge, input_sg);
        const auto target = boost::target(input_edge, input_sg);
        using out_edge_iterator =
                boost::graph_traits<GraphType>::out_edge_iterator;
        out_edge_iterator ei, ei_end;
        std::vector<edge_descriptor> edges_with_same_source_than_input_edge;
        for (std::tie(ei, ei_end) = boost::out_edges(source, input_sg);
             ei != ei_end; ++ei) {
            if (*ei != input_edge) {
                const auto edge_target = boost::target(*ei, input_sg);
                const auto edge_pair_exist =
                        boost::edge(target, edge_target, input_sg);
                // if both targets are connected, ignore this edge.
                if (edge_pair_exist.second) {
                    continue;
                }
                edges_with_same_source_than_input_edge.push_back(*ei);
            }
        }
        return edges_with_same_source_than_input_edge;
    }

    std::vector<size_t> get_indices_with_smallest_distance_from_root(
            const std::vector<size_t> &distances_from_root) {
        std::vector<size_t> indices_with_smallest_distance_from_root;
        const size_t min_distance_from_root = *std::min_element(
                distances_from_root.cbegin(), distances_from_root.cend());
        auto iter = distances_from_root.cbegin();
        while ((iter = std::find_if(
                        iter, distances_from_root.cend(),
                        [&min_distance_from_root](const size_t dist) {
                            return dist == min_distance_from_root;
                        })) != distances_from_root.cend()) {
            indices_with_smallest_distance_from_root.push_back(
                    std::distance(distances_from_root.cbegin(), iter));
            iter++;
        }
        return indices_with_smallest_distance_from_root;
    }

    bool all_out_edge_targets_have_generation_populated(
            vertex_descriptor input_target,
            const std::vector<edge_descriptor> &other_out_edges,
            const SpatialGraph &input_sg) {
        // If generation of input_target is not populate, return false.
        auto find_input_target_generation =
                m_vertex_to_generation_map.find(input_target);
        if (find_input_target_generation ==
            std::end(m_vertex_to_generation_map)) {
            return false;
        }
        // Since we accept an input_fix_generation_map that populates the
        // m_vertex_to_generation_map, even before having visit that edge: also
        // check if m_vertex_to_distance_from_root_map is populated.
        //
        // If distance_from_root of input_target is not populate, return false.
        auto find_input_target_distance_from_root =
                m_vertex_to_distance_from_root_map.find(input_target);
        if (find_input_target_distance_from_root ==
            std::end(m_vertex_to_distance_from_root_map)) {
            return false;
        }
        for (auto &edge : other_out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            // Check if generation is populated
            auto find_edge_target_generation =
                    m_vertex_to_generation_map.find(edge_target);
            if (find_edge_target_generation ==
                std::end(m_vertex_to_generation_map)) {
                // not all generations are populated
                return false;
            }
            // Check if distance_from_root is populated
            auto find_edge_target_distance_from_root =
                    m_vertex_to_distance_from_root_map.find(edge_target);
            if (find_edge_target_distance_from_root ==
                std::end(m_vertex_to_distance_from_root_map)) {
                // not all distance_from_roots are populated
                return false;
            }
        }
        // all_targets_have_generation_populated and distance_from_root
        return true;
    }

    std::vector<edge_descriptor> get_out_edges_with_lowest_same_generation(
            const std::vector<edge_descriptor> &other_out_edges,
            const SpatialGraph &input_sg) {
        std::vector<size_t> generations;
        for (auto &edge : other_out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            auto find_edge_target_generation =
                    m_vertex_to_generation_map.find(edge_target);
            if (find_edge_target_generation ==
                std::end(m_vertex_to_generation_map)) {
                // not all generations are populated, return empty vector.
                return std::vector<edge_descriptor>();
            } else {
                const auto edge_target_generation =
                        find_edge_target_generation->second;
                generations.push_back(edge_target_generation);
            }
        }
        std::map<size_t, size_t> count_generations;
        for (const auto &gen : generations) {
            count_generations[gen]++;
        }
        // Get generations with count > 1 and choose the lowest
        const size_t init_value = std::numeric_limits<size_t>::max();
        size_t lowest_generation_with_multiple_counts = init_value;
        for (const auto &gen_count_pair : count_generations) {
            const auto &gen = gen_count_pair.first;
            const auto &count = gen_count_pair.second;
            if (count > 1) {
                if (gen < lowest_generation_with_multiple_counts)
                    lowest_generation_with_multiple_counts = gen;
            }
        }
        // Return the edges with the lowest_generation_with_multiple_counts
        std::vector<edge_descriptor> output;
        for (size_t i = 0; i < generations.size(); ++i) {
            const auto &gen = generations[i];
            const auto &edge = other_out_edges[i];
            if (gen == lowest_generation_with_multiple_counts) {
                output.push_back(edge);
            }
        }
        return output;
    }

    /**
     * Check if the generation of target is the same than the other out_edges
     * Use @ref get_edges_with_same_source_than_input_edge to populate
     * other_out_edges
     *
     * @param input_target
     * @param other_out_edges
     * @param input_sg
     *
     * @return true if all generations are the same
     */
    bool all_out_edge_targets_have_same_generation_than_input_target(
            vertex_descriptor input_target,
            const std::vector<edge_descriptor> &other_out_edges,
            const SpatialGraph &input_sg) {
        // If generation of input_target is not populate, return false.
        auto find_input_target_generation =
                m_vertex_to_generation_map.find(input_target);
        if (find_input_target_generation ==
            std::end(m_vertex_to_generation_map)) {
            return false;
        }
        const size_t target_generation = find_input_target_generation->second;
        for (auto &edge : other_out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            auto find_edge_target_generation =
                    m_vertex_to_generation_map.find(edge_target);
            if (find_edge_target_generation ==
                std::end(m_vertex_to_generation_map)) {
                // not all generations are populated
                return false;
            } else {
                const auto edge_target_generation =
                        find_edge_target_generation->second;
                if (edge_target_generation != target_generation) {
                    return false;
                }
            }
        }

        // all_targets_have_same_generation;
        return true;
    }

    std::optional<std::vector<vertex_descriptor>>
    out_edges_with_same_generation(
            vertex_descriptor input_target,
            const std::vector<edge_descriptor> &other_out_edges,
            const SpatialGraph &input_sg) {
        // Get all edge target generations and find duplicates
        // If generation of input_target is not populate, return false.
        auto find_input_target_generation =
                m_vertex_to_generation_map.find(input_target);
        if (find_input_target_generation ==
            std::end(m_vertex_to_generation_map)) {
            return {};
        }
        std::vector<vertex_descriptor> edge_targets_generations;
        for (auto &edge : other_out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            auto find_edge_target_generation =
                    m_vertex_to_generation_map.find(edge_target);
            if (find_edge_target_generation ==
                std::end(m_vertex_to_generation_map)) {
                // not all generations are populated
                return {};
            } else {
                const auto edge_target_generation =
                        find_edge_target_generation->second;
                edge_targets_generations.push_back(edge_target_generation);
            }
        }
        // Find duplicates
        using Generation = size_t;
        std::unordered_map<Generation, size_t> frequencies;
        for (size_t i = 0; i < edge_targets_generations.size(); ++i) {
            frequencies[edge_targets_generations[i]]++;
        }
    }

    /**
     * After the generation has been set in the target of input edge, use this
     * to force an increase of generation in the case the generation hasn't
     * been increased in any edge with the same source than input edge
     *
     *             t1
     *             o
     * source     /
     *     o-----o
     *            \
     *             o
     *             t2
     *
     *   if t1 and t2 both have the same generation than source, force
     *   an increase in the one with greatest radius.
     *
     * PRECONDITION: Only use when source and target of input_edge exists in
     * m_vertex_to_generation_map. This can be ensured calling this only at the
     * end of tree_edge.
     *
     * @param input_edge
     * @param input_sg
     *
     * @return
     */
    std::optional<vertex_descriptor>
    force_increase_of_generation_in_at_least_one_branch_based_on_radius(
            edge_descriptor input_edge, const SpatialGraph &input_sg) {
        // Interested in studying target of the edge_descriptor
        // Obtain the edges that share the source node with e.
        const auto source = boost::source(input_edge, input_sg);
        const auto target = boost::target(input_edge, input_sg);
        const auto edges_with_same_source_than_input_edge =
                get_edges_with_same_source_than_input_edge(input_edge,
                                                           input_sg);
        // This might happen in the first edge from the root
        if (edges_with_same_source_than_input_edge.empty()) {
            return {};
        }
        bool all_targets_have_same_generation =
                all_out_edge_targets_have_same_generation_than_input_target(
                        target, edges_with_same_source_than_input_edge,
                        input_sg);
        if (!all_targets_have_same_generation) {
            return {};
        }

        // Force that at least one of the edges see its generation increased
        // Which one to increase?
        // - Discard the node(s) with the smallest distance_from_root
        // - From the others, increase the one with smaller radius.
        std::vector<edge_descriptor> out_edges =
                edges_with_same_source_than_input_edge;
        out_edges.push_back(input_edge);
        std::vector<vertex_descriptor> out_targets;
        std::vector<size_t> distances_from_root;
        std::vector<double> radiuses;
        for (auto &edge : out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            out_targets.push_back(edge_target);
            distances_from_root.push_back(
                    m_vertex_to_distance_from_root_map.at(edge_target));
            radiuses.push_back(m_vertex_to_local_radius_map.at(edge_target));
        }

        const auto indices_with_smallest_distance_from_root =
                get_indices_with_smallest_distance_from_root(
                        distances_from_root);

        // Remplace the radiuses of the discarded indices by huge numbers.
        // We are interested in the smaller radiuses only.
        std::vector<double> filtered_radiuses;
        for (size_t index = 0; index < out_targets.size(); ++index) {
            auto find_index_to_discard = std::find(
                    indices_with_smallest_distance_from_root.begin(),
                    indices_with_smallest_distance_from_root.end(), index);
            if (find_index_to_discard ==
                indices_with_smallest_distance_from_root.end()) {
                filtered_radiuses.push_back(radiuses[index]);
            } else {
                filtered_radiuses.push_back(std::numeric_limits<double>::max());
            }
        }

        auto it_min_radius = std::min_element(filtered_radiuses.begin(),
                                              filtered_radiuses.end());

        vertex_descriptor target_to_increase = out_targets[std::distance(
                filtered_radiuses.begin(), it_min_radius)];
        // dev: We know that m_vertex_to_generation_map[v] exists.
        m_vertex_to_generation_map[target_to_increase]++;
        return target_to_increase;
    };

    double angle_between_edges_with_same_source(
            const edge_descriptor &edge_coming_from_root,
            const edge_descriptor &edge_other,
            const SpatialGraph &input_sg,
            const size_t &num_of_edge_points_to_compute_angle) {
        if (boost::source(edge_coming_from_root, input_sg) !=
            boost::source(edge_other, input_sg)) {
            throw std::runtime_error("angle_between_edges_with_same_source: "
                                     "edges don't have the same source.");
        }
        // Get the source (both edges should share it if we have used
        // boost::out_edges in source)
        const auto source = boost::source(edge_coming_from_root, input_sg);
        const auto source_pos = input_sg[source].pos;

        const auto target_coming_from_root =
                boost::target(edge_coming_from_root, input_sg);
        const auto target_other = boost::target(edge_other, input_sg);

        // Initialize the angle point to the node positions of targets.
        SG::PointType angle_point_of_edge_coming_from_root =
                input_sg[target_coming_from_root].pos;
        SG::PointType angle_point_of_edge_target = input_sg[target_other].pos;
        // coming_from_root: Get angle_point from an edge point (if possible)
        {
            const auto spatial_edge_coming_from_root =
                    input_sg[edge_coming_from_root];
            const size_t num_edge_points_edge_coming_from_root =
                    spatial_edge_coming_from_root.edge_points.size();
            if (num_edge_points_edge_coming_from_root >=
                num_of_edge_points_to_compute_angle) {
                const auto &first_edge_point =
                        spatial_edge_coming_from_root.edge_points[0];
                const auto dist_to_source =
                        ArrayUtilities::distance(source_pos, first_edge_point);
                const auto dist_to_target = ArrayUtilities::distance(
                        angle_point_of_edge_coming_from_root, first_edge_point);
                if (dist_to_source < dist_to_target) {
                    const auto index = num_of_edge_points_to_compute_angle - 1;
                    angle_point_of_edge_coming_from_root =
                            spatial_edge_coming_from_root.edge_points[index];
                } else {
                    const auto index = num_edge_points_edge_coming_from_root -
                                       num_of_edge_points_to_compute_angle;
                    angle_point_of_edge_coming_from_root =
                            spatial_edge_coming_from_root.edge_points[index];
                }
            }
        }
        // other: Get angle_point from an edge point (if possible)
        {
            const auto spatial_edge_target = input_sg[edge_other];
            const size_t num_edge_points_edge_target =
                    spatial_edge_target.edge_points.size();
            if (num_edge_points_edge_target >=
                num_of_edge_points_to_compute_angle) {
                const auto &first_edge_point =
                        spatial_edge_target.edge_points[0];
                const auto dist_to_source =
                        ArrayUtilities::distance(source_pos, first_edge_point);
                const auto dist_to_target = ArrayUtilities::distance(
                        angle_point_of_edge_target, first_edge_point);
                if (dist_to_source < dist_to_target) {
                    const auto index = num_of_edge_points_to_compute_angle - 1;
                    angle_point_of_edge_target =
                            spatial_edge_target.edge_points[index];
                } else {
                    const auto index = num_edge_points_edge_target -
                                       num_of_edge_points_to_compute_angle;
                    angle_point_of_edge_target =
                            spatial_edge_target.edge_points[index];
                }
            }
        }
        const auto angle_between_edge_and_root = ArrayUtilities::angle(
                ArrayUtilities::minus(angle_point_of_edge_target, source_pos),
                ArrayUtilities::minus(source_pos,
                                      angle_point_of_edge_coming_from_root));
        const auto angle_in_degrees =
                angle_between_edge_and_root * 180 / RNG::pi;
        return angle_in_degrees;
    }

    /**
     * After the generation has been set in the target of input edge, use this
     * to force an increase of generation in the case the generation hasn't
     * been increased in any edge with the same source than input edge
     *
     *             t1
     *             o
     * source     /
     *     o-----o
     *            \
     *             o
     *             t2
     *
     *   if t1 and t2 both have the same generation than source, force
     *   an increase in the one with greatest angle.
     *
     * PRECONDITION: Only use when source and target of input_edge exists in
     * m_vertex_to_generation_map. This can be ensured calling this only at the
     * end of tree_edge.
     *
     * @param input_edge
     * @param input_sg
     *
     * @return
     */
    std::optional<std::vector<vertex_descriptor>>
    force_increase_of_generation_in_at_least_one_branch_based_on_angle(
            edge_descriptor input_edge, const SpatialGraph &input_sg) {
        // Interested in studying target of the edge_descriptor
        // Obtain the edges that share the source node with e.
        const auto target = boost::target(input_edge, input_sg);
        const auto edges_with_same_source_than_input_edge =
                get_edges_with_same_source_than_input_edge(input_edge,
                                                           input_sg);
        // This might happen in the first edge from the root
        if (edges_with_same_source_than_input_edge.empty()) {
            return {};
        }
        bool all_targets_have_generation_populated =
                all_out_edge_targets_have_generation_populated(
                        target, edges_with_same_source_than_input_edge,
                        input_sg);
        if (!all_targets_have_generation_populated) {
            return {};
        }

        // Force that at least one of the edges see its generation increased
        // Which one to increase?
        // - Discard the node(s) with the smallest distance_from_root
        // - From the others, increase all of them if their angles from the
        //   root edge are big
        std::vector<edge_descriptor> out_edges =
                edges_with_same_source_than_input_edge;
        out_edges.push_back(input_edge);
        std::vector<vertex_descriptor> out_targets;
        std::vector<size_t> distances_from_root;
        for (auto &edge : out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            out_targets.push_back(edge_target);
            distances_from_root.push_back(
                    m_vertex_to_distance_from_root_map.at(edge_target));
        }
        // Discard angle analysis if there are multiple nodes with the
        // minimum distance from root.  Aka, a diamond structure.
        const auto indices_with_smallest_distance_from_root =
                get_indices_with_smallest_distance_from_root(
                        distances_from_root);
        if (std::size(indices_with_smallest_distance_from_root) != 1) {
            return {};
        }
        const auto &index_closer_to_root =
                indices_with_smallest_distance_from_root[0];
        // Compute the angle between the edge with the target closer to the
        // root, and the other edges.
        const auto &edge_coming_from_root = out_edges[index_closer_to_root];
        // Discard angle analysis is the input_edge is the one coming from root
        if (edge_coming_from_root == input_edge) {
            return {};
        }
        // sibling edges (one is input edge), i.e all out_edges but the one
        // coming from root.
        std::vector<edge_descriptor> sibling_edges;
        for (auto &edge : out_edges) {
            if (edge != edge_coming_from_root) {
                sibling_edges.push_back(edge);
            }
        }

        // bool all_sibling_targets_have_same_generation =
        //         all_out_edge_targets_have_same_generation_than_input_target(
        //                 target, sibling_edges, input_sg);
        // if (!all_sibling_targets_have_same_generation) {
        //     return {};
        // }

        auto sibling_edges_with_multiple_lowest_generation =
                get_out_edges_with_lowest_same_generation(sibling_edges,
                                                          input_sg);
        if (sibling_edges_with_multiple_lowest_generation.size() < 2) {
            return {};
        }

        std::vector<double> angles;
        std::vector<double> out_targets_siblings_with_lowest_same_genration;
        for (auto &edge : sibling_edges_with_multiple_lowest_generation) {
            const auto edge_target = boost::target(edge, input_sg);
            // if (edge_target == target_of_edge_coming_from_root) {
            //     angles.push_back(std::numeric_limits<double>::lowest());
            //     continue;
            // }
            const auto angle_in_degrees = angle_between_edges_with_same_source(
                    edge_coming_from_root, edge, input_sg,
                    m_num_of_edge_points_to_compute_angle);
            angles.push_back(angle_in_degrees);
            out_targets_siblings_with_lowest_same_genration.push_back(
                    edge_target);
        }

        std::vector<vertex_descriptor> targets_increased;
        for (size_t i = 0; i < angles.size(); ++i) {
            const auto &angle = angles[i];
            if (angle > m_increase_generation_if_angle_greater_than) {
                const vertex_descriptor &target_to_maybe_increase =
                        out_targets_siblings_with_lowest_same_genration[i];
                if (!m_vertex_already_increased.count(
                            target_to_maybe_increase)) {
                    targets_increased.push_back(target_to_maybe_increase);
                    m_vertex_already_increased.emplace(target_to_maybe_increase,
                                                       true);
                    // dev: We know that m_vertex_to_generation_map[v] exists.
                    m_vertex_to_generation_map[target_to_maybe_increase]++;
                }
            }
        }
        return targets_increased;
    }

    std::optional<double>
    angle_between_input_and_root_edges(edge_descriptor input_edge,
                                       const SpatialGraph &input_sg) {
        const auto edges_with_same_source_than_input_edge =
                get_edges_with_same_source_than_input_edge(input_edge,
                                                           input_sg);
        // Get node (and its edge) closer to the root node
        std::vector<edge_descriptor> out_edges =
                edges_with_same_source_than_input_edge;
        out_edges.push_back(input_edge);
        std::vector<vertex_descriptor> out_targets;
        std::vector<size_t> distances_from_root;
        for (auto &edge : out_edges) {
            const auto &edge_target = boost::target(edge, input_sg);
            out_targets.push_back(edge_target);
            // Only study targets that are already visited on tree_edge
            auto find_edge_target_root_distance =
                    m_vertex_to_distance_from_root_map.find(edge_target);
            if (find_edge_target_root_distance ==
                std::end(m_vertex_to_distance_from_root_map)) {
                distances_from_root.push_back(
                        std::numeric_limits<size_t>::max());
            } else {
                distances_from_root.push_back(
                        find_edge_target_root_distance->second);
            }
        }
        // Discard angle analysis if there are multiple nodes with the
        // minimum distance from root.  Aka, a diamond structure.
        const auto indices_with_smallest_distance_from_root =
                get_indices_with_smallest_distance_from_root(
                        distances_from_root);
        if (std::size(indices_with_smallest_distance_from_root) != 1) {
            return {};
        }
        const auto &index_closer_to_root =
                indices_with_smallest_distance_from_root[0];
        // Compute the angle between the edge with the target closer to the
        // root, and the other edges.
        const auto &edge_coming_from_root = out_edges[index_closer_to_root];
        // Discard angle analysis is the input_edge is the one coming from root
        if (edge_coming_from_root == input_edge) {
            return {};
        }
        const auto angle_in_degrees = angle_between_edges_with_same_source(
                edge_coming_from_root, input_edge, input_sg,
                m_num_of_edge_points_to_compute_angle);
        return angle_in_degrees;
    }

    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        const auto source = boost::source(e, input_sg);
        const auto target = boost::target(e, input_sg);

        if (m_verbose) {
            std::cout << "tree_edge: " << e << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;
        }

        if (!m_vertex_to_distance_from_root_map.count(target)) {
            const size_t source_distance =
                    m_vertex_to_distance_from_root_map.at(source);
            const size_t target_distance = source_distance + 1;
            m_vertex_to_distance_from_root_map.emplace(target, target_distance);
        }

        // This would only happen when the user has provided an
        // input_fixed_generation_map
        if (m_vertex_to_generation_map.count(target)) {
            // Mark the node as already increased to avoid modifying it in the
            // angle analysis at the end.
            m_vertex_already_increased.emplace(target, true);
            if (m_verbose) {
                std::cout << "target generation already populated with value: "
                          << m_vertex_to_generation_map.at(target) << std::endl;
            }
            return;
        }
        // Get the radius value of source and target
        const auto &source_radius = m_vertex_to_local_radius_map.at(source);
        const auto &target_radius = m_vertex_to_local_radius_map.at(target);
        const double radius_ratio = target_radius / source_radius;
        const double decrease_ratio = 1.0 - radius_ratio;
        // We consider an anomaly if target has a bigger radius
        // if (radius_ratio > 1.) {
        //     m_vertex_anomalies[target] = radius_ratio;
        // }
        // We consider an anomaly if target has similar radius than source
        // and it is a short branch.
        if (decrease_ratio <= m_decrease_radius_ratio_to_increase_generation &&
            input_sg[e].edge_points.size() < 20 &&
            boost::degree(target, input_sg) == 1) {
            m_vertex_anomalies[target] = 1.;
        }

        /* *****************************************************************/
        // Radius ratio decrease between nodes

        const bool increase_generation_because_radius_of_nodes =
                increase_generation_based_on_radius_of_nodes(
                        decrease_ratio,
                        m_decrease_radius_ratio_to_increase_generation);

        /* *****************************************************************/
        // Radius decrease along edge (help to diffentiate main vessel)
        const size_t minimum_size_edge_points = 5;
        const double differences_ratio_with_midpoint = 2.0;
        const boost::tribool increase_generation_because_radius_of_edge =
                increase_generation_based_on_radius_of_edge(
                        e, input_sg, minimum_size_edge_points,
                        differences_ratio_with_midpoint);

        /* *****************************************************************/
        // Radius ratio decrease when target is an end node
        // Disable: end point is going to be increased by angle most likely
        const bool target_is_end_point =
                boost::degree(target, input_sg) == 1 ? true : false;
        // If the target is an end point, be more willing to increase the
        // generation even if there is less reduction of radius. The false
        // positives because this will only come from noisy graphs.
        const double decrease_ratio_factor_end_point = 0.0;
        const bool increase_generation_because_end_point_and_radius =
                target_is_end_point
                        ? increase_generation_based_on_radius_of_nodes(
                                  decrease_ratio,
                                  m_decrease_radius_ratio_to_increase_generation *
                                          decrease_ratio_factor_end_point)
                        : false;

        // Study angle
        const auto angle_between_input_edge_and_root_edge =
                angle_between_input_and_root_edges(e, input_sg);
        bool small_angle_between_input_edge_and_root_edge = false;
        if (angle_between_input_edge_and_root_edge.has_value()) {
            small_angle_between_input_edge_and_root_edge =
                    (std::abs(*angle_between_input_edge_and_root_edge) <
                     m_keep_generation_if_angle_less_than)
                            ? true
                            : false;
        }

        /* *****************************************************************/
        // Increase the generation if any previous condition is true

        // Force to keep the generation if the radius analysis on edge is false.
        // And the angle is small.
        bool do_not_increase = false;
        if (!increase_generation_because_radius_of_edge) {
            if (small_angle_between_input_edge_and_root_edge) {
                do_not_increase = true;
            }
        }

        const bool keep_same_generation =
                do_not_increase
                        ? true
                        : !(increase_generation_because_radius_of_nodes ||
                            increase_generation_because_end_point_and_radius);

        // source generation should be always initialized (we start the
        // visit at root)
        const size_t source_generation = m_vertex_to_generation_map.at(source);
        const size_t target_generation = keep_same_generation
                                                 ? source_generation
                                                 : source_generation + 1;

        // We know for sure that target hasn't already been emplaced.
        auto emplaced_pair =
                m_vertex_to_generation_map.emplace(target, target_generation);

        if (!keep_same_generation) {
            // Mark it to avoid future analysis to increase it further.
            m_vertex_already_increased.emplace(target, true);
        }

        // Decrease generation (after radius analysis) if angle is small
        if (small_angle_between_input_edge_and_root_edge &&
            !keep_same_generation) {
            auto emplaced_it = emplaced_pair.first;
            emplaced_it->second--;
        }

        // Apply correction for the case a bifurcation does not increase
        // generation in any branch.
        // const auto vertex_increased =
        //         force_increase_of_generation_in_at_least_one_branch_based_on_radius(
        //                 e, input_sg);
        const auto vertices_increased_based_on_angle =
                force_increase_of_generation_in_at_least_one_branch_based_on_angle(
                        e, input_sg);

        if (m_verbose) {
            std::ios_base::fmtflags cout_flags(std::cout.flags());
            std::cout << "  source_generation: " << source_generation
                      << ", target_generation: " << target_generation
                      << std::endl;
            if (m_vertex_anomalies.count(target)) {
                std::cout << "anomaly: target has a bigger radius than source, "
                             "ratio: "
                          << m_vertex_anomalies.at(target) << std::endl;
            }
            std::cout << "  increase_generation_because_radius_of_edge: "
                      << std::boolalpha
                      << increase_generation_because_radius_of_edge
                      << std::endl;
            std::cout << "  increase_generation_because_radius_of_nodes: "
                      << increase_generation_because_radius_of_nodes
                      << std::endl;
            std::cout << "  increase_generation_because_end_point_and_radius: "
                      << increase_generation_because_end_point_and_radius
                      << std::endl;
            if (small_angle_between_input_edge_and_root_edge) {
                std::cout << " - generation reduced because small angle: "
                          << *angle_between_input_edge_and_root_edge
                          << std::endl;
            }
            if (vertices_increased_based_on_angle.has_value()) {
                std::cout
                        << "  - generation increased at the end for targets: ";
                for (const auto &vertex_increased :
                     *vertices_increased_based_on_angle) {
                    std::cout << vertex_increased << ", ";
                }
                if ((*vertices_increased_based_on_angle).empty()) {
                    std::cout << "EMPTY (angles too small or already increased)"
                              << std::endl;
                }
                std::cout << std::endl;
            }
            std::cout << "-----------" << std::endl;
            std::cout.flags(cout_flags); // restore after boolalpha modification
        }
    }
};

} // end namespace SG
#endif
