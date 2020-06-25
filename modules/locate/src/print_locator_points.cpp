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

#include "print_locator_points.hpp"
#include "vtkDataSet.h"
#include <assert.h>
#include <iostream>

namespace SG {

void print_locator_points(vtkAbstractPointLocator *locator) {
    auto num_points = locator->GetDataSet()->GetNumberOfPoints();
    if (!num_points) {
        std::cout << "point locator is EMPTY" << std::endl;
        return;
    }

    for (vtkIdType index = 0; index < num_points; ++index) {
        auto a_point = locator->GetDataSet()->GetPoint(index);
        std::cout << index << ": ";
        std::cout << "(" << a_point[0] << ", " << a_point[1] << ", "
                  << a_point[2] << ")";
        std::cout << std::endl;
    }
}

void print_points(vtkPoints *points) {
    size_t num_points = points->GetNumberOfPoints();
    if (!num_points) {
        std::cout << "points are EMPTY" << std::endl;
        return;
    }
    for (size_t pointIndex = 0; pointIndex < num_points; ++pointIndex) {
        auto a_point = points->GetPoint(pointIndex);
        std::cout << pointIndex << ": ";
        std::cout << "(" << a_point[0] << ", " << a_point[1] << ", "
                  << a_point[2] << ")";
        std::cout << std::endl;
    }
}

void print_point_from_id(const vtkIdType id, vtkPoints *points) {
    assert(id < points->GetNumberOfPoints());
    auto a_point = points->GetPoint(id);
    std::cout << id << ": ";
    std::cout << "(" << a_point[0] << ", " << a_point[1] << ", " << a_point[2]
              << ")";
    std::cout << std::endl;
}

void print_point_from_id(const vtkIdType id,
                         vtkAbstractPointLocator *pointLocator) {
    auto a_point = pointLocator->GetDataSet()->GetPoint(id);
    std::cout << id << ": ";
    std::cout << "(" << a_point[0] << ", " << a_point[1] << ", " << a_point[2]
              << ")";
    std::cout << std::endl;
}

void print_point_list(vtkIdList *idList, vtkPoints *points) {
    auto num_ids = idList->GetNumberOfIds();
    if (!num_ids) {
        std::cout << "point list is EMPTY" << std::endl;
        return;
    }

    for (auto idIndex = 0; idIndex < num_ids; ++idIndex) {
        print_point_from_id(idList->GetId(idIndex), points);
    }
}

void print_point_list(vtkIdList *idList,
                      vtkAbstractPointLocator *pointLocator) {
    auto num_ids = idList->GetNumberOfIds();
    if (!num_ids) {
        std::cout << "point list is EMPTY" << std::endl;
        return;
    }

    for (auto idIndex = 0; idIndex < num_ids; ++idIndex) {
        print_point_from_id(idList->GetId(idIndex), pointLocator);
    }
}

} // namespace SG
