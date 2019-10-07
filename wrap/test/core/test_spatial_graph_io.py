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

import _sgext.core as core
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
        sn.pos = core.array.array3d(1.0,1.0,1.0)
        graph.set_vertex(1, sn)
        core.io.write_graphviz_sg(self.graphviz_file, graph)

    def test_b_read_graphviz_sg(self):
        graph = core.io.read_graphviz_sg(self.graphviz_file)
        self.assertEqual(graph.num_vertices(), 2)
        self.assertAlmostEqual(graph.vertex(1).pos[0], 1.0)

    def test_a_write_serialized_sg(self):
        graph = core.spatial_graph(2);
        sn = core.spatial_node()
        sn.pos = core.array.array3d(1.0,1.0,1.0)
        graph.set_vertex(1, sn)
        core.io.write_serialized_sg(self.serialized_file, graph)

    def test_b_read_serialized_sg(self):
        graph = core.io.read_serialized_sg(self.serialized_file)
        self.assertEqual(graph.num_vertices(), 2)
        self.assertAlmostEqual(graph.vertex(1).pos[0], 1.0)

if __name__ == '__main__':
    unittest.main()
