/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "generate_common.hpp"
#include "rng.hpp"
#include <tuple> // for std::tie

namespace SG {

GraphType::vertex_descriptor select_random_node(const GraphType &graph) {

    const auto num_vertices = boost::num_vertices(graph);
    return RNG::rand_range_int(0, num_vertices - 1);
}
GraphType::edge_descriptor select_random_edge(const GraphType &graph) {

    const auto num_edges = boost::num_edges(graph);
    const auto rand_edge_num = RNG::rand_range_int(0, num_edges - 1);
    using edge_iterator = GraphType::edge_iterator;

    edge_iterator eit, eit_end; // eit_rand1, eit_rand2;
    std::tie(eit, eit_end) = boost::edges(graph);
    for (int count = 0; eit != eit_end; ++eit, ++count) {
        if (count == rand_edge_num) {
            // eit iterator points to the random edge.
            break;
        }
    }
    // return the edge descriptor of the edge
    return *eit;
}

PointType generate_random_array(const double &max_modulus) {

    const double rand_modulus = RNG::rand_range(0, max_modulus);
    return RNG::random_orientation(rand_modulus);
}

std::vector<double> cosine_directors_from_connected_edges(
        const std::vector<VectorType> &outgoing_edges) {
    std::vector<double> cosine_directors;
    for (auto first = outgoing_edges.begin(); first != outgoing_edges.end();
         ++first) {
        for (auto second = first + 1; second != outgoing_edges.end();
             ++second) {
            // Cosines
            cosine_directors.push_back(
                    ArrayUtilities::cos_director(*first, *second));
        }
    }
    return cosine_directors;
};

std::vector<double> cosine_directors_between_edges_and_target_edge(
        const std::vector<VectorType> &outgoing_edges,
        const VectorType &outgoing_target_edge) {
    std::vector<double> cosine_directors;
    for (auto out_edge = outgoing_edges.begin();
         out_edge != outgoing_edges.end(); ++out_edge) {
        cosine_directors.push_back(
                ArrayUtilities::cos_director(*out_edge, outgoing_target_edge));
    }
    return cosine_directors;
}

std::vector<double> get_all_end_to_end_distances_of_edges(
        const GraphType &graph, const ArrayUtilities::boundary_condition &bc) {

    std::vector<double> unordered_distances;
    auto edges = boost::edges(graph);
    for (auto ei = edges.first, e_end = edges.second; ei != e_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        double dist;
        if (bc == ArrayUtilities::boundary_condition::NONE) {
            dist = ArrayUtilities::distance(graph[target].pos,
                                            graph[source].pos);
        } else if (bc == ArrayUtilities::boundary_condition::PERIODIC) {
            dist = ArrayUtilities::distance_with_boundary_condition_periodic(
                    graph[target].pos, graph[source].pos);
        }
        unordered_distances.push_back(dist);
    }
    return unordered_distances;
}

std::vector<double> get_all_cosine_directors_between_connected_edges(
        const GraphType &graph, const ArrayUtilities::boundary_condition &bc) {

    std::vector<double> unordered_cosines;
    auto [vi, vi_end] = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        std::vector<VectorType> outgoing_edges;
        auto [ei, ei_end] = boost::out_edges(*vi, graph);
        for (; ei != ei_end; ++ei) {
            const auto source_pos = graph[source(*ei, graph)].pos;
            const auto target_pos = graph[target(*ei, graph)].pos;
            auto target_image_pos = target_pos;
            if (bc == ArrayUtilities::boundary_condition::PERIODIC) {
                target_image_pos = ArrayUtilities::closest_image_from_reference(
                        source_pos, target_image_pos);
            }
            outgoing_edges.push_back(
                    ArrayUtilities::minus(target_image_pos, source_pos));
        }
        const auto cosine_directors_from_vertex =
                cosine_directors_from_connected_edges(outgoing_edges);
        unordered_cosines.insert(std::end(unordered_cosines),
                                 std::begin(cosine_directors_from_vertex),
                                 std::end(cosine_directors_from_vertex));
    }
    return unordered_cosines;
}
} // namespace SG
