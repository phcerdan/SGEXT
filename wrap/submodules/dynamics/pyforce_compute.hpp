/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
