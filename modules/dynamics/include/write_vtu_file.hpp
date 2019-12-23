/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
