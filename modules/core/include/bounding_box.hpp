/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include "common_types.hpp" // PointType typedef
#include <vector>

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
    PointType GetSize() const;
    PointType GetRadius() const;
    PointType GetCenter() const;
    bool is_point_inside(const PointType &input_point) const;
    bool are_bounds_inside(double *external_bounds) const;
    void Print(double *bounds, const std::string &label = "BoundingBox") const;
    void Print(const std::string &label = "BoundingBox") const;
};

inline bool is_inside(const PointType &input, const BoundingBox &box) {
    return box.is_point_inside(input);
};

} // namespace SG

#endif
