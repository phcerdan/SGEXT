/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

PYBIND11_DECLARE_HOLDER_TYPE(VtkType, vtkSmartPointer<VtkType>);

#endif
