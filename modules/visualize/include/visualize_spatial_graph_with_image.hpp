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

#ifndef VISUALIZE_SPATIAL_GRAPH_WITH_IMAGE_HPP
#define VISUALIZE_SPATIAL_GRAPH_WITH_IMAGE_HPP
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkImageMapper.h>
#include <vtkImagePlaneWidget.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkStatisticsImageFilter.h>
#include <itkFixedArray.h>

#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkMatrix4x4.h>
#include <vtkContourValues.h>

#include "visualize_common.hpp"
#include "visualize_spatial_graph.hpp"

#include "convert_to_vtk_graph.hpp"
#include "transform_to_physical_point.hpp"

namespace SG {
template <typename TImage>
void visualize_spatial_graph_with_image(
        const GraphType & sg,
        const TImage* img,
        const std::string& win_title = "itkViewGraph",
        size_t win_x = 600,
        size_t win_y = 600 )
{
  typedef itk::ImageToVTKImageFilter< TImage > ConnectorType;
  typename ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput(img);
  connector->Update();
  connector->UpdateLargestPossibleRegion();

  // Setup renderers
  vtkSmartPointer< vtkRenderer > renderer = vtkSmartPointer< vtkRenderer >::New();

  // Setup render window
  vtkSmartPointer< vtkRenderWindow > renderWindow = vtkSmartPointer< vtkRenderWindow >::New();
  renderWindow->SetWindowName(win_title.c_str());
  renderWindow->SetSize(win_x, win_y);
  renderWindow->AddRenderer(renderer);

  // Setup render window interactor
  vtkSmartPointer< vtkRenderWindowInteractor > renderWindowInteractor =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  vtkSmartPointer< vtkInteractorStyleRubberBand3D > style =
    vtkSmartPointer< vtkInteractorStyleRubberBand3D >::New();
  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);


  typedef itk::StatisticsImageFilter< TImage > FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(img);
  filter->Update();
  filter->UpdateLargestPossibleRegion();
  double min_intensity = filter->GetMinimum();
  double max_intensity = filter->GetMaximum();

  auto volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
  volumeMapper->SetBlendModeToIsoSurface(); // only supported in GPU OpenGL2
  volumeMapper->SetInputData(connector->GetOutput());

  auto opacityFun = vtkSmartPointer<vtkPiecewiseFunction>::New();
  // opacityFun->AddPoint(0.0, 0.0);
  // opacityFun->AddPoint(0.1, 0.0);
  // opacityFun->AddPoint(255, 1.0);
  opacityFun->AddPoint(0.0, 0.0, 0.5, 1.0);
  opacityFun->AddPoint(0.1, 0.0, 0.5, 1.0);
  opacityFun->AddPoint(max_intensity, 1.0, 1.0, 1.0);

  auto colorFun = vtkSmartPointer<vtkColorTransferFunction>::New();
  colorFun->AddRGBPoint(0.0, 0, 0, 0, 0.0, 1.0);
  colorFun->AddRGBPoint(0.1, 100, 100, 100, 0.5, 1.0);
  colorFun->AddRGBPoint(max_intensity, 100, 100, 100, 1.0, 1.0);
  // colorFun->AddRGBPoint(0.0  ,0.0,0.0,1.0);
  // colorFun->AddRGBPoint(1.0  ,100.0,100.0,100.0);
  // colorFun->AddRGBPoint(40.0  ,1.0,0.0,0.0);
  // colorFun->AddRGBPoint(255.0,1.0,1.0,1.0);

  auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
  // volumeProperty->SetIndependentComponents(independentComponents);
  volumeProperty->SetColor( colorFun );
  volumeProperty->SetScalarOpacity( opacityFun );
  volumeProperty->GetIsoSurfaceValues()->SetNumberOfContours(1);
  // volumeProperty->GetIsoSurfaceValues()->SetValue(0, 1);
  volumeProperty->GetIsoSurfaceValues()->SetValue(0, max_intensity);
  // volumeProperty->SetInterpolationTypeToLinear();
  // volumeProperty->ShadeOff();
  // volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  auto volume = vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  // Here we take care of position and orientation
  // so that volume is in DICOM patient physical space
  auto direction = img->GetDirection();
  auto mat = vtkSmartPointer<vtkMatrix4x4>::New(); //start with identity matrix
  for (int i=0; i<3; i++)
      for (int k=0; k<3; k++)
          mat->SetElement(i,k, direction(i,k));

  // Counteract the built-in translation by origin
  auto origin = img->GetOrigin();
  volume->SetPosition(-origin[0], -origin[1], -origin[2]);

  // Add translation to the user matrix
  for (int i=0; i<3; i++)
    {
    mat->SetElement(i,3, origin[i]);
    }
  volume->SetUserMatrix(mat);

  // moved to the bottom
  // renderer->ResetCamera();
  // renderWindowInteractor->Initialize();
  // renderWindowInteractor->Start();

  auto graphLayoutView = create_graph_layout_view_from_spatial_graph(sg);
  // From: https://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/BackgroundImage
  graphLayoutView->GetRenderer()->SetLayer(1);
  graphLayoutView->GetRenderer()->InteractiveOff();
  graphLayoutView->GetRenderer()->SetActiveCamera( renderer->GetActiveCamera() );

  renderWindow->SetNumberOfLayers(2);
  // renderer->SetLayer(0); // This is the default
  renderWindow->AddRenderer(graphLayoutView->GetRenderer());
  // Don't make the z buffer transparent of the graph layout
  graphLayoutView->GetRenderer()->EraseOff();

  renderer->AddViewProp(volume);

  // Flip camera because VTK-ITK different corner for origin.
  vtkCamera *cam = renderer->GetActiveCamera();
  flip_camera(cam);

  renderer->ResetCamera();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();
}
} // end namespace
#endif
