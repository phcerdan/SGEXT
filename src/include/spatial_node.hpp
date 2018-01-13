#ifndef SPATIAL_NODE_HPP
#define SPATIAL_NODE_HPP
#include "array_utilities.hpp"
namespace SG {
    struct SpatialNode
    {
        /// Use Array to store the position.
        using Point = ArrayUtilities::Array3D;
        /** Position of node. */
        Point pos;
    };
} // end namespace
#endif
