/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_NODE_HPP
#define SPATIAL_NODE_HPP
#include "array_utilities.hpp"
#include <boost/serialization/array.hpp>
#include <iostream>
#include <sstream>
#include <string>
namespace SG {

struct SpatialNode {
    /** id of the node, used in some situations, i.e read_graphviz
     * * @sa read_graphviz
     * Don't expect that the node_id in the .dot files are the same ids than
     * in the graph after read. */
    size_t id;
    /// Use Array to store the position.
    using PointType = ArrayUtilities::Array3D;
    /** Position of node. */
    PointType pos;
};

/* Stream operators */
inline static std::ostream &operator<<(std::ostream &os,
                                       const SpatialNode &sn) {
    os.precision(100);
    os << sn.pos[0] << " " << sn.pos[1] << " " << sn.pos[2];
    return os;
}
inline static std::istream &operator>>(std::istream &is, SpatialNode &sn) {
    auto &x = sn.pos[0];
    auto &y = sn.pos[1];
    auto &z = sn.pos[2];
    is >> std::skipws >> x >> y >> z;
    return is;
}

} // end namespace SG

namespace boost {
namespace serialization {
template <class Archive>
void serialize(Archive &ar, SG::SpatialNode &sn, unsigned /*version*/) {
    ar &sn.id;
    ar &sn.pos;
}
} // namespace serialization
} // namespace boost

#endif
