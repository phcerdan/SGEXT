# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import scripts
from sgext import core
from sgext import itk
import unittest
import os
import tempfile
import numpy as np

class ThreeConnectedNodesGraphFixture:
    def __init__(self):
        self.graph = core.spatial_graph(6)
        # Set vertex positions
        v0 = self.graph.spatial_node(0);
        v0.pos = [0, 0, 0]
        self.graph.set_vertex(0, v0)
        v1 = self.graph.spatial_node(1);
        v1.pos = [1, 1, 0]
        self.graph.set_vertex(1, v1)
        v2 = self.graph.spatial_node(2);
        v2.pos = [1, 0, 1]
        self.graph.set_vertex(2, v2)
        # Set edges between three nodes
        core.graph.add_edge(0, 1, core.spatial_edge(), self.graph)
        core.graph.add_edge(0, 2, core.spatial_edge(), self.graph)
        core.graph.add_edge(1, 2, core.spatial_edge(), self.graph)
        # Add end-points (degree 1)
        v3 = self.graph.spatial_node(3);
        v3.pos = [-1, 0, 0]
        self.graph.set_vertex(3, v3)
        v4 = self.graph.spatial_node(4);
        v4.pos = [1, 2, 0]
        self.graph.set_vertex(4, v4)
        v5 = self.graph.spatial_node(5);
        v5.pos = [1, 0, 2]
        self.graph.set_vertex(5, v5)
        # Connect end-points
        core.graph.add_edge(0, 3, core.spatial_edge(), self.graph)
        core.graph.add_edge(1, 4, core.spatial_edge(), self.graph)
        core.graph.add_edge(2, 5, core.spatial_edge(), self.graph)

class TestVoxelize(unittest.TestCase):
    def setUp(self):
        fixture = ThreeConnectedNodesGraphFixture()
        self.graph = fixture.graph
        self.reference_image = itk.IUC3P()
        np_index = np.array([-5, -5, -5])
        np_size = np.array([10, 10, 10])
        np_origin = np.array([0, 0, 0])
        np_spacing = np.array([1, 1, 1])
        self.reference_image.set_region(index=np_index, size=np_size)
        self.reference_image.set_origin(np_origin)
        self.reference_image.set_spacing(np_spacing)
        # Populate vertex_to_label_map
        value = 255
        self.vertex_to_label_map = {0:value, 1:value, 2:value, 3:value, 4:value, 5:value}

    def edge_label_func_const(source_label, target_label):
        value = 255
        return value

    def test_voxelize(self):
        edge_to_label_map = scripts.create_edge_to_label_map_from_vertex_to_label_map(
            graph=self.graph, vertex_to_label_map=self.vertex_to_label_map,
            edge_label_func=TestVoxelize.edge_label_func_const)
        print("vertex_to_label_map: ", self.vertex_to_label_map)
        print("edge_to_label_map: " , edge_to_label_map)

        voxelized_graph = scripts.voxelize_graph(
            graph=self.graph,
            reference_image=self.reference_image,
            vertex_to_label_map=self.vertex_to_label_map,
            edge_to_label_map=edge_to_label_map,
            graph_positions_are_in_physical_space=False)
        # Check non-graph is zero
        self.assertEqual(voxelized_graph.get_pixel([3,3,3]), 0)
        # Check vertex 5 is 255
        self.assertEqual(voxelized_graph.get_pixel([1,0,2]), 255)

