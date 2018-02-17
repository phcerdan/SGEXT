#ifndef SPATIAL_GRAPH_HPP
#define SPATIAL_GRAPH_HPP
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include "spatial_node.hpp"
#include "spatial_edge.hpp"

namespace SG {
    using GraphAL = boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, SpatialNode, SpatialEdge>;

} // end namespace
#endif
