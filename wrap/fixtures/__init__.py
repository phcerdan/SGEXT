# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core as _core

class six_nodes:
    def __init__(self):
        self.graph = _core.spatial_graph(6)
        # Set vertex positions
        v0 = self.graph.spatial_node(0);
        v0.pos = [0,0,0]
        self.graph.set_vertex(0, v0)
        v1 = self.graph.spatial_node(1);
        v1.pos = [1,1,0]
        self.graph.set_vertex(1, v1)
        v2 = self.graph.spatial_node(2);
        v2.pos = [1,0,1]
        self.graph.set_vertex(2, v2)
        # Set edges between three nodes
        ed01 = _core.spatial_edge()
        ed01.edge_points = [[0.4, 0.6, 0]]
        _core.graph.add_edge(0, 1, ed01, self.graph)
        ed02 = _core.spatial_edge()
        ed02.edge_points = [[0.4, 0, 0.6]]
        _core.graph.add_edge(0, 2, ed02, self.graph)
        ed12 = _core.spatial_edge()
        ed12.edge_points = [[1, 0.4 , 0.6]]
        _core.graph.add_edge(1, 2, ed12, self.graph)
        # Add end-points (degree 1)
        v3 = self.graph.spatial_node(3);
        v3.pos = [-1,0,0]
        self.graph.set_vertex(3, v3)
        v4 = self.graph.spatial_node(4);
        v4.pos = [1,2,0]
        self.graph.set_vertex(4, v4)
        v5 = self.graph.spatial_node(5);
        v5.pos = [1,0,2]
        self.graph.set_vertex(5, v5)
        # Connect end-points
        _core.graph.add_edge(0, 3, _core.spatial_edge(), self.graph)
        _core.graph.add_edge(1, 4, _core.spatial_edge(), self.graph)
        _core.graph.add_edge(2, 5, _core.spatial_edge(), self.graph)

