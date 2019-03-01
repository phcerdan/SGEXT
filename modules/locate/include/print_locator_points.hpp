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
