#ifndef UPDATESTEP_HPP
#define UPDATESTEP_HPP

#include "boundary_conditions.hpp"
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
} // namespace SG
#endif
