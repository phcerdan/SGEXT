/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "update_step_generate_contour_length.hpp"
#include "contour_length_generator_functions.hpp"
#include "edge_points_utilities.hpp" // For contour_length()
#include "generate_common.hpp"

namespace SG {
void update_step_generate_contour_length::clear_stored_parameters(
        double &old_distance,
        double &new_distance,
        PointContainer &old_edge_points,
        PointContainer &new_edge_points) const {
    old_distance = 0.0;
    new_distance = 0.0;
    old_edge_points.clear();
    new_edge_points.clear();
}
void update_step_generate_contour_length::update_graph(
        GraphType &graph, const edge_descriptor &selected_edge) const {
    graph[selected_edge].edge_points = new_edge_points_;
}

void update_step_generate_contour_length::update_distances_histogram(
        Histogram &histo_distances,
        const double &old_distance,
        const double &new_distance) const {
    const auto new_bin = histo_distances.IndexFromValue(new_distance);
    histo_distances.counts[new_bin]++;
    const auto old_bin = histo_distances.IndexFromValue(old_distance);
    histo_distances.counts[old_bin]--;
}

void update_step_generate_contour_length::undo(Histogram &histo,
                                               double &old_distance,
                                               double &new_distance) const {
    this->update_distances_histogram(histo, new_distance, old_distance);
}
void update_step_generate_contour_length::randomize(
        GraphType &graph,
        edge_descriptor &selected_edge,
        bool &randomized_flag) const {
    selected_edge = SG::select_random_edge(graph);
    randomized_flag = true;
}

void update_step_generate_contour_length::print(std::ostream &os) const {
    os << "old_distance: " << old_distance_ << std::endl;
    os << "new_distance: " << new_distance_ << std::endl;
    os << "old_edge_points_: " << std::endl;
    print_edge_points(old_edge_points_, os);
    os << "new_edge_points_: " << std::endl;
    print_edge_points(new_edge_points_, os);
}

void update_step_generate_contour_length::perform(
        // in parameters
        const double &k_bending,
        const size_t &num_monomers,
        // in/out parameters
        GraphType &graph,
        Histogram &histo_distances,
        edge_descriptor &selected_edge,
        bool &randomized_flag,
        // out parameters
        PointContainer &old_edge_points,
        PointContainer &new_edge_points,
        double &old_distance,
        double &new_distance) const {
    this->clear_stored_parameters(old_distance, new_distance, old_edge_points,
                                  new_edge_points);
    // TODO think about periodic conditions for edge_points
    // -- some functions, such as SG::contour_length do not take into account
    // boundary_conditions yet.
    // const bool is_periodic = (boundary_condition ==
    //                           ArrayUtilities::boundary_condition::PERIODIC);
    // select an to generate a contour length at random
    if (!randomized_flag) {
        this->randomize(graph, selected_edge, randomized_flag);
    }

    // get old distance and edge_points;
    old_edge_points = graph[selected_edge].edge_points;
    old_distance = SG::contour_length(selected_edge, graph);

    // generate a new contour length given input parameters
    const auto &source_pos = graph[selected_edge.m_source].pos;
    const auto &target_pos = graph[selected_edge.m_target].pos;
    auto result = SG::generate_contour_length(source_pos, target_pos, k_bending,
                                              num_monomers);
    new_edge_points = result.first;
    auto &contour_end_to_end_ratio = result.second;
    new_distance = contour_end_to_end_ratio *
                   ArrayUtilities::distance(source_pos, target_pos);
    // update histogram
    this->update_distances_histogram(histo_distances, old_distance,
                                     new_distance);
    // clear flag
    randomized_flag = false;
}

} // namespace SG
