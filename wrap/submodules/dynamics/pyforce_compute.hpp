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

#ifndef SG_PYFORCE_COMPUTE_HPP
#define SG_PYFORCE_COMPUTE_HPP

#include "sgdynamics_common_py.hpp"
#include "force_compute.hpp"

namespace SG {

struct PyForceCompute : public ForceCompute {
    /* Inherit the constructors */
    using ForceCompute::ForceCompute;
    /* Trampoline (need one for each virtual function) */
    void compute() override {
        PYBIND11_OVERLOAD_PURE(
                void,             /* Return type */
                ForceCompute, /* Parent class */
                compute /* Name of function in C++ (must match Python name) */
        );
    }
    std::string get_type() override {
        PYBIND11_OVERLOAD(
                std::string,             /* Return type */
                ForceCompute, /* Parent class */
                get_type /* Name of function in C++ (must match Python name) */
        );
    }
};
} // end namespace SG
#endif
