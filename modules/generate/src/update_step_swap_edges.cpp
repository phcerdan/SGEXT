#include "update_step_swap_edges.hpp"
#include "array_utilities.hpp"
#include "boundary_conditions.hpp"
#include "generate_common.hpp"
#include "rng.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "spatial_node.hpp"
#include <iostream>

namespace SG {

void update_step_swap_edges::randomize(const GraphType &graph,
                                       edge_descriptor_pair &selected_edges,
                                       bool &randomized_flag) const {
    selected_edges = this->select_two_valid_edges(graph);
    randomized_flag = true;
};

void update_step_swap_edges::perform(
        // in/out parameters
        GraphType &graph,
        Histogram &histo_distances,
        Histogram &histo_cosines,
        edge_descriptor_pair &selected_edges,
        bool &randomized_flag,
        // out parameters
        bool &is_swap_parallel,
        edge_descriptor_pair &new_edges,
        std::vector<double> &old_distances,
        std::vector<double> &old_cosines,
        std::vector<double> &new_distances,
        std::vector<double> &new_cosines) const {

    if (!randomized_flag) {
        this->randomize(graph, selected_edges, randomized_flag);
    }
    // select two valid edges to swap at random
    auto edge1 = selected_edges.first;
    auto edge2 = selected_edges.second;
    // get positions of nodes of both edges
    const auto &edge1_source_pos = graph[edge1.m_source].pos;
    const auto &edge2_source_pos = graph[edge2.m_source].pos;
    const auto &edge1_target_pos = graph[edge1.m_target].pos;
    const auto &edge2_target_pos = graph[edge2.m_target].pos;
    auto edge1_target_image_pos = edge1_target_pos;
    auto edge2_target_image_pos = edge2_target_pos;
    if (boundary_condition == ArrayUtilities::boundary_condition::PERIODIC) {
        edge1_target_image_pos = ArrayUtilities::closest_image_from_reference(
                edge1_source_pos, edge1_target_image_pos);
        edge2_target_image_pos = ArrayUtilities::closest_image_from_reference(
                edge2_source_pos, edge2_target_image_pos);
    }
    // get old distances
    old_distances.push_back(
            ArrayUtilities::distance(edge1_source_pos, edge1_target_image_pos));
    old_distances.push_back(
            ArrayUtilities::distance(edge2_source_pos, edge2_target_image_pos));
    // get old cosines
    auto old_cosines1_source = compute_cosine_directors_from_source(
            edge1.m_source, edge1.m_target, edge1_source_pos,
            edge1_target_image_pos, graph);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines1_source),
                       std::end(old_cosines1_source));

    auto old_cosines1_target = compute_cosine_directors_from_source(
            edge1.m_target, edge1.m_source, edge1_target_image_pos,
            edge1_source_pos, graph);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines1_target),
                       std::end(old_cosines1_target));

    auto old_cosines2_source = compute_cosine_directors_from_source(
            edge2.m_source, edge2.m_target, edge2_source_pos,
            edge2_target_image_pos, graph);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines2_source),
                       std::end(old_cosines2_source));

    auto old_cosines2_target = compute_cosine_directors_from_source(
            edge2.m_target, edge2.m_source, edge2_target_image_pos,
            edge2_source_pos, graph);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines2_target),
                       std::end(old_cosines2_target));
    // swap edges
    // The swap has two possibilies, from the initial state:
    // - S1-T1; S2-T2 to "parallel" or "crossed"
    /*
    S1 --- T1  |  S1     T1  |  S1-\ /-T1
               |  |      |   |      .
    S2 --- T2  |  S2     T2  |  S2_/ \_T2
    */
    // Flip a coin to decide what kind of swap.
    is_swap_parallel = RNG::random_bool(0.5);
    // Get positions of vertices of the new edges
    const auto new_sources_targets = get_sources_and_targets_of_new_edges(
            is_swap_parallel, edge1, edge2);
    auto nedge1_source = new_sources_targets[0];
    auto nedge1_target = new_sources_targets[1];
    auto nedge2_source = new_sources_targets[2];
    auto nedge2_target = new_sources_targets[3];
    const auto new_positions =
            get_positions_of_new_edges(is_swap_parallel, edge1, edge2, graph);
    auto nedge1_source_pos = new_positions[0];
    auto nedge1_target_pos = new_positions[1];
    auto nedge2_source_pos = new_positions[2];
    auto nedge2_target_pos = new_positions[3];

    // Store new edges
    auto enew1 = edge_descriptor();
    enew1.m_source = nedge1_source;
    enew1.m_target = nedge1_target;
    auto enew2 = edge_descriptor();
    enew2.m_source = nedge2_source;
    enew2.m_target = nedge2_target;
    new_edges = std::make_pair(enew1, enew2);

    // handle boundary_condition for histograms computation
    PointType nedge1_target_image_pos = nedge1_target_pos;
    PointType nedge2_target_image_pos = nedge2_target_pos;
    if (boundary_condition == ArrayUtilities::boundary_condition::PERIODIC) {
        nedge1_target_image_pos = ArrayUtilities::closest_image_from_reference(
                nedge1_source_pos, nedge1_target_image_pos);
        nedge2_target_image_pos = ArrayUtilities::closest_image_from_reference(
                nedge2_source_pos, nedge2_target_image_pos);
    }
    // get new distances
    new_distances.push_back(ArrayUtilities::distance(nedge1_source_pos,
                                                     nedge1_target_image_pos));
    new_distances.push_back(ArrayUtilities::distance(nedge2_source_pos,
                                                     nedge2_target_image_pos));
    // get new cosines
    auto ignore_node_edge1_source = edge1.m_target;
    const auto new_cosines1_source = compute_cosine_directors_from_source(
            nedge1_source, ignore_node_edge1_source, nedge1_source_pos,
            nedge1_target_image_pos, graph);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines1_source),
                       std::end(new_cosines1_source));

    auto ignore_node_edge1_target =
            is_swap_parallel ? edge2.m_target : edge2.m_source;
    const auto new_cosines1_target = compute_cosine_directors_from_source(
            nedge1_target, ignore_node_edge1_target, nedge1_target_image_pos,
            nedge1_source_pos, graph);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines1_target),
                       std::end(new_cosines1_target));

    auto ignore_node_edge2_source = edge1.m_source;
    const auto new_cosines2_source = compute_cosine_directors_from_source(
            nedge2_source, ignore_node_edge2_source, nedge2_source_pos,
            nedge2_target_image_pos, graph);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines2_source),
                       std::end(new_cosines2_source));

    auto ignore_node_edge2_target =
            is_swap_parallel ? edge2.m_source : edge2.m_target;
    const auto new_cosines2_target = compute_cosine_directors_from_source(
            nedge2_target, ignore_node_edge2_target, nedge2_target_image_pos,
            nedge2_source_pos, graph);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines2_target),
                       std::end(new_cosines2_target));

    // update histograms
    this->update_distances_histogram(histo_distances, old_distances,
                                     new_distances);
    this->update_cosines_histogram(histo_cosines, old_cosines, new_cosines);
    // clear flag
    randomized_flag = false;
}
void update_step_swap_edges::clear_selected_edges(
        edge_descriptor_pair &selected_edges,
        edge_descriptor_pair &new_edges) const {
    selected_edges.first.m_source =
            std::numeric_limits<vertex_descriptor>::max();
    selected_edges.first.m_target =
            std::numeric_limits<vertex_descriptor>::max();
    selected_edges.second.m_source =
            std::numeric_limits<vertex_descriptor>::max();
    selected_edges.second.m_target =
            std::numeric_limits<vertex_descriptor>::max();
    new_edges.first.m_source = std::numeric_limits<vertex_descriptor>::max();
    new_edges.first.m_target = std::numeric_limits<vertex_descriptor>::max();
    new_edges.second.m_source = std::numeric_limits<vertex_descriptor>::max();
    new_edges.second.m_target = std::numeric_limits<vertex_descriptor>::max();
}

