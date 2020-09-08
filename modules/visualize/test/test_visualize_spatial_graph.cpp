/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "test_fixtures.hpp"

// TODO Workaround vtk_module_autoinit not working for test files.
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);

TEST_F(sg_3D, visualize_spatial_graph) { SG::visualize_spatial_graph(g); }

