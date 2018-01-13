#ifndef SPATIAL_EDGE_HPP
#define SPATIAL_EDGE_HPP

#include "array_utilities.hpp"

namespace SG {

using Point = ArrayUtilities::Array3D;
using PointContainer = std::vector<Point>;

struct SpatialEdge
{
    using Array3D = ArrayUtilities::Array3D;
    using Point = Array3D;
    using PointContainer = std::vector<Point>;
    /**  Distance between end to end (ete) of edges.
     * @warning This is different to edge length --or fibril length. */
    double ete_distance;

    /** Vector between the two ends of the edge, it can return both directions.
     * ete_vector_ is used to calculate director cosines(angles) between edges.
     * To calculate angles you must be sure that you compute the dot product of vector that
     * POINTS OUT of the node.
     * To change the direction from point-in to point-out just multiply the vector by the scalar -1.
     * @warning  It has a direction, settled when SpatialEdge was constructed. But it is useless by itself because source(edgeID) and target(edgeID)
     * could returns switched values the next time you call them (unordered pair).*/
    Array3D ete_vector;
    /// Spatial Points between the nodes of the edge.
    PointContainer edge_points;
};
} // end namespace
#endif
