#ifndef UPDATESTEP_MOVENODE_HPP
#define UPDATESTEP_MOVENODE_HPP

#include "array_utilities.hpp"
#include "boundary_conditions.hpp"
#include "generate_common.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "spatial_node.hpp"
#include "update_step.hpp"
#include <iostream>

namespace SG {
class update_step_move_node : public update_step {
  public:
    update_step_move_node() = delete;
    update_step_move_node(GraphType &graph_input,
                          Histogram &histo_distances_input,
                          Histogram &histo_cosines_input)
            : graph_(graph_input), histo_distances_(histo_distances_input),
              histo_cosines_(histo_cosines_input){};

    inline void set_input_parameters(const double &max_step_distance) {
        max_step_distance_ = max_step_distance;
    }

    void clear_stored_parameters(PointType &old_node_position,
                                 PointType &new_node_position,
                                 GraphType::vertex_descriptor &selected_node,
                                 std::vector<double> &old_distances,
                                 std::vector<double> &old_cosines,
                                 std::vector<double> &new_distances,
                                 std::vector<double> &new_cosines) const;
    void undo(
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
            std::vector<double> &new_cosines) const;
    inline void undo() override {
        this->undo(histo_distances_, histo_cosines_, selected_node_,
                   old_node_position_, new_node_position_, old_distances_,
                   old_cosines_, new_distances_, new_cosines_);
    }

    /**
     * Select a random node, perform a random movement in the node position, and
     * update histograms taking into account the change of position.
     * Note that the graph is not updated, in case the movement has to be
     * undone with undo()
     *
     * @param max_step_distance
     * @param graph
     * @param histo_distances
     * @param histo_cosines
     * @param selected_node
     * @param old_node_position
     * @param new_node_position
     * @param old_distances
     * @param old_cosines
     * @param new_distances
     * @param new_cosines
     */
    void perform(
            // in parameters
            const double &max_step_distance,
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
            std::vector<double> &new_cosines) const;

    inline void perform() override {
        this->perform(max_step_distance_, graph_, histo_distances_,
                      histo_cosines_, selected_node_, old_node_position_,
                      new_node_position_, old_distances_, old_cosines_,
                      new_distances_, new_cosines_);
    }
    /**
     * Remove old distances and add new from histogram.
     *
     * @param histo_distances
     * @param old_distances
     * @param new_distances
     */
    void
    update_distances_histogram(Histogram &histo_distances,
                               const std::vector<double> &old_distances,
                               const std::vector<double> &new_distances) const;
    /**
     * Remove old cosines and add new ones from histogram.
     *
     * @param histo_cosines
     * @param old_cosines
     * @param new_cosines
     */
    void update_cosines_histogram(Histogram &histo_cosines,
                                  const std::vector<double> &old_cosines,
                                  const std::vector<double> &new_cosines) const;

    void update_graph() override {
        this->update_graph(graph_, selected_node_, new_node_position_);
    };
    /**
     * Update the SpatialNode.pos of the selected node to the new_node_position.
     *
     * @param graph
     * @param node_id
     * @param new_node_position
     */
    void update_graph(GraphType &graph,
                      const GraphType::vertex_descriptor &node_id,
                      const PointType &new_node_position) const;
    GraphType &graph_;
    Histogram &histo_distances_;
    Histogram &histo_cosines_;
    double max_step_distance_ = 1.0;
    GraphType::vertex_descriptor selected_node_;
    PointType old_node_position_;
    PointType new_node_position_;

  protected:
    // Members needed to be stored for undo capabilities
    std::vector<double> old_distances_;
    std::vector<double> old_cosines_;
    std::vector<double> new_distances_;
    std::vector<double> new_cosines_;
};
} // namespace SG
#endif
