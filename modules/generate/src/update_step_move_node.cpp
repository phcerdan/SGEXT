#include "update_step_move_node.hpp"
#include "array_utilities.hpp"
#include "boundary_conditions.hpp"
#include "generate_common.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "spatial_node.hpp"
#include <iostream>

namespace SG {

void update_step_move_node::randomize(
        const GraphType &graph,
        GraphType::vertex_descriptor &selected_node,
        bool &randomized_flag) const {
    selected_node = select_random_node(graph);
    randomized_flag = true;
}
void update_step_move_node::perform(const double &max_step_distance,
                                    // in/out parameters
                                    GraphType &graph,
                                    Histogram &histo_distances,
                                    Histogram &histo_cosines,
                                    GraphType::vertex_descriptor &selected_node,
                                    bool &randomized_flag,
                                    // out parameters
                                    PointType &old_node_position,
                                    PointType &new_node_position,
                                    std::vector<double> &old_distances,
                                    std::vector<double> &old_cosines,
                                    std::vector<double> &new_distances,
                                    std::vector<double> &new_cosines) const {
    this->clear_stored_parameters(old_distances, old_cosines, new_distances,
                                  new_cosines);
    if (!randomized_flag) {
        selected_node = select_random_node(graph);
    }
    // Store the old position of the node you are going to move.
    old_node_position = graph[selected_node].pos;
    // std::cout << "SELECTED_NODE: " << selected_node << std::endl;
    // std::cout << "POSITION: "; SG::print_pos(std::cout, old_node_position);
    // std::cout << std::endl;
    if (boundary_condition == ArrayUtilities::boundary_condition::NONE) {
        new_node_position = ArrayUtilities::plus(
                old_node_position, generate_random_array(max_step_distance));
    } else if (boundary_condition ==
               ArrayUtilities::boundary_condition::PERIODIC) {
        new_node_position =
                ArrayUtilities::plus_with_boundary_condition_periodic(
                        old_node_position,
                        generate_random_array(max_step_distance));
    }

    AdjacentVerticesPositions adjacent_vertices_positions =
            get_adjacent_vertices_positions(selected_node, graph);

    std::vector<VectorType> old_edges;
    std::vector<VectorType> new_edges;
    const auto num_neighbors =
            adjacent_vertices_positions.neighbours_positions.size();
    for (size_t i = 0; i < num_neighbors; i++) {
        const auto &p = adjacent_vertices_positions.neighbours_positions[i];
        const auto &vd = adjacent_vertices_positions.neighbours_descriptors[i];
        auto p_image_old = p;
        auto p_image_new = p;
        if (boundary_condition ==
            ArrayUtilities::boundary_condition::PERIODIC) {
            p_image_old = ArrayUtilities::closest_image_from_reference(
                    old_node_position, p);
            p_image_new = ArrayUtilities::closest_image_from_reference(
                    new_node_position, p);
        }
        // Distances
        old_distances.push_back(
                ArrayUtilities::distance(p_image_old, old_node_position));
        // std::cout << "ADJ_POSITION: ";
        // SG::print_pos(std::cout, p); std::cout << std::endl;
        // std::cout << "OLD_DISTANCE: " << old_distances.back() << std::endl;
        new_distances.push_back(
                ArrayUtilities::distance(p_image_new, new_node_position));

        // Cosines
        const auto old_array =
                ArrayUtilities::minus(p_image_old, old_node_position);
        const auto new_array =
                ArrayUtilities::minus(p_image_new, new_node_position);
        // Store edges (pointing OUT selected_node_) to compute angles around
        // the edges connected to the moved node.
        old_edges.push_back(old_array);
        new_edges.push_back(new_array);
        // Moving a node affects the angles from the edges having as source the
        // moved node, but also those edges having it as target.
        const auto adjacent_arrays_target = get_adjacent_edges_from_source(
                vd, selected_node /*ignore */, graph, boundary_condition);

        const auto old_target_cosines =
                cosine_directors_between_edges_and_target_edge(
                        adjacent_arrays_target, old_array);
        old_cosines.insert(std::end(old_cosines),
                           std::begin(old_target_cosines),
                           std::end(old_target_cosines));

        const auto new_target_cosines =
                cosine_directors_between_edges_and_target_edge(
                        adjacent_arrays_target, new_array);
        new_cosines.insert(std::end(new_cosines),
                           std::begin(new_target_cosines),
                           std::end(new_target_cosines));
    }

    // Cosines from old and new edges
    const auto old_cosines_between_edges_sharing_moved_node =
            cosine_directors_from_connected_edges(old_edges);
    const auto new_cosines_between_edges_sharing_moved_node =
            cosine_directors_from_connected_edges(new_edges);
    old_cosines.insert(std::end(old_cosines),
                       std::begin(old_cosines_between_edges_sharing_moved_node),
                       std::end(old_cosines_between_edges_sharing_moved_node));
    new_cosines.insert(std::end(new_cosines),
                       std::begin(new_cosines_between_edges_sharing_moved_node),
                       std::end(new_cosines_between_edges_sharing_moved_node));

#if !defined(NDEBUG)
    const bool verbose = false;
    if (verbose) {
        std::cout << "Old Node Position: ";
        std::cout << ArrayUtilities::to_string(old_node_position) << std::endl;
        std::cout << "New Node Position: ";
        std::cout << ArrayUtilities::to_string(new_node_position) << std::endl;
        std::cout << "Old distances: ";
        this->print(std::cout);
        std::cout << "Old edges: ";
        for (const auto &v : old_edges) {
            std::cout << "{ " << ArrayUtilities::to_string(v) << "} ,";
        }
        std::cout << std::endl;
        std::cout << "New edges: ";
        for (const auto &v : new_edges) {
            std::cout << "{ " << ArrayUtilities::to_string(v) << "} ,";
        }
        std::cout << std::endl;
    }
#endif
    // Update Histograms:
    //  Remove old_distances and old_cosines and
    //  add new_distances, new_cosines
    this->update_distances_histogram(histo_distances, old_distances,
                                     new_distances);
    this->update_cosines_histogram(histo_cosines, old_cosines, new_cosines);
    // clear flag
    randomized_flag = false;
}

void update_step_move_node::clear_move_node_parameters(
        PointType &old_node_position,
        PointType &new_node_position,
        GraphType::vertex_descriptor &selected_node) const {
    old_node_position.fill(0);
    new_node_position.fill(0);
    selected_node = std::numeric_limits<decltype(selected_node_)>::max();
}

void update_step_move_node::undo(
        // in/out parameters
        Histogram &histo_distances,
        Histogram &histo_cosines,
        // out parameters
        GraphType::vertex_descriptor &selected_node,
        PointType &old_node_position,
        PointType &new_node_position,
        std::vector<double> &old_distances,
        std::vector<double> &old_cosines,
        std::vector<double> &new_distances,
        std::vector<double> &new_cosines) const {
    parent_class::undo(histo_distances, histo_cosines, old_distances,
                       old_cosines, new_distances, new_cosines);
    this->clear_move_node_parameters(old_node_position, new_node_position,
                                     selected_node);
}

void update_step_move_node::update_graph(
        GraphType &graph,
        const GraphType::vertex_descriptor &node_id,
        const PointType &new_node_position) const {
    // Change pos of the SpatialNode of the moved graph.
    graph[node_id].pos = new_node_position;
    // The change of the positions has no extra change in the graph.
    // SpatialEdge.edge_points are not involved in this update step.
    // Create a new class and overwrite this function if needed.
}

} // namespace SG
