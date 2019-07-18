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

#include "shortest_path.hpp"
#include "edge_points_utilities.hpp"
#include <boost/property_map/function_property_map.hpp>

namespace SG {

SpatialEdge create_edge_from_path(
        const std::vector<GraphType::vertex_descriptor> &vertex_path,
        const GraphType &input_g) {
    SpatialEdge sg_edge;
    auto &sg_edge_points = sg_edge.edge_points;
    // Add node for keeping a sensible order, it will be removed at the end.
    sg_edge_points.push_back(input_g[vertex_path[0]].pos);
    using vertex_descriptor = GraphType::vertex_descriptor;
    using edge_descriptor = GraphType::edge_descriptor;
    // TODO: Check that vertex_path is connected
    for (size_t index = 1; index < vertex_path.size(); ++index) {
        vertex_descriptor target = vertex_path[index];
        vertex_descriptor source = vertex_path[index - 1];
        const auto edge_between = boost::edge(source, target, input_g);
        if (!edge_between.second)
            throw("create_edge_from_path: edge does not exist between "
                  "consecutive "
                  "vertices in the input path");
        const auto ed = edge_between.first;
        auto eps = input_g[ed].edge_points; // copied, might be modified

        if (!eps.empty()) { // sg_edge_points is never empty
            double dist_first_to_back = 0.0;
            double dist_back_to_back = 0.0;
            double dist_first_to_first = 0.0;
            double dist_back_to_first = 0.0;
            dist_first_to_back =
                    ArrayUtilities::distance(sg_edge_points[0], eps.back());
            dist_back_to_back =
                    ArrayUtilities::distance(sg_edge_points.back(), eps.back());
            dist_first_to_first =
                    ArrayUtilities::distance(sg_edge_points[0], eps[0]);
            dist_back_to_first =
                    ArrayUtilities::distance(sg_edge_points.back(), eps[0]);
            // Because the graph is undirected, source and target might be
            // switced. The new edge will have an order, based on the position
            // of the first node in the path. We reverse the order of the
            // elements if needed.
            const std::vector<std::reference_wrapper<const double>> dists{
                    std::cref(dist_first_to_back), std::cref(dist_back_to_back),
                    std::cref(dist_first_to_first),
                    std::cref(dist_back_to_first)};
            const auto result_it = std::min_element(dists.begin(), dists.end());
            // If the min distance is any of the _to_back (indices: 0 and 1)
            // reverse the edge points of the edge that is about to be added.
            if (std::distance(std::begin(dists), result_it) < 2) {
                std::reverse(eps.begin(), eps.end());
            }
        }
        // The first or the last node won't be added here
        if (index != 1) {
            // std::cout << "...About to add vertex position..." << std::endl;
            auto vertex_pos = input_g[source].pos;
            SG::insert_unique_edge_point_with_distance_order(sg_edge_points,
                                                             vertex_pos);
        }

        // TODO this could be slow... optimization welcome to append the whole
        // vector
        for (const auto &p : eps) {
            SG::insert_unique_edge_point_with_distance_order(sg_edge_points, p);
        }
    }
    // Remove the  node pos added just for keeping a sensible order.
    // That added node might be at the beggining or at the end on the final
    // vector.
    sg_edge_points.erase(std::remove(sg_edge_points.begin(),
                                     sg_edge_points.end(),
                                     input_g[vertex_path[0]].pos),
                         sg_edge_points.end());
    return sg_edge;
}

std::vector<GraphType::vertex_descriptor>
compute_shortest_path(GraphType::vertex_descriptor start_vertex,
                      GraphType::vertex_descriptor end_vertex,
                      const GraphType &input_g,
                      bool verbose) {
    using vertex_descriptor = GraphType::vertex_descriptor;
    using edge_descriptor = GraphType::edge_descriptor;
    using edge_iterator = GraphType::edge_iterator;
    size_t visited = 0;
    std::vector<boost::default_color_type> colors(boost::num_vertices(input_g),
                                                  boost::default_color_type{});
    std::vector<vertex_descriptor> _pred(boost::num_vertices(input_g),
                                         input_g.null_vertex());
    std::vector<size_t> _dist(boost::num_vertices(input_g), -1ull);

    auto colormap = colors.data();
    auto predmap = _pred.data();
    auto distmap = _dist.data();

    // From sehe:
    // https://stackoverflow.com/questions/29348724/dijkstra-graph-with-a-table-of-weights-on-each-edge
    auto weight_f = [&input_g](edge_descriptor ed) {
        // auto source = boost::source(ed, input_g);
        // auto target = boost::target(ed, input_g);
        // return ArrayUtilities::distance(input_g[source].pos,
        // input_g[target].pos);
        auto cl = SG::contour_length(ed, input_g);
        // std::cout << ed << std::endl;
        // std::cout << "contour_length: " << cl << std::endl;
        return cl;
    };

    auto weightmap = boost::make_function_property_map<edge_descriptor, double>(
            weight_f);
    shortest_path_visitor vis(end_vertex, visited);

    try {
        if (verbose)
            std::cout << "Searching from #" << start_vertex << " to #"
                      << end_vertex << "...\n";
        boost::dijkstra_shortest_paths(input_g, start_vertex,
                                       boost::visitor(vis)
                                               .color_map(colormap)
                                               .distance_map(distmap)
                                               .predecessor_map(predmap)
                                               .weight_map(weightmap));
        // .weight_map(boost::make_constant_property<edge_descriptor>(1ul)));
    } catch (shortest_path_visitor::done &) {
        if (verbose)
            std::cout << "Completed. Percentage visited: "
                      << (100.0 * visited / boost::num_vertices(input_g))
                      << "%\n";
    }

    size_t dist = distmap[end_vertex];
    if (verbose)
        std::cout << "Distance from #" << start_vertex << " to #" << end_vertex
                  << ": " << dist << "\n";

    std::vector<vertex_descriptor> path_out;
    if (dist != size_t(-1)) {
        std::deque<vertex_descriptor> path;
        for (vertex_descriptor current = end_vertex;
             current != input_g.null_vertex() && predmap[current] != current &&
             current != start_vertex;) {
            path.push_front(predmap[current]);
            current = predmap[current];
        }

        if (verbose) {
            std::cout << "Path from #" << start_vertex << " to #" << end_vertex
                      << ": ";
            std::copy(
                    path.begin(), path.end(),
                    std::ostream_iterator<vertex_descriptor>(std::cout, ", "));
            std::cout << end_vertex << "\n";
        }

        std::copy(path.begin(), path.end(), std::back_inserter(path_out));
        path_out.push_back(end_vertex);
    }
    return path_out;
}
} // end namespace SG
