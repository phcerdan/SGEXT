# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
from sgext import visualize
from sgext import fixtures
import unittest
import os, tempfile

class TestConvertToVtkUnstructuredGrid(unittest.TestCase):
    @classmethod
    def setUp(cls):
        cls.graph = fixtures.six_nodes().graph
        cls.test_dir = tempfile.mkdtemp()

    def test_write_graph_to_vtk_unstructured_grid(self):
        filename = os.path.join(self.test_dir, "test1.vtu")
        print("filename= ", filename)
        visualize.write_graph_to_vtk_unstructured_grid(
            graph=self.graph,
            filename=filename,
            with_edge_points=False)
        self.assertTrue(os.path.exists(filename))
