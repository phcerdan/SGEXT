# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

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
