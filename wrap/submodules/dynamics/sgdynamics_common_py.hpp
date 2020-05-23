/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGDYNAMICS_COMMON_PYBIND_HPP
#define SGDYNAMICS_COMMON_PYBIND_HPP

#include "particle.hpp"

#include "pybind11_common.h"
#include <pybind11/stl_bind.h>
// #include "bond.hpp"
// #include <memory>

PYBIND11_MAKE_OPAQUE(std::vector<SG::Particle>);
// PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<SG::Bond>>);
#endif
