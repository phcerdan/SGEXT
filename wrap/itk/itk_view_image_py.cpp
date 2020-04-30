/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgitk_common_py.hpp"
#include "view_image_function.hpp"
#include "declare_itk_view_image_py.h"

namespace py = pybind11;

void init_itk_view_image(py::module &m) {
    using TImageFromFile = SG::IF3;
    m.def("view_image",
            py::overload_cast< const std::string&, const std::string&,
                const size_t&, const size_t&>(&SG::view_image<TImageFromFile>),
            "Visualize image from file.\n"
            "To maximize usability among all types of images, "
            "it treats the input image as having float pixels.",
            py::arg("input_file"),
            py::arg("win_title") = SG::defaults::view_image_win_title,
            py::arg("win_width") = SG::defaults::view_image_win_width,
            py::arg("win_height") = SG::defaults::view_image_win_height
         );

    declare_itk_view_image<SG::IF3>(m, "float (IF3)");
    declare_itk_view_image<SG::IUC3>(m, "unsigned char (IUC3)");
}
