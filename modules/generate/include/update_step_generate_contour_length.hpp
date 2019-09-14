/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef UPDATE_STEP_GENERATE_CONTOUR_LENGTH_HPP
#define UPDATE_STEP_GENERATE_CONTOUR_LENGTH_HPP

#include "update_step.hpp"

namespace SG {
class update_step_generate_contour_length : public update_step {
  public:
    using update_step::update_step;
    update_step_generate_contour_length() = default;
    update_step_generate_contour_length(
            GraphType &graph_input,
            Histogram &histo_contour_length_distances_input)
            : graph_(&graph_input),
              histo_distances_(&histo_contour_length_distances_input){};

    using vertex_descriptor = GraphType::vertex_descriptor;
    using edge_descriptor = GraphType::edge_descriptor;

    void clear_stored_parameters(double &old_distance,
                                 double &new_distance,
                                 PointContainer &old_edge_points,
                                 PointContainer &new_edge_points) const;
    void update_distances_histogram(Histogram &histo_distances,
                                    const double &old_distance,
                                    const double &new_distance) const;
    /**
     * Select a random edge, generate a contour length (with edge_points), and
     * update histograms taking into account this change of contour length.
     * Note that the graph is not updated, in case the movement has to be
     * undone with undo()
     */
    void perform(
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
        double &new_distance
        ) const;
    inline void perform() override {
        this->perform();
    }
    void undo(
            // in/out parameters
            Histogram &histo,
            double &old_distance,
            double &new_distance) const;

    inline void undo() override {
        this->undo(*histo_distances_, old_distance_, new_distance_);
        this->clear_stored_parameters(old_distance_, new_distance_,
                                      old_edge_points_, new_edge_points_);
    }

    /**
     * Remove old distances and add new from histogram.
     *
     * @param histo_distances
     * @param old_distances
     * @param new_distances
     */
    void update_histogram(Histogram &histo_distances,
                          const double &old_distance,
                          const double &new_distance) const;

    void update_graph() override {
        if (selected_edge_.m_source ==
                    std::numeric_limits<vertex_descriptor>::max() ||
            selected_edge_.m_target ==
                    std::numeric_limits<vertex_descriptor>::max()) {
            throw std::logic_error("update_graph() has to be called after "
                                   "perform(), not before.");
        }
        this->update_graph(*graph_, selected_edge_);
    };
    void update_graph(GraphType &graph,
                      const edge_descriptor &selected_edge) const;

    void randomize(GraphType &graph,
                   edge_descriptor &selected_edge,
                   bool &randomized_flag) const;
    inline void randomize() {
        this->randomize(*graph_, selected_edge_, randomized_flag_);
    }

    void print(std::ostream &os) const;
    GraphType *graph_;
    Histogram *histo_distances_;
    edge_descriptor selected_edge_;
    bool randomized_flag_;

  protected:
    /// Stored values for undo capabilities.
    double old_distance_;
    double new_distance_;
    PointContainer old_edge_points_;
    PointContainer new_edge_points_;
};
} // namespace SG
#endif
