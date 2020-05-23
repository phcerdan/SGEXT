/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "integrator.hpp"
#include "pyintegrator.hpp" // Trampolins for pure virtual classes
#include "sgdynamics_common_py.hpp"

namespace py = pybind11;
using namespace SG;

void init_integrator_methods(py::module &m) {
    // pure virtual:
    py::class_<IntegratorMethod, PyIntegratorMethod,
        std::shared_ptr<IntegratorMethod>>(m, "integrator_method")
        .def(py::init<System*>())
        .def(py::init<System*, double>())
        .def("integrate", &IntegratorMethod::integrate)
        .def_readwrite("deltaT", &IntegratorMethod::deltaT);
    py::class_<TwoStepIntegratorMethod, PyTwoStepIntegratorMethod,
        std::shared_ptr<TwoStepIntegratorMethod>>(m, "integrator_method_two_step")
        .def(py::init<System*>())
        .def(py::init<System*, double>())
        .def("integrateStepOne", &TwoStepIntegratorMethod::integrateStepOne)
        .def("integrateStepTwo", &TwoStepIntegratorMethod::integrateStepTwo)
        ;

    py::class_<VerletVelocitiesIntegratorMethod, TwoStepIntegratorMethod,
        std::shared_ptr<VerletVelocitiesIntegratorMethod>>(m, "integrator_method_verlet_velocities")
        .def(py::init<System*>())
        .def(py::init<System*, double>())
        // .def_readwrite("deltaT", &VerletVelocitiesIntegratorMethod::deltaT)
        ;
}

void init_integrator(py::module &m) {
    // pure virtual
    py::class_<Integrator, PyIntegrator>(m, "integrator")
        .def(py::init<System*>())
        .def_readwrite("force_types", &Integrator::force_types)
        .def("compute_net_forces", &Integrator::compute_net_forces)
        .def("update", &Integrator::update)
        // .def("add_force", &Integrator::add_force)
        // TODO wrap forces first
        .def("add_force",  py::overload_cast<std::shared_ptr<ForceCompute>>(&Integrator::add_force<ForceCompute>))
        ;

    py::class_<IntegratorTwoStep, Integrator>(m, "integrator_two_step")
        .def(py::init<System*>())
        .def_readwrite("integrator_method", &IntegratorTwoStep::integrator_method)
        ;
}
