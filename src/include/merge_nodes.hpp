#ifndef MERGE_NODES_HPP
#define MERGE_NODES_HPP

#include <iostream>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG
{

    /**
     * Insert point in the input container.
     * The input container is a list of points ordered by connectvity, consecutive points in the container are connected.
     *
     * This computes the distance of the new_point with all the existing edge_points.
     * TODO, an optimization would be to only compute it against first and last.
     *
     * PRECONDITION: edge_points are already ordered.
     *
     * @param edge_points container with existing points.
     * Ordered by connectivity, adjacent points are connected.
     * @param new_point point to intert.
     */
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
            [&new_point](const SG::SpatialEdge::PointType & ep){
            return ArrayUtilities::distance(ep, new_point);
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
    else if(min_index == distances_to_in_point.size() - 1) // This is safe, as vector is not empty.
        // edge_points.insert(std::end(edge_points), new_point);
        edge_points.push_back(new_point);
    else  // illogical error
        throw std::runtime_error("The impossible, node_to_remove closer edge_point is not at the beggining or end position in edge_points.");
}
/**
 * Some nodes that are connected between them,
 * could be merged.
 * Transforming 3 nodes with degree 3, to 1 node with degree 3
 * and adding the old nodes into the corresponding spatial edge.
 *
 * TODO:
 * Pitfall1: what happen when 2 of those nodes are connected between them with an extra parallel edge?
 *     (a)
 *      o--
 *(b) o/|  |
 *     \o--
 *     (c)
 *
 * Solution1: Don't do it in that case, (or use split_loop?)
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return number of nodes merged/cleared.
 */
template <class SpatialGraph>
size_t merge_three_connected_nodes(SpatialGraph & sg) {
    size_t node_was_merged = 0;
    using vertex_descriptor = typename boost::graph_traits<
        SpatialGraph>::vertex_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using adjacency_iterator =
        typename boost::graph_traits<SpatialGraph>::adjacency_iterator;
    using VertexPair = std::pair<vertex_descriptor, vertex_descriptor>;
    using VertexTriple = std::tuple<vertex_descriptor, vertex_descriptor, vertex_descriptor>;
    std::vector<VertexPair> nodes_to_remove;
    std::vector<VertexTriple> edges_to_remove;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        auto degree_current = boost::out_degree(*vi, sg);
        // limit to degree 3 nodes
        if (degree_current == 3){
            // Check that current node is not already selected to remove:
            {
                auto already_selected_to_delete = std::find_if(
                        std::begin(nodes_to_remove),
                        std::end(nodes_to_remove),
                        [&vi](const VertexPair & p) {
                        return p.first == *vi;
                        }
                        );
                if(already_selected_to_delete != std::end(nodes_to_remove))
                    continue;
            }
            adjacency_iterator neighbor_it, neighbor_end_it;
            std::tie(neighbor_it, neighbor_end_it) = boost::adjacent_vertices(*vi, sg);
            std::vector<std::pair<vertex_descriptor, vertex_descriptor>> neighbors_connected;
            for(; neighbor_it != neighbor_end_it; ++neighbor_it) {
                auto other_neighbor_it = neighbor_it;
                ++other_neighbor_it;
                if (other_neighbor_it != neighbor_end_it){
                    for(; other_neighbor_it != neighbor_end_it; ++other_neighbor_it) {
                        auto edge_exist_pair = boost::edge(*neighbor_it, *other_neighbor_it, sg);
                        // Check edge between neighbors exist.
                        if(edge_exist_pair.second){
                            // Check there are more than one edge between them.
                            // Store pair of vertex_descriptor of connected neighbors
                            // Because removing edges causes invalidation of adjacency_iterator
                            neighbors_connected.emplace_back(*neighbor_it, *other_neighbor_it);
                        }
                    }
                }
            }

            for(auto & nv : neighbors_connected){
                auto & sn_first = sg[nv.first];
                auto & sn_second = sg[nv.second];
                auto & sn_current = sg[*vi];

                auto degree_first = boost::out_degree(nv.first, sg);
                auto degree_second = boost::out_degree(nv.second, sg);
                // If neighbors have more than degree 3, abort merge
                // note that sn_current can only have degree 3 as well.
                if (degree_first != 3 && degree_second != 3)
                    continue;

                // If there are more than one edge connecting the node trio, abort.
                {
                    bool node_trio_has_parallel_edges = false;
                    std::vector<vertex_descriptor> tri_nodes = {*vi, nv.first, nv.second};
                    for(auto it = std::begin(tri_nodes) ; it != std::end(tri_nodes) && !node_trio_has_parallel_edges ; ++it){
                        auto it_other = it;
                        for(++it_other; it_other != std::end(tri_nodes) && !node_trio_has_parallel_edges ; ++it_other){
                            auto out_edges = boost::out_edges(*it, sg);
                            auto &ei = out_edges.first;
                            auto &ei_end = out_edges.second;
                            size_t number_of_edges_between_neighbors = 0;
                            for (; ei != ei_end ; ++ei )
                                if(boost::target(*ei, sg) ==
                                        *it_other )
                                    ++number_of_edges_between_neighbors;
                            if(number_of_edges_between_neighbors > 1)
                                node_trio_has_parallel_edges = true;
                        }
                    }
                    if(node_trio_has_parallel_edges)
                        continue;
                }

                auto edge_neighbors_pair = boost::edge(nv.first, nv.second, sg);
                auto edge_first_pair = boost::edge(*vi, nv.first, sg);
                auto edge_second_pair = boost::edge(*vi, nv.second, sg);
                auto & edge_neighbors = sg[edge_neighbors_pair.first];
                auto & edge_first = sg[edge_first_pair.first];
                auto & edge_second = sg[edge_second_pair.first];

                // If the edge_points are not empty, abort merge
                if (!(edge_neighbors.edge_points.empty() &&
                      edge_first.edge_points.empty() &&
                      edge_second.edge_points.empty()))
                    continue;

                nodes_to_remove.emplace_back(nv.first, *vi);
                nodes_to_remove.emplace_back(nv.second, *vi);
                edges_to_remove.emplace_back(nv.first, nv.second, *vi);
            }
        }
    }
    // First, remove edges between 3 connected nodes
    for(auto & edge_triple : edges_to_remove) {
        boost::remove_edge(
                std::get<0>(edge_triple),
                std::get<1>(edge_triple),
                sg);
        boost::remove_edge(
                std::get<0>(edge_triple),
                std::get<2>(edge_triple),
                sg);
        boost::remove_edge(
                std::get<1>(edge_triple),
                std::get<2>(edge_triple),
                sg);
    }

    // Connect new nodes to node_to_merge_into.
    for(auto & nodes : nodes_to_remove) {
        auto & node_to_remove = nodes.first;
        auto & node_to_merge_into = nodes.second;
        auto edges = boost::out_edges(node_to_remove, sg);
        // Create an edge to the node to merge into.
        for(auto ei = edges.first ; ei != edges.second; ++ei) {
            auto source = boost::source(*ei, sg);
            auto target = boost::target(*ei, sg);
            auto & spatial_edge = sg[*ei];
            auto & sn_to_remove = sg[node_to_remove];
            SG::insert_edge_point_with_distance_order(
                    spatial_edge.edge_points,
                    sn_to_remove.pos);
            boost::add_edge(node_to_merge_into, target, spatial_edge, sg);
            // boost::remove_edge(node_to_remove, target, sg);
        }
        // Remove vertex destroys all iterators/descripts
        // when using a vecS as vertex_intex_t
        // boost::remove_vertex(node_to_remove, sg);
        boost::clear_vertex(node_to_remove, sg);
        node_was_merged++;
    }
    return node_was_merged;
}

