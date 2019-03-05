/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "print_locator_points.hpp"
#include "vtkDataSet.h"
#include <iostream>
#include <assert.h>

namespace SG {

void print_locator_points(vtkAbstractPointLocator * locator)
{
    auto num_points = locator->GetDataSet()->GetNumberOfPoints();
    if(!num_points) {
        std::cout << "point locator is EMPTY" << std::endl;
        return;
    }

    for(vtkIdType index = 0; index < num_points; ++index){
        auto a_point = locator->GetDataSet()->GetPoint(index);
        std::cout << index << ": ";
        std::cout << "(" << a_point[0] << ", " << a_point[1] << ", " << a_point[2] << ")";
        std::cout << std::endl;
    }
}

void print_points(vtkPoints * points)
{
    size_t num_points = points->GetNumberOfPoints();
    if(!num_points) {
        std::cout << "points are EMPTY" << std::endl;
        return;
    }
    for (auto pointIndex = 0; pointIndex < num_points; ++pointIndex) {
        auto a_point = points->GetPoint(pointIndex);
        std::cout << pointIndex << ": ";
        std::cout << "(" << a_point[0] << ", " << a_point[1] << ", " << a_point[2] << ")";
        std::cout << std::endl;
    }
}

void print_point_from_id(const vtkIdType id, vtkPoints * points)
{
    assert(id < points->GetNumberOfPoints());
    auto a_point = points->GetPoint(id);
    std::cout << id << ": ";
    std::cout << "(" << a_point[0] << ", " << a_point[1] << ", " << a_point[2] << ")";
    std::cout << std::endl;
}

void print_point_list(vtkIdList* idList, vtkPoints * points)
{
    auto num_ids = idList->GetNumberOfIds();
    if(!num_ids) {
        std::cout << "point list is EMPTY" << std::endl;
        return;
    }

    for (auto idIndex = 0; idIndex < num_ids; ++idIndex) {
        print_point_from_id(idList->GetId(idIndex), points);
    }
}

} //end ns SG

