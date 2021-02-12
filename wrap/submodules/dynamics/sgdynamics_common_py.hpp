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

#ifndef SGDYNAMICS_COMMON_PYBIND_HPP
#define SGDYNAMICS_COMMON_PYBIND_HPP

#include "particle.hpp"

#include "pybind11_common.h"
#include <pybind11/stl_bind.h>
// #include "bond.hpp"
// #include <memory>

PYBIND11_MAKE_OPAQUE(std::vector<SG::Particle>)
// PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<SG::Bond>>);
#endif
