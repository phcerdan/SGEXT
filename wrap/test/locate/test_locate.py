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
        print("test_get_vtk_points_from_graph")
        [vtk_points, id_map] = locate.get_vtk_points_from_graph(self.graph)
        print(vtk_points)
        self.assertEqual(vtk_points.get_number_of_points(), 9)

    def test_get_vtk_points_from_graphs(self):
        print("test_get_vtk_points_from_graphs")
        graphs = list([self.graph, self.graph])
        [vtk_merge_points, id_map] = locate.get_vtk_points_from_graphs(graphs)
        print(vtk_merge_points)

class TestGraphPointsLocator(unittest.TestCase):
    @classmethod
    def setUp(cls):
        cls.graph = fixtures.six_nodes().graph

    def test_id_with_graph_descriptor(self):
        print("test_id_with_graph_descriptor")
        id_with_desc = locate.id_with_graph_descriptor()
        self.assertFalse(id_with_desc.exist)

    def test_build_octree_locator(self):
        print("test_build_octree_locator")
        [vtk_points, id_map] = locate.get_vtk_points_from_graph(self.graph)
        octree_locator = locate.build_octree_locator(vtk_points)
        print(octree_locator)

    def test_graph_closest_points_by_radius_locator(self):
        print("test_graph_closest_points_by_radius_locator")
        [vtk_points, id_map] = locate.get_vtk_points_from_graph(self.graph)
        octree_locator = locate.build_octree_locator(vtk_points)
        query_point = [1.0, 1.05, 0]
        radius = 0.2
        id_list = locate.graph_closest_points_by_radius_locator(
            query_point=query_point,
            octree=octree_locator,
            radius=radius)
        self.assertEqual(id_list.get_number_of_ids(), 1)
        graph_id = id_list.get_id(0)
        self.assertEqual(id_map[graph_id][0].vertex_d, 1)

    def test_graph_closest_n_points_locator(self):
        print("test_graph_closest_n_points_locator")
        [vtk_points, id_map] = locate.get_vtk_points_from_graph(self.graph)
        octree_locator = locate.build_octree_locator(vtk_points)
        query_point = [1.0, 1.05, 0]
        number_of_points = 2
        id_list = locate.graph_closest_n_points_locator(
            query_point=query_point,
            octree=octree_locator,
            number_of_points=number_of_points)
        self.assertEqual(id_list.get_number_of_ids(), number_of_points)
        # Get the closest of the number_of_points (sorted)
        graph_id = id_list.get_id(0)
        self.assertEqual(id_map[graph_id][0].vertex_d, 1)
