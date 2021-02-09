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

#include "visualize_common.hpp"

#include <vtkButtonWidget.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

namespace SG {

void flip_camera(vtkCamera *cam) {
    double pos[3];
    double vup[3];
    cam->GetPosition(pos);
    cam->GetViewUp(vup);
    for (unsigned int i = 0; i < 3; ++i) {
        pos[i] = -pos[i];
        vup[i] = -vup[i];
    }
    cam->SetPosition(pos);
    cam->SetViewUp(vup);
}

vtkSmartPointer<vtkActor> create_actor_visualize_points_as_cubes(
        /* const */ vtkPoints *points,
        const double inputOpacity,
        const std::array<double, 3> cube_length) {
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    vtkSmartPointer<vtkPolyData> glyph = vtkSmartPointer<vtkPolyData>::New();

    // Create anything you want here, we will use a cube for the demo.
    vtkSmartPointer<vtkCubeSource> cubeSource =
            vtkSmartPointer<vtkCubeSource>::New();
    cubeSource->SetXLength(cube_length[0]);
    cubeSource->SetYLength(cube_length[1]);
    cubeSource->SetZLength(cube_length[2]);

    vtkSmartPointer<vtkGlyph3DMapper> glyph3Dmapper =
            vtkSmartPointer<vtkGlyph3DMapper>::New();
    glyph3Dmapper->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3Dmapper->SetInputData(polydata);
    glyph3Dmapper->Update();

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetOpacity(inputOpacity);
    actor->SetMapper(glyph3Dmapper);

    return actor;
}

vtkSmartPointer<vtkImageData>
create_texture_for_button(unsigned char color[4],
                          const std::array<int, 2> &dimensions) {
    auto image = vtkSmartPointer<vtkImageData>::New();
    image->SetDimensions(dimensions[0], dimensions[1], 1);
    int center[2];
    center[0] = dimensions[0] / 2;
    center[1] = dimensions[1] / 2;
    const int radius = std::min(center[0], center[1]) - 1;
    const int radiusSquared = radius * radius;

    image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);

    int *dims = image->GetDimensions();

    // Fill the image with
    for (int y = 0; y < dims[1]; y++) {
        for (int x = 0; x < dims[0]; x++) {
            auto *pixel = static_cast<unsigned char *>(
                    image->GetScalarPointer(x, y, 0));
            if ((x - center[0]) * (x - center[0])  +
                (y - center[1]) * (y - center[1]) < radiusSquared) {
                pixel[0] = color[0];
                pixel[1] = color[1];
                pixel[2] = color[2];
                pixel[3] = color[3];
            } else {
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
                pixel[3] = 0; // transparent
            }
        }
    }
    return image;
}

std::array<double, 6>
create_bounds_for_button(const double button_size = 30.0) {
    std::array<double, 6> bounds;
    bounds[0] = 0.0;
    bounds[1] = button_size;
    bounds[2] = 0.0;
    bounds[3] = button_size;
    bounds[4] = bounds[5] = 0.0;
    return bounds;
}

vtkSmartPointer<vtkTexturedButtonRepresentation2D>
create_on_off_representation_for_button(
        vtkImageData *texture_off,
        vtkImageData *texture_on,
        std::array<double, 6> &bounds) {
    auto button_rep = vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
    button_rep->SetNumberOfStates(2);
    button_rep->SetButtonTexture(0, texture_off);
    button_rep->SetButtonTexture(1, texture_on);
    button_rep->SetPlaceFactor(1);
    button_rep->PlaceWidget(bounds.data());
    return button_rep;
}

vtkSmartPointer<vtkCaptionActor2D>
create_caption_actor_for_button(
        const double button_size,
        const std::string & caption_init,
        const double pad)
{
    auto caption_actor = vtkSmartPointer<vtkCaptionActor2D>::New();
    caption_actor->SetCaption(caption_init.c_str());
    caption_actor->SetDisplayPosition(button_size, button_size / 5.0);
    auto *attach_point = caption_actor->GetAttachmentPointCoordinate();
    attach_point->SetCoordinateSystemToDisplay();
    caption_actor->BorderOff();
    caption_actor->LeaderOff();
    caption_actor->ThreeDimensionalLeaderOff();
    // caption_actor->GetCaptionTextProperty()->SetFontSize(5);
    caption_actor->GetCaptionTextProperty()->BoldOff();
    caption_actor->GetCaptionTextProperty()->ItalicOff();
    caption_actor->GetCaptionTextProperty()->ShadowOn();
    caption_actor->GetTextActor()->SetTextScaleModeToNone();
    return caption_actor;
}

void toggleTexturedButtonCallbackFunction(
            vtkObject* caller,
            long unsigned int /*eventId*/,
            void* clientData /* button_widget */,
            void* /* callData*/ )
{
    auto * iren = static_cast<vtkRenderWindowInteractor*>(caller);
    auto * button_widget = static_cast<vtkButtonWidget*>(clientData);
    if(iren->GetKeyCode() == 's' || iren->GetKeyCode() == 'S') {
        static_cast<vtkTexturedButtonRepresentation2D*>(button_widget->GetRepresentation())->NextState();
        button_widget->InvokeEvent(vtkCommand::StateChangedEvent, nullptr);
        }
    };

} // namespace SG
