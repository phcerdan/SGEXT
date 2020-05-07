# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
import unittest

class TestEdgePointsUtilities(unittest.TestCase):
    def test_edge_points_length(self):
        se = core.spatial_edge()
        se.edge_points = [[0,0,0], [1,0,0]]
        contour = core.edge_points_length(se)
        self.assertGreaterEqual(contour, 1)

    def test_contour_length(self):
        graph = core.spatial_graph(2)
        sn = core.spatial_node()
        sn.pos = [1.0,1.0,1.0]
        graph.set_vertex(1, sn)
        [ed, added] = core.graph.add_edge(0,1,core.spatial_edge(), graph)
        contour = core.contour_length(ed, graph)
        self.assertGreaterEqual(contour, 1)

    def test_insert_unique_edge_point_with_distance_order(self):
        point0 = [0,0,0]
        point1 = [1,0,0]
        point2 = [2,0,0]
        new_point = [3,0,0]
        point_container = [point0, point1, point2]
        new_point_container = core.insert_unique_edge_point_with_distance_order(
            point_container, new_point)
        self.assertEqual(len(point_container), 3)
        self.assertEqual(len(new_point_container), 4)

if __name__ == '__main__':
    unittest.main()


