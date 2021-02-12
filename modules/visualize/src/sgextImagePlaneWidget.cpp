/* ********************************************************************
 * Copyright (C) 2021 Pablo Hernandez-Cerdan.
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

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    sgextImagePlaneWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "sgextImagePlaneWidget.h"

#include "vtkAbstractPropPicker.h"
#include "vtkAlgorithmOutput.h"
#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkImageReslice.h"
#include "vtkObjectFactory.h"
#include "vtkPickingManager.h"
#include "vtkPlaneSource.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"

vtkStandardNewMacro(sgextImagePlaneWidget)

//------------------------------------------------------------------------------
void sgextImagePlaneWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ImagesForCursorDisplay:\n";
  for(size_t img_index = 0; img_index <= ImagesForCursorDisplay.size(); ++img_index) {
    auto &img = ImagesForCursorDisplay[img_index];
    auto &name = ImagesNamesForCursorDisplay[img_index];
    auto &current_value = CurrentImageValueForCursorDisplay[img_index];
    if(img)
    {
      os << indent << "ImagesForCursorDisplay index: " << img_index << "\n";
      os << indent << "ImagesNameForCursorDisplay: " << name << "\n";
      os << indent << "CurrentImageValueForCursorDisplay: " << current_value << "\n";
      os << indent << "ImageForCursorDisplay:\n";
      img->PrintSelf(os, indent.GetNextIndent());
    }
  }
}

//------------------------------------------------------------------------------
void sgextImagePlaneWidget::StartCursor()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
  {
    this->State = sgextImagePlaneWidget::Outside;
    return;
  }

  // Okay, we can process this. If anything is picked, then we
  // can start pushing the plane.
  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

  int found = 0;
  int i;
  if (path != nullptr)
  {
    // Deal with the possibility that we may be using a shared picker
    vtkCollectionSimpleIterator sit;
    path->InitTraversal(sit);
    vtkAssemblyNode* node;
    for (i = 0; i < path->GetNumberOfItems() && !found; i++)
    {
      node = path->GetNextNode(sit);
      if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
      {
        found = 1;
      }
    }
  }

  if (!found || path == nullptr)
  {
    this->State = sgextImagePlaneWidget::Outside;
    this->HighlightPlane(0);
    this->ActivateCursor(0);
    this->ActivateText(0);
    return;
  }
  else
  {
    this->State = sgextImagePlaneWidget::Cursoring;
    this->HighlightPlane(1);
    this->ActivateCursor(1);
    this->ActivateText(1);
    this->UpdateCursor(X, Y);
    this->ManageTextDisplay();
  }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void sgextImagePlaneWidget::StartWindowLevel()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, make sure that the pick is in the current renderer
  if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
  {
    this->State = sgextImagePlaneWidget::Outside;
    return;
  }

  // Okay, we can process this. If anything is picked, then we
  // can start window-levelling.
  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

  int found = 0;
  int i;
  if (path != nullptr)
  {
    // Deal with the possibility that we may be using a shared picker
    vtkCollectionSimpleIterator sit;
    path->InitTraversal(sit);
    vtkAssemblyNode* node;
    for (i = 0; i < path->GetNumberOfItems() && !found; i++)
    {
      node = path->GetNextNode(sit);
      if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
      {
        found = 1;
      }
    }
  }

  this->InitialWindow = this->CurrentWindow;
  this->InitialLevel = this->CurrentLevel;

  if (!found || path == nullptr)
  {
    this->State = sgextImagePlaneWidget::Outside;
    this->HighlightPlane(0);
    this->ActivateText(0);
    return;
  }
  else
  {
    this->State = sgextImagePlaneWidget::WindowLevelling;
    this->HighlightPlane(1);
    this->ActivateText(1);
    this->StartWindowLevelPositionX = X;
    this->StartWindowLevelPositionY = Y;
    this->ManageTextDisplay();
  }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();

  double wl[2] = { this->CurrentWindow, this->CurrentLevel };
  this->InvokeEvent(vtkCommand::StartWindowLevelEvent, wl);

  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void sgextImagePlaneWidget::OnMouseMove()
{
  // See whether we're active
  //
  if (this->State == sgextImagePlaneWidget::Outside || this->State == sgextImagePlaneWidget::Start)
  {
    return;
  }

  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  //
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z, vpn[3];

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  if (!camera)
  {
    return;
  }

  // Compute the two points defining the motion vector
  //
  this->ComputeWorldToDisplay(
    this->LastPickPosition[0], this->LastPickPosition[1], this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];

  this->ComputeDisplayToWorld(double(this->Interactor->GetLastEventPosition()[0]),
    double(this->Interactor->GetLastEventPosition()[1]), z, prevPickPoint);

  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

  if (this->State == sgextImagePlaneWidget::WindowLevelling)
  {
    this->WindowLevel(X, Y);
    this->ManageTextDisplay();
  }
  else if (this->State == sgextImagePlaneWidget::Pushing)
  {
    this->Push(prevPickPoint, pickPoint);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if (this->State == sgextImagePlaneWidget::Spinning)
  {
    this->Spin(prevPickPoint, pickPoint);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if (this->State == sgextImagePlaneWidget::Rotating)
  {
    camera->GetViewPlaneNormal(vpn);
    this->Rotate(prevPickPoint, pickPoint, vpn);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if (this->State == sgextImagePlaneWidget::Scaling)
  {
    this->Scale(prevPickPoint, pickPoint, X, Y);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if (this->State == sgextImagePlaneWidget::Moving)
  {
    this->Translate(prevPickPoint, pickPoint);
    this->UpdatePlane();
    this->UpdateMargins();
    this->BuildRepresentation();
  }
  else if (this->State == sgextImagePlaneWidget::Cursoring)
  {
    this->UpdateCursor(X, Y);
    this->ManageTextDisplay();
  }

  // Interact, if desired
  //
  this->EventCallbackCommand->SetAbortFlag(1);

  if (this->State == sgextImagePlaneWidget::WindowLevelling)
  {
    double wl[2] = { this->CurrentWindow, this->CurrentLevel };
    this->InvokeEvent(vtkCommand::WindowLevelEvent, wl);
  }
  else
  {
    this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
  }

  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void sgextImagePlaneWidget::ManageTextDisplay()
{
  if (!this->DisplayText)
  {
    return;
  }

  if (this->State == sgextImagePlaneWidget::WindowLevelling)
  {
    snprintf(this->TextBuff, VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF, "Window, Level: ( %g, %g )",
      this->CurrentWindow, this->CurrentLevel);
  }
  else if (this->State == sgextImagePlaneWidget::Cursoring)
  {
    if (this->CurrentImageValue == VTK_DOUBLE_MAX)
    {
      snprintf(this->TextBuff, VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF, "Off Image");
    }
    else
    {
      if(ImagesForCursorDisplay.empty()) {
        snprintf(this->TextBuff, VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF, "( %g, %g, %g ): %g",
          this->CurrentCursorPosition[0], this->CurrentCursorPosition[1],
          this->CurrentCursorPosition[2], this->CurrentImageValue);
      } else {
          int cx0 = snprintf(this->TextBuff, VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF, "index: ( %g, %g, %g )",
            this->CurrentCursorPosition[0], this->CurrentCursorPosition[1],
            this->CurrentCursorPosition[2]);

          int cx_acum = cx0;

          const size_t num_images = this->ImagesForCursorDisplay.size();
          for(size_t img_index = 0; img_index < num_images; ++img_index) {
            const std::string show_string = "\n " + this->ImagesNamesForCursorDisplay[img_index] + " : %g";
            cx_acum += snprintf(this->TextBuff + cx_acum,
                VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF - cx_acum,
                show_string.c_str(),
                this->CurrentImageValueForCursorDisplay[img_index]);
          }
      }
    }
  }

  this->TextActor->SetInput(this->TextBuff);
  this->TextActor->Modified();
}

//------------------------------------------------------------------------------
void sgextImagePlaneWidget::UpdateCursor(int X, int Y)
{
  if (!this->ImageData)
  {
    return;
  }
  // We're going to be extracting values with GetScalarComponentAsDouble(),
  // we might as well make sure that the data is there.  If the data is
  // up to date already, this call doesn't cost very much.  If we don't make
  // this call and the data is not up to date, the GetScalar... call will
  // cause a segfault.
  this->Reslice->GetInputAlgorithm()->Update();

  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

  this->CurrentImageValue = VTK_DOUBLE_MAX;

  int found = 0;
  int i;
  if (path)
  {
    // Deal with the possibility that we may be using a shared picker
    vtkCollectionSimpleIterator sit;
    path->InitTraversal(sit);
    vtkAssemblyNode* node;
    for (i = 0; i < path->GetNumberOfItems() && !found; i++)
    {
      node = path->GetNextNode(sit);
      if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
      {
        found = 1;
      }
    }
  }

  if (!found || path == nullptr)
  {
    this->CursorActor->VisibilityOff();
    return;
  }
  else
  {
    this->CursorActor->VisibilityOn();
  }

  double q[3];
  this->PlanePicker->GetPickPosition(q);

  if (this->UseContinuousCursor)
  {
    found = this->UpdateContinuousCursor(q);
  }
  else
  {
    found = this->UpdateDiscreteCursor(q);
  }

  if (!found)
  {
    this->CursorActor->VisibilityOff();
    return;
  }

  double o[3];
  this->PlaneSource->GetOrigin(o);

  // q relative to the plane origin
  //
  double qro[3];
  qro[0] = q[0] - o[0];
  qro[1] = q[1] - o[1];
  qro[2] = q[2] - o[2];

  double p1o[3];
  double p2o[3];

  this->GetVector1(p1o);
  this->GetVector2(p2o);

  double Lp1 = vtkMath::Dot(qro, p1o) / vtkMath::Dot(p1o, p1o);
  double Lp2 = vtkMath::Dot(qro, p2o) / vtkMath::Dot(p2o, p2o);

  double p1[3];
  this->PlaneSource->GetPoint1(p1);
  double p2[3];
  this->PlaneSource->GetPoint2(p2);

  double a[3];
  double b[3];
  double c[3];
  double d[3];

  for (i = 0; i < 3; i++)
  {
    a[i] = o[i] + Lp2 * p2o[i];  // left
    b[i] = p1[i] + Lp2 * p2o[i]; // right
    c[i] = o[i] + Lp1 * p1o[i];  // bottom
    d[i] = p2[i] + Lp1 * p1o[i]; // top
  }

  vtkPoints* cursorPts = this->CursorPolyData->GetPoints();

  cursorPts->SetPoint(0, a);
  cursorPts->SetPoint(1, b);
  cursorPts->SetPoint(2, c);
  cursorPts->SetPoint(3, d);
  cursorPts->GetData()->Modified();

  this->CursorPolyData->Modified();
}


//------------------------------------------------------------------------------
int sgextImagePlaneWidget::UpdateDiscreteCursor(double* q)
{
  // vtkImageData will find the nearest implicit point to q
  //
  vtkIdType ptId = this->ImageData->FindPoint(q);

  if (ptId == -1)
  {
    return 0;
  }

  double closestPt[3];
  this->ImageData->GetPoint(ptId, closestPt);

  double origin[3];
  this->ImageData->GetOrigin(origin);
  double spacing[3];
  this->ImageData->GetSpacing(spacing);
  int extent[6];
  this->ImageData->GetExtent(extent);

  int iq[3];
  int iqtemp;
  for (int i = 0; i < 3; i++)
  {
    // compute world to image coords
    iqtemp = static_cast<int>(std::round((closestPt[i] - origin[i]) / spacing[i]));

    // we have a valid pick already, just enforce bounds check
    iq[i] = (iqtemp < extent[2 * i]) ? extent[2 * i]
                                     : ((iqtemp > extent[2 * i + 1]) ? extent[2 * i + 1] : iqtemp);

    // compute image to world coords
    q[i] = iq[i] * spacing[i] + origin[i];

    this->CurrentCursorPosition[i] = iq[i];
  }

  this->CurrentImageValue =
    this->ImageData->GetScalarComponentAsDouble(static_cast<int>(this->CurrentCursorPosition[0]),
      static_cast<int>(this->CurrentCursorPosition[1]),
      static_cast<int>(this->CurrentCursorPosition[2]), 0);

  if (!ImagesForCursorDisplay.empty()) {
    const size_t num_images = this->ImagesForCursorDisplay.size();
    for(size_t img_index = 0; img_index < num_images; ++img_index) {
      auto & img = this->ImagesForCursorDisplay[img_index];
      auto & value = this->CurrentImageValueForCursorDisplay[img_index];
      value = img->GetScalarComponentAsDouble(static_cast<int>(this->CurrentCursorPosition[0]),
        static_cast<int>(this->CurrentCursorPosition[1]),
        static_cast<int>(this->CurrentCursorPosition[2]), 0);
    }
  }
  return 1;
}

void sgextImagePlaneWidget::AddImageForCursorDisplay(vtkImageData* input, const std::string & name)
{
  this->ImagesForCursorDisplay.push_back(input);
  this->ImagesNamesForCursorDisplay.push_back(name);
  this->CurrentImageValueForCursorDisplay.push_back(VTK_DOUBLE_MAX);
}
