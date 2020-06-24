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

namespace py = pybind11;
void init_itk_image(py::module &);
void init_itk_image_io(py::module &);
void init_itk_view_image(py::module &);
void init_transform_to_physical_point_with_itk(py::module &);

void init_sgitk(py::module & mparent) {
    auto m = mparent.def_submodule("itk");
    m.doc() = "ITK wrapping of image "; // optional module docstring
    init_itk_image(m);
    init_itk_image_io(m);
    init_itk_view_image(m);
    init_transform_to_physical_point_with_itk(m);
}
