/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "convert_to_vtk_unstructured_grid.hpp"
#include "edge_points_utilities.hpp" // for contour_length
#include <unordered_map>

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkIntArray.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyLine.h>
#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridWriter.h>

namespace SG {
void write_vertices_to_vtk_unstructured_grid(
        const GraphType &sg,
        vtkUnstructuredGrid *ugrid,
        vtkPoints *vtk_points,
        std::unordered_map<boost::graph_traits<GraphType>::vertex_descriptor,
                           vtkIdType> &vertex_id_map) {
    // vertices are linked to geometric points
    auto [vi, vi_end] = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        vertex_id_map[*vi] = vtk_points->InsertNextPoint(
                sg[*vi].pos[0], sg[*vi].pos[1], sg[*vi].pos[2]);
    }
    ugrid->SetPoints(vtk_points);
}

void write_vertex_descriptors_to_vtk_unstructured_grid(const GraphType &sg,
                                            vtkUnstructuredGrid *ugrid) {
    auto point_data = ugrid->GetPointData();
    const auto number_of_points = ugrid->GetNumberOfPoints();

    auto vertex_descriptors = vtkIntArray::New();
    vertex_descriptors->SetName("vertex_descriptor");
    vertex_descriptors->SetNumberOfComponents(1);
    vertex_descriptors->SetNumberOfTuples(number_of_points);

    {
        long long counter = 0;
        for (auto [vi, vi_end] = boost::vertices(sg); vi != vi_end; ++vi) {
            vertex_descriptors->SetTuple1(counter, *vi );
            counter++;
        }
        // Assume that if there are more points than vertices is because
        // the rest of the points are edge_points (assign id = -1)
        while(counter < number_of_points) {
            vertex_descriptors->SetTuple1(counter, -1);
            counter++;
        }
    }
    point_data->AddArray(vertex_descriptors);
    point_data->Update();

}

void write_degrees_to_vtk_unstructured_grid(const GraphType &sg,
                                            vtkUnstructuredGrid *ugrid) {
    auto point_data = ugrid->GetPointData();
    const auto number_of_points = ugrid->GetNumberOfPoints();

    auto degree = vtkIntArray::New();
    degree->SetName("degree");
    degree->SetNumberOfComponents(1);
    degree->SetNumberOfTuples(number_of_points);

    {
        long long counter = 0;
        for (auto [vi, vi_end] = boost::vertices(sg); vi != vi_end; ++vi) {
            degree->SetTuple1(counter, boost::degree(*vi, sg) );
            counter++;
        }
        // Assume that if there are more points than vertices is because
        // the rest of the points are edge_points (with degree 2)
        while(counter < number_of_points) {
            degree->SetTuple1(counter, 2);
            counter++;
        }
    }
    point_data->AddArray(degree);
    point_data->Update();
}

void write_spatial_node_ids_to_vtk_unstructured_grid(
        const GraphType &sg,
        vtkUnstructuredGrid *ugrid) {
    auto point_data = ugrid->GetPointData();
    const auto number_of_points = ugrid->GetNumberOfPoints();

    auto spatial_node_ids = vtkIntArray::New();
    spatial_node_ids->SetName("spatial_node_id");
    spatial_node_ids->SetNumberOfComponents(1);
    spatial_node_ids->SetNumberOfTuples(number_of_points);

    {
        long long counter = 0;
        for (auto [vi, vi_end] = boost::vertices(sg); vi != vi_end; ++vi) {
            spatial_node_ids->SetTuple1(counter, sg[*vi].id);
            counter++;
        }
        // Assume that if there are more points than vertices is because
        // the rest of the points are edge_points (assign id = -1)
        while(counter < number_of_points) {
            spatial_node_ids->SetTuple1(counter, -1);
            counter++;
        }
    }
    point_data->AddArray(spatial_node_ids);
    point_data->Update();

}

void write_ete_distances_to_vtk_unstructured_grid(const GraphType &sg,
                                                  vtkUnstructuredGrid *ugrid) {
    auto cell_data = ugrid->GetCellData();
    const auto ncells = ugrid->GetNumberOfCells();
    const std::string array_name = "end_to_end_distance";
    auto vtk_array = vtkDoubleArray::New();
    vtk_array->SetName(array_name.c_str());
    vtk_array->SetNumberOfComponents(1);
    vtk_array->SetNumberOfTuples(ncells);
    cell_data->AddArray(vtk_array);
    // populate the array tuple:
    {
        size_t cell_id = 0;
        for (auto [ei, ei_end] = boost::edges(sg); ei != ei_end; ++ei) {

            auto source = boost::source(*ei, sg);
            auto target = boost::target(*ei, sg);
            const auto &source_pos = sg[source].pos;
            const auto &target_pos = sg[target].pos;
            vtk_array->SetTuple1(
                    cell_id, ArrayUtilities::distance(target_pos, source_pos));
            cell_id++;
        }
    }
    cell_data->Update();
}

