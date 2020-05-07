# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
import unittest

class TestGraph(unittest.TestCase):
    def test_spatial_node(self):
        print("test_spatial_node")
        # Constructor
        sn = core.spatial_node()
        self.assertEqual(sn.id, 0)
        self.assertEqual(sn.pos, [0,0,0])
        norm = core.array.norm(sn.pos)
        self.assertAlmostEqual(norm, 0.0)
        self.assertEqual(sn.pos[0], 0)
        # invalid to modify a readwrite property with  a reference argument
        # price to pay for non MAKE_OPAQUE
        sn.pos[0] = 3.0
        self.assertNotEqual(sn.pos[0], 3.0)
        sn.set_pos(0, 3.0)
        self.assertEqual(sn.pos[0], 3.0)
        print(sn)
        print("end test_spatial_node")

    def test_spatial_edge(self):
        print("test_spatial_edge")
        se = core.spatial_edge()
        arr1 = [1,1,1]
        se.edge_points = [arr1, arr1];
        self.assertEqual(len(se.edge_points),2)
        # iter works for vector and array3d
        for v in se.edge_points:
            for a in arr1:
                self.assertEqual(a, 1)
        print(se)
        print("end test_spatial_edge")

    def test_edge_descriptor(self):
        print("test_edge_descriptor")
        ed0 = core.graph.edge_descriptor()
        # self.assertNotEqual(ed0.source, 0)
        ed0.source = 2
        self.assertEqual(ed0.source, 2)

        # v0 = core.graph.vertex_descriptor()
        # ed2 = core.graph.edge_descriptor(2, 3)
        # self.assertEqual(ed0.source, 2)
        # self.assertEqual(ed0.target, 3)

        print("end test_edge_descriptor")

    def test_spatial_graph(self):
        print("test_spatial_graph")
        graph = core.spatial_graph()
        graph = core.spatial_graph(2)
        [ed, added] = core.graph.add_edge(0,1,core.spatial_edge(), graph)
        self.assertEqual(ed.source, 0)
        self.assertEqual(ed.target, 1)
        se2 = core.spatial_edge()
        arr0 = [0,0,0]
        arr1 = [1,1,1]
        se2.edge_points = [arr0, arr1];
        [ed, added] = core.graph.add_edge(0,1, se2, graph)
        num_edge_points = core.graph.num_edge_points(graph)
        self.assertEqual(num_edge_points, 2)
        self.assertEqual(graph.num_vertices(), 2)
        self.assertEqual(graph.num_edges(), 2)
        self.assertEqual(graph.num_edge_points(), 2)
        print(graph)
        print("end test_spatial_graph")

    def test_spatial_graph_source_target(self):
        print("test_spatial_graph_source_target")
        graph = core.spatial_graph(2)
        [ed, added] = core.graph.add_edge(0,1,core.spatial_edge(), graph)
        self.assertEqual(graph.source(ed), 0)
        self.assertEqual(graph.target(ed), 1)
        self.assertEqual(core.graph.source(ed, graph), 0)
        self.assertEqual(core.graph.target(ed, graph), 1)
        print("end test_spatial_graph_source_target")

    def test_spatial_graph_vertices(self):
        print("test_spatial_graph_vertices")
        graph = core.spatial_graph(2)
        [ed, added] = core.graph.add_edge(0,1,core.spatial_edge(), graph)
        vertices = graph.vertices()
        print(vertices)
        self.assertEqual(vertices[0], 0)
        self.assertEqual(vertices[1], 1)
        self.assertEqual(core.graph.vertices(graph), vertices)
        print("end test_spatial_graph_vertices")

    def test_spatial_graph_edges(self):
        print("test_spatial_graph_edges")
        graph = core.spatial_graph(2)
        [ed, added] = core.graph.add_edge(0,1,core.spatial_edge(), graph)
        edges = graph.edges()
        print(edges)
        self.assertEqual(edges[0].source, 0)
        self.assertEqual(edges[0].target, 1)
        self.assertEqual(core.graph.edges(graph), edges)
        print("end test_spatial_graph_edges")


    def test_spatial_graph_out_edges(self):
        print("test_spatial_graph_out_edges")
        graph = core.spatial_graph(2)
        [ed, added] = core.graph.add_edge(0,1,core.spatial_edge(), graph)
        oe = graph.out_edges(0)
        self.assertEqual(len(oe), 1)
        self.assertEqual(oe[0].source, 0)
        self.assertEqual(oe[0].target, 1)
        oe_func = core.graph.out_edges(1, graph)
        self.assertEqual(len(oe_func), 1)
        self.assertEqual(oe_func[0].source, 1)
        self.assertEqual(oe_func[0].target, 0)
        print("end test_spatial_graph_out_edges")

    def test_spatial_graph_vertex_edge(self):
        print("test_spatial_graph_vertex_edge")
        graph = core.spatial_graph(2)
        arr0 = [0,0,0]
        arr1 = [1,1,1]
        se2 = core.spatial_edge()
        se2.edge_points = [arr0, arr1];
        [ed, added] = core.graph.add_edge(0,1, se2, graph)

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
        # edge_points of the graph is unmodified. To modify use the set_edge function
        self.assertEqual(len(graph.edge(ed).edge_points), 2)
        graph.set_edge(ed, ref)
        self.assertEqual(len(graph.edge(ed).edge_points), 0)
        print("end test_spatial_graph_vertex_edge")

    def test_get_all_points(self):
        print("test_get_all_points")
        graph = core.spatial_graph(2)
        arr0 = [0,0,0]
        v0 = graph.vertex(0)
        v0.pos = arr0
        graph.set_vertex(0, v0)
        arr3 = [3,3,3]
        v1 = graph.vertex(1)
        v1.pos = arr3
        graph.set_vertex(1, v1)
        arr1 = [1,1,1]
        arr2 = [2,2,2]
        se2 = core.spatial_edge()
        se2.edge_points = [arr1, arr2];
        [ed, added] = core.graph.add_edge(0,1, se2, graph)
        [points, descriptors] = core.graph.get_all_points(graph)
        num_points = 4
        print("print points:\n", points)
        self.assertEqual(len(points), num_points)
        self.assertAlmostEqual(points[0][0], 0)
        self.assertAlmostEqual(points[1][0], 3.)
        self.assertAlmostEqual(points[2][0], 1.)
        self.assertAlmostEqual(points[3][0], 2.)
        self.assertEqual(len(descriptors), num_points)
        self.assertTrue(descriptors[0].is_vertex)
        self.assertTrue(descriptors[1].is_vertex)
        self.assertTrue(descriptors[2].is_edge)
        self.assertEqual(descriptors[2].edge_points_index, 0)
        self.assertTrue(descriptors[3].is_edge)
        self.assertEqual(descriptors[3].edge_points_index, 1)
        print("end test_get_all_points")


if __name__ == '__main__':
    unittest.main()
