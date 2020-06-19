# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
from sgext import locate
from sgext import fixtures
import unittest
import os, tempfile

class TestGetVtkPointsFromGraph(unittest.TestCase):
    @classmethod
    def setUp(cls):
        cls.graph = fixtures.six_nodes().graph

    def test_get_vtk_points_from_graph(self):
        [vtk_points, pg_map] = locate.get_vtk_points_from_graph(self.graph)
        print(vtk_points)
        self.assertEqual(vtk_points.get_number_of_points(), 9)

    def test_get_vtk_points_from_graphs(self):
        graphs = list([self.graph, self.graph])
        [vtk_merge_points, pg_map] = locate.get_vtk_points_from_graphs(graphs)
        print(vtk_merge_points)


