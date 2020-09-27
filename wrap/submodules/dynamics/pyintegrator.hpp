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

    void compute_net_forces(System *sys) const override {
        PYBIND11_OVERLOAD(void,               /* Return type */
                          Integrator,         /* Parent class */
                          compute_net_forces,
                          sys);
    }
};

} // end namespace SG
#endif
