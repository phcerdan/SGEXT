/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "edge_points_utilities.hpp"
#include "spatial_graph_utilities.hpp" // for print
#include <algorithm>
#include <sstream>
#include <vector>

namespace SG {

double ete_distance(const GraphType::edge_descriptor &edge_desc,
                    const GraphType &sg) {
    const auto source = boost::source(edge_desc, sg);
    const auto target = boost::target(edge_desc, sg);
    const auto &source_pos = sg[source].pos;
    const auto &target_pos = sg[target].pos;
    return ArrayUtilities::distance(target_pos, source_pos);
}

double edge_points_length(const SpatialEdge &se) {
    const auto &eps = se.edge_points;
    size_t npoints = eps.size();
    // if empty or only one point, return null distance
    if (npoints < 2)
        return 0.0;
    double length = 0.0;
    for (size_t i = 1; i != npoints; i++) {
        length += ArrayUtilities::distance(eps[i], eps[i - 1]);
    }
    return length;
}

double contour_length(const GraphType::edge_descriptor &edge_desc,
                      const GraphType &sg) {
    const auto &se = sg[edge_desc];
    const auto &eps = se.edge_points;
    auto source = boost::source(edge_desc, sg);
    auto target = boost::target(edge_desc, sg);
    if (eps.empty())
        return ArrayUtilities::distance(sg[target].pos, sg[source].pos);
    // Because the graph is unordered, source and target are not guaranteed
    // to be the closer to eps[0] or eps.back() respectively, so we compute
    // distance between source to eps[0] to be sure. If they are not connected,
    // we know that source is connected to eps.back().
    auto dist_source_first = ArrayUtilities::distance(sg[source].pos, eps[0]);
    // Dev: It can happen that eps[0] is connected to both, source and target.
    // But eps.back() is only connected to one end of the spatial edge, see test
    // corner case
    auto dist_target_last =
            ArrayUtilities::distance(sg[target].pos, eps.back());

    auto dist_source_last =
            ArrayUtilities::distance(sg[source].pos, eps.back());
    auto dist_target_first = ArrayUtilities::distance(sg[target].pos, eps[0]);

    double dist_to_source = 0.0;
    double dist_to_target = 0.0;
    // This cannot assume that spacing is unity as it does.
    // static auto max_connected_distance = sqrt(3.0) + 2.0 *
    // std::numeric_limits<double>::epsilon();

    if (dist_source_first < dist_source_last &&
        dist_target_last < dist_target_first) {
        dist_to_source = dist_source_first;
        dist_to_target = dist_target_last;
    } else {
        dist_to_source = dist_source_last;
        dist_to_target = dist_target_first;
    }

    // sanity check: this won't throw if edge_points are ordered.
    // but it creates a false positive error when spacing between pos isn't 1.0
    // {
    //     if(dist_to_source > max_connected_distance || dist_to_target >
    //     max_connected_distance ) {
    //         std::stringstream ss;
    //         ss << "Contour_length failure, node positions are not connected
    //         to the edge_points. Or the edge_points are disordered.\n"; ss <<
    //         "Source:    " << sg[source] << " , Target: " << sg[target] <<
    //         "\n"; ss << "Distances: " << dist_to_source << ", " <<
    //         dist_to_target <<
    //         "\n"; ss << "SpatialEdge:\n " << se; throw std::runtime_error(
    //         ss.str() );
    //     }
    // }

    return dist_to_source + edge_points_length(se) + dist_to_target;
}

bool check_edge_points_are_contiguous(
        SpatialEdge::PointContainer &edge_points) {
    bool all_points_contigous = true;
    if (edge_points.empty()) {
        return all_points_contigous;
    }
    const auto num_points = edge_points.size();
    const auto num_distances = num_points - 1;
    std::vector<double> distances_between_contigous_points(num_distances);
    for (size_t i = 0; i < num_distances; ++i) {
        distances_between_contigous_points[i] =
                ArrayUtilities::distance(edge_points[i], edge_points[i + 1]);
    }

    // Assuming spacing = [1.0, 1.0, 1.0]
    const double expected_min_distance = 1.0;
    const double expected_max_distance = sqrt(3.0);
    // + 2.0 * std::numeric_limits<double>::epsilon();
#if !defined(NDEBUG)
    const auto comma_separated_points = true;
#endif
    for (size_t i = 0; i < num_distances; ++i) {
        const auto &dist = distances_between_contigous_points[i];
        if (dist - expected_max_distance >
            2.0 * std::numeric_limits<double>::epsilon()) {
            all_points_contigous = false;
#if !defined(NDEBUG)
            const auto pA = ArrayUtilities::to_string(edge_points[i],
                                                      comma_separated_points);
            const auto pB = ArrayUtilities::to_string(edge_points[i + 1],
                                                      comma_separated_points);
            std::cerr << "FAIL. Points at index " << i << ": ||{" + pA
                      << "} - {" + pB + "}|| = " << dist
                      << ". But the difference should be less than "
                      << expected_max_distance << std::endl;
#endif
        } else if (dist - expected_min_distance < 0) {
            all_points_contigous = false;
#if !defined(NDEBUG)
            const auto pA = ArrayUtilities::to_string(edge_points[i],
                                                      comma_separated_points);
            const auto pB = ArrayUtilities::to_string(edge_points[i + 1],
                                                      comma_separated_points);
            std::cerr << "FAIL. Points at index " << i << ": ||{" + pA
                      << "} - {" + pB + "}|| = " << dist
                      << ". But the difference should be greater than "
                      << expected_min_distance << std::endl;
#endif
        }
    }
    return all_points_contigous;
}

void insert_unique_edge_point_with_distance_order(
        SpatialEdge::PointContainer &edge_points,
        const SpatialEdge::PointType &new_point) {
    // std::cout << "-- point to add: "; print_pos(std::cout, new_point);
    // std::cout << std::endl;
    const auto edge_points_size = edge_points.size();
    if (edge_points_size == 0) {
        edge_points.push_back(new_point);
        // std::cout << "--- After insertion: "; print_edge_points(edge_points,
        // std::cout); std::cout << std::endl;
        return;
    }
    // Insert it between closer points.
    // Compute distance between in-point and all the points.
    SpatialEdge::PointContainer start_and_end_edge_points;
    start_and_end_edge_points.push_back(edge_points[0]);
    if(edge_points_size > 1) {
        start_and_end_edge_points.push_back(edge_points.back());
    }
    std::vector<double> distances_to_in_point(start_and_end_edge_points.size());
    std::transform(std::begin(start_and_end_edge_points), std::end(start_and_end_edge_points),
                   std::begin(distances_to_in_point),
                   [&new_point](const SpatialEdge::PointType &edge_point) {
                       return ArrayUtilities::distance(edge_point, new_point);
                   });
    // Note: edge_points are contiguous (from DFS)
    // the new pos, should be at the beggining or at the end.
    // If at the beginning, we put the first, if at the end, we put it last.
    // Ordering the edge_points if they get disordered is not trivial at all.
    // Check "spatial data" structures elsewhere.
    auto min_it = std::min_element(std::begin(distances_to_in_point),
                                   std::end(distances_to_in_point));
    // Check they are connected for sanity.
    // Note:
    // This check is only valid if the spacing is 1.0 (object/indices space)
    // {
    //   auto min_dist = *min_it;
    //   if(min_dist > sqrt(3.0) + 2.0 * std::numeric_limits<double>::epsilon())
    //     throw std::runtime_error(
    //         "The impossible, new_point in "
    //         "insert_unique_edge_point_with_distance_order is not connected to
    //         " "the edge_points");
    // }
    // Check that you are not inserting a duplicate
    {
        auto min_dist = *min_it;
        if (std::abs(min_dist) < 2.0 * std::numeric_limits<double>::epsilon()) {
#if !defined(NDEBUG)
            std::cout << "Note: insert_unique_edge_point_with_distance_order "
                         "tried "
                         "to insert a duplicated point, but it was rejected."
                      << std::endl;
            std::cout << ArrayUtilities::to_string(new_point) << std::endl;
#endif
            return;
        }
    }
    auto min_index = std::distance(std::begin(distances_to_in_point), min_it);
    if (min_index == 0) {
        edge_points.insert(std::begin(edge_points), new_point);
        // std::cout << "--- After insertion: "; print_edge_points(edge_points,
        // std::cout); std::cout << std::endl;
    } else if (static_cast<unsigned int>(min_index) ==
               distances_to_in_point.size() -
                       1) { // This is safe, as vector is not empty.
        // edge_points.insert(std::end(edge_points), new_point);
        edge_points.push_back(new_point);
        // std::cout << "--- After insertion: "; print_edge_points(edge_points,
        // std::cout); std::cout << std::endl;
    } else { // impossible error
        std::cerr << "Current edge_points: ";
        print_edge_points(edge_points, std::cerr);
        std::cerr << std::endl;
        std::cerr << "Failed when inserting new_point: ";
        print_pos(std::cerr, new_point);
        std::cerr << std::endl;
        throw std::runtime_error("The impossible, new point in "
                                 "insert_unique_edge_point_with_distance "
                                 "is not at the beggining or end position in "
                                 "edge_points. min_index: " +
                                 std::to_string(min_index));
    }
}

} // namespace SG
