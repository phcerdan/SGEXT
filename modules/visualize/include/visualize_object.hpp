/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VISUALIZE_OBJECT_HPP
#define VISUALIZE_OBJECT_HPP

#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPoints.h>
#include <DGtal/helpers/StdDefs.h>

namespace SG {

vtkSmartPointer<vtkRenderWindowInteractor>
visualize_points_as_cubes( /* const */ vtkPoints * points,
    const double inputOpacity);

vtkSmartPointer<vtkRenderWindowInteractor>
visualize_object( const DGtal::Z3i::Object26_6 & obj,
    const double inputOpacity = 0.8);

} // end namespace SG
#endif
