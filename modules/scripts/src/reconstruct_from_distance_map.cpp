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
#include "visualize_common.hpp"
// for visualize_poly_data_and_graph
#include "convert_to_vtk_unstructured_grid.hpp"

#include <itkCastImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <vtkActor2D.h>
#include <vtkButtonWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkCaptionActor2D.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkImageStencil.h>
#include <vtkNamedColors.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPointData.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTexturedButtonRepresentation2D.h>

#include <tuple>

#include "itksys/SystemTools.hxx"

#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkUnsignedLongArray.h>
// viualize
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
// writer
#include <vtkXMLPolyDataWriter.h>

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

    const auto isotropic_spacing_pair = detail::checkIsotropy(distance_map_image);
    const auto & is_isotropic = isotropic_spacing_pair.first;
    const auto & radius_multiplier = isotropic_spacing_pair.second;
    if(!distance_map_image_use_image_spacing && !is_isotropic) {
        std::cerr << detail::isotropyWarning << std::endl;
        std::cerr << "radius_multiplier: " << radius_multiplier << std::endl;
    }

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
                    distance_map_image_use_image_spacing,
                    radius_multiplier);
            if (vertex_to_label_map_provided) {
                const auto found = vertex_to_label_map.find(source);
                size_t color_label = std::numeric_limits<int>::max();
                if (found != vertex_to_label_map.end()) {
                    color_label = found->second;
                }
                detail::applyColorToSphere(sphereSource, color_label);
            }
            appendFilter->AddInputData(sphereSource->GetOutput());
        }
        // Add target
        {
            auto sphereSource = detail::createSphereSource(
                    input_sg[target].pos, distance_map_image,
                    spatial_nodes_position_are_in_physical_space,
                    distance_map_image_use_image_spacing,
                    radius_multiplier);
            if (vertex_to_label_map_provided) {
                const auto found = vertex_to_label_map.find(target);
                size_t color_label = std::numeric_limits<int>::max();
                if (found != vertex_to_label_map.end()) {
                    color_label = found->second;
                }
                detail::applyColorToSphere(sphereSource, color_label);
            }
            appendFilter->AddInputData(sphereSource->GetOutput());
        }
        // Add all edge points
        const auto &edge_points = input_sg[*ei].edge_points;
        for (const auto &ep : edge_points) {
            // Label associated to the edge points is the one corresponding
            // to the smallest label from source and target nodes.
            // If there is no map provided, set it to int max().
            auto sphereSource = detail::createSphereSource(
                    ep, distance_map_image,
                    spatial_nodes_position_are_in_physical_space,
                    distance_map_image_use_image_spacing,
                    radius_multiplier);
            if (vertex_to_label_map_provided) {
                const auto found_source = vertex_to_label_map.find(source);
                const auto found_target = vertex_to_label_map.find(target);
                size_t color_label = std::numeric_limits<int>::max();
                if (found_source != vertex_to_label_map.end() &&
                    found_target != vertex_to_label_map.end()) {
                    // Dev: max of size_t is not mapping to NaN, use int instead
                    // : std::numeric_limits<size_t>::max();
                    color_label = apply_color_to_edges
                                          ? std::max(found_source->second,
                                                     found_target->second)
                                          : std::numeric_limits<int>::max();
                }
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

    auto recomputeNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
    recomputeNormals->SetInputConnection(cleanFilter->GetOutputPort());
    recomputeNormals->Update();

    // // Smooth
    // vtkNew< vtkSmoothPolyDataFilter > smoothFilter;
    // smoothFilter->SetInputConnection( cleanFilter->GetOutputPort() );
    // smoothFilter->SetNumberOfIterations( 25 );
    // smoothFilter->SetRelaxationFactor( 0.15 );
    // smoothFilter->BoundarySmoothingOn();
    // smoothFilter->Update();

    // Populate the output.poly_data
    output.poly_data = recomputeNormals->GetOutput();

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
BinaryImageType::Pointer
poly_data_to_binary_image(vtkPolyData *poly_data,
                          const FloatImageType::Pointer &reference_image) {
    // The content of the image is irrelevant
    auto cast_to_binary_filter =
            itk::CastImageFilter<FloatImageType, BinaryImageType>::New();
    cast_to_binary_filter->SetInput(reference_image);
    cast_to_binary_filter->Update();
    return poly_data_to_binary_image(poly_data,
                                     cast_to_binary_filter->GetOutput());
}

BinaryImageType::Pointer
poly_data_to_binary_image(vtkPolyData *poly_data,
                          const BinaryImageType::Pointer &reference_image) {
    const auto &ref_size =
            reference_image->GetLargestPossibleRegion().GetSize();
    const auto &ref_origin = reference_image->GetOrigin();
    const auto &ref_spacing = reference_image->GetSpacing();

    // Create a raw pointer, its memory will be owned (deleted when needed)
    // by itkImageContainer of the output_itk_image
    vtkImageData *output_vtk_image = vtkImageData::New();
    output_vtk_image->SetDimensions(ref_size[0], ref_size[1], ref_size[2]);
    output_vtk_image->SetOrigin(ref_origin[0], ref_origin[1], ref_origin[2]);
    output_vtk_image->SetSpacing(ref_spacing[0], ref_spacing[1],
                                 ref_spacing[2]);
    output_vtk_image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    // DirectionMatrix only since VTK9
    // const auto &ref_direction = reference_image->GetDirection();
    // output_vtk_image->SetDirectionMatrix(
    //     ref_direction[0], ref_direction[1], ref_direction[2],
    //     ref_direction[3], ref_direction[4], ref_direction[5],
    //     ref_direction[6], ref_direction[7], ref_direction[8]
    //     );

    vtkSmartPointer<vtkPolyDataToImageStencil> data_to_stencil =
            vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    data_to_stencil->SetInputData(poly_data);
    data_to_stencil->SetInformationInput(output_vtk_image);
    data_to_stencil->Update();

    vtkSmartPointer<vtkImageStencil> stencil_to_vtk_image =
            vtkSmartPointer<vtkImageStencil>::New();
    stencil_to_vtk_image->SetInputData(output_vtk_image);
    stencil_to_vtk_image->SetStencilConnection(
            data_to_stencil->GetOutputPort());
    stencil_to_vtk_image->ReverseStencilOn(); // foreground will be 255
    stencil_to_vtk_image->SetBackgroundValue(255);
    stencil_to_vtk_image->Update();
    // DeepCopy to avoid dangling pointers outside this scope.
    output_vtk_image->DeepCopy(stencil_to_vtk_image->GetOutput());

    itk::VTKImageToImageFilter<BinaryImageType>::Pointer vtk_to_itk_filter =
            itk::VTKImageToImageFilter<BinaryImageType>::New();
    vtk_to_itk_filter->SetInput(output_vtk_image);
    vtk_to_itk_filter->Update();
    auto output_itk_image = vtk_to_itk_filter->GetOutput();
    output_itk_image->SetDirection(reference_image->GetDirection());
    // We let the itk container to own (manage the deletion) of the vtkImageData
    // managed by a raw pointer. If ImageData is managed by a vtkSmartPointer
    // we will have a dangling pointer outside of this function.
    output_itk_image->GetPixelContainer()->ContainerManageMemoryOn();
    return output_itk_image;
}

void visualize_poly_data(vtkPolyData *poly_data,
                         vtkLookupTable *lut,
                         const std::string &winTitle,
                         const size_t &winWidth,
                         const size_t &winHeight) {
    // Create a mapper
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly_data);
    if (lut != nullptr) {
        mapper->SetColorModeToMapScalars();
        mapper->SetLookupTable(lut);
    }
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);

    // Setup renderer
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetWindowName(winTitle.c_str());
    renderWindow->SetSize(winWidth, winHeight);
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    auto style = vtkSmartPointer<
            vtkInteractorStyleTrackballCamera>::New(); // like paraview
    renderWindowInteractor->SetInteractorStyle(style);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    // Flip camera because VTK-ITK different corner for origin.
    vtkCamera *cam = renderer->GetActiveCamera();
    flip_camera(cam);

    // Use DepthPeeling: https://vtk.org/Wiki/VTK/Depth_Peeling
    renderWindow->SetAlphaBitPlanes(1);
    renderWindow->SetMultiSamples(0);
    renderer->SetUseDepthPeeling(1);
    renderer->SetOcclusionRatio(0.1);
    renderer->SetMaximumNumberOfPeels(100);

    renderer->ResetCamera();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
}

