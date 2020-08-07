/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "scripts_types.hpp" // for FloatImageType
#include "spatial_graph.hpp"

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
