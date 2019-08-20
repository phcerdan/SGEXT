#ifndef SIMULATEDANNEALING_HPP
#define SIMULATEDANNEALING_HPP

#include "boundary_conditions.hpp" // for boundary_condition
#include "generate_common.hpp"     // for Histogram
#include "simulated_annealing_generator_config_tree.hpp"
#include "simulated_annealing_generator_parameters.hpp"
#include "update_step_move_node.hpp"
#include "update_step_swap_edges.hpp"

namespace SG {
/**
 * simulated_annealing engine to drive the network to the desired state.
 */
class simulated_annealing_generator {

  protected:
    using Self = simulated_annealing_generator;

  public:
    simulated_annealing_generator();
    simulated_annealing_generator(const Self &) = delete;
    Self &operator=(const Self &) = delete;
    simulated_annealing_generator(Self &&) = delete;
    Self &operator=(const Self &&) = delete;
    simulated_annealing_generator(const size_t &num_vertices);
    simulated_annealing_generator(const GraphType &input_graph);
    simulated_annealing_generator(
            const simulated_annealing_generator_config_tree &tree);
    simulated_annealing_generator(const std::string &input_parameters_file);
    void init_parameters();
    void set_parameters_from_file(const std::string &input_file);
    void save_parameters_to_file(const std::string &output_file);
    simulated_annealing_generator_config_tree
    save_parameters_to_configuration_tree();
    void set_parameters_from_configuration_tree(
            const simulated_annealing_generator_config_tree &tree);

    /// Possible transitions after the move occurred updating the
    /// network in simulated_annealing. Used in checkTransition()
    enum class transition {
        REJECTED,          ///< Transition is rejected. Energy/score of current
                           ///< network is higher than before, and no annealing
                           ///< transition.
        ACCEPTED,          ///< Transition is accepted. Energy/score is lower
                           ///< than before.
        ACCEPTED_HIGH_TEMP ///< Transition is accepted. Energy/score is
                           ///< higher than before, but simulated annealing.
    };

    cosine_directors_distribution_parameters cosine_params;
    degree_distribution_parameters degree_params;
    domain_parameters domain_params;
    end_to_end_distances_distribution_parameters ete_distance_params;
    physical_scaling_parameters physical_scaling_params;
    transition_parameters transition_params;

  public:
    GraphType graph_;
    Histogram histo_ete_distances_;
    Histogram histo_cosines_;
    std::vector<double> target_cumulative_distro_histo_ete_distances_;
    std::vector<double> target_cumulative_distro_histo_cosines_;
    // TODO(optimization): update_steps can be a vector to parallelize the
    // update. The condition would be:
    // - The selected randomized nodes/edges do not have neighbors in common.
    //   In the case of move_node, this includes neighbors of all the nodes
    //   connected to the moved node.
    update_step_move_node step_move_node_;
    update_step_swap_edges step_swap_edges_;
    bool verbose = false;

    /**
     * Create a random graph from a degree distribution (@sa
     * degree_distribution_parameters). For randomize the graph, it uses
     * @create_graph_from_degree_sequence
     *
     * @param num_vertices number of vertices of the generated graph.
     */
    void init_graph_degree(const size_t &num_vertices);
    /**
     * Assign a random position (inside the domain_parameters) to each
     * vertex.
     */
    void init_graph_vertex_positions();

