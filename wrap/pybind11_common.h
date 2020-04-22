/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file should be included in all cpp files using pybind11
// to avoid -Wodr violation (Warning).
// See https://github.com/pybind/pybind11/issues/1055
#ifndef PYBIND11_COMMON_H
#define PYBIND11_COMMON_H

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include "core/sgcore_common_py.hpp" // OPAQUE std::array<double, 3>

#endif
