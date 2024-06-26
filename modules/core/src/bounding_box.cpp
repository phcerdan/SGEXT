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

#include "bounding_box.hpp"
#include <iostream>
#include <limits> // std::numeric_limits

namespace SG {

BoundingBox::BoundingBox(const PointType &input_ini, const PointType &input_end)
        : ini(input_ini), end(input_end){}

BoundingBox::BoundingBox(const PointType &center,
                         const std::array<size_t, 3> &radius,
                         const bool /*unused*/) {
    for (unsigned int i = 0; i < 3; ++i) {
        ini[i] = center[i] - radius[i];
        end[i] = center[i] + radius[i];
    }
}
BoundingBox::BoundingBox(const PointType &center, size_t radius)
        : BoundingBox::BoundingBox(
                  center,
                  std::array<size_t, 3>{{radius, radius, radius}},
                  true){}

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
BoundingBox::BoundingBox(const double bounds[6]) { this->SetBounds(bounds); }

BoundingBox::BoundingBox(double xMin,
                         double xMax,
                         double yMin,
                         double yMax,
                         double zMin,
                         double zMax) {
    this->SetBounds(xMin, xMax, yMin, yMax, zMin, zMax);
}

void BoundingBox::SetBounds(double xMin,
                            double xMax,
                            double yMin,
                            double yMax,
                            double zMin,
                            double zMax) {
    ini = {{xMin, yMin, zMin}};
    end = {{xMax, yMax, zMax}};
}
void BoundingBox::SetBounds(const double b[6]) {
    this->SetBounds(b[0], b[1], b[2], b[3], b[4], b[5]);
}
/**
 * Caller needs to allocate bounds before calling this method (VTK style).
 * \code{cpp}
 * SG::BoundingBox::BoundingBox box(0,1,0,2,0,3);
 * double bounds[6];
 * box.GetBounds(bounds);
 * \endcode
 *
 * @param bounds
 */
void BoundingBox::GetBounds(double *bounds) const {
    bounds[0] = ini[0];
    bounds[1] = end[0];
    bounds[2] = ini[1];
    bounds[3] = end[1];
    bounds[4] = ini[2];
    bounds[5] = end[2];
}

BoundingBox
BoundingBox::BuildEnclosingBox(const std::vector<double *> &bounds_vector) {
    double xMin = std::numeric_limits<double>::max();
    double yMin = std::numeric_limits<double>::max();
    double zMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::lowest();
    double yMax = std::numeric_limits<double>::lowest();
    double zMax = std::numeric_limits<double>::lowest();

    for (const auto &bounds : bounds_vector) {
        xMin = bounds[0] < xMin ? bounds[0] : xMin;
        yMin = bounds[2] < yMin ? bounds[2] : yMin;
        zMin = bounds[4] < zMin ? bounds[4] : zMin;
        xMax = bounds[1] > xMax ? bounds[1] : xMax;
        yMax = bounds[3] > yMax ? bounds[3] : yMax;
        zMax = bounds[5] > zMax ? bounds[5] : zMax;
    }

    return BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
}

BoundingBox BoundingBox::BuildEnclosingBox(
        const std::vector<BoundingBox> &bounding_box_vector) {
    double xMin = std::numeric_limits<double>::max();
    double yMin = std::numeric_limits<double>::max();
    double zMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::lowest();
    double yMax = std::numeric_limits<double>::lowest();
    double zMax = std::numeric_limits<double>::lowest();

    for (const auto &box : bounding_box_vector) {
        xMin = box.ini[0] < xMin ? box.ini[0] : xMin;
        yMin = box.ini[1] < yMin ? box.ini[1] : yMin;
        zMin = box.ini[2] < zMin ? box.ini[2] : zMin;
        xMax = box.end[0] > xMax ? box.end[0] : xMax;
        yMax = box.end[1] > yMax ? box.end[1] : yMax;
        zMax = box.end[2] > zMax ? box.end[2] : zMax;
    }

    return BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
}

PointType BoundingBox::GetSize() const {
    return {{std::abs(end[0] - ini[0]), std::abs(end[1] - ini[1]),
             std::abs(end[2] - ini[2])}};
}

PointType BoundingBox::GetRadius() const {
    auto size = this->GetSize();
    return {{size[0] / 2.0, size[1] / 2.0, size[2] / 2.0}};
}

PointType BoundingBox::GetCenter() const {
    auto radius = this->GetRadius();
    return {{ini[0] + radius[0], ini[1] + radius[1], ini[2] + radius[2]}};
}

bool BoundingBox::is_point_inside(const PointType &input_point) const {
    return input_point[0] >= ini[0] && input_point[0] <= end[0] &&
           input_point[1] >= ini[1] && input_point[1] <= end[1] &&
           input_point[2] >= ini[2] && input_point[2] <= end[2];
}

bool BoundingBox::are_bounds_inside(const double *external_bounds) const {
    return external_bounds[0] >= ini[0] && external_bounds[0] <= end[0] &&
           external_bounds[1] >= ini[0] && external_bounds[1] <= end[0] &&
           external_bounds[2] >= ini[1] && external_bounds[2] <= end[1] &&
           external_bounds[3] >= ini[1] && external_bounds[3] <= end[1] &&
           external_bounds[4] >= ini[2] && external_bounds[4] <= end[2] &&
           external_bounds[5] >= ini[2] && external_bounds[5] <= end[2];
}

bool BoundingBox::are_bounds_inside(const BoundingBox &external_box) const {
    return is_point_inside(external_box.ini) &&
           is_point_inside(external_box.end);
}

void BoundingBox::Print(double *b,
                        const std::string &label,
                        std::ostream &os) const {

    os << label << ": " << std::endl;
    os << " Xmin, Xmax: (" << b[0] << ", " << b[1] << "), " << std::endl;
    os << " Ymin, Ymax: (" << b[2] << ", " << b[3] << "), " << std::endl;
    os << " Zmin, Zmax: (" << b[4] << ", " << b[5] << "), " << std::endl;
}

void BoundingBox::Print(const std::string &label, std::ostream &os, bool one_line) const {
    if(one_line) {
        const bool comma_separated = true;
        const auto label_with_space = !label.empty() ? label + " " : label;
        os << label_with_space << "ini: (" <<
            ArrayUtilities::to_string(ini, comma_separated) << ") | end: (" <<
            ArrayUtilities::to_string(end, comma_separated) << ")" << std::endl;
    } else {
        double bounds[6];
        this->GetBounds(bounds);
        this->Print(bounds, label, os);
    }
}
} // namespace SG
