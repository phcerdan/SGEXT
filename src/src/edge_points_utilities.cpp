/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * See LICENSE on top of this repository. */

#include "edge_points_utilities.hpp"
#include <vector>
#include <algorithm>

namespace SG {


double edge_points_length(const SG::SpatialEdge &se)
{
    const auto & eps = se.edge_points;
    size_t npoints = eps.size();
    // if empty or only one point, return null distance
    if(npoints < 2)
        return 0.0;
    double length = 0.0;
    for(size_t i = 1; i != npoints; i++){
        length += ArrayUtilities::distance(eps[i], eps[i-1]);
    }
    return length;
}

double contour_length(const SG::GraphType::edge_descriptor e,
    const SG::GraphType & sg)
{
    const auto & se = sg[e];
    const auto & eps = se.edge_points;
    auto source = boost::source(e, sg);
    auto target = boost::target(e, sg);
    if(eps.empty())
        return ArrayUtilities::distance(sg[target].pos, sg[source].pos);
    // Because the graph is unordered, source and target are not guaranteed
    // to be the closer to eps[0] or eps.back() respectively, so we compute
    // distance between source to eps[0] to be sure. If they are not connected,
    // we know that source is connected to eps.back().
    auto dist_source_first = ArrayUtilities::distance(sg[source].pos, eps[0]);
    // We can avoid calculate the next one and just compare with sqrt(3.0).
    // If eps has only one point, we calculate the same distance, but it is correct.
    auto dist_source_last = ArrayUtilities::distance(sg[source].pos, eps.back());

    double dist_to_source = 0.0;
    double dist_to_target = 0.0;

    if(dist_source_first < dist_source_last){
        dist_to_source = dist_source_first;
        dist_to_target = ArrayUtilities::distance(sg[target].pos, eps.back());
    } else {
        dist_to_source = dist_source_last;
        dist_to_target = ArrayUtilities::distance(sg[target].pos, eps[0]);
    }

    // sanity check: this won't throw if edge_points are ordered.
    {
        auto max_connected_distance = sqrt(3.0) + 2.0 * std::numeric_limits<double>::epsilon();
        if(dist_to_source > max_connected_distance || dist_to_target > max_connected_distance )
            throw std::runtime_error("contour_length failure, node positions are not connected to the edge_points. Or the edge_points are disordered. Distances: " +
                    std::to_string(dist_to_source) + ", " +
                    std::to_string(dist_to_target)
                    );
    }

    return dist_to_source + SG::edge_points_length(se) + dist_to_target;
}

void insert_edge_point_with_distance_order(
        SG::SpatialEdge::PointContainer & edge_points,
        const SG::SpatialEdge::PointType & new_point)
{
    if(edge_points.empty())
        edge_points.push_back(new_point);
    // Insert it between closer points.
    // Compute distance between in-point and all the points.
    std::vector<double> distances_to_in_point(edge_points.size());
    std::transform(
            std::begin(edge_points),
            std::end(edge_points),
            std::begin(distances_to_in_point),
            [&new_point](const SG::SpatialEdge::PointType & edge_point){
            return ArrayUtilities::distance(edge_point, new_point);
            }
            );
    // Note: edge_points are contiguous (from DFS)
    // the new pos, should be at the beggining or at the end.
    // If at the beginning, we put the first, if at the end, we put it last.
    // Ordering the edge_points if they get disordered is not trivial at all.
    // Check "spatial data" structures elsewhere.
    auto min_it = std::min_element(std::begin(distances_to_in_point),
            std::end(distances_to_in_point));
    // Check they are connected for sanity.
    // TODO we might check this only in debug mode.
    {
        auto min_dist = *min_it;
        if(min_dist > sqrt(3.0) + 2.0 * std::numeric_limits<double>::epsilon())
            throw std::runtime_error("The impossible, new_point in insert_edge_point_with_distance_order is not connected to the edge_points");
    }
    auto min_index = std::distance(std::begin(distances_to_in_point), min_it);
    if (min_index == 0)
        edge_points.insert(std::begin(edge_points), new_point);
    else if(static_cast<unsigned int>(min_index) == distances_to_in_point.size() - 1) // This is safe, as vector is not empty.
        // edge_points.insert(std::end(edge_points), new_point);
        edge_points.push_back(new_point);
    else  // illogical error
        throw std::runtime_error("The impossible, node_to_remove closer edge_point is not at the beggining or end position in edge_points.");
}


} // end namespace
