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

#ifndef SGLOCATE_COMMON_H
#define SGLOCATE_COMMON_H

#include "pybind11_common.h"

#include <vtkSmartPointer.h>

// Only needed if the type's `.get()` goes by another name
namespace pybind11 { namespace detail {
    template <typename T>
    struct holder_helper<vtkSmartPointer<T>> { // <-- specialization
        static const T *get(const vtkSmartPointer<T> &p) { return p.GetPointer(); }
    };
}}

PYBIND11_DECLARE_HOLDER_TYPE(VtkType, vtkSmartPointer<VtkType>)

#endif
