/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef SIMULATED_ANNEALING_GENERATOR_PARAMETERS_HPP
#define SIMULATED_ANNEALING_GENERATOR_PARAMETERS_HPP

#include "array_utilities.hpp"
#include "boundary_conditions.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>

namespace SG {

struct domain_parameters {
    ArrayUtilities::boundary_condition boundary_condition =
            ArrayUtilities::boundary_condition::PERIODIC;
    // Assumes x0, y0, z0 = 0.0
    std::array<double, 3> domain = {1.0, 1.0, 1.0};
};

struct physical_scaling_parameters {
    /** Number of vertices of the graph. */
    size_t num_vertices = 1000;
    /** nodes per unit of volume measured experimentally */
    double node_density = 0.066627e18;
    /** S = (num_vertices/node_density)^1/3 */
    double length_scaling_factor = 0.0;
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
    double temp_cooling_rate = 1.0 - 0.5e-03;

    /** Max consecutive failures allowed before engine() stops.*/
    size_t MAX_CONSECUTIVE_FAILURES = 100000000;
    /** Max iterations of engine() if convergence has not been achieved.*/
    size_t MAX_ENGINE_ITERATIONS = 100000000;
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
           << std::left << std::setw(spaces) << "E_initial= " << energy_initial
           << '\n'
           << std::left << std::setw(spaces)
           << "Energy_reduction %= " << 1. - energy / energy_initial << '\n'
           << std::left << std::setw(spaces) << "time_elapsed=" << time_elapsed
           << '\n'
           << std::left << std::setw(spaces)
           << "accepted_transitions= " << accepted_transitions << '\n'
           << std::left << std::setw(spaces)
           << "high_temp_transitions= " << high_temp_transitions
           << "  %HighTempTransitions: "
           << static_cast<double>(high_temp_transitions) / accepted_transitions
           << '\n'
           << std::left << std::setw(spaces)
           << "total_failures= " << total_failures << '\n'
           << std::left << std::setw(spaces)
           << "consecutive_failures= " << consecutive_failures << '\n'
           << std::left << std::setw(spaces) << "temp_initial= " << temp_initial
           << '\n'
           << std::left << std::setw(spaces) << "temp_final= " << temp_current
           << '\n'
           << std::left << std::setw(spaces)
           << "temp_cooling_rate= " << temp_cooling_rate << '\n'
           << std::left << std::setw(spaces)
           << "update_step_move_node_max_step= "
           << update_step_move_node_max_step_distance << '\n';
    }
};

struct degree_distribution_parameters {
    double mean = 3.379692;
    size_t min_degree = 3;
    size_t max_degree = 999;
};

struct end_to_end_distances_distribution_parameters {

    /** Experimental values, not scaled with the simulation box */
    double physical_normal_mean = 1.96e-6;
    double physical_normal_std_deviation = 0.0;
    double normalized_normal_mean = 0.0;
    /** v = physical_normal_std_deviation^2 * S^(2/3), where S is the scale
     * factor. (n/physical_node_density)^1/3 */
    double normalized_normal_std_deviation = 0.253;
    double normalized_log_std_deviation = 0.0;
    double normalized_log_mean = 0.0;
    size_t num_bins = 100;

    inline void set_normalized_log_std_deviation(
            const double &input_normalized_normal_mean,
            const double &input_normalized_normal_std_deviation) {
        normalized_log_std_deviation =
                sqrt(log(input_normalized_normal_std_deviation *
                                 input_normalized_normal_std_deviation /
                                 (input_normalized_normal_mean *
                                  input_normalized_normal_mean) +
                         1));
    }
    inline void
    set_normalized_log_mean(const double &input_normalized_normal_mean,
                            const double &input_normalized_log_std_deviation) {
        normalized_log_mean = log(input_normalized_normal_mean) -
                              input_normalized_log_std_deviation *
                                      input_normalized_log_std_deviation / 2.0;
    }
};

struct cosine_directors_distribution_parameters {
    double b1 = 0.62;
    double b2 = -0.1025;
    // b3 = -(3/32.) * (-1 + 2*b1 + 4*b2)
    double b3 = 0.0159375;
    size_t num_bins = 100;
};
} // end namespace SG

#endif
