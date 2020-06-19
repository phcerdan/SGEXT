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

#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include "common_types.hpp" // PointType typedef
#include <vector>
#include <iostream>

namespace SG {

struct BoundingBox {
    PointType ini = {{0.0, 0.0, 0.0}};
    PointType end = {{1.0, 1.0, 1.0}};

    BoundingBox() = default;
    BoundingBox(const BoundingBox &) = default;
    BoundingBox(BoundingBox &&) = default;
    BoundingBox &operator=(const BoundingBox &) = default;
    BoundingBox &operator=(BoundingBox &&) = default;

    BoundingBox(const PointType &input_ini, const PointType &input_end);
    BoundingBox(const PointType &center,
                const std::array<size_t, 3> &radius,
                const bool use_center_and_radius);
    BoundingBox(const PointType &center, size_t radius);
    /**
     * VTK interface
     * [0] //xmin
     * [1] //xmax
     * [2] //ymin
     * [3] //ymax
     * [4] //zmin
     * [5] //zmax
     * @param bounds[6]
     */
    BoundingBox(const double bounds[6]);
    BoundingBox(double xMin,
                double xMax,
                double yMin,
                double yMax,
                double zMin,
                double zMax);
    void SetBounds(double xMin,
                   double xMax,
                   double yMin,
                   double yMax,
                   double zMin,
                   double zMax);
    void SetBounds(const double b[6]);
    /**
     * Caller needs to allocate bounds before calling this method.
     * SG::BoundingBox box(0,1,0,2,0,3);
     * double bounds[6];
     * box.GetBounds(bounds);
     *
     * @param bounds
     */
    void GetBounds(double *bounds) const;
    static BoundingBox
    BuildEnclosingBox(const std::vector<double *> &bounds_vector);
    static BoundingBox
    BuildEnclosingBox(const std::vector<BoundingBox> &bounding_box_vector);
    PointType GetSize() const;
    PointType GetRadius() const;
    PointType GetCenter() const;
    bool is_point_inside(const PointType &input_point) const;
    /// Check bounds of this box are inside external_bounds/box
    bool are_bounds_inside(double *external_bounds) const;
    bool are_bounds_inside(const BoundingBox & external_box) const;
    void Print(double *bounds,
               const std::string &label = "BoundingBox",
               std::ostream &os = std::cout) const;
    void Print(const std::string &label = "BoundingBox",
               std::ostream &os = std::cout,
               bool one_line = true
               ) const;
};

inline bool is_inside(const PointType &input, const BoundingBox &box) {
    return box.is_point_inside(input);
};

} // namespace SG

#endif
