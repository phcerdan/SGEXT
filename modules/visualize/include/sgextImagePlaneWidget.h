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
  Module:    sgextImagePlaneWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   sgextImagePlaneWidget
 * @brief   3D widget for reslicing image data
 *
 * This 3D widget defines a plane that can be interactively placed in an
 * image volume. A nice feature of the object is that the
 * sgextImagePlaneWidget, like any 3D widget, will work with the current
 * interactor style. That is, if sgextImagePlaneWidget does not handle an
 * event, then all other registered observers (including the interactor
 * style) have an opportunity to process the event. Otherwise, the
 * sgextImagePlaneWidget will terminate the processing of the event that it
 * handles.
 *
 * The core functionality of the widget is provided by a vtkImageReslice
 * object which passes its output onto a texture mapping pipeline for fast
 * slicing through volumetric data. See the key methods: GenerateTexturePlane()
 * and UpdatePlane() for implementation details.
 *
 * To use this object, just invoke SetInteractor() with the argument of the
 * method a vtkRenderWindowInteractor.  You may also wish to invoke
 * "PlaceWidget()" to initially position the widget. If the "i" key (for
 * "interactor") is pressed, the sgextImagePlaneWidget will appear. (See
 * superclass documentation for information about changing this behavior.)
 *
 * Selecting the widget with the middle mouse button with and without holding
 * the shift or control keys enables complex reslicing capablilites.
 * To facilitate use, a set of 'margins' (left, right, top, bottom) are shown as
 * a set of plane-axes aligned lines, the properties of which can be changed
 * as a group.
 * Without keyboard modifiers: selecting in the middle of the margins
 * enables translation of the plane along its normal. Selecting one of the
 * corners within the margins enables spinning around the plane's normal at its
 * center.  Selecting within a margin allows rotating about the center of the
 * plane around an axis aligned with the margin (i.e., selecting left margin
 * enables rotating around the plane's local y-prime axis).
 * With control key modifier: margin selection enables edge translation (i.e., a
 * constrained form of scaling). Selecting within the margins enables
 * translation of the entire plane.
 * With shift key modifier: uniform plane scaling is enabled.  Moving the mouse
 * up enlarges the plane while downward movement shrinks it.
 *
 * Window-level is achieved by using the right mouse button.  Window-level
 * values can be reset by shift + 'r' or control + 'r' while regular reset
 * camera is maintained with 'r' or 'R'.
 * The left mouse button can be used to query the underlying image data
 * with a snap-to cross-hair cursor.  Currently, the nearest point in the input
 * image data to the mouse cursor generates the cross-hairs.  With oblique
 * slicing, this behaviour may appear unsatisfactory. Text display of
 * window-level and image coordinates/data values are provided by a text
 * actor/mapper pair.
 *
 * Events that occur outside of the widget (i.e., no part of the widget is
 * picked) are propagated to any other registered obsevers (such as the
 * interaction style). Turn off the widget by pressing the "i" key again
 * (or invoke the Off() method). To support interactive manipulation of
 * objects, this class invokes the events StartInteractionEvent,
 * InteractionEvent, and EndInteractionEvent as well as StartWindowLevelEvent,
 * WindowLevelEvent, EndWindowLevelEvent and ResetWindowLevelEvent.
 *
 * The sgextImagePlaneWidget has several methods that can be used in
 * conjunction with other VTK objects. The GetPolyData() method can be used
 * to get the polygonal representation of the plane and can be used as input
 * for other VTK objects. Typical usage of the widget is to make use of the
 * StartInteractionEvent, InteractionEvent, and EndInteractionEvent
 * events. The InteractionEvent is called on mouse motion; the other two
 * events are called on button down and button up (either left or right
 * button).
 *
 * Some additional features of this class include the ability to control the
 * properties of the widget. You can set the properties of: the selected and
 * unselected representations of the plane's outline; the text actor via its
 * vtkTextProperty; the cross-hair cursor. In addition there are methods to
 * constrain the plane so that it is aligned along the x-y-z axes.  Finally,
 * one can specify the degree of interpolation (vtkImageReslice): nearest
 * neighbour, linear, and cubic.
 *
 * A simpler version of this widget is the combination of
 * vtkImplicitPlaneWidget2 combined with the representation
 * vtkImplicitImageRepresentation.
 *
 * @par Thanks:
 * Thanks to Dean Inglis for developing and contributing this class.
 * Based on the Python SlicePlaneFactory from Atamai, Inc.
 *
 * @sa
 * vtk3DWidget vtkBoxWidget vtkLineWidget  vtkPlaneWidget vtkPointWidget
 * vtkPolyDataSourceWidget vtkSphereWidget vtkImplicitPlaneWidget
 * vtkImplicitPlaneWidget2 vtkImplicitImageRepresentation
 */

#ifndef sgextImagePlaneWidget_h
#define sgextImagePlaneWidget_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkImagePlaneWidget.h"
#include <vector>

/** Input to this widget should be a blend image.
 * It has methods to display the values of the original images (before the blend)
 */
class VTKINTERACTIONWIDGETS_EXPORT sgextImagePlaneWidget : public vtkImagePlaneWidget
{
public:
  /**
   * Instantiate the object.
   */
  static sgextImagePlaneWidget* New();

  vtkTypeMacro(sgextImagePlaneWidget, vtkImagePlaneWidget)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Only ManageTextDisplay and UpdateDiscreteCursor are modified.
  // But they are not virtual, so we copy/override the methods that use them.
  int UpdateDiscreteCursor(double* q);
  void ManageTextDisplay();
  void UpdateCursor(int, int);
  void StartCursor() override;
  void StartWindowLevel() override;
  void OnMouseMove() override;

  /// These methods handle the extra logic to display multiple values
  // from the original images that created the blend (input image).
  void AddImageForCursorDisplay(vtkImageData* input, const std::string & name);
  std::vector<vtkImageData*> ImagesForCursorDisplay;
  std::vector<double> CurrentImageValueForCursorDisplay;
  std::vector<std::string> ImagesNamesForCursorDisplay;

  // Make text actor public
  using vtkImagePlaneWidget::TextActor;

protected:
  sgextImagePlaneWidget() = default;
  ~sgextImagePlaneWidget() override = default;

private:
  sgextImagePlaneWidget(const sgextImagePlaneWidget&) = delete;
  void operator=(const sgextImagePlaneWidget&) = delete;
};

#endif
