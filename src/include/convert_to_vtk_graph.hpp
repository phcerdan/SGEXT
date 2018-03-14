#ifndef CONVERT_TO_VTK_GRAPH_HPP
#define CONVERT_TO_VTK_GRAPH_HPP
#include "spatial_graph.hpp"
#include <vtkSmartPointer.h>
#include <vtkMutableUndirectedGraph.h>

namespace SG {

/**
 * Convert spatial graph to a vtk graph. Used for visualization.
 *
 * @param sg input spatial graph
 *
 * @return vtkGraph
 */
vtkSmartPointer<vtkMutableUndirectedGraph> convert_to_vtk_graph(
        const GraphType & sg);
} // end namespace
#endif