struct HideActorButtonCallbackCommand : public vtkCommand {
    static HideActorButtonCallbackCommand *New() {
        return new HideActorButtonCallbackCommand;
    }

    HideActorButtonCallbackCommand() = default;

    void SetLabelActor(vtkActor2D *input_label_actor) {
        label_actor = input_label_actor;
    }
    void SetCaptionActor(vtkCaptionActor2D *input_caption_actor) {
        caption_actor = input_caption_actor;
    }
    void SetUnstructuredGridActor(vtkActor *input_ugrid_actor) {
        ugrid_actor = input_ugrid_actor;
    }
    void Execute(vtkObject *caller,
                 unsigned long /*eventId*/,
                 void * /*callData*/) override {
        auto *buttonWidget = reinterpret_cast<vtkButtonWidget *>(caller);
        auto *rep = reinterpret_cast<vtkTexturedButtonRepresentation2D *>(
                buttonWidget->GetRepresentation());
        auto state = rep->GetState();
        if (state != 0) {
            label_actor->VisibilityOn();
            caption_actor->SetCaption("Show Id's: ON");
            ugrid_actor->VisibilityOn();

        } else {
            label_actor->VisibilityOff();
            caption_actor->SetCaption("Show Id's: OFF");
            ugrid_actor->VisibilityOff();
        }
    }
    vtkActor2D *label_actor{nullptr};
    vtkCaptionActor2D *caption_actor{nullptr};
    vtkActor *ugrid_actor{nullptr};
};

