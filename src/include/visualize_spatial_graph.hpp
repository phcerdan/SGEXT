#ifndef VISUALIZE_SPATIAL_GRAPH_HPP
#define VISUALIZE_SPATIAL_GRAPH_HPP
#include "spatial_graph.hpp"

namespace SG {

/**
 * Visualize sg with a vtk graph layout view.
 *
 * @param sg
 */
void visualize_spatial_graph(const GraphType & sg);

} // namespace SG
#endif
