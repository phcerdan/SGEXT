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

#ifndef SG_WRITE_VTU_FILE_HPP
#define SG_WRITE_VTU_FILE_HPP

#ifdef SG_USING_VTK
#include "system.hpp"
#include <vtkUnstructuredGrid.h>

namespace SG {
/**
 * Write a .vtu file (vtkUnstructuredGrid) with the current state of the system.
 *
 * @param sys
 * @param file_name
 */
void write_vtu_file(const System &sys, const std::string &file_name);
/**
 * Populate system from a vtu file (vtkUnstructuredGrid).
 *
 * @param file_name path to .vtu file
 *
 * @sa read_vtu_point_data read_vtu_bond_ids
 *
 * @return
 */
std::unique_ptr<System> read_vtu_file(const std::string &file_name);
/**
 * Populate particles in the system, including dynamics (velocity, acceleration)
 * and material properties
 *
 * @param ugrid
 * @param sys
 * 
 * @sa read_vtu_file
 */
void read_vtu_point_data(vtkUnstructuredGrid *ugrid, System *sys);
/**
 * Read the cells (vtkLines) and modify sys to hold bonds of the base type Bond
 * with the bonds ids information.
 *
 * @param ugrid
 * @param sys
 *
 * @sa read_vtu_file
 */
void read_vtu_bond_ids(vtkUnstructuredGrid *ugrid, System *sys);

/**
 * Bonds of sys are of changed to be type BondChain to hold the contour_length
 * information
 *
 * @param ugrid
 * @param sys
 *
 * @sa read_vtu_file
 */
void read_vtu_bond_contour_length(vtkUnstructuredGrid *ugrid, System *sys);
} // namespace SG
#endif // SG_USING_VTK
#endif // header guard
