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

#include "reconstruct_from_distance_map.hpp"

#include <tuple>

#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkUnsignedLongArray.h>

namespace SG {

ReconstructOutput reconstruct_from_distance_map(
        const GraphType &input_sg,
        FloatImageType *distance_map_image,
        const bool spatial_nodes_position_are_in_physical_space,
        const bool distance_map_image_use_image_spacing,
        const std::unordered_map<GraphType::vertex_descriptor, size_t>
                &vertex_to_label_map,
        const bool apply_color_to_edges) {

    ReconstructOutput output;
    vtkSmartPointer<vtkAppendPolyData> appendFilter =
            vtkSmartPointer<vtkAppendPolyData>::New();
    const bool vertex_to_label_map_provided = !vertex_to_label_map.empty();

    GraphType::edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(input_sg);
    for (; ei != ei_end; ++ei) {
        const auto source = boost::source(*ei, input_sg);
        const auto target = boost::target(*ei, input_sg);
        // Add source
        {
            auto sphereSource = detail::createSphereSource(
                    input_sg[source].pos, distance_map_image,
                    spatial_nodes_position_are_in_physical_space,
                    distance_map_image_use_image_spacing);
            if (vertex_to_label_map_provided) {
                const size_t color_label = vertex_to_label_map.at(source);
                detail::applyColorToSphere(sphereSource, color_label);
            }
            appendFilter->AddInputData(sphereSource->GetOutput());
        }
        // Add target
        {
            auto sphereSource = detail::createSphereSource(
                    input_sg[target].pos, distance_map_image,
                    spatial_nodes_position_are_in_physical_space,
                    distance_map_image_use_image_spacing);
            if (vertex_to_label_map_provided) {
                const size_t color_label = vertex_to_label_map.at(target);
                detail::applyColorToSphere(sphereSource, color_label);
            }
            appendFilter->AddInputData(sphereSource->GetOutput());
        }
        // Add all edge points
        auto edge_points = input_sg[*ei].edge_points;
        for (const auto &ep : edge_points) {
            // Label associated to the edge points is the one corresponding
            // to the smallest label from source and target nodes.
            // If there is no map provided, set it to int max().
            auto sphereSource = detail::createSphereSource(
                    ep, distance_map_image,
                    spatial_nodes_position_are_in_physical_space,
                    distance_map_image_use_image_spacing);
            if (vertex_to_label_map_provided) {
                // Dev: max of size_t is not mapping to NaN, use int instead
                // : std::numeric_limits<size_t>::max();
                const size_t color_label =
                        apply_color_to_edges
                                ? std::max(vertex_to_label_map.at(source),
                                           vertex_to_label_map.at(target))
                                : std::numeric_limits<int>::max();
                detail::applyColorToSphere(sphereSource, color_label);
            }
            appendFilter->AddInputData(sphereSource->GetOutput());
        }
    }
    appendFilter->Update();
    // Clean: Remove any duplicate points.
    vtkSmartPointer<vtkCleanPolyData> cleanFilter =
            vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    cleanFilter->Update();

    // // Smooth
    // vtkNew< vtkSmoothPolyDataFilter > smoothFilter;
    // smoothFilter->SetInputConnection( cleanFilter->GetOutputPort() );
    // smoothFilter->SetNumberOfIterations( 25 );
    // smoothFilter->SetRelaxationFactor( 0.15 );
    // smoothFilter->BoundarySmoothingOn();
    // smoothFilter->Update();

    // Populate the output.poly_data
    output.poly_data = cleanFilter->GetOutput();

    // Populate ouput.lut in casse the map is provided
    if (vertex_to_label_map_provided) {
        const size_t max_label =
                std::max_element(
                        vertex_to_label_map.begin(), vertex_to_label_map.end(),
                        [](const std::pair<GraphType::vertex_descriptor, size_t>
                                   &p1,
                           const std::pair<GraphType::vertex_descriptor, size_t>
                                   &p2) { return p1.second < p2.second; })
                        ->second;
        output.lut =
                detail::createLookupTable(max_label, "Brewer Qualitative Set1");
    }

    return output;
}

namespace detail {
void applyColorToSphere(vtkSphereSource *sphereSource,
                        const size_t &color_label) {
    auto sphereData = sphereSource->GetOutput();
    auto colors = vtkSmartPointer<vtkUnsignedLongArray>::New();
    colors->SetNumberOfComponents(1);
    colors->SetName("Colors");
    colors->SetNumberOfTuples(sphereData->GetNumberOfCells());
    colors->Fill(color_label);
    sphereData->GetCellData()->SetScalars(colors);
}

vtkSmartPointer<vtkSphereSource>
createSphereSource(const ArrayUtilities::Array3D &input_point,
                   FloatImageType *distance_map_image,
                   const bool spatial_nodes_position_are_in_physical_space,
                   const bool distance_map_image_use_image_spacing) {

    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    double dmap_value;

    // Set Center
    if (spatial_nodes_position_are_in_physical_space) {
        sphereSource->SetCenter(input_point[0], input_point[1],
                                input_point[2]); // point
    } else {
        FloatImageType::PointType itk_point;
        FloatImageType::IndexType itk_index;
        for (size_t i = 0; i < FloatImageType::ImageDimension; ++i) {
            itk_index[i] = input_point[i];
        }
        distance_map_image->TransformIndexToPhysicalPoint(itk_index, itk_point);
        sphereSource->SetCenter(itk_point[0], itk_point[1],
                                itk_point[2]); // point
    }
    // Set Radius:
    if (spatial_nodes_position_are_in_physical_space) {
        FloatImageType::PointType itk_point;
        FloatImageType::IndexType itk_index;
        for (size_t i = 0; i < FloatImageType::ImageDimension; ++i) {
            itk_point[i] = input_point[i];
        }
        distance_map_image->TransformPhysicalPointToIndex(itk_point, itk_index);
        dmap_value = distance_map_image->GetPixel(itk_index);
    } else {
        FloatImageType::IndexType itk_index;
        for (size_t i = 0; i < FloatImageType::ImageDimension; ++i) {
            itk_index[i] = input_point[i];
        }
        dmap_value = distance_map_image->GetPixel(itk_index);
    }
    // dmap_value can represent number of pixels to the object border,
    // or a physical distance
    if (distance_map_image_use_image_spacing) {
        sphereSource->SetRadius(dmap_value);
    } else {
        // TODO: We have to assume that the spacing is isotropic. Raise
        // warning if not.
        const double radius = dmap_value * distance_map_image->GetSpacing()[0];
        sphereSource->SetRadius(radius);
    }

    // sphereSource->SetThetaResolution(100);
    // sphereSource->SetPhiResolution(100);
    sphereSource->Update();
    return sphereSource;
}

vtkSmartPointer<vtkLookupTable>
createLookupTable(const size_t max_label, const std::string &color_scheme) {
    auto colorSeries = vtkSmartPointer<vtkColorSeries>::New();
    colorSeries->SetColorSchemeByName(color_scheme.c_str());

    vtkSmartPointer<vtkLookupTable> lut =
            vtkSmartPointer<vtkLookupTable>::New();
    lut->IndexedLookupOn(); // categorical
    lut->SetNumberOfTableValues(max_label + 1);
    lut->SetNanColor(1.0, 1.0, 1.0, 0.3); // Nan to white (with transparency)
    for (size_t i = 0; i < max_label + 1; ++i) {
        auto color = colorSeries->GetColorRepeating(i);
        lut->SetAnnotation(i, std::to_string(i));
        lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                           color.GetBlue() / 255., 1.0);
    }
    return lut;
}

} // end namespace detail
} // end namespace SG
