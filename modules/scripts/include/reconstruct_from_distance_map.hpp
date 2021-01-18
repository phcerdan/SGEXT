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

#ifndef RECONSTRUCT_FROM_DISTANCE_MAP_HPP
#define RECONSTRUCT_FROM_DISTANCE_MAP_HPP

#include "image_types.hpp" // for FloatImageType
#include "spatial_graph.hpp"

#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace SG {
/**
 * Resulting output from @ref reconstruct_from_distance_map.
 */
struct ReconstructOutput {
    /**
     * Resulting poly_data after appending and cleaning all the sphere sources
     * of the graph points.
     */
    vtkSmartPointer<vtkPolyData> poly_data;
    /**
     * LookupTable to use in a vtkMapper for visualization.
     * (only populated if vertex_to_label_map is provided to @ref
     * reconstruct_from_distance_map
     */
    vtkSmartPointer<vtkLookupTable> lut;
};

/**
 * Create a mesh using all the points (including edge_points) from input graph
 * and the distance_map_image. The distance map can be in Voxels (DGtal), or
 * taking into account the image spacing using ITK (less precise) see
 * @ref create_distance_map.
 *
 * @param input_sg input graph
 *
 * @param distance_map_image input distance map image @ref create_distance_map
 *
 * @param spatial_nodes_position_are_in_physical_space true if the positions
 *  are in physical space.
 *
 * @param distance_map_image_use_image_spacing the distance map values takes
 *  into account image spacing (false for DGtal, maybe true if computed via ITK)
 *  @ref create_distance_map
 *
 * @param vertex_to_label_map map to assign different colors to the poly data,
 * empty by default.
 *
 * @param apply_color_to_edges Edge points have a label associated to
 * max(source_label,target_label) of that edge. Useful for generation analysis.
 */

ReconstructOutput reconstruct_from_distance_map(
        const GraphType &input_sg,
        FloatImageType *distance_map_image,
        const bool spatial_nodes_position_are_in_physical_space = false,
        const bool distance_map_image_use_image_spacing = false,
        const std::unordered_map<GraphType::vertex_descriptor,
                                 size_t> &vertex_to_label_map =
                std::unordered_map<GraphType::vertex_descriptor, size_t>(),
        const bool apply_color_to_edges = true);

namespace defaults {
const std::string polydata_win_title = "SGEXT PolyData";
const size_t polydata_win_width = 600;
const size_t polydata_win_height = 600;
} // end namespace defaults

/**
 * Use vtkPolyDataToImageStencil to create an (ITK) image from input poly data.
 * It needs a reference_image to gather dimensions and metadata.
 *
 * TODO WARNING: This functions is in development. The surface using
 * vtkAppendPolyData with spheres in reconstruct_from_distance_map produces
 * internal holes in the output image.
 *
 * @param poly_data input poly data, for example from @ref
 * reconstruct_from_distance_map
 * @param lut
 * @param reference_image Binary or FloatImageType, used as a reference for
 * dimensions and metadata (origin, spacing, direction)
 *
 * @return binary image
 */
BinaryImageType::Pointer
poly_data_to_binary_image(vtkPolyData *poly_data,
                          const BinaryImageType::Pointer &reference_image);
BinaryImageType::Pointer
poly_data_to_binary_image(vtkPolyData *poly_data,
                          const FloatImageType::Pointer &reference_image);

/**
 * Visualize polydata using optionally a lookup table with colors of integer
 * type.
 *
 * @param poly_data input data
 * @param lut lookup table
 * @param winTitle title of VTK window
 * @param winWidth width of VTK window
 * @param winHeight height of VTK window
 */
void visualize_poly_data(
        vtkPolyData *poly_data,
        vtkLookupTable *lut = nullptr,
        const std::string &winTitle = defaults::polydata_win_title,
        const size_t &winWidth = defaults::polydata_win_width,
        const size_t &winHeight = defaults::polydata_win_width);

/**
 * Write the polydata using XMLPolyDataWriter, filename needs to have suffix
 * vtk, or vtu.
 *
 * @param poly_data PolyData to write, obtained from reconstruct methods
 * @param filename filename of the output. It should have the prefix vtk or vtu.
 * @param is_binary if true: SetDataModeToBinary is applied
 */
void write_poly_data(vtkPolyData *poly_data,
                     const std::string & filename,
                     bool is_binary = false);

/**
 * Visualize polydata using optionally a lookup table with colors of integer
 * type and the associated graph.
 *
 * @param poly_data input data
 * @param graph associated spatial graph
 * @param lut lookup table
 * @param winTitle title of VTK window
 * @param winWidth width of VTK window
 * @param winHeight height of VTK window
 */
void visualize_poly_data_and_graph(
        vtkPolyData *poly_data,
        const GraphType &graph,
        vtkLookupTable *lut = nullptr,
        const std::string &winTitle = defaults::polydata_win_title,
        const size_t &winWidth = defaults::polydata_win_width,
        const size_t &winHeight = defaults::polydata_win_width);

/*****************************************************************************/

namespace detail {
/**
 * Create a sphere source with center given by the input_point and radius
 * given by pixel of that position in the distance map.
 * No extra information/scalars is associated to the sphere. It is possible to
 * attach a color array afterwards, see @ref applyColorToSphere.
 *
 * @param input_point point of the spatial graph.A position of a node or an
 * edge_point of an edge)
 *
 * @param distance_map_image input distance map image @ref create_distance_map
 *
 * @param spatial_nodes_position_are_in_physical_space true if the positions
 *  are in physical space.
 *
 * @param distance_map_image_use_image_spacing the distance map values takes
 *  into account image spacing (false for DGtal, maybe true if computed via ITK)
 *  @ref create_distance_map
 *
 * @return a sphereSource
 */
vtkSmartPointer<vtkSphereSource>
createSphereSource(const ArrayUtilities::Array3D &input_point,
                   FloatImageType *distance_map_image,
                   const bool spatial_nodes_position_are_in_physical_space,
                   const bool distance_map_image_use_image_spacing);
/**
 * Associate an integer array to the sphere cell data. Used to color the spheres
 * based on the label.
 *
 * @param sphereSource sphere source from @ref createSphereSource
 * @param color_label label that can be used to color, see @ref
 * createLookupTable
 */
void applyColorToSphere(vtkSphereSource *sphereSource,
                        const size_t &color_label);

/**
 * Create a categorical lookup table to match labels (applied to the data for
 * example using
 * @ref applyColorToSphere) to colors.
 *
 * @param max_label Max meaningful label (starts at 0). Labels with a greater
 * value are mapped to a white NaN.
 * @param color_scheme color_scheme from vtkColorSeries
 *
 * @return look up tables to be used for example in a vtkMapper for
 * visualization.
 */
vtkSmartPointer<vtkLookupTable>
createLookupTable(const size_t max_label,
                  const std::string &color_scheme = "Brewer Qualitative Set1");

} // end namespace detail
} // end namespace SG

#endif
