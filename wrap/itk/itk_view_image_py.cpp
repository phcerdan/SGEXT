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
