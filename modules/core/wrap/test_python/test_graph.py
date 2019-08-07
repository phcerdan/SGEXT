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

import sgcore
import unittest

class TestGraph(unittest.TestCase):
    def test_spatial_node(self):
        # Constructor
        sn = sgcore.spatial_node()
        self.assertEqual(sn.label, '')
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
        # NOT WRAPPED: stored_vertex
        # vertices = graph.vertices
        # self.assertEqual(len(vertices), 1)
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
        edges = graph.edges
        self.assertEqual(len(edges), 2)
        print(graph)

if __name__ == '__main__':
    unittest.main()
