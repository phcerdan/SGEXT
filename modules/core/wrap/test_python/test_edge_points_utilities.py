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

class TestEdgePointsUtilities(unittest.TestCase):
    def test_edge_points_length(self):
        se = sgcore.spatial_edge()
        se.edge_points = [sgcore.array.array3d(0,0,0), sgcore.array.array3d(1,0,0)]
        contour = sgcore.edge_points_length(se)
        self.assertGreaterEqual(contour, 1)

    def test_contour_length(self):
        graph = sgcore.spatial_graph(2)
        sn = sgcore.spatial_node()
        sn.pos = sgcore.array.array3d(1.0,1.0,1.0)
        graph.set_vertex(1, sn)
        [ed, added] = sgcore.graph.add_edge(0,1,sgcore.spatial_edge(), graph)
        contour = sgcore.contour_length(ed, graph)
        self.assertGreaterEqual(contour, 1)

    def test_insert_unique_edge_point_with_distance_order(self):
        point0 = sgcore.array.array3d(0,0,0)
        point1 = sgcore.array.array3d(1,0,0)
        point2 = sgcore.array.array3d(2,0,0)
        new_point = sgcore.array.array3d(3,0,0)
        point_container = [point0, point1, point2]
        new_point_container = sgcore.insert_unique_edge_point_with_distance_order(
            point_container, new_point)
        self.assertEqual(len(point_container), 3)
        self.assertEqual(len(new_point_container), 4)

if __name__ == '__main__':
    unittest.main()


