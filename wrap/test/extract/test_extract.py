# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

import _sgext.core as core
import _sgext.extract as extract
import unittest

class ThreeConnectedNodesGraphFixture:
    def __init__(self):
        self.graph = core.spatial_graph(6)
        # Set vertex positions
        v0 = self.graph.vertex(0);
        v0.pos = [0,0,0]
        self.graph.set_vertex(0, v0)
        v1 = self.graph.vertex(1);
        v1.pos = [1,1,0]
        self.graph.set_vertex(1, v1)
        v2 = self.graph.vertex(2);
        v2.pos = [1,0,1]
        self.graph.set_vertex(2, v2)
        # Set edges between three nodes
        core.graph.add_edge(0, 1, core.spatial_edge(), self.graph)
        core.graph.add_edge(0, 2, core.spatial_edge(), self.graph)
        core.graph.add_edge(1, 2, core.spatial_edge(), self.graph)
        # Add end-points (degree 1)
        v3 = self.graph.vertex(3);
        v3.pos = [-1,0,0]
        self.graph.set_vertex(3, v3)
        v4 = self.graph.vertex(4);
        v4.pos = [1,2,0]
        self.graph.set_vertex(4, v4)
        v5 = self.graph.vertex(5);
        v5.pos = [1,0,2]
        self.graph.set_vertex(5, v5)
        # Connect end-points
        core.graph.add_edge(0, 3, core.spatial_edge(), self.graph)
        core.graph.add_edge(1, 4, core.spatial_edge(), self.graph)
        core.graph.add_edge(2, 5, core.spatial_edge(), self.graph)

# The order of the test is important.
# They are run alphabetically, ie
# test_a_x will run before test_b_y
class TestSplitLoop(unittest.TestCase):
    def setUp(self):
        self.graph_loop = core.spatial_graph(1)
        self.ed = core.spatial_edge()
        self.ed.edge_points = [[0,0,0], [1,1,1]]
        core.graph.add_edge(0,0, self.ed, self.graph_loop)
    def test_split_loop(self):
        loop_vertex_id = 0
        # just testing interface, there is no loop
        extract.split_loop(loop_vertex_id, self.ed, self.graph_loop)
        self.assertEqual(self.graph_loop.num_vertices(), 2)
        self.assertEqual(self.graph_loop.num_edges(), 2)

class TestMergeNodes(unittest.TestCase):
    def setUp(self):
        fixture = ThreeConnectedNodesGraphFixture()
        self.graph = fixture.graph
        self.assertEqual(self.graph.num_vertices(), 6)
        self.assertEqual(self.graph.num_edges(), 6)

    def test_merge_nodes_in_place(self):
        inPlace = True
        nodes_merged = extract.merge_three_connected_nodes(self.graph, inPlace)
        self.assertEqual(nodes_merged, 2)
        self.assertEqual(self.graph.num_vertices(), 4)
        self.assertEqual(self.graph.num_edges(), 3)

class TestRemoveExtraEdges(unittest.TestCase):
    def setUp(self):
        fixture = ThreeConnectedNodesGraphFixture()
        self.graph = fixture.graph
    def test_remove_extra_edges_do_nothing(self):
        any_node_removed = extract.remove_extra_edges(self.graph)
        self.assertFalse(any_node_removed)

class TestReduceSpatialGraph(unittest.TestCase):
    def setUp(self):
        fixture = ThreeConnectedNodesGraphFixture()
        self.graph = fixture.graph
    def test_reduce_spatial_graph_to_same_graph(self):
        verbose = False
        reduced_graph = extract.reduce_spatial_graph_via_dfs(self.graph, verbose)
        self.assertEqual(reduced_graph.num_vertices(), self.graph.num_vertices())
        self.assertEqual(reduced_graph.num_edges(), self.graph.num_edges())
