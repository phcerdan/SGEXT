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
