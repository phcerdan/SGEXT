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
        const GraphType &g, const ArrayUtilities::boundary_condition &bc) {

    std::vector<double> unordered_distances;
    auto edges = boost::edges(g);
    for (auto ei = edges.first, e_end = edges.second; ei != e_end; ++ei) {
        auto source = boost::source(*ei, g);
        auto target = boost::target(*ei, g);
        double dist;
        if (bc == ArrayUtilities::boundary_condition::NONE) {
            dist = ArrayUtilities::distance(g[target].pos, g[source].pos);
        } else if (bc == ArrayUtilities::boundary_condition::PERIODIC) {
            dist = ArrayUtilities::distance_with_boundary_condition_periodic(
                    g[target].pos, g[source].pos);
        }
        unordered_distances.push_back(dist);
    }
    return unordered_distances;
}
} // namespace SG
