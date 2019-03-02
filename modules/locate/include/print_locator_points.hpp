/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef PRINT_LOCATOR_POINTS_HPP
#define PRINT_LOCATOR_POINTS_HPP

#include "vtkAbstractPointLocator.h"

namespace SG {

/** Print points of vtk locator
 *
 * @param locator input locator with points
 */
void print_locator_points(vtkAbstractPointLocator * locator);
} // end ns SG
#endif