void visualize_poly_data_and_graph(vtkPolyData *poly_data,
                                   const GraphType &graph,
                                   vtkLookupTable *lut,
                                   const std::string &winTitle,
                                   const size_t &winWidth,
                                   const size_t &winHeight) {
    // Create a mapper for poly_data
    auto pd_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    pd_mapper->SetInputData(poly_data);
    if (lut != nullptr) {
        pd_mapper->SetColorModeToMapScalars();
        pd_mapper->SetLookupTable(lut);
    }
    auto pd_actor = vtkSmartPointer<vtkActor>::New();
    pd_actor->SetMapper(pd_mapper);
    pd_actor->GetProperty()->SetOpacity(0.5);

    // Add an actor with ugrid
    auto ugrid = convert_to_vtk_unstructured_grid(graph);
    auto ugrid_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    ugrid_mapper->SetInputData(ugrid);
    ugrid_mapper->Update();
    auto ugrid_actor = vtkSmartPointer<vtkActor>::New();
    ugrid_actor->SetMapper(ugrid_mapper);
    ugrid_actor->VisibilityOff();

    // Add vertex_descriptor labels
    // auto ugrid_point_data = ugrid->GetPointData();
    // auto descriptors_array = ugrid_point_data->GetArray("vertex_descriptor");
    auto label_mapper_vertex = vtkSmartPointer<vtkLabeledDataMapper>::New();
    label_mapper_vertex->SetInputData(ugrid);
    // default FontSize is 12
    // label_mapper_vertex->GetLabelTextProperty()->SetFontSize(14);
    label_mapper_vertex->Update();
    auto label_vertex_actor = vtkSmartPointer<vtkActor2D>::New();
    label_vertex_actor->SetMapper(label_mapper_vertex);
    label_vertex_actor->VisibilityOff(); // use button to toggle

    // Setup renderer
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(pd_actor);
    renderer->AddActor(ugrid_actor);
    renderer->AddActor(label_vertex_actor);
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetWindowName(winTitle.c_str());
    renderWindow->SetSize(winWidth, winHeight);
    renderWindow->AddRenderer(renderer);
    // Use DepthPeeling: https://vtk.org/Wiki/VTK/Depth_Peeling
    renderWindow->SetAlphaBitPlanes(1);
    renderWindow->SetMultiSamples(0);
    renderer->SetUseDepthPeeling(1);
    renderer->SetOcclusionRatio(0.1);
    renderer->SetMaximumNumberOfPeels(100);

    // Setup render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    auto style = vtkSmartPointer<
            vtkInteractorStyleTrackballCamera>::New(); // like paraview
    renderWindowInteractor->SetInteractorStyle(style);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    // Flip camera because VTK-ITK different corner for origin.
    vtkCamera *cam = renderer->GetActiveCamera();
    flip_camera(cam);
    renderer->ResetCamera();

    // Add button to hide/show vertex_descriptors
    auto namedColors = vtkSmartPointer<vtkNamedColors>::New();
    unsigned char color_off[4];
    namedColors->GetColor("DarkRed", color_off);
    unsigned char color_on[4];
    namedColors->GetColor("DarkGreen", color_on);
    auto button_texture_off = create_texture_for_button(color_off);
    auto button_texture_on = create_texture_for_button(color_on);
    const double button_border_margin = 30.0;
    auto button_bounds = create_bounds_for_button(button_border_margin);
    auto button_rep = create_on_off_representation_for_button(
            button_texture_off, button_texture_on, button_bounds);

    // Create an actor for the text
    const std::string caption_off = "Show Id's: OFF";
    auto caption_actor = create_caption_actor_for_button(button_border_margin,
                                                         caption_off);

    renderer->AddViewProp(caption_actor);

    auto button_callback =
            vtkSmartPointer<HideActorButtonCallbackCommand>::New();
    button_callback->SetLabelActor(label_vertex_actor);
    button_callback->SetUnstructuredGridActor(ugrid_actor);
    button_callback->SetCaptionActor(caption_actor);

    auto button_widget = vtkSmartPointer<vtkButtonWidget>::New();
    button_widget->SetInteractor(renderWindowInteractor);
    button_widget->SetRepresentation(button_rep);
    button_widget->AddObserver(vtkCommand::StateChangedEvent, button_callback);
    button_widget->On();

    // Press "s" to toggle button, using SG::toggleTexturedButtonCallbackFunction.
    auto button_keypress_callback = vtkSmartPointer<vtkCallbackCommand>::New();
    button_keypress_callback->SetClientData(button_widget.GetPointer());
    button_keypress_callback->SetCallback(SG::toggleTexturedButtonCallbackFunction);
    renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, button_keypress_callback);

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
}

