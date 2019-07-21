#ifndef SIMULATEDANNEALING_HPP
#define SIMULATEDANNEALING_HPP

#include "update_step_move_node.hpp"
#include "update_step_swap_edges.hpp"

namespace SG {
/**
 * simulated_annealing engine to drive the network to the desired state.
 */
class simulated_annealing {

    void von_mises_test_distances(Histogram histo) const;
    void von_mises_test_cosines(Histogram histo) const;
    /// Possible transitions after the move occurred updating the
    /// network in simulated_annealing. Used in checkTransition()
    enum class transition {
        REJECTED,          ///< Transition is rejected. Energy/score of current
                           ///< network is higher than before, and no annealing
                           ///< transition.
        ACCEPTED,          ///< Transition is accepted. Energy/score is lower
                           ///< than before.
        ACCEPTED_HIGH_TEMP ///< Transition is accepted. Energy/score is
                           ///< higher than before, but
    };
    /// Move occurred updating the network. Last move stored in @sa
    /// last_movement_ .
    enum class move {
        NOMOVE,    ///< No move occurred.
        NODEMOVED, ///< A node was moved to another position. node_tuple
        TWOEDGES   ///< Two edges were moved. The degree of the nodes must be
                   ///< conserved, so after selecting one edge, only 2
                   ///< possibilities. @sa crossflag_
    };
};
} // namespace SG
#endif
