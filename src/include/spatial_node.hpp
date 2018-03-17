#ifndef SPATIAL_NODE_HPP
#define SPATIAL_NODE_HPP
#include "array_utilities.hpp"
#include <string>
#include <iostream>
#include <sstream>
namespace SG {

    struct SpatialNode
    {
        /** Please note that node_id "label" is meaningless, but required for
         * the graphviz reader.
         * @sa read_graphviz
         * I haven't found a way, ie with:
         * static_property_map, ref_property_map or dummy_property_map, to ignore it.
         * Don't expect that the node_id in the .dot files are the same ids than
         * in the graph after read. */
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
