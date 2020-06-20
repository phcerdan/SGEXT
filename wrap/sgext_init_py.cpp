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
#ifdef SG_MODULE_COMPARE_ENABLED
void init_sgcompare(py::module &);
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
#ifdef SG_MODULE_COMPARE_ENABLED
    init_sgcompare(m);
#endif
}