void update_step_swap_edges::undo(
        // in/out parameters
        Histogram &histo_distances,
        Histogram &histo_cosines,
        // out parameters
        edge_descriptor_pair &selected_edges,
        edge_descriptor_pair &new_edges,
        std::vector<double> &old_distances,
        std::vector<double> &old_cosines,
        std::vector<double> &new_distances,
        std::vector<double> &new_cosines) const {
    parent_class::undo(histo_distances, histo_cosines, old_distances,
                       old_cosines, new_distances, new_cosines);
    this->clear_selected_edges(selected_edges, new_edges);
}

void update_step_swap_edges::update_graph(
        GraphType &graph,
        const edge_descriptor_pair &selected_edges,
        const bool &is_swap_parallel) const {
    edge_descriptor edge1, edge2;
    std::tie(edge1, edge2) = selected_edges;
    const auto arr = get_sources_and_targets_of_new_edges(is_swap_parallel,
                                                          edge1, edge2);
    const auto &nedge1_source = arr[0];
    const auto &nedge1_target = arr[1];
    const auto &nedge2_source = arr[2];
    const auto &nedge2_target = arr[3];
    // Swap edges
    // SpatialEdges are ignored
    // auto spatial_edge1 = graph[edge1]; auto spatial_edge2 = graph[edge2];
    // remove the old edges
    boost::remove_edge(edge1, graph);
    boost::remove_edge(edge2, graph);
    // add the new edges
    boost::add_edge(nedge1_source, nedge1_target, graph);
    boost::add_edge(nedge2_source, nedge2_target, graph);
}

std::pair<update_step_swap_edges::edge_descriptor,
          update_step_swap_edges::edge_descriptor>