void write_poly_data(vtkPolyData *poly_data,
                     const std::string &filename,
                     bool is_binary) {
    const std::string file_ext =
            itksys::SystemTools::GetFilenameLastExtension(filename);

    if (file_ext == ".vtk" || file_ext == ".vtu") {
        vtkNew<vtkXMLPolyDataWriter> vtkMeshWriter;
        vtkMeshWriter->SetInputData(poly_data);
        vtkMeshWriter->SetFileName(filename.c_str());
        if (is_binary) {
            vtkMeshWriter->SetDataModeToBinary();
        }
        vtkMeshWriter->Update();
    }
    // else if ( file_ext==".stl" )
    //   {
    //   vtkNew< vtkSTLWriter > stlWriter;
    //   stlWriter->SetInputData( poly_data );
    //   stlWriter->SetFileName( filename.c_str() );
    //   if(is_binary) stlWriter->SetFileTypeToBinary();
    //   stlWriter->Write();
    //   }
    else {
        throw std::runtime_error("Unrecognized output file extension: " +
                                 file_ext + ". Use .vtk or .vtu.");
    }
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

std::pair<bool, double> checkIsotropy(const FloatImageType *distance_map_image) {
    const auto & dmap_spacing = distance_map_image->GetSpacing();
    const auto min_max_spacing_pair = std::minmax_element(
            dmap_spacing.Begin(), dmap_spacing.End());

    const auto & min_sp = *min_max_spacing_pair.first;
    const auto & max_sp = *min_max_spacing_pair.second;
    if (itk::Math::FloatAlmostEqual(min_sp, max_sp)) {
        return std::make_pair(true, max_sp);
    } else {
        return std::make_pair(false, (min_sp + max_sp)/2.0);
    }
}

vtkSmartPointer<vtkSphereSource>
createSphereSource(const ArrayUtilities::Array3D &input_point,
                   FloatImageType *distance_map_image,
                   const bool spatial_nodes_position_are_in_physical_space,
                   const bool distance_map_image_use_image_spacing,
                   const double radius_multiplier) {

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
        // Use checkIsotropic to get the radius multiplier.
        // Only works reliable if image is isotropic, a warning is raised
        // in the function calling createSphereSource if not.
        sphereSource->SetRadius(dmap_value * radius_multiplier);
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
