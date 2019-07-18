#include "update_step_move_node.hpp"
#include "generate_common.hpp"

namespace SG {

void update_step_move_node::perform(const double &max_step_distance,
                                    // in/out parameters
                                    GraphType &graph,
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
    selected_node = select_random_node(graph);
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
}

void update_step_move_node::clear_stored_parameters(
        PointType &old_node_position,
        PointType &new_node_position,
        GraphType::vertex_descriptor &selected_node,
        std::vector<double> &old_distances,
        std::vector<double> &old_cosines,
        std::vector<double> &new_distances,
        std::vector<double> &new_cosines) const {
    old_node_position.fill(0);
    new_node_position.fill(0);
    selected_node = 0;
    old_distances.clear();
    old_cosines.clear();
    new_distances.clear();
    new_cosines.clear();
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

    if (new_distances_.empty()) {
        std::cerr << "update_step_move_node::undo called before calling "
                     "perform(). undo call has no effect."
                  << std::endl;
        return;
    }
    // Restore histograms (new_distances are now the old_distances in the
    // function)
    this->update_distances_histogram(histo_distances, new_distances,
                                     old_distances);
    this->update_cosines_histogram(histo_cosines, new_cosines, old_cosines);
    this->clear_stored_parameters(old_node_position, new_node_position,
                                  selected_node, old_distances, old_cosines,
                                  new_distances, new_cosines);
}

void update_step_move_node::update_distances_histogram(
        Histogram &histo_distances,
        const std::vector<double> &old_distances,
        const std::vector<double> &new_distances) const {
    std::cout << "OLD_DISTANCES_UPDATING_HISTOGRAM: " << old_distances.size()
              << std::endl;
    std::cout << "NEW_DISTANCES_UPDATING_HISTOGRAM: " << new_distances.size()
              << std::endl;
    for (const auto &dist : old_distances) {
        const auto bin = histo_distances.IndexFromValue(dist);
        std::cout << "bin: " << bin << "; old_distance: " << dist << std::endl;
        histo_distances.counts[bin]--;
    }
    for (const auto &dist : new_distances) {
        const auto bin = histo_distances.IndexFromValue(dist);
        std::cout << "bin: " << bin << "; new_distance: " << dist << std::endl;
        histo_distances.counts[bin]++;
    }
}
void update_step_move_node::update_cosines_histogram(
        Histogram &histo_cosines,
        const std::vector<double> &old_cosines,
        const std::vector<double> &new_cosines) const {

    for (const auto &cosine : old_cosines) {
        histo_cosines.counts[histo_cosines.IndexFromValue(cosine)]--;
    }
    for (const auto &cosine : new_cosines) {
        histo_cosines.counts[histo_cosines.IndexFromValue(cosine)]++;
    }
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
