#ifndef UPDATE_STEP_SWAP_EDGES_HPP
#define UPDATE_STEP_SWAP_EDGES_HPP

#include "generate_common.hpp" // for Histogram
#include "update_step.hpp"
#include <utility> // for std::pair

namespace SG {
class update_step_swap_edges
        : public update_step_with_distance_and_cosine_histograms {
  protected:
    using parent_class = update_step_with_distance_and_cosine_histograms;

  public:
    using parent_class::parent_class;
    update_step_swap_edges(GraphType &graph_input,
                           Histogram &histo_distances_input,
                           Histogram &histo_cosines_input)
            : parent_class(
                      graph_input, histo_distances_input, histo_cosines_input) {
        this->clear_selected_edges(selected_edges_, new_edges_);
    }

    using vertex_descriptor = GraphType::vertex_descriptor;
    using edge_descriptor = GraphType::edge_descriptor;
    using edge_descriptor_pair = std::pair<edge_descriptor, edge_descriptor>;

    void clear_selected_edges(edge_descriptor_pair &selected_edges,
                              edge_descriptor_pair &new_edges) const;
    void undo(
            // in/out parameters
            Histogram &histo_distances,
            Histogram &histo_cosines,
            // out parameters
            edge_descriptor_pair &selected_edges,
            edge_descriptor_pair &new_edges,
            std::vector<double> &old_distances,
            std::vector<double> &old_cosines,
            std::vector<double> &new_distances,
            std::vector<double> &new_cosines) const;

    inline void undo() override {
        this->undo(*histo_distances_, *histo_cosines_, selected_edges_,
                   new_edges_, old_distances_, old_cosines_, new_distances_,
                   new_cosines_);
    }
    void randomize(const GraphType &graph,
                   edge_descriptor_pair &selected_edges,
                   bool &randomized_flag) const;
    inline void randomize() {
        this->randomize(*graph_, selected_edges_, randomized_flag_);
    }

    void perform(
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
            std::vector<double> &new_cosines) const;

    inline void perform() override {
        this->perform(*graph_, *histo_distances_, *histo_cosines_,
                      selected_edges_, randomized_flag_, is_swap_parallel_,
                      new_edges_, old_distances_, old_cosines_, new_distances_,
                      new_cosines_);
    }
    void update_graph() override {
        if (selected_edges_.first.m_source ==
                    std::numeric_limits<vertex_descriptor>::max() ||
            selected_edges_.first.m_target ==
                    std::numeric_limits<vertex_descriptor>::max() ||
            selected_edges_.second.m_source ==
                    std::numeric_limits<vertex_descriptor>::max() ||
            selected_edges_.second.m_target ==
                    std::numeric_limits<vertex_descriptor>::max()) {
            throw std::logic_error("update_graph() has to be called after "
                                   "perform(), not before.");
        }
        this->update_graph(*graph_, selected_edges_, is_swap_parallel_);
    };
    /**
     * Update Graph, given the selected edges and if the swap is parallel. If
     * the swap is not paralell, it is crossed.
     *
     * @param graph
     * @param selected_edges
     * @param is_swap_parallel
     */
    void update_graph(GraphType &graph,
                      const edge_descriptor_pair &selected_edges,
                      const bool &is_swap_parallel) const;

    edge_descriptor_pair selected_edges_;
    edge_descriptor_pair new_edges_;
    bool is_swap_parallel_;

  public:
    /**
     * Returns two edge descriptor that are valid for swapping
     *
     * It might throw if the edges of the graph are not swapable.
     * For example if all of them are adjacent.
     *
     * @param graph
     * @param recursive_count
     *
     * @return
     */
    std::pair<edge_descriptor, edge_descriptor>
    select_two_valid_edges(const GraphType &graph,
                           int recursive_count = 0) const;

    /**
     * Depending on the boolean is_swap_parallel returns the source and targets
     * of the new edges.
     * The return order is:
     * {new_edge1_source, new_edge1_target, new_edge2_source, new_edge2_target}
     *
     *
     *
        S1 --- T1  |  S1       S1-\
                   |  |    or      .
                   |  S2            \_T2
        ---------------------------------------------
        S2 --- T2  |  T1            /-T1
                   |  |    or      .
                   |  T2       S2_/
     * @param is_swap_parallel
     * @param edge1
     * @param edge2
     *
     * @return
     */
    std::array<GraphType::vertex_descriptor, 4>
    get_sources_and_targets_of_new_edges(const bool &is_swap_parallel,
                                         const edge_descriptor &edge1,
                                         const edge_descriptor &edge2) const;
    /**
     * Depending on the boolean is_swap_parallel returns the positions of the
     * vertices of the new edges. The return order is: {new_edge1_source_pos,
     * new_edge1_target_pos, new_edge2_source_pos, new_edge2_target_pos}
     *
     * @param is_swap_parallel
     * @param edge1
     * @param edge2
     * @param graph
     *
     * @return
     */
    std::array<PointType, 4>
    get_positions_of_new_edges(const bool &is_swap_parallel,
                               const edge_descriptor &edge1,
                               const edge_descriptor &edge2,
                               const GraphType &graph) const;

}; // namespace SG
} // namespace SG
#endif
