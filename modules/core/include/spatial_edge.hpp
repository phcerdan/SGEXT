/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_EDGE_HPP
#define SPATIAL_EDGE_HPP

#include "array_utilities.hpp"
#include "common_types.hpp"
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>

namespace SG {
struct SpatialEdge {
    using PointType = SG::PointType;
    using PointContainer = SG::PointContainer;
    /// Spatial Points between the nodes of the edge.
    PointContainer edge_points;
};

/**
 * Print edge points with default precission.
 * Use the << operator on a spatial edge for an uniform precission.
 *
 * print_edge_points(edge_points, std::cout);
 * std::cout << std::endl; // flush to show
 *
 * @param edge_points
 * @param os any ostream
 */
inline void print_edge_points(const PointContainer &edge_points,
                              std::ostream &os) {
    auto size = edge_points.size();
    os << "[";
    for (size_t i = 0; i + 1 < size; ++i) {
        os << "{" << edge_points[i][0] << " " << edge_points[i][1] << " "
           << edge_points[i][2] << "},";
    }
    if (size > 0) {
        os << "{" << edge_points[size - 1][0] << " " << edge_points[size - 1][1]
           << " " << edge_points[size - 1][2] << "}";
    }
    os << "]";
}
/* Stream operators */
inline static std::ostream &operator<<(std::ostream &os,
                                       const SpatialEdge &se) {
    os.precision(100);
    print_edge_points(se.edge_points, os);
    return os;
}
inline static std::istream &operator>>(std::istream &is, SpatialEdge &se) {
    auto &edge_points = se.edge_points;

    std::string s(std::istreambuf_iterator<char>(is), {});
    std::string delim_start = "{";
    std::string delim_end = "}";
    auto first = s.find(delim_start);
    auto last = s.find(delim_end);
    auto pos = first;
    std::string clean;
    while (pos != std::string::npos) {
        double x = 0;
        double y = 0;
        double z = 0;
        clean = s.substr(pos + delim_start.length(), last - delim_end.length());
        std::istringstream is_clean(clean);
        is_clean >> x >> y >> z;
        edge_points.push_back({{x, y, z}});
        s.erase(0, last + delim_end.length());
        pos = s.find(delim_start);
    }
    // set is to the end or lexical_cast fails.
    is.seekg(0, is.end);
    return is;
}
} // namespace SG

namespace boost {
namespace serialization {
template <class Archive>
void serialize(Archive &ar, SG::SpatialEdge &se, unsigned /*version*/) {
    ar &se.edge_points;
}
} // namespace serialization
} // namespace boost
#endif
