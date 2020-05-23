/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_PYINTEGRATOR_HPP
#define SG_PYINTEGRATOR_HPP

#include "sgdynamics_common_py.hpp"
#include "integrator.hpp"
// Trampolins for pure virtual classes

namespace SG {
struct PyIntegratorMethod : public IntegratorMethod {
    /* Inherit the constructors */
    using IntegratorMethod::IntegratorMethod;
    /* Trampoline (need one for each virtual function) */
    void integrate() override {
        PYBIND11_OVERLOAD_PURE(
                void,             /* Return type */
                IntegratorMethod, /* Parent class */
                integrate /* Name of function in C++ (must match Python name) */
        );
    }
};

struct PyTwoStepIntegratorMethod : public TwoStepIntegratorMethod {
    /* Inherit the constructors */
    using TwoStepIntegratorMethod::TwoStepIntegratorMethod;

    /* Trampoline (need one for each virtual function) */
    void integrate() override {
        PYBIND11_OVERLOAD_PURE(
                void,                    /* Return type */
                TwoStepIntegratorMethod, /* Parent class */
                integrate /* Name of function in C++ (must match Python name) */
        );
    }
    void integrateStepOne() override {
        PYBIND11_OVERLOAD_PURE(void,                    /* Return type */
                               TwoStepIntegratorMethod, /* Parent class */
                               integrateStepOne 
        );
    }
    void integrateStepTwo() override {
        PYBIND11_OVERLOAD_PURE(void,                    /* Return type */
                               TwoStepIntegratorMethod, /* Parent class */
                               integrateStepTwo
        );
    }
};

struct PyIntegrator : public Integrator {
    /* Inherit the constructors */
    using Integrator::Integrator;
    /* Trampoline (need one for each virtual function) */
    void update(unsigned int time_step) override {
        PYBIND11_OVERLOAD_PURE(
                void,       /* Return type */
                Integrator, /* Parent class */
                update, /* Name of function in C++ (must match Python name) */
                time_step);
    }

    void compute_net_forces(System *sys) {
        PYBIND11_OVERLOAD(void,               /* Return type */
                          Integrator,         /* Parent class */
                          compute_net_forces,
                          sys);
    }
};

} // end namespace SG
#endif