update_step_swap_edges::select_two_valid_edges(const GraphType &graph,
                                               int recursive_count) const {
    if (recursive_count > 10) {
        throw std::runtime_error("select_two_valid_edges() is not able to find "
                                 "two valid (non-adjacent) edges."

                                 " recursive_count too high: " +
                                 std::to_string(recursive_count) +
                                 ".\nMaybe the graph is too "
                                 "small? or over-connected?");
    }
    // select two edges to swap at random
    edge_descriptor edge1 = select_random_edge(graph);
    edge_descriptor edge2 = select_random_edge(graph);

    if (boost::num_edges(graph) <= 1) {
        throw std::logic_error("select_two_valid_edges in "
                               "update_step_swap_edges needs graph "
                               "with more than"
                               "one edge");
    }
    while (edge1 == edge2) {
        edge2 = select_random_edge(graph);
    }

    // Check that edges are not adjacent. Just comparing if any source or
    // target of the two edges are repeated
    std::vector<GraphType::vertex_descriptor> edge_nodes{
            edge1.m_source, edge1.m_target, edge2.m_source, edge2.m_target};
    std::sort(std::begin(edge_nodes), std::end(edge_nodes));
    auto pos = std::adjacent_find(std::begin(edge_nodes), std::end(edge_nodes));
    bool duplicate_exist = (pos != std::end(edge_nodes)) ? true : false;
    if (duplicate_exist) {
        // try again
        return select_two_valid_edges(graph, ++recursive_count);
    } else {
        return std::make_pair(edge1, edge2);
    }
}

std::vector<VectorType> update_step_swap_edges::get_adjacent_edges_from_source(
        const GraphType::vertex_descriptor source,
        const GraphType::vertex_descriptor ignore_node,
        const GraphType &graph) const {

    std::vector<VectorType> adj_edges; // output

    AdjacentVerticesPositions adjacents =
            get_adjacent_vertices_positions(source, graph);
    const auto source_pos = graph[source].pos;

    for (size_t neigh_index = 0;
         neigh_index < adjacents.neighbours_descriptors.size(); ++neigh_index) {

        if (adjacents.neighbours_descriptors[neigh_index] == ignore_node) {
            continue;
        }

        auto neigh_pos_image = adjacents.neighbours_positions[neigh_index];

        if (boundary_condition ==
            ArrayUtilities::boundary_condition::PERIODIC) {
            neigh_pos_image = ArrayUtilities::closest_image_from_reference(
                    source_pos, neigh_pos_image);
        }
        adj_edges.push_back(ArrayUtilities::minus(neigh_pos_image, source_pos));
    }
    return adj_edges;
}

std::vector<double>
update_step_swap_edges::compute_cosine_directors_from_source(
        const GraphType::vertex_descriptor source,
        const GraphType::vertex_descriptor ignore_node,
        const PointType source_pos,
        const PointType target_pos,
        const GraphType &graph) const {

    const auto adj_edges =
            get_adjacent_edges_from_source(source, ignore_node, graph);
    VectorType fixed_edge = ArrayUtilities::minus(target_pos, source_pos);
    return cosine_directors_between_edges_and_target_edge(adj_edges,
                                                          fixed_edge);
}

std::array<GraphType::vertex_descriptor, 4>
update_step_swap_edges::get_sources_and_targets_of_new_edges(
        const bool &is_swap_parallel,
        const edge_descriptor &edge1,
        const edge_descriptor &edge2) const {

    GraphType::vertex_descriptor nedge1_source, nedge1_target;
    GraphType::vertex_descriptor nedge2_source, nedge2_target;
    if (is_swap_parallel) {
        nedge1_source = edge1.m_source;
        nedge1_target = edge2.m_source;
        nedge2_source = edge1.m_target;
        nedge2_target = edge2.m_target;
    } else { // crossed
        nedge1_source = edge1.m_source;
        nedge1_target = edge2.m_target;
        nedge2_source = edge2.m_source;
        nedge2_target = edge1.m_target;
    }
    return {nedge1_source, nedge1_target, nedge2_source, nedge2_target};
}

std::array<PointType, 4> update_step_swap_edges::get_positions_of_new_edges(
        const bool &is_swap_parallel,
        const edge_descriptor &edge1,
        const edge_descriptor &edge2,
        const GraphType &graph) const {

    const auto edge1_source_pos = graph[edge1.m_source].pos;
    const auto edge1_target_pos = graph[edge1.m_target].pos;
    const auto edge2_source_pos = graph[edge2.m_source].pos;
    const auto edge2_target_pos = graph[edge2.m_target].pos;
    PointType nedge1_source_pos;
    PointType nedge1_target_pos;
    PointType nedge2_source_pos;
    PointType nedge2_target_pos;
    if (is_swap_parallel) {
        nedge1_source_pos = edge1_source_pos;
        nedge1_target_pos = edge2_source_pos;
        nedge2_source_pos = edge1_target_pos;
        nedge2_target_pos = edge2_target_pos;
    } else { // crossed
        nedge1_source_pos = edge1_source_pos;
        nedge1_target_pos = edge2_target_pos;
        nedge2_source_pos = edge2_source_pos;
        nedge2_target_pos = edge1_target_pos;
    }
    return {nedge1_source_pos, nedge1_target_pos, nedge2_source_pos,
            nedge2_target_pos};
}

} // namespace SG
