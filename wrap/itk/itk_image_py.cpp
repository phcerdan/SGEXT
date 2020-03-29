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

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "declare_itk_image_py.h"

namespace py = pybind11;

using IUC3P = itk::Image<unsigned char, 3>::Pointer;
using IF3P = itk::Image<float, 3>::Pointer;
void init_itk_image(py::module &m) {
    declare_itk_image_ptr<IUC3P>(m, "IUC3P");
    declare_itk_image_ptr<IF3P>(m, "IF3P");

}
