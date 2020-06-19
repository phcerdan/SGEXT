"""
Copyright (C) 2020 Pablo Hernande-Cerdan.

This file is part of SGEXT: http://github.com/phcerdan/sgext.

This file may be used under the terms of the GNU General Public License
version 3 as published by the Free Software Foundation and appearing in
the file LICENSE.GPL included in the packaging of this file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

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
            id_map=id_map,
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
            id_map=id_map,
            number_of_points=number_of_points)
        self.assertEqual(id_list.get_number_of_ids(), number_of_points)
        # Get the closest of the number_of_points (sorted)
        graph_id = id_list.get_id(0)
        self.assertEqual(id_map[graph_id][0].vertex_d, 1)