    void init_histograms(const size_t &num_bins_ete_distances,
                         const size_t &num_bins_cosines);
    /**
     * Create an histogram with num_bins and uniformly distributed breaks to
     * store the end-to-end distances. The min and max distance are taken
     * from domain_parameters.domain
     *
     * @param num_bins of the histogram. This has an impact
     * on performance when computing cramer-von-mises tests.
     */
    void init_histogram_ete_distances(const size_t &num_bins);
    /**
     * Create an histogram with num_bins and uniformly distributed breaks to
     * store director cosines. The min and max are -1.0 and 1.0
     * respectively.
     *
     * @param num_bins of the histogram. This has an impact
     * on performance when computing cramer-von-mises tests.
     */
    void init_histogram_cosines(const size_t &num_bins);
    /**
     * Reset and populate the end-to-end distances histogram with the
     * current status of the graph.
     */
    void populate_histogram_ete_distances();
    /**
     * Reset and populate the director cosines histogram with the current
     * status of the graph.
     */
    void populate_histogram_cosines();
    /**
     * Populate the target_cumulative_distro_histo_ete_distances_
     * and the LUT_cumulative_histo_ete_distances_.
     *
     * Call this function with a lambda of whatever cumulative distribution
     * function wanted.
     *
     * @param histo_centers
     * @param cumulative_func
     */
    void populate_target_cumulative_distro_histo_ete_distances(
            const std::vector<double> &histo_centers,
            const std::function<double(double)> &cumulative_func);
    /**
     * Populate the target_cumulative_distro_histo_cosines_
     * and the LUT_cumulative_histo_cosines_.
     *
     * Call this function with a lambda of whatever cumulative distribution
     * function wanted.
     *
     * @param histo_centers
     * @param cumulative_func
     */
    void populate_target_cumulative_distro_histo_cosines(
            const std::vector<double> &histo_centers,
            const std::function<double(double)> &cumulative_func);

    /**
     * Performs cramer_von_mises_test in the histograms at the moment of
     * execution. Ensure that you call it when histograms are updated.
     *
     * @sa SG::cramer_von_mises_test
     *
     * @return the result of the test.
     */
    double compute_energy() const;
    /**
     * cramer_von_mises_test for the ete_distances histogram
     * plus @energy_ete_distances_extra_penalty
     *
     * @return value of the von_mises_test + extra penalty
     */
    double energy_ete_distances() const;
    /**
     * Extra penalty factor for long ete_distances.
     * Involves computation of the mean ete_distances.
     *
     * @return cost to avoid long fibers
     */
    double energy_ete_distances_extra_penalty() const;
    /**
     * cramer_von_mises_test for the cosines histogram
     * plus @energy_cosines_extra_penalty
     *
     * @return value of the von_mises_test + extra penalty
     */
    double energy_cosines() const;
    /**
     * Extra penalty to avoid accumulation in the last bin of histo_cosines
     * [0.9x, 1.000]. The approximation of Lindstrom et al to
     * the cramer_von_mises_test involving histogram doesn't impose an important
     * cost value when dealing with the last bin of the histogram.
     *
     * @return extra penalty to avoid many counts in the last bin
     */
    double energy_cosines_extra_penalty() const;

    /**
     * Start the simulation
     * Precondition: all the parameters and histograms are initialized
     *
     * The simulation stops when any of MAX_ENGINE_ITERATIONS,
     * MAX_CONSECUTIVE_FAILURES, or ENERGY_CONVERGENCE criterias are met.
     *
     * As output, the transition parameters are populated with the
     * simulation results and the graph_ is modified to follow the input
     * distributions.
     */
    void engine();
    simulated_annealing_generator::transition check_transition();
    void set_boundary_condition(const ArrayUtilities::boundary_condition &bc);
    void print(std::ostream &os, int spaces = 35) const;
    void print_histo_and_target_distribution(
            std::ostream &os,
            const Histogram &histo,
            const std::vector<double> &distro) const;
    void
    print_histo_and_target_distribution_ete_distances(std::ostream &os) const;
    void print_histo_and_target_distribution_cosines(std::ostream &os) const;

  private:
    /** Used in cramer_von_mises_test computation: Modify it with
     * populate_target_cumulative_distro_histo_ete_distances
     * target_cumulative_distro_histo_distances_ * histo_distances_.bins + 0.5*/
    std::vector<double> LUT_cumulative_histo_ete_distances_;
    /** Used in cramer_von_mises_test computation: Modify it with
     * populate_target_cumulative_distro_histo_cosines
     * target_cumulative_distro_histo_cosines_ * histo_cosines_.bins + 0.5*/
    std::vector<double> LUT_cumulative_histo_cosines_;
    size_t total_counts_ete_distances_ = 0;
    size_t total_counts_cosines_ = 0;
};
} // namespace SG
#endif
