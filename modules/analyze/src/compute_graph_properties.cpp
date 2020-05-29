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

#include "compute_graph_properties.hpp"
#include "edge_points_utilities.hpp"

namespace SG {

std::vector<unsigned int> compute_degrees(const SG::GraphType &sg) {
    std::vector<unsigned int> degrees;
    const auto verts = boost::vertices(sg);
    for (auto vi = verts.first; vi != verts.second; ++vi) {
        degrees.push_back(boost::degree(*vi, sg));
    }
    return degrees;
}

std::vector<double> compute_ete_distances(const SG::GraphType &sg,
                                          const size_t minimum_size_edges,
                                          bool ignore_end_nodes) {
    std::vector<double> ete_distances;
    const auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto &eps = sg[*ei].edge_points;
        if (eps.size() < minimum_size_edges)
            continue;
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        if (ignore_end_nodes &&
            (boost::degree(source, sg) == 1 || boost::degree(target, sg) == 1))
            continue;

        ete_distances.emplace_back(SG::ete_distance(*ei, sg));
    }
    return ete_distances;
}

std::vector<double> compute_contour_lengths(const SG::GraphType &sg,
                                            const size_t minimum_size_edges,
                                            bool ignore_end_nodes) {
    std::vector<double> contour_lengths;
    const auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto &eps = sg[*ei].edge_points;
        if (eps.size() < minimum_size_edges)
            continue;
        if (ignore_end_nodes &&
            (boost::degree(boost::source(*ei, sg), sg) == 1 ||
             boost::degree(boost::target(*ei, sg), sg) == 1))
            continue;
        contour_lengths.emplace_back(SG::contour_length(*ei, sg));
    }
    return contour_lengths;
}

std::vector<double> compute_angles(const SG::GraphType &sg,
                                   const size_t minimum_size_edges,
                                   const bool ignore_parallel_edges,
                                   const bool ignore_end_nodes) {
    std::vector<double> ete_angles;
    const auto verts = boost::vertices(sg);
    // From
    // http://www.boost.org/doc/libs/1_66_0/libs/graph/doc/IncidenceGraph.html
    // It is guaranteed that given: e=out_edge(v); then source(e) == v.
    for (auto vi = verts.first; vi != verts.second; ++vi) {
        // Don't analyze degree 2 nodes (they are only left to mark self-loops.
        // Degree 0 and 1 won't be computed even without this guard.
        // auto degree =  boost::out_degree(*vi,sg);
        // if (degree < 3)
        //     continue;
        const auto out_edges = boost::out_edges(*vi, sg);
        for (auto ei1 = out_edges.first; ei1 != out_edges.second; ++ei1) {
            const auto &eps1 = sg[*ei1].edge_points;
            if (eps1.size() < minimum_size_edges)
                continue;
            auto source = boost::source(*ei1, sg); // = *vi
            auto target1 = boost::target(*ei1, sg);
            if (ignore_end_nodes && (boost::degree(source, sg) == 1 ||
                                     boost::degree(target1, sg) == 1))
                continue;
            // Copy edge iterator and plus one (to avoid compare the edge with
            // itself)
            auto ei2 = ei1;
            ei2++;
            for (; ei2 != out_edges.second; ++ei2) {
                const auto &eps2 = sg[*ei2].edge_points;
                if (eps2.size() < minimum_size_edges)
                    continue;
                auto target2 = boost::target(*ei2, sg);
                if (ignore_end_nodes && boost::degree(target2, sg) == 1)
                    continue;
                // Don't compute angle on parallel edges
                // WARNING: do not check target2 == source
                // source(ei2) is guaranteed (by out_edges) to be equal to
                // source(ei1)
                if (ignore_parallel_edges && target2 == target1)
                    continue;

                ete_angles.emplace_back(ArrayUtilities::angle(
                        ArrayUtilities::minus(sg[target1].pos, sg[source].pos),
                        ArrayUtilities::minus(sg[target2].pos,
                                              sg[source].pos)));
            }
        }
    }
    return ete_angles;
}

std::vector<double> compute_cosines(const std::vector<double> &angles) {
    std::vector<double> cosines(angles.size());
    // cosines.resize(angles.size())
    std::transform(angles.cbegin(), angles.cend(), cosines.begin(),
                   [](const double &a) { return std::cos(a); });
    return cosines;
}
} // namespace SG
