/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VISUALIZE_OBJECT_HPP
#define VISUALIZE_OBJECT_HPP

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <DGtal/helpers/StdDefs.h>

namespace SG {

/**
 * Create an actor to visualize the default 3D DGtal::Object
 *
 * @param obj
 * @param inputOpacity
 *
 * @return
 */
vtkSmartPointer<vtkActor>
create_actor_visualize_object( const DGtal::Z3i::Object26_6 & obj,
    const double inputOpacity = 0.8);

/**
 * Render object
 *
 * @ref create_actor_visualize_object
 *
 * @param obj
 * @param inputOpacity
 */
void
visualize_object( const DGtal::Z3i::Object26_6 & obj,
    const double inputOpacity = 0.8);
} // end namespace SG
#endif
