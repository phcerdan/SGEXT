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

#ifndef DECLARE_VIEW_IMAGE_WITH_LABEL_PY_H
#define DECLARE_VIEW_IMAGE_WITH_LABEL_PY_H

#include "pybind11_common.h"

#include "visualize_with_label_image.hpp"

template <typename TImage, typename TLabelImage>
void declare_view_image_with_label(pybind11::module &m) {
    namespace py = pybind11;
    const std::string help_str =
R"(Visualize input image along an associated label image with input label_opacity.
Parameters:
----------
input: FloatImageType or BinaryImageType
    input image (any type)

label_image: BinaryImageType
    associated binary/label image

label_opacity: float
    opacity used to superimpose (blend) the label_image on top of the input image.

win_title: str
    Title of the render window

win_x: int
    width of the render window

win_y: int
    height of the render window
)";
    m.def("view_image_with_label",
            [](const typename TImage::Pointer &input,
               const typename TLabelImage::Pointer &label_image,
               const double &label_opacity,
               const std::string &win_title,
               const size_t &win_x, const size_t &win_y) {
                return SG::view_image_with_label<TImage, TLabelImage>(
                        input, label_image,
                        label_opacity,
                        win_title, win_x, win_y);
            },
            help_str.c_str(),
            py::arg("input"), py::arg("label_image"),
            py::arg("label_opacity") = 0.8,
            py::arg("win_title") = SG::defaults::view_image_win_title,
            py::arg("win_x") = SG::defaults::view_image_win_width,
            py::arg("win_y") = SG::defaults::view_image_win_height);
}

#endif
