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