void write_contour_lengths_to_vtk_unstructured_grid(
        const GraphType &sg, vtkUnstructuredGrid *ugrid) {
    auto cell_data = ugrid->GetCellData();
    const auto ncells = ugrid->GetNumberOfCells();
    const std::string array_name = "contour_length";
    auto vtk_array = vtkDoubleArray::New();
    vtk_array->SetName(array_name.c_str());
    vtk_array->SetNumberOfComponents(1);
    vtk_array->SetNumberOfTuples(ncells);
    cell_data->AddArray(vtk_array);
    // populate the array tuple:
    {
        size_t cell_id = 0;
        for (auto [ei, ei_end] = boost::edges(sg); ei != ei_end; ++ei) {
            vtk_array->SetTuple1(cell_id, contour_length(*ei, sg));
            cell_id++;
        }
    }
    cell_data->Update();
}

vtkSmartPointer<vtkUnstructuredGrid>
convert_to_vtk_unstructured_grid(const GraphType &sg) {
    auto ugrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using VertexIdMap = std::unordered_map<vertex_descriptor, vtkIdType>;
    VertexIdMap vertex_id_map;
    auto vtk_points = vtkSmartPointer<vtkPoints>::New();

    write_vertices_to_vtk_unstructured_grid(sg, ugrid, vtk_points,
                                            vertex_id_map);
    // Add more properties of vertices to PointData, if any

    // edges are the cells, they can be represented by a vtkLine,
    // or a vtkPolyLine if the edge_points need to be stored too.
    // or maybe even better to store both cell types for further flexibility

    // Allocate the number of cells, a vtkLine and a vtkPolyLines per edge
    const size_t num_cells = boost::num_edges(sg);
    ugrid->Allocate(num_cells);

    auto [ei, ei_end] = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        // Add vtkLine
        {
            auto vtk_id_list = vtkIdList::New();
            vtk_id_list->InsertNextId(vertex_id_map.at(source));
            vtk_id_list->InsertNextId(vertex_id_map.at(target));
            auto line = vtkLine::New();
            ugrid->InsertNextCell(line->GetCellType(), vtk_id_list);
        }
    }

    // Append to PointData
    write_vertex_descriptors_to_vtk_unstructured_grid(sg, ugrid);
    write_degrees_to_vtk_unstructured_grid(sg, ugrid);
    write_spatial_node_ids_to_vtk_unstructured_grid(sg, ugrid);

    // Append to CellData
    write_ete_distances_to_vtk_unstructured_grid(sg, ugrid);
    write_contour_lengths_to_vtk_unstructured_grid(sg, ugrid);
    return ugrid;
};

vtkSmartPointer<vtkUnstructuredGrid>
convert_to_vtk_unstructured_grid_with_edge_points(const GraphType &sg) {
    auto ugrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using VertexIdMap = std::unordered_map<vertex_descriptor, vtkIdType>;
    VertexIdMap vertex_id_map;
    auto vtk_points = vtkSmartPointer<vtkPoints>::New();

    write_vertices_to_vtk_unstructured_grid(sg, ugrid, vtk_points,
                                            vertex_id_map);

    auto [ei, ei_end] = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        auto &sg_edge = sg[*ei];
        // Add vtkPolyLine
        auto &sg_edge_points = sg_edge.edge_points;
        // Check if the first point in edge_points, is closer to source, or to
        // target. Only do the check if there is at least one edge_point.
        const bool source_is_closer_to_begin =
                sg_edge_points.empty()
                        ? true
                        : ArrayUtilities::distance(sg[source].pos,
                                                   sg_edge_points[0]) <
                                  ArrayUtilities::distance(sg[target].pos,
                                                           sg_edge_points[0]);
        {
            auto vtk_id_list = vtkIdList::New();
            if (source_is_closer_to_begin) {
                vtk_id_list->InsertNextId(vertex_id_map.at(source));
            } else {
                vtk_id_list->InsertNextId(vertex_id_map.at(target));
            }

            // Insert edge_points first
            for (const auto &p : sg_edge_points) {
                vtk_id_list->InsertNextId(
                        vtk_points->InsertNextPoint(p[0], p[1], p[2]));
            }
            if (source_is_closer_to_begin) {
                vtk_id_list->InsertNextId(vertex_id_map.at(target));
            } else {
                vtk_id_list->InsertNextId(vertex_id_map.at(source));
            }

            auto poly_line = vtkPolyLine::New();
            ugrid->InsertNextCell(poly_line->GetCellType(), vtk_id_list);
        }
    }
    // Append to PointData
    write_vertex_descriptors_to_vtk_unstructured_grid(sg, ugrid);
    write_degrees_to_vtk_unstructured_grid(sg, ugrid);
    write_spatial_node_ids_to_vtk_unstructured_grid(sg, ugrid);

    // Append to CellData
    write_ete_distances_to_vtk_unstructured_grid(sg, ugrid);
    write_contour_lengths_to_vtk_unstructured_grid(sg, ugrid);
    return ugrid;
}

void write_vtk_unstructured_grid(vtkUnstructuredGrid *ugrid,
                                 const std::string &file_name) {
    auto ugrid_writer = vtkXMLUnstructuredGridWriter::New();
    ugrid_writer->SetFileName(file_name.c_str());
    ugrid_writer->SetInputData(ugrid);
    ugrid_writer->Update();
}
} // end namespace SG
