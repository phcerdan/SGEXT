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
from sgext import visualize
from sgext import fixtures
import unittest
import os, tempfile

class TestConvertToVtkUnstructuredGrid(unittest.TestCase):
    @classmethod
    def setUp(cls):
        cls.graph = fixtures.six_nodes().graph
        cls.test_dir = tempfile.mkdtemp()

    def test_write_graph_to_vtk_unstructured_grid(self):
        filename = os.path.join(self.test_dir, "test1.vtu")
        print("filename= ", filename)
        visualize.write_graph_to_vtk_unstructured_grid(
            graph=self.graph,
            filename=filename,
            with_edge_points=False)
        self.assertTrue(os.path.exists(filename))