template <class SpatialGraph>
std::vector< std::pair<typename boost::graph_traits< SpatialGraph>::edge_descriptor,
                       typename boost::graph_traits< SpatialGraph>::edge_descriptor > >
get_parallel_edges(const SpatialGraph & sg)
{
    using vertex_descriptor = typename boost::graph_traits<
        SpatialGraph>::vertex_descriptor;
    using edge_descriptor = typename boost::graph_traits<
        SpatialGraph>::edge_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using adjacency_iterator =
        typename boost::graph_traits<SpatialGraph>::adjacency_iterator;
    using EdgePair = std::pair<edge_descriptor, edge_descriptor>;
    std::vector<EdgePair> parallel_edges;

    const auto verts = boost::vertices(sg);
    // From http://www.boost.org/doc/libs/1_66_0/libs/graph/doc/IncidenceGraph.html
    // It is guaranteed that given: e=out_edge(v); then source(e) == v.
    for (auto vi = verts.first; vi != verts.second; ++vi)
    {
        const auto out_edges = boost::out_edges(*vi, sg);
        for (auto ei1 = out_edges.first; ei1 != out_edges.second; ++ei1) {
            auto source = boost::source(*ei1, sg); // = *vi
            auto target1 = boost::target(*ei1, sg);
            // Copy edge iterator and plus one (to avoid compare the edge with itself)
            auto ei2 = ei1;
            ei2++;
            for (; ei2 != out_edges.second; ++ei2)
            {
                auto target2 = boost::target(*ei2, sg);
                // WARNING: do not check target2 == source
                // source(ei2) is guaranteed (by out_edges) to be equal to source(ei1)
                if(target2 == target1) // parallel edges
                {
                    parallel_edges.emplace_back(std::make_pair(*ei1, *ei2));
                }
            }
        }
    }
    return parallel_edges;
}

