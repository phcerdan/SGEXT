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

#ifndef VISUALIZE_COMMON_HPP
#define VISUALIZE_COMMON_HPP
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkImageData.h>
#include <vtkTexturedButtonRepresentation2D.h>
#include <vtkSmartPointer.h>

#include <array>

namespace SG {
/**
 * Flip camera because VTK-ITK different corner for origin.
 *
 * @param cam
 */
void flip_camera(vtkCamera *cam);

/**
 * Create an actor to visualize points as cubes.
 *
 * @param points
 * @param inputOpacity
 *
 * @return
 */
vtkSmartPointer<vtkActor> create_actor_visualize_points_as_cubes(
        /* const */ vtkPoints *points,
        const double inputOpacity = 0.8,
        const std::array<double, 3> cube_length = {{1.0, 1.0, 1.0}});

/**
 * Create an image texture representing an ON/OFF button.
 *
 * @param color of the circular button (background will be always transparent)
 * @param dimensions std::array with dimensions of the button (default to 60,60)
 *
 * @return vtkImageData
 */
vtkSmartPointer<vtkImageData>
create_texture_for_button(unsigned char color[4],
                          const std::array<int, 2> &dimensions = {{60, 60}});

std::array<double, 6>
create_bounds_for_button(const double button_size);

vtkSmartPointer<vtkTexturedButtonRepresentation2D>
create_on_off_representation_for_button(
        vtkImageData *texture_off,
        vtkImageData *texture_on,
        std::array<double, 6> & button_bounds);

vtkSmartPointer<vtkCaptionActor2D>
create_caption_actor_for_button(
        const double button_size = 30.0,
        const std::string & caption_init = "Off|On",
        const double pad = 3);


/**
 * Callback. Press "s" to toggle textured button.
 * To be used through vtkCallbackCommand.
 *
 * SetClientData of vtkCallbackCommand must be set to an instance of
 * button_widget, with a representation of type vtkTexturedButtonRepresentation2D.
 *
 * Example:
 *
    auto button_keypress_callback = vtkSmartPointer<vtkCallbackCommand>::New();
    button_keypress_callback->SetClientData(button_widget.GetPointer());
    button_keypress_callback->SetCallback(toggleTexturedButtonCallbackFunction);
    renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, button_keypress_callback);
 *
 * @param caller a RenderWindowInteractor*
 * @param eventId unused
 * @param clientData a button_widget*
 * @param callData unused
 */
void toggleTexturedButtonCallbackFunction(
            vtkObject* caller /* renderWindowInteractor* */,
            long unsigned int eventId /*unused */,
            void* clientData /* button_widget */,
            void* callData /*unused*/ );

} // namespace SG
#endif
