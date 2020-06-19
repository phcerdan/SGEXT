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
        [vtk_points, pg_map] = locate.get_vtk_points_from_graph(self.graph)
        print(vtk_points)
        self.assertEqual(vtk_points.get_number_of_points(), 9)

    def test_get_vtk_points_from_graphs(self):
        graphs = list([self.graph, self.graph])
        [vtk_merge_points, pg_map] = locate.get_vtk_points_from_graphs(graphs)
        print(vtk_merge_points)


