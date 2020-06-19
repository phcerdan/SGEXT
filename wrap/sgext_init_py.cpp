/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
namespace py = pybind11;

void init_sgcore(py::module &);
#ifdef SG_MODULE_GENERATE_ENABLED
void init_sggenerate(py::module &);
#endif
#ifdef SG_MODULE_DYNAMICS_ENABLED
void init_sgdynamics(py::module &);
#endif
void init_sgextract(py::module &);
#ifdef SG_MODULE_ANALYZE_ENABLED
void init_sganalyze(py::module &);
#endif
#ifdef SG_MODULE_SCRIPTS_ENABLED
void init_sgscripts(py::module &);
void init_sgitk(py::module &);
#endif
#ifdef SG_MODULE_VISUALIZE_ENABLED
void init_sgvisualize(py::module &);
#endif
#ifdef SG_MODULE_LOCATE_ENABLED
void init_sglocate(py::module &);
#endif

PYBIND11_MODULE(_sgext, m) {
    m.doc() = "SGEXT, Spatial Graph Extraction, Analysis and Generation";
    init_sgcore(m);
#ifdef SG_MODULE_GENERATE_ENABLED
    init_sggenerate(m);
#endif
#ifdef SG_MODULE_DYNAMICS_ENABLED
    init_sgdynamics(m);
#endif

    init_sgextract(m);

#ifdef SG_MODULE_ANALYZE_ENABLED
    init_sganalyze(m);
#endif
#ifdef SG_MODULE_SCRIPTS_ENABLED
    init_sgscripts(m);
    init_sgitk(m);
#endif
#ifdef SG_MODULE_VISUALIZE_ENABLED
    init_sgvisualize(m);
#endif
#ifdef SG_MODULE_LOCATE_ENABLED
    init_sglocate(m);
#endif
}
