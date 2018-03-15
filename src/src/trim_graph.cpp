#include "trim_graph.hpp"
#include "spatial_histograms.hpp"
#include "compute_graph_properties.hpp"

namespace SG {

// Iterate over all edges of input_sg
// Keep a std::unordered_map<vertex_descriptor, vertex_descriptor>
// between input and output sg
// if source, target of edge are not are already added, add them and the edge.
// This includes SpatialNode and SpatialEdge.
// TODO
GraphType trim_graph(const GraphType & input_sg) {
    auto histo = histogram_degrees(compute_degrees(input_sg));
    return GraphType(0);
}


} // end namespace SG
