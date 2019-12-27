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
import _sgext.analyze as analyze
from analyze_fixture import AnalyzeGraphFixture
import unittest

class TestComputeGraphProperties(unittest.TestCase):
    def setUp(self):
        fixture = AnalyzeGraphFixture()
        self.graph = fixture.graph
        self.assertEqual(self.graph.num_vertices(), 6)
        self.assertEqual(self.graph.num_edges(), 6)

    def test_degrees(self):
        degrees = analyze.compute_degrees(self.graph)
        self.assertEqual(len(degrees), self.graph.num_vertices())
        self.assertEqual(degrees, [3,3,3,1,1,1])
    def test_ete_distances(self):
        ete_distances = analyze.compute_ete_distances(self.graph)
        ete_distances = analyze.compute_ete_distances(self.graph, min_edge_points = 0, ignore_end_nodes = False)
        self.assertEqual(len(ete_distances), self.graph.num_vertices())
        self.assertAlmostEqual(ete_distances, [1.4142135623730951, 1.4142135623730951, 1.4142135623730951,
                                               1.0, 1.0, 1.0])
    def test_contour_lenghts(self):
        contour_lengths = analyze.compute_contour_lengths(self.graph)
        contour_lengths = analyze.compute_contour_lengths(self.graph, min_edge_points = 0, ignore_end_nodes = False)
        self.assertEqual(len(contour_lengths), self.graph.num_vertices())
        self.assertAlmostEqual(contour_lengths, [1.4422205101855958, 1.4422205101855958, 1.4142135623730951,
                                                 1.0, 1.0, 1.0])
    def test_angles(self):
        angles = analyze.compute_angles(self.graph)
        angles = analyze.compute_angles(self.graph, min_edge_points = 0,
                                        ignore_parallel_edges = False, ignore_end_nodes = False)
        self.assertEqual(len(angles), 9)
        self.assertAlmostEqual(angles, [1.0471975511965976, 2.356194490192345, 2.356194490192345,
                                        1.0471975511965976, 2.356194490192345, 2.356194490192345,
                                        1.0471975511965976, 2.356194490192345, 2.356194490192345])
    def test_cosines(self):
        angles = analyze.compute_angles(self.graph, min_edge_points = 0,
                                        ignore_parallel_edges = False, ignore_end_nodes = False)
        cosines = analyze.compute_cosines(angles)
        self.assertEqual(len(cosines), 9)
        # NOTE, weird bug here with pybind11/stl in compute_cosines, where size of the python list is huge
        # Solved changing the order of inclusion of modules in wrap/CMakeLists.txt
        # If analyze module is before generate it works...
        # No OPAQUE std::vector<double> anywhere, not sure the reason...
        # print(cosines)
        # self.assertAlmostEqual(cosines, [1.0471975511965976, 2.356194490192345, 2.356194490192345,
        #                                 1.0471975511965976, 2.356194490192345, 2.356194490192345,
        #                                 1.0471975511965976, 2.356194490192345, 2.356194490192345])
