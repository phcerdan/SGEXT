# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

import sgcore
import unittest

class TestGraph(unittest.TestCase):
    def test_spatial_node(self):
        # Constructor
        sn = sgcore.spatial_node()
        self.assertEqual(sn.id, 0)
        self.assertEqual(sn.pos, sgcore.array.array3d())
        norm = sgcore.array.norm(sn.pos)
        self.assertAlmostEqual(norm, 0.0)
        print(sn)

    def test_spatial_edge(self):
        se = sgcore.spatial_edge()
        arr1 = sgcore.array.array3d(1,1,1)
        se.edge_points = [arr1, arr1];
        self.assertEqual(len(se.edge_points),2)
        # iter works for vector and array3d
        for v in se.edge_points:
            for a in arr1:
                self.assertEqual(a, 1)
        print(se)

    def test_spatial_graph(self):
        graph = sgcore.spatial_graph()
        graph = sgcore.spatial_graph(2)
        [ed, added] = sgcore.graph.add_edge(0,1,sgcore.spatial_edge(), graph)
        self.assertEqual(ed.source, 0)
        self.assertEqual(ed.target, 1)
        se2 = sgcore.spatial_edge()
        arr0 = sgcore.array.array3d(0,0,0)
        arr1 = sgcore.array.array3d(1,1,1)
        se2.edge_points = [arr0, arr1];
        [ed, added] = sgcore.graph.add_edge(0,1, se2, graph)
        num_edge_points = sgcore.graph.num_edge_points(graph)
        self.assertEqual(num_edge_points, 2)
        print(graph)

    def test_spatial_graph_vertex_edge(self):
        graph = sgcore.spatial_graph(2)
        arr0 = sgcore.array.array3d(0,0,0)
        arr1 = sgcore.array.array3d(1,1,1)
        se2 = sgcore.spatial_edge()
        se2.edge_points = [arr0, arr1];
        [ed, added] = sgcore.graph.add_edge(0,1, se2, graph)

        v0 = graph.vertex(0)
        print(v0)
        self.assertAlmostEqual(v0.pos[0], arr0[0])
        e0 = graph.edge(ed)
        print(e0)
        self.assertEqual(e0.edge_points, se2.edge_points)

        # mutable test vertex
        graph.vertex(0).pos = arr1
        self.assertNotAlmostEqual(graph.vertex(0).pos[0], arr1[0])
        v0 = graph.vertex(0)
        v0.pos = arr1
        graph.set_vertex(0, v0)
        self.assertAlmostEqual(graph.vertex(0).pos[0], arr1[0])

        # mutable test edge
        ref = graph.edge(ed)
        self.assertEqual(len(ref.edge_points), 2)
        ref.edge_points = []
        self.assertEqual(len(ref.edge_points), 0)
        self.assertEqual(len(graph.edge(ed).edge_points), 2) # how to fix? use a set function
        graph.set_edge(ed, ref)
        self.assertEqual(len(graph.edge(ed).edge_points), 0)

if __name__ == '__main__':
    unittest.main()
