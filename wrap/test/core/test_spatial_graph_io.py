# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
import unittest

# The order of the test is important.
# They are run alphabetically, ie
# test_a_x will run before test_b_y
# we write the files before reading them
class TestSpatialGraphIo(unittest.TestCase):
    def setUp(self):
        self.graphviz_file = "graphviz_out.dot"
        self.serialized_file = "serialized_out.txt"

    def test_a_write_graphviz_sg(self):
        graph = core.spatial_graph(2);
        sn = core.spatial_node()
        sn.pos = [1.0,1.0,1.0]
        graph.set_vertex(1, sn)
        core.io.write_graphviz_sg(self.graphviz_file, graph)

    def test_b_read_graphviz_sg(self):
        graph = core.io.read_graphviz_sg(self.graphviz_file)
        self.assertEqual(graph.num_vertices(), 2)
        self.assertAlmostEqual(graph.vertex(1).pos[0], 1.0)

    def test_a_write_serialized_sg(self):
        graph = core.spatial_graph(2);
        sn = core.spatial_node()
        sn.pos = [1.0,1.0,1.0]
        graph.set_vertex(1, sn)
        core.io.write_serialized_sg(self.serialized_file, graph)

    def test_b_read_serialized_sg(self):
        graph = core.io.read_serialized_sg(self.serialized_file)
        self.assertEqual(graph.num_vertices(), 2)
        self.assertAlmostEqual(graph.vertex(1).pos[0], 1.0)

if __name__ == '__main__':
    unittest.main()
