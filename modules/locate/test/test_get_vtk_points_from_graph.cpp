/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "get_vtk_points_from_graph.hpp"
#include "spatial_graph.hpp"
#include "gmock/gmock.h"

struct GetVtkPointsFromGraphFixture : public ::testing::Test {
    using GraphType = SG::GraphType;
    GraphType g;

    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(3);

        // Add edge with an associated SpatialEdge at construction.
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        g[0].pos = p0;
        g[1].pos = e1;
        g[2].pos = e2;

        SG::PointType e05{{0.5, 0, 0}};
        SG::PointType e15{{1.5, 0, 0}};
        SG::SpatialEdge se05;
        se05.edge_points.insert(std::end(se05.edge_points), {e05});
        add_edge(0, 1, se05, g);
        SG::SpatialEdge se15;
        se15.edge_points.insert(std::end(se15.edge_points), {e15});
        add_edge(1, 2, se15, g);
    };
};

TEST_F(GetVtkPointsFromGraphFixture, get_vtk_points_from_graph) {
    auto points_map_pair = SG::get_vtk_points_from_graph(g);
    const auto &points = points_map_pair.first;
    const auto &idMap = points_map_pair.second;
    EXPECT_EQ(points->GetNumberOfPoints(), 5);
    EXPECT_EQ(idMap.size(), 5);

    size_t id = 0;
    auto pptr0 = points->GetPoint(id);
    EXPECT_EQ(pptr0[0], 0);
    EXPECT_EQ(pptr0[1], 0);
    EXPECT_EQ(pptr0[2], 0);
    auto gdescs = idMap.at(id);
    EXPECT_EQ(gdescs.size(), 1);
    auto gdesc = gdescs[0];
    EXPECT_TRUE(gdesc.exist);
    EXPECT_TRUE(gdesc.is_vertex);
    EXPECT_FALSE(gdesc.is_edge);
    EXPECT_EQ(gdesc.vertex_d, 0);
    id = 3; // the first edge 0--1
    gdesc = idMap.at(id)[0];
    EXPECT_TRUE(gdesc.exist);
    EXPECT_FALSE(gdesc.is_vertex);
    EXPECT_TRUE(gdesc.is_edge);
    EXPECT_EQ(gdesc.edge_d.m_source, 0);
    EXPECT_EQ(gdesc.edge_d.m_target, 1);
    EXPECT_EQ(gdesc.edge_points_index, 0);
}

TEST_F(GetVtkPointsFromGraphFixture,
       get_vtk_points_from_graphs_with_two_equal_graphs) {
    const auto &g0 = g;
    const auto g1 = g;
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    // Check that we get the same result than applying get_vtk_points_from_graph
    const auto points = merger_map_pair.first->GetPoints();
    const auto &idMap = merger_map_pair.second;
    EXPECT_EQ(points->GetNumberOfPoints(), 5);
    EXPECT_EQ(idMap.size(), 5);

    size_t id = 0;
    auto pptr0 = points->GetPoint(id);
    EXPECT_EQ(pptr0[0], 0);
    EXPECT_EQ(pptr0[1], 0);
    EXPECT_EQ(pptr0[2], 0);
    auto gdescs = idMap.at(id);
    EXPECT_EQ(gdescs.size(), 2);
    for (unsigned int n = 0; n < gdescs.size(); ++n) {
        auto gdesc = gdescs[n];
        EXPECT_TRUE(gdesc.exist);
        EXPECT_TRUE(gdesc.is_vertex);
        EXPECT_FALSE(gdesc.is_edge);
        EXPECT_EQ(gdesc.vertex_d, 0);
        id = 3; // the first edge 0--1
        gdesc = idMap.at(id)[n];
        EXPECT_TRUE(gdesc.exist);
        EXPECT_FALSE(gdesc.is_vertex);
        EXPECT_TRUE(gdesc.is_edge);
        EXPECT_EQ(gdesc.edge_d.m_source, 0);
        EXPECT_EQ(gdesc.edge_d.m_target, 1);
        EXPECT_EQ(gdesc.edge_points_index, 0);
    }
}

TEST_F(GetVtkPointsFromGraphFixture,
       get_vtk_points_from_graphs_with_two_different_graphs) {
    const auto &g0 = g;
    auto g1 = g;
    SG::SpatialNode sn;
    sn.pos = {{4.0, 5.0, 3.0}};
    boost::add_vertex(sn, g1);
    EXPECT_EQ(boost::num_vertices(g1), 4);
    EXPECT_EQ(boost::num_edges(g1), 2);

    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    const auto points = merger_map_pair.first->GetPoints();
    auto &idMap = merger_map_pair.second;
    EXPECT_EQ(points->GetNumberOfPoints(), 6);
    EXPECT_EQ(idMap.size(), 6);

    size_t id = 0;
    auto pptr0 = points->GetPoint(id);
    EXPECT_EQ(pptr0[0], 0);
    EXPECT_EQ(pptr0[1], 0);
    EXPECT_EQ(pptr0[2], 0);
    auto gdescs = idMap[id];
    EXPECT_EQ(gdescs.size(), 2);
    for (unsigned int n = 0; n < gdescs.size(); ++n) {
        auto gdesc = gdescs[n];
        EXPECT_TRUE(gdesc.exist);
        EXPECT_TRUE(gdesc.is_vertex);
        EXPECT_FALSE(gdesc.is_edge);
        EXPECT_EQ(gdesc.vertex_d, 0);
        id = 5; // the last node (from the second graph)
        gdesc = idMap[id][n];
        EXPECT_EQ(idMap[id].size(), 2);
        if (n == 0) {
            EXPECT_FALSE(gdesc.exist);
        } else {
            EXPECT_TRUE(gdesc.exist);
            EXPECT_TRUE(gdesc.is_vertex);
            EXPECT_FALSE(gdesc.is_edge);
        }
    }

    for (const auto &key_value_pair : idMap) {
        const auto &id = key_value_pair.first;
        const auto &gdescs = key_value_pair.second;
        for (const auto &gdesc : gdescs) {
            SG::print_graph_descriptor(gdesc, std::to_string(id));
        }
    }
}