template<typename SpatialGraph>
std::vector<
std::pair<typename boost::graph_traits< SpatialGraph>::edge_descriptor,
          typename boost::graph_traits< SpatialGraph>::edge_descriptor > >
get_equal_parallel_edges(
    const std::vector<
    std::pair<typename boost::graph_traits< SpatialGraph>::edge_descriptor,
              typename boost::graph_traits< SpatialGraph>::edge_descriptor > >
    & parallel_edges, const SpatialGraph & sg)
{
    bool there_are_equal_parallel_edges = false;
    using edge_descriptor = typename boost::graph_traits<
        SpatialGraph>::edge_descriptor;
    using EdgePair = std::pair<edge_descriptor, edge_descriptor>;
    std::vector<EdgePair> equal_parallel_edges;

    for (const auto & edge_pair : parallel_edges)
    {
        const auto & points_first = sg[edge_pair.first].edge_points;
        const auto & points_second = sg[edge_pair.second].edge_points;
        if(points_first.empty() && points_second.empty())
        {
            // std::cout << "Empty parallel edges between:\n"
            //     << boost::source(edge_pair.first, sg)
            //     << "---"
            //     << boost::target(edge_pair.first, sg)
            //     << std::endl;
            there_are_equal_parallel_edges = true;
            equal_parallel_edges.push_back(edge_pair);
            continue;
        }
        if(points_first.size() == points_second.size())
        {
            auto sorted_points_first = points_first;
            std::sort(std::begin(sorted_points_first), std::end(sorted_points_first));
            auto sorted_points_second = points_second;
            std::sort(std::begin(sorted_points_second), std::end(sorted_points_second));
            if(sorted_points_first == sorted_points_second)
            {
            // std::cout << "Parallel edges of size " << points_first.size()
            //     << " between:\n"
            //     << boost::source(edge_pair.first, sg)
            //     << "---"
            //     << boost::target(edge_pair.first, sg)
            //     << std::endl;
            there_are_equal_parallel_edges = true;
            equal_parallel_edges.push_back(edge_pair);
            continue;
            }
        }

    }

    return equal_parallel_edges;

}


} //end namespace

#endif
