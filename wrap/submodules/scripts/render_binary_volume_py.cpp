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

#include "pybind11_common.h"

#include "render_binary_volume.hpp"
#include "visualize_common.hpp" // for SG::flip_camera
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include "view_image_function.hpp" // for defaults::view_image_win_width

namespace py = pybind11;

void init_render_binary_volume(py::module &m) {
    m.def("render_binary_volume",
        [](const SG::BinaryImageType::Pointer & binary_image,
           const unsigned char label,
           const double opacity,
           const std::string & winTitle,
           const size_t & winWidth,
           const size_t & winHeight
           ) {
        auto volume_actor = SG::volume_actor_for_binary_image(binary_image, label);
        // Render the actor with VTK window
        vtkNew<vtkRenderer> renderer ;
        vtkNew<vtkRenderWindow> renderWindow ;
        renderWindow->SetWindowName((winTitle + " label: " + std::to_string(label)).c_str());
        renderWindow->SetSize(winWidth, winHeight);
        renderWindow->AddRenderer(renderer);
        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor ;
        renderWindowInteractor->SetRenderWindow(renderWindow);

        renderer->AddVolume(volume_actor);

        // Flip camera because VTK-ITK different corner for origin.
        vtkCamera *cam = renderer->GetActiveCamera();
        SG::flip_camera(cam);
        renderer->ResetCamera();

        renderWindow->Render();
        renderWindowInteractor->Start();
        return EXIT_SUCCESS;
        }, py::arg("input"), py::arg("label") = 255, py::arg("opacity") = 0.8,
        py::arg("win_title") = "SGEXT render_binary_volume",
        py::arg("win_width") = SG::defaults::view_image_win_width,
        py::arg("win_height") = SG::defaults::view_image_win_height
        );
}
