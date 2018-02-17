#ifndef SPATIAL_NODE_HPP
#define SPATIAL_NODE_HPP
#include "array_utilities.hpp"
#include <string>
#include <iostream>
#include <sstream>
namespace SG {

    struct SpatialNode
    {
        std::string label;
        /// Use Array to store the position.
        using PointType = ArrayUtilities::Array3D;
        /** Position of node. */
        PointType pos;

    };

/* Stream operators */
inline static std::ostream& operator<<(std::ostream& os,
        const SpatialNode& sn) {
    os << ArrayUtilities::to_string(sn.pos);
    return os;
}
inline static std::istream& operator>>(std::istream& is,
        SpatialNode& sn) {
    auto & x = sn.pos[0];
    auto & y = sn.pos[1];
    auto & z = sn.pos[2];
    is >> std::skipws >> x >> y >> z;
    return is;
}

} // end namespace SG
#endif
