# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import compare
from sgext import locate
from sgext import fixtures
import unittest

class TestCompare(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.graph = fixtures.six_nodes().graph

    def test_extend_low_info_graph(self):
        print("test_extend_low_info_graph")
        graphs = [self.graph, self.graph]
        [vtk_merge_points, id_map] = locate.get_vtk_points_from_graphs(graphs)
        octree = locate.build_octree_locator(vtk_merge_points)
        radius = 0.5
        extended_graph = compare.extend_low_info_graph(
            graphs=graphs,
            id_map=id_map,
            octree=octree,
            radius=radius,
            verbose=True)
        # TODO: One edge is gone in the result? Why?
        # from sgext import scripts
        # scripts.view_spatial_graph(extended_graph)

    def test_add_graph_peninsulas(self):
        print("test_add_graph_peninsulas")
        graphs = [self.graph, self.graph]
        # result = compare.add_graph_peninsulas(
        #     graphs=graphs,
        #     extended_graph_index=0,
        #     high_info_graph_index=1,
        #     merge_points_locator=,
        #     id_map=id_map,
        #     radius_touch=radius_touch
        #     verbose=True)

