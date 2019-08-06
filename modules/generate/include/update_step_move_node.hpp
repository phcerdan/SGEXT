#ifndef UPDATE_STEP_MOVE_NODE_HPP
#define UPDATE_STEP_MOVE_NODE_HPP

#include "generate_common.hpp" // for Histogram
#include "update_step.hpp"

namespace SG {
class update_step_move_node
        : public update_step_with_distance_and_cosine_histograms {
  protected:
    using parent_class = update_step_with_distance_and_cosine_histograms;

  public:
    // inherit constructors
    using parent_class::parent_class;
    inline void set_input_parameters(const double &max_step_distance) {
        max_step_distance_ = max_step_distance;
    }

    void clear_move_node_parameters(
            PointType &old_node_position,
            PointType &new_node_position,
            GraphType::vertex_descriptor &selected_node) const;

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
        this->undo(*histo_distances_, *histo_cosines_, selected_node_,
                   old_node_position_, new_node_position_, old_distances_,
                   old_cosines_, new_distances_, new_cosines_);
    }

    void randomize(const GraphType &graph,
                   GraphType::vertex_descriptor &selected_node,
                   bool &randomized_flag) const;
    inline void randomize() {
        this->randomize(*graph_, selected_node_, randomized_flag_);
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
            GraphType::vertex_descriptor &selected_node,
            bool &randomized_flag,
            // out parameters
            PointType &old_node_position,
            PointType &new_node_position,
            std::vector<double> &old_distances,
            std::vector<double> &old_cosines,
            std::vector<double> &new_distances,
            std::vector<double> &new_cosines) const;

    inline void perform() override {
        this->perform(max_step_distance_, *graph_, *histo_distances_,
                      *histo_cosines_, selected_node_, randomized_flag_,
                      old_node_position_, new_node_position_, old_distances_,
                      old_cosines_, new_distances_, new_cosines_);
    }

    void update_graph() override {
        if (selected_node_ ==
            std::numeric_limits<decltype(selected_node_)>::max()) {
            throw std::logic_error("update_graph() has to be called after "
                                   "perform(), not before.");
        }
        this->update_graph(*graph_, selected_node_, new_node_position_);
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
    /** Max distance (modulus) allowed the node to move.
     * Modulus is random from [0, max_step_distance_) */
    double max_step_distance_ = 0.1;
    GraphType::vertex_descriptor selected_node_ =
            std::numeric_limits<decltype(selected_node_)>::max();
    PointType old_node_position_;
    PointType new_node_position_;
};
} // namespace SG
#endif
