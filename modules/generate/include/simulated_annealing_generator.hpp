#ifndef SIMULATEDANNEALING_HPP
#define SIMULATEDANNEALING_HPP

#include "update_step_move_node.hpp"
#include "update_step_swap_edges.hpp"

namespace SG {
/**
 * simulated_annealing engine to drive the network to the desired state.
 */
class simulated_annealing_generator {

    // void von_mises_test(Histogram histo) const;
    // void von_mises_test_distances(Histogram histo) const;
    // void von_mises_test_director_cosines(Histogram histo) const;
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
    /// Move occurred updating the network. Last move stored in @sa
    /// last_movement_ .
    enum class move {
        NOMOVE,        ///< No move occurred.
        NODE_MOVED,    ///< A node was moved to another position.
        SWAP_TWO_EDGES ///< Two edges were moved.
    };
    void engine();
    struct transition_parameters {

        double energy_;         ///< Current energy or score.
        double energy_initial_; ///< Energy after network initialization.
        /** accepted transition:ACCEPTED since engine() started. */
        size_t accepted_transitions_;
        /** total transition::REJECTED since engine() started. */
        size_t rejected_transitions_;
        /** total number of transition::ACCEPTED_HIGH_TEMP since engine()
         * started. */
        size_t high_temp_transitions_;
        /** maximum of consecutive failures since engine() started */
        size_t consecutive_failures_;
        /** time elapsed since engine() started. */
        double time_elapsed_;
        /** Temperature before engine() started. Associated  with the annealing
         * process, @sa  transition::ACCEPTED_HIGH_TEMP */
        double temp_initial_;
        /** Current temperature. The temperature decreases with each
         * transition::ACCEPTED_HIGH_TEMP. */
        double temp_actual_;

        // TODO DELETE/UNUSED
        /** number of errors associated with 2 nodes in the same positions since
         * engine() started. */
        // size_t eng_error_zero_count_;
        /** total number of moves that went out of bonds since engine()
         * started. */
        // size_t eng_out_of_bounds_move_;
        /** total number of  transition::ACCEPTED_HIGH_TEMP  since engine()
         * started. */
    } transition_parameters;
};
} // namespace SG
#endif
