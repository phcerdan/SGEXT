#ifndef UPDATESTEP_HPP
#define UPDATESTEP_HPP

#include "boundary_conditions.hpp"
#include "generate_common.hpp" // for Histogram
#include <vector>

namespace SG {

/**
 * Abstract class performing single changes in the graph.
 * For example, moving a node, or swapping two edges.
 * This class only provides an interface, the implementation of the
 * different types of update steps are implemented in derived classes.
 * Also provide classes to manipulate histograms, such as undo, and perform
 */
class update_step {
  public:
    virtual void undo() = 0;
    virtual void perform() = 0;
    virtual void update_graph() = 0;
    ArrayUtilities::boundary_condition boundary_condition =
            ArrayUtilities::boundary_condition::PERIODIC;
};

class update_step_with_distance_and_cosine_histograms : public update_step {
  public:
    update_step_with_distance_and_cosine_histograms() = default;
    update_step_with_distance_and_cosine_histograms(
            GraphType &graph_input,
            Histogram &histo_distances_input,
            Histogram &histo_cosines_input)
            : graph_(&graph_input), histo_distances_(&histo_distances_input),
              histo_cosines_(&histo_cosines_input){};

    void clear_stored_parameters(std::vector<double> &old_distances,
                                 std::vector<double> &old_cosines,
                                 std::vector<double> &new_distances,
                                 std::vector<double> &new_cosines) const;
    void undo(
            // in/out parameters
            Histogram &histo_distances,
            Histogram &histo_cosines,
            std::vector<double> &old_distances,
            std::vector<double> &old_cosines,
            std::vector<double> &new_distances,
            std::vector<double> &new_cosines) const;

    inline void undo() override {
        this->undo(*histo_distances_, *histo_cosines_, old_distances_,
                   old_cosines_, new_distances_, new_cosines_);
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
    void print(std::ostream &os) const;
    GraphType *graph_;
    Histogram *histo_distances_;
    Histogram *histo_cosines_;

    /**
     * Flag indicating that a random node or edge has been selected. perform()
     * would select a random node/edge if this flag is false. Useful if multiple
     * update_steps need to work in parallel and checking that there are not
     * nodes/edges in common between steps.
     */
    bool randomized_flag_ = false;

  protected:
    // Members needed to be stored for undo capabilities
    std::vector<double> old_distances_;
    std::vector<double> old_cosines_;
    std::vector<double> new_distances_;
    std::vector<double> new_cosines_;
};
} // namespace SG
#endif
