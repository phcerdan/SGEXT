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

    this->clear_stored_parameters(old_distances, old_cosines, new_distances,
                                  new_cosines);
    const bool is_periodic = (boundary_condition ==
                              ArrayUtilities::boundary_condition::PERIODIC);
    // select two valid edges to swap at random
    if (!randomized_flag) {
        this->randomize(graph, selected_edges, randomized_flag);
    }
    auto edge1 = selected_edges.first;
    auto edge2 = selected_edges.second;
    // get positions of nodes of both edges
    const auto &edge1_source_pos = graph[edge1.m_source].pos;
    const auto &edge2_source_pos = graph[edge2.m_source].pos;
    const auto &edge1_target_pos = graph[edge1.m_target].pos;
    const auto &edge2_target_pos = graph[edge2.m_target].pos;
    auto edge1_target_image_pos = edge1_target_pos;
    auto edge2_target_image_pos = edge2_target_pos;
    auto edge1_source_image_pos = edge1_source_pos;
    auto edge2_source_image_pos = edge2_source_pos;
    if (is_periodic) {
        edge1_target_image_pos = ArrayUtilities::closest_image_from_reference(
                edge1_source_pos, edge1_target_image_pos);
        edge2_target_image_pos = ArrayUtilities::closest_image_from_reference(
                edge2_source_pos, edge2_target_image_pos);
        edge1_source_image_pos = ArrayUtilities::closest_image_from_reference(
                edge1_target_pos, edge1_source_image_pos);
        edge2_source_image_pos = ArrayUtilities::closest_image_from_reference(
                edge2_target_pos, edge2_source_image_pos);
    }
    // get old distances
    old_distances.push_back(
            ArrayUtilities::distance(edge1_source_pos, edge1_target_image_pos));
    old_distances.push_back(
            ArrayUtilities::distance(edge2_source_pos, edge2_target_image_pos));

    const auto adjacent_arrays_edge1_source = get_adjacent_edges_from_source(
            edge1.m_source, edge1.m_target, graph, boundary_condition);
    const auto adjacent_arrays_edge1_target = get_adjacent_edges_from_source(
            edge1.m_target, edge1.m_source, graph, boundary_condition);
    const auto adjacent_arrays_edge2_source = get_adjacent_edges_from_source(
            edge2.m_source, edge2.m_target, graph, boundary_condition);
    const auto adjacent_arrays_edge2_target = get_adjacent_edges_from_source(
            edge2.m_target, edge2.m_source, graph, boundary_condition);

    const auto old_fixed_edge1_out_source =
            ArrayUtilities::minus(edge1_target_image_pos, edge1_source_pos);
    const auto old_fixed_edge1_out_target =
            ArrayUtilities::minus(edge1_source_image_pos, edge1_target_pos);
    const auto old_fixed_edge2_out_source =
            ArrayUtilities::minus(edge2_target_image_pos, edge2_source_pos);
    const auto old_fixed_edge2_out_target =
            ArrayUtilities::minus(edge2_source_image_pos, edge2_target_pos);

    // get old cosines
    const auto old_cosines1_source =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge1_source, old_fixed_edge1_out_source);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines1_source),
                       std::end(old_cosines1_source));

    const auto old_cosines1_target =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge1_target, old_fixed_edge1_out_target);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines1_target),
                       std::end(old_cosines1_target));

    const auto old_cosines2_source =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge2_source, old_fixed_edge2_out_source);
    old_cosines.insert(std::end(old_cosines), std::begin(old_cosines2_source),
                       std::end(old_cosines2_source));

    const auto old_cosines2_target =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge2_target, old_fixed_edge2_out_target);
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
    const auto [nedge1_source, nedge1_target, nedge2_source, nedge2_target] =
            get_sources_and_targets_of_new_edges(is_swap_parallel, edge1,
                                                 edge2);
    const auto [nedge1_source_pos, nedge1_target_pos, nedge2_source_pos,
                nedge2_target_pos] =
            get_positions_of_new_edges(is_swap_parallel, edge1, edge2, graph);

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
    PointType nedge1_source_image_pos = nedge1_source_pos;
    PointType nedge2_source_image_pos = nedge2_source_pos;
    if (is_periodic) {
        nedge1_target_image_pos = ArrayUtilities::closest_image_from_reference(
                nedge1_source_pos, nedge1_target_image_pos);
        nedge2_target_image_pos = ArrayUtilities::closest_image_from_reference(
                nedge2_source_pos, nedge2_target_image_pos);
        nedge1_source_image_pos = ArrayUtilities::closest_image_from_reference(
                nedge1_target_pos, nedge1_source_image_pos);
        nedge2_source_image_pos = ArrayUtilities::closest_image_from_reference(
                nedge2_target_pos, nedge2_source_image_pos);
    }
    // get new distances
    new_distances.push_back(ArrayUtilities::distance(nedge1_source_pos,
                                                     nedge1_target_image_pos));
    new_distances.push_back(ArrayUtilities::distance(nedge2_source_pos,
                                                     nedge2_target_image_pos));
    // get new cosines
    /*
    S1 --- T1  |  S1       S1-\
               |  |    or      .
               |  S2            \_T2
    ---------------------------------------------
    S2 --- T2  |  T1            /-T1
               |  |    or      .
               |  T2       S2_/
    */
    const auto new_fixed_edge1_out_source =
            ArrayUtilities::minus(nedge1_target_image_pos, nedge1_source_pos);
    const auto new_fixed_edge1_out_target =
            ArrayUtilities::minus(nedge1_source_image_pos, nedge1_target_pos);
    const auto new_fixed_edge2_out_source =
            ArrayUtilities::minus(nedge2_target_image_pos, nedge2_source_pos);
    const auto new_fixed_edge2_out_target =
            ArrayUtilities::minus(nedge2_source_image_pos, nedge2_target_pos);

    // get new cosines
    const auto new_cosines1_source =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge1_source, new_fixed_edge1_out_source);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines1_source),
                       std::end(new_cosines1_source));

    const auto new_cosines1_target =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge1_target, new_fixed_edge1_out_target);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines1_target),
                       std::end(new_cosines1_target));

    const auto new_cosines2_source =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge2_source, new_fixed_edge2_out_source);
    new_cosines.insert(std::end(new_cosines), std::begin(new_cosines2_source),
                       std::end(new_cosines2_source));

    const auto new_cosines2_target =
            cosine_directors_between_edges_and_target_edge(
                    adjacent_arrays_edge2_target, new_fixed_edge2_out_target);
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
    const bool duplicate_exist = (pos != std::end(edge_nodes));
    if (duplicate_exist) {
        // try again
        return select_two_valid_edges(graph, ++recursive_count);
    }
    return std::make_pair(edge1, edge2);
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
