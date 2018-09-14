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
#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include <itkFixedArray.h>

#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include "convert_to_vtk_graph.hpp"

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

  // Prepare for slices.
  typedef itk::StatisticsImageFilter< TImage > FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(img);
  filter->Update();
  filter->UpdateLargestPossibleRegion();
  double min_intensity = filter->GetMinimum();
  double max_intensity = filter->GetMaximum();
  double window = max_intensity - min_intensity;
  double level  = min_intensity + window / 2;
  /** SLICES */
  itk::FixedArray< vtkSmartPointer< vtkImagePlaneWidget >, 3 > slice_planes;
  for ( unsigned i = 0; i < 3; ++i )
    {
    slice_planes[i] = vtkSmartPointer< vtkImagePlaneWidget >::New();
    slice_planes[i]->SetResliceInterpolateToCubic();
    slice_planes[i]->DisplayTextOn();
    slice_planes[i]->SetInteractor(renderWindowInteractor);
    slice_planes[i]->PlaceWidget();
    slice_planes[i]->SetSliceIndex(0);
    slice_planes[i]->SetMarginSizeX(0);
    slice_planes[i]->SetMarginSizeY(0);
    slice_planes[i]->SetRightButtonAction(
      vtkImagePlaneWidget::VTK_SLICE_MOTION_ACTION);
    slice_planes[i]->SetMiddleButtonAction(
      vtkImagePlaneWidget::VTK_WINDOW_LEVEL_ACTION);
    slice_planes[i]->TextureInterpolateOff();

    slice_planes[i]->SetInputData(connector->GetOutput());
    slice_planes[i]->SetPlaneOrientation(i);
    slice_planes[i]->UpdatePlacement();
    slice_planes[i]->SetWindowLevel(window, level);
    slice_planes[i]->On();
    }
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

  // moved to the bottom
  // renderer->ResetCamera();
  // renderWindowInteractor->Initialize();
  // renderWindowInteractor->Start();

  // Render graph:
  vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
      convert_to_vtk_graph(sg);
  auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
  graphLayoutView->AddRepresentationFromInput(vtk_graph);
  graphLayoutView->SetLayoutStrategyToPassThrough();
  auto style_graph = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
  graphLayoutView->SetInteractorStyle(style_graph) ;
  // 1 is the default (squares or circles), 9 is spheres ( extremely slow)
  graphLayoutView->SetGlyphType(1);
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
  renderer->ResetCamera();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();
}
} // end namespace
#endif
