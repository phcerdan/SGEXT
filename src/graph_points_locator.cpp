/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "graph_points_locator.hpp"
#include "vtkPolyData.h"
namespace SG {
vtkSmartPointer<vtkKdTreePointLocator>
build_kdtree_locator(vtkPoints * inputPoints)
{
    auto kdtree = vtkSmartPointer<vtkKdTreePointLocator>::New();
    // Need to build a data set
    auto dataSet = vtkSmartPointer<vtkPolyData>::New();
    dataSet->SetPoints(inputPoints);
    kdtree->SetDataSet(dataSet);
    kdtree->BuildLocator();
    return kdtree;
};
} // ns SG
