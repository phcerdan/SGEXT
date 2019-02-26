/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

  // Render graph:
  // // Copy graph to transform it into index space
  // auto sg_index_space = sg;
  // SG::transform_graph_to_index_space( sg_index_space, img );
  vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
      convert_to_vtk_graph(sg);
  auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
  graphLayoutView->AddRepresentationFromInput(vtk_graph);
  graphLayoutView->SetLayoutStrategyToPassThrough();
  auto style_graph = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
  graphLayoutView->SetInteractorStyle(style_graph) ;
  // 1 is the default (squares or circles), 9 is spheres ( extremely slow)
  graphLayoutView->SetGlyphType(7);
  // graphLayoutView->Set
  // Scaled Glyphs on requires a way to scale it. If uncommented
  // the glyphs are gone with an error (useful for debugging)
  // graphLayoutView->ScaledGlyphsOn();
  graphLayoutView->ResetCamera();
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
  double pos[3];
  double vup[3];
  vtkCamera *cam = renderer->GetActiveCamera();
  cam->GetPosition(pos);
  cam->GetViewUp(vup);
  for ( unsigned int i = 0; i < 3; ++i )
    {
    pos[i] = -pos[i];
    vup[i] = -vup[i];
    }
  cam->SetPosition(pos);
  cam->SetViewUp(vup);

  renderer->ResetCamera();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();
}
} // end namespace
#endif
