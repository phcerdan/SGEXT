/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
