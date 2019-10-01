/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGCORE_COMMON_PYBIND_HPP
#define SGCORE_COMMON_PYBIND_HPP

#include <pybind11/stl_bind.h>
PYBIND11_MAKE_OPAQUE(std::array<double, 3>);
#endif
