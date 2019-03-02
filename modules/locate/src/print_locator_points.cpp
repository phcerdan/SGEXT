/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "print_locator_points.hpp"
#include "vtkDataSet.h"
#include <iostream>

namespace SG {
void print_locator_points(vtkAbstractPointLocator * locator)
{
    auto npoints = locator->GetDataSet()->GetNumberOfPoints();
    for(vtkIdType index = 0; index < npoints; ++index){
        auto point = locator->GetDataSet()->GetPoint(index);
        std::cout << index << ": " << point[0] << ", " << point[1] << " , " << point[2] << std::endl;
    }
};
} //end ns SG

