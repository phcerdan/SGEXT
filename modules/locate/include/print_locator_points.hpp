/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef PRINT_LOCATOR_POINTS_HPP
#define PRINT_LOCATOR_POINTS_HPP

#include "vtkAbstractPointLocator.h"
#include "vtkIdList.h"
#include "vtkPoints.h"

namespace SG {

/** Print points of vtk locator
 *
 * @param locator input locator with points
 */
void print_locator_points(vtkAbstractPointLocator *pointLocator);

/**
 * Print the positions of all the points
 *
 * @param points
 */
void print_points(vtkPoints *points);

/**
 * Utility function to print point from id.
 *
 * @param id
 * @param points
 */
void print_point_from_id(const vtkIdType id, vtkPoints *points);
void print_point_from_id(const vtkIdType id,
                         vtkAbstractPointLocator *pointLocator);

/**
 * Utility function to print the ids of a list
 * and the points associated to it.
 *
 * @param idList
 * @param points
 */
void print_point_list(vtkIdList *idList, vtkPoints *points);
void print_point_list(vtkIdList *idList, vtkAbstractPointLocator *pointLocator);
} // namespace SG
#endif
