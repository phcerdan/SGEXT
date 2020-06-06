/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VISUALIZE_SPATIAL_GRAPH_WITH_IMAGE_HPP
#define VISUALIZE_SPATIAL_GRAPH_WITH_IMAGE_HPP
#include <itkFixedArray.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkStatisticsImageFilter.h>
#include <vtkCamera.h>
#include <vtkImageMapper.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkColorTransferFunction.h>
#include <vtkContourValues.h>
#include <vtkGraphLayoutView.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>

#include "visualize_common.hpp"
#include "visualize_spatial_graph.hpp"

#include "convert_to_vtk_graph.hpp"
#include "transform_to_physical_point.hpp"

#include "vtkInteractorStyleTrackballCameraGraph.h"

namespace SG {
template <typename TImage>
void visualize_spatial_graph_with_image(
        const GraphType &sg,
        const TImage *img,
        const std::string &win_title = "sgext: SpatialGrap and Image",
        size_t win_x = 600,
        size_t win_y = 600) {
    typedef itk::ImageToVTKImageFilter<TImage> ConnectorType;
    typename ConnectorType::Pointer connector = ConnectorType::New();
    connector->SetInput(img);
    connector->Update();
    connector->UpdateLargestPossibleRegion();

    using StatisticsImageFilter = itk::StatisticsImageFilter<TImage>;
    auto stats_filter = StatisticsImageFilter::New();
    stats_filter->SetInput(img);
    stats_filter->Update();
    stats_filter->UpdateLargestPossibleRegion();
    double min_intensity = stats_filter->GetMinimum();
    double max_intensity = stats_filter->GetMaximum();

    auto volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    volumeMapper->SetBlendModeToIsoSurface(); // only supported in GPU OpenGL2
    volumeMapper->SetInputData(connector->GetOutput());

    auto opacityFun = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFun->AddPoint(min_intensity, 0.2);
    opacityFun->AddPoint(max_intensity, 0.2);

    auto colorFun = vtkSmartPointer<vtkColorTransferFunction>::New();
    // A palid blue
    colorFun->AddRGBPoint(min_intensity, 0.0, 1.0, 1.0);
    colorFun->AddRGBPoint(max_intensity, 0.0, 1.0, 1.0);

    auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(colorFun);
    volumeProperty->SetScalarOpacity(opacityFun);
    volumeProperty->GetIsoSurfaceValues()->SetNumberOfContours(1);
    volumeProperty->GetIsoSurfaceValues()->SetValue(0, max_intensity);

    auto volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    // Here we take care of position and orientation
    // so that volume is in DICOM patient physical space
    auto direction = img->GetDirection();
    auto mat = vtkSmartPointer<vtkMatrix4x4>::New(); // start with identity
                                                     // matrix
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++)
            mat->SetElement(i, k, direction(i, k));

    // Counteract the built-in translation by origin
    auto origin = img->GetOrigin();
    volume->SetPosition(-origin[0], -origin[1], -origin[2]);

    // Add translation to the user matrix
    for (int i = 0; i < 3; i++) {
        mat->SetElement(i, 3, origin[i]);
    }
    volume->SetUserMatrix(mat);


    // Setup renderer and style
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddViewProp(volume);
    auto style = vtkSmartPointer<
        vtkInteractorStyleTrackballCameraGraph>::New(); // like paraview

    // graphLayoutView provides our window and interactor
    auto graphLayoutView = create_graph_layout_view_from_spatial_graph(sg);
    auto renderWindowInteractor = graphLayoutView->GetInteractor();
    renderWindowInteractor->SetInteractorStyle(style);
    auto renderWindow = graphLayoutView->GetRenderWindow();
    renderWindow->SetWindowName(win_title.c_str());
    renderWindow->SetSize(win_x, win_y);
    renderWindow->SetNumberOfLayers(2);
    // From: https://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/BackgroundImage
    graphLayoutView->GetRenderer()->SetLayer(1);
    graphLayoutView->GetRenderer()->InteractiveOff();
    graphLayoutView->GetRenderer()->SetActiveCamera(
            renderer->GetActiveCamera());
    // Don't make the z buffer transparent of the graph layout
    graphLayoutView->GetRenderer()->EraseOff();

    renderWindow->AddRenderer(renderer);

    // Flip camera because VTK-ITK different corner for origin.
    vtkCamera *cam = renderer->GetActiveCamera();
    flip_camera(cam);

    renderer->ResetCamera();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
}
} // namespace SG
#endif
