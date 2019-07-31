#ifndef SIMULATEDANNEALING_HPP
#define SIMULATEDANNEALING_HPP

#include "boundary_conditions.hpp" // for boundary_condition
#include "generate_common.hpp"     // for Histogram
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
    simulated_annealing_generator()
            : step_move_node_(graph_, histo_ete_distances_, histo_cosines_),
              step_swap_edges_(graph_, histo_ete_distances_, histo_cosines_) {}
    simulated_annealing_generator(const Self &) = delete;
    Self &operator=(const Self &) = delete;
    simulated_annealing_generator(Self &&) = delete;
    Self &operator=(const Self &&) = delete;
    simulated_annealing_generator(const size_t &num_vertices);
    simulated_annealing_generator(const GraphType &input_graph);
    void set_default_parameters();

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
    struct transition_parameters {

        double energy = 0.0;         ///< Current energy or score.
        double energy_initial = 0.0; ///< Energy after network initialization.
        /** accepted transition:ACCEPTED since engine() started. */
        size_t accepted_transitions = 0;
        /** total transition::REJECTED since engine() started. */
        size_t rejected_transitions = 0;
        /** total number of transition::ACCEPTED_HIGH_TEMP since engine()
         * started. */
        size_t high_temp_transitions = 0;
        /** maximum of consecutive failures since engine() started */
        size_t consecutive_failures = 0;
        /** total number of failures in the simulation */
        size_t total_failures = 0;
        /** time elapsed since engine() started. */
        double time_elapsed = 0.0;
        /** Temperature before engine() started. Associated  with the annealing
         * process, @sa  transition::ACCEPTED_HIGH_TEMP.*/
        double temp_initial = 0.0;
        /** Current temperature. The temperature decreases with each
         * transition::ACCEPTED_HIGH_TEMP. */
        double temp_current = 0.0;
        /** Not energitically favourable transitions are less probable over
         * time. The analogy is that the system cools down.  */
        double temp_cooling_rate = 1.0 - 0.5e-04;

        /** Max consecutive failures allowed before engine() stops.*/
        size_t MAX_CONSECUTIVE_FAILURES = 100000000;
        /** Max iterations of engine() if convergence has not been achieved.*/
        // size_t MAX_ENGINE_ITERATIONS = 100000000;
        size_t MAX_ENGINE_ITERATIONS = 100000;
        /** Energy necessary to consider that convergence has been achieved,
         * highly tuneable.*/
        double ENERGY_CONVERGENCE = 0.01;
        /** Chances that the update step is of type update_step_move_node.
         * The other method is update_step_swap_edges.*/
        double UPDATE_STEP_MOVE_NODE_PROBABILITY = 0.5;
        double update_step_move_node_max_step_distance = 5.0e-2;
        inline void print(std::ostream &os, int spaces = 30) {
            os << "%/************TRANSITION "
                  "PARAMETERS*****************/"
               << '\n'
               << '\n'
               << std::left << std::setw(spaces) << "E= " << energy << '\n'
               << std::left << std::setw(spaces)
               << "E_initial= " << energy_initial << '\n'
               << std::left << std::setw(spaces)
               << "Energy_reduction %= " << 1. - energy / energy_initial << '\n'
               << std::left << std::setw(spaces)
               << "time_elapsed=" << time_elapsed << '\n'
               << std::left << std::setw(spaces)
               << "accepted_transitions= " << accepted_transitions << '\n'
               << std::left << std::setw(spaces)
               << "high_temp_transitions= " << high_temp_transitions
               << "  %HighTempTransitions: "
               << static_cast<double>(high_temp_transitions) /
                            accepted_transitions
               << '\n'
               << std::left << std::setw(spaces)
               << "total_failures= " << total_failures << '\n'
               << std::left << std::setw(spaces)
               << "consecutive_failures= " << consecutive_failures << '\n'
               << std::left << std::setw(spaces)
               << "temp_initial= " << temp_initial << '\n'
               << std::left << std::setw(spaces)
               << "temp_final= " << temp_current << '\n'
               << std::left << std::setw(spaces)
               << "temp_cooling_rate= " << temp_cooling_rate << '\n'
               << std::left << std::setw(spaces)
               << "update_step_move_node_max_step= "
               << update_step_move_node_max_step_distance << '\n';
        }
    } transition_parameters;

    struct degree_distribution_parameters {
        double mean = 3.379692;
        size_t min_degree = 3;
        size_t max_degree = 999;
    } degree_distribution_parameters;

    struct end_to_end_distances_distribution_parameters {
        /** Experimental values, not scaled with the simulation box */
        double physical_normal_mean = 1.96e-6;
        double physical_normal_std_deviation;
        double normalized_normal_mean;
        /** v = physical_normal_std_deviation^2 * S^(2/3), where S is the scale
         * factor. (n/physical_node_density)^1/3 */
        double normalized_normal_std_deviation = 0.253;
        double normalized_log_std_deviation;
        double normalized_log_mean;

        inline void set_normalized_log_std_deviation(
                const double &input_normalized_normal_mean,
                const double &input_normalized_normal_std_deviation) {
            normalized_log_std_deviation =
                    sqrt(log(input_normalized_normal_std_deviation *
                                     input_normalized_normal_std_deviation /
                                     (input_normalized_normal_mean *
                                      input_normalized_normal_mean) +
                             1));
        };
        inline void set_normalized_log_mean(
                const double &input_normalized_normal_mean,
                const double &input_normalized_log_std_deviation) {
            normalized_log_mean = log(input_normalized_normal_mean) -
                                  input_normalized_log_std_deviation *
                                          input_normalized_log_std_deviation /
                                          2.0;
        };
    } end_to_end_distances_distribution_parameters;

    struct cosine_directors_distribution_parameters {
        double b1 = 0.6232085;
        double b2 = 0.01684390;
        // b3 = -(3/32.) * (-1 + 2*b1 + 4*b2)
        double b3 = -0.029418056250000008;
    } cosine_directors_distribution_parameters;

    struct domain_parameters {
        ArrayUtilities::boundary_condition boundary_condition =
                ArrayUtilities::boundary_condition::PERIODIC;
        // Assumes x0, y0, z0 = 0.0
        std::array<double, 3> domain = {1.0, 1.0, 1.0};
    } domain_parameters;

    struct physical_scaling_parameters {
        /** nodes per unit of volume measured experimentally */
        double node_density = 0.066627;
        /** S = (num_vertices/node_density)^1/3 */
        double length_scaling_factor;
    } physical_scaling_parameters;

  public:
    GraphType graph_;
    Histogram histo_ete_distances_;
    Histogram histo_cosines_;
    std::vector<double> target_cumulative_distro_histo_ete_distances_;
    std::vector<double> target_cumulative_distro_histo_cosines_;
    // TODO: update_steps can be a vector to parallelize the update.
    // The only condition would be the selected randomized nodes/edges do
    // not have neighbors in common.
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
    void populate_target_cumulative_distro_histo_ete_distances(
            const std::vector<double> &histo_centers);
    void populate_target_cumulative_distro_histo_cosines(
            const std::vector<double> &histo_centers);
    void engine();

    /**
     * Performs cramer_von_mises_test in the histograms at the moment of
     * execution. Ensure that you call it when histograms are updated.
     *
     * @sa SG::cramer_von_mises_test
     *
     * @return the result of the test.
     */
    double compute_energy() const;
    double energy_ete_distances() const;
    double energy_cosines() const;
    simulated_annealing_generator::transition check_transition();
    void print(std::ostream &os, int spaces = 30);
};
} // namespace SG
#endif
