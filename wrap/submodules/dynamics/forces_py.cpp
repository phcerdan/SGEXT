/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"

#include "bonded_forces.hpp"
#include "unbonded_forces.hpp"

namespace py = pybind11;
using namespace SG;

void init_unbonded_forces(py::module &m);
void init_bonded_forces(py::module &m);

void init_forces(py::module &mparent) {
    auto m = mparent.def_submodule("forces");
    init_bonded_forces(m);
    init_unbonded_forces(m);
}

void init_bonded_forces(py::module &m) {
    m.def("ev_wlc_low_force_normalized", &force_extension_ev_wlc_low_force_normalized);
    m.def("ev_wlc_high_force_normalized", &force_extension_ev_wlc_high_force_normalized);
    m.def("ev_wlc_normalized", &force_extension_ev_wlc_normalized);

    m.def("low_regime_ideal_chain_normalized", &force_extension_low_regime_ideal_chain_normalized);
    m.def("low_regime_excluded_volume_pincus_normalized", &force_extension_low_regime_excluded_volume_pincus_normalized);

    m.def("fjc_pretrosyan_normalized", &force_extension_fjc_petrosyan_normalized);
    m.def("wlc_pretrosyan_normalized", &force_extension_wlc_petrosyan_normalized);
    m.def("wlc_pretrosyan", &force_extension_wlc_petrosyan);

    m.def("wlc_marko_siggia_normalized", &force_extension_wlc_marko_siggia_normalized);
    m.def("wlc_marko_siggia", &force_extension_wlc_marko_siggia);

    m.def("extension_force_wlc_pretrosyan", &extension_force_wlc_petrosyan);
}

void init_unbonded_forces(py::module &m) {
    m.def("linear_drag", py::overload_cast<double,
            const ArrayUtilities::Array3D &>(&force_linear_drag),
            "Use damping parameter and velocity of particle.");
    m.def("linear_drag", py::overload_cast<double,
            double, const ArrayUtilities::Array3D &>(&force_linear_drag),
            "Use Stokes relationship, with radius, viscosity and velocity.");
}

