/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
