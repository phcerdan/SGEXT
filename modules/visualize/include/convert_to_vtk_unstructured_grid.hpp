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

#ifndef SG_CONVERT_TO_VTK_UNSTRUCTURED_GRID_HPP
#define SG_CONVERT_TO_VTK_UNSTRUCTURED_GRID_HPP

#include "spatial_graph.hpp"
#include <unordered_map>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace SG {
/**
 * Convert spatial graph to a vtk unstructured grid with vtkPolyLine cells
 * representing the bonds.
 *
 * @param sg input spatial graph
 *
 * @return vtkUnstructuredGrid
 */
vtkSmartPointer<vtkUnstructuredGrid>
convert_to_vtk_unstructured_grid(const GraphType &sg);

vtkSmartPointer<vtkUnstructuredGrid>
convert_to_vtk_unstructured_grid_with_edge_points(const GraphType &sg);

void write_vertices_to_vtk_unstructured_grid(
        const GraphType &sg,
        vtkUnstructuredGrid *ugrid,
        vtkPoints *vtk_points,
        std::unordered_map<boost::graph_traits<GraphType>::vertex_descriptor,
                           vtkIdType> &vertex_id_map);
void write_contour_lengths_to_vtk_unstructured_grid(const GraphType &sg,
                                                    vtkUnstructuredGrid *ugrid);
void write_ete_distances_to_vtk_unstructured_grid(const GraphType &sg,
                                                  vtkUnstructuredGrid *ugrid);
void write_degrees_to_vtk_unstructured_grid(const GraphType &sg,
                                            vtkUnstructuredGrid *ugrid);
void write_vertex_descriptors_to_vtk_unstructured_grid(const GraphType &sg,
                                            vtkUnstructuredGrid *ugrid);

/**
 * Write vtu file
 *
 * @param ugrid
 * @param file_name
 */
void write_vtk_unstructured_grid(vtkUnstructuredGrid *ugrid,
                                 const std::string &file_name);
} // namespace SG
#endif
