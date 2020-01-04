/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

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
