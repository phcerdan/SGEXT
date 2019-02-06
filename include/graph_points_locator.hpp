/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GRAPH_POINTS_LOCATOR_HPP
#define GRAPH_POINTS_LOCATOR_HPP

#include "graph_descriptor.hpp"
#include <vtkDataSet.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace SG {

  vtkSmartPointer<vtkKdTreePointLocator> build_kdtree_locator(vtkPoints * inputPoints);

} // ns SG
#endif
