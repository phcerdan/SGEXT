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

