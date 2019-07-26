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
    for (const auto &p : adjacent_vertices_positions.neighbours_positions) {
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
        // Store edges (pointing OUT selected_node_) to compute angles
        old_edges.push_back(
                ArrayUtilities::minus(p_image_old, old_node_position));
        new_edges.push_back(
                ArrayUtilities::minus(p_image_new, new_node_position));
    }
    // Cosines from old and new edges
    old_cosines = cosine_directors_from_connected_edges(old_edges);
    new_cosines = cosine_directors_from_connected_edges(new_edges);

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
