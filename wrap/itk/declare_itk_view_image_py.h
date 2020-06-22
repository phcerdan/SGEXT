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

#ifndef DECLARE_ITK_VIEW_IMAGE_PY_H
#define DECLARE_ITK_VIEW_IMAGE_PY_H

#include "pybind11_common.h"

#include "view_image_function.hpp"

/** For now make copies of data to numpy arrays.
 * Some examples (pybind11 source code docs are non-existant)
 * From: https://github.com/pybind/pybind11/issues/323
 * Also: https://github.com/pybind/pybind11/issues/1042
 * dtypes: https://docs.scipy.org/doc/numpy/user/basics.types.html
 * py::dtype::of<TIMG::ObjectType::PixelType>(),
 */
template<typename TImage>
void declare_itk_view_image(pybind11::module &m, const std::string &typestr) {
    namespace py = pybind11;
    const std::string help_str =
        "Visualize image from input image of type: " + typestr + ".";
    m.def("view_image",
            py::overload_cast< const typename TImage::Pointer &, const std::string&,
                const size_t&, const size_t&>(&SG::view_image<TImage>),
            help_str.c_str(),
            py::arg("input"),
            py::arg("win_title") = SG::defaults::view_image_win_title,
            py::arg("win_x") = SG::defaults::view_image_win_width,
            py::arg("win_y") = SG::defaults::view_image_win_height
         );
}
#endif
