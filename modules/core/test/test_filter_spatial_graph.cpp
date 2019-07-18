/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "array_utilities.hpp"
#include "filter_spatial_graph.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "gmock/gmock.h"
#include <iostream>

struct FilterByBoundingBoxFixture : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;
    SG::BoundingBox box;

    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(3);
        this->box.ini = {{0, 0, 0}};
        this->box.end = {{1, 1, 1}};

        // Add edge with an associated SpatialEdge at construction.
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType e05{{0.5, 0, 0}};
        SG::PointType e15{{1.5, 0, 0}};

        g[0].pos = p0;
        g[1].pos = e1;
        g[2].pos = e2;

        SG::SpatialEdge se05;
        se05.edge_points.insert(std::end(se05.edge_points), {e05});
        add_edge(0, 1, se05, g);
        SG::SpatialEdge se15;
        se15.edge_points.insert(std::end(se15.edge_points), {e15});
        add_edge(1, 2, se15, g);
    }
};

TEST_F(FilterByBoundingBoxFixture, node_outside_bounding_box) {
    GraphType filtered_graph = SG::filter_by_bounding_box(box, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    // There is one node outside the unit box that gets filtered out
    EXPECT_THAT(boost::num_vertices(filtered_graph), 2);
    EXPECT_THAT(boost::num_edges(filtered_graph), 1);
}

TEST_F(FilterByBoundingBoxFixture, node_outside_but_edge_inside_bounding_box) {
    SG::PointType e05{{0.5, 0, 0}};
    SG::SpatialEdge se_inside_box;
    se_inside_box.edge_points.insert(std::end(se_inside_box.edge_points),
                                     {e05});
    add_edge(1, 2, se_inside_box, g);

    GraphType filtered_graph = SG::filter_by_bounding_box(box, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    // There is one node outside of the unit box, but now it contains
    // an edge with edge_points inside the bounding box, so it is kept.
    EXPECT_THAT(boost::num_vertices(filtered_graph), 3);
    EXPECT_THAT(boost::num_edges(filtered_graph), 2);
}

struct FilterBySetsFixture : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;

    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(3);
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType e05{{0.5, 0, 0}};
        SG::PointType e15{{1.5, 0, 0}};

        g[0].pos = p0;
        g[1].pos = e1;
        g[2].pos = e2;

        SG::SpatialEdge se05;
        se05.edge_points.insert(std::end(se05.edge_points), {e05});
        add_edge(0, 1, se05, g);
        SG::SpatialEdge se15;
        se15.edge_points.insert(std::end(se15.edge_points), {e15});
        add_edge(1, 2, se15, g);
    }
};

TEST_F(FilterBySetsFixture, remove_nodes) {
    std::unordered_set<GraphType::vertex_descriptor> remove_nodes;
    remove_nodes.insert(2);
    GraphType filtered_graph = SG::filter_by_sets({}, remove_nodes, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    EXPECT_THAT(boost::num_vertices(filtered_graph), 2);
    EXPECT_THAT(boost::num_edges(filtered_graph), 1);
}

TEST_F(FilterBySetsFixture, remove_edges) {
    SG::EdgeDescriptorUnorderedSet remove_edges;
    GraphType::edge_descriptor ed;
    bool ed_exist;
    std::tie(ed, ed_exist) = boost::edge(1, 2, g);
    remove_edges.insert(ed);
    GraphType filtered_graph = SG::filter_by_sets(remove_edges, {}, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    EXPECT_THAT(boost::num_vertices(filtered_graph), 3);
    EXPECT_THAT(boost::num_edges(filtered_graph), 1);
}

TEST_F(FilterBySetsFixture, remove_nodes_and_edges) {
    std::unordered_set<GraphType::vertex_descriptor> remove_nodes;
    remove_nodes.insert(2);
    SG::EdgeDescriptorUnorderedSet remove_edges;
    GraphType::edge_descriptor ed;
    bool ed_exist;
    std::tie(ed, ed_exist) = boost::edge(0, 1, g);
    remove_edges.insert(ed);
    GraphType filtered_graph =
            SG::filter_by_sets(remove_edges, remove_nodes, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    EXPECT_THAT(boost::num_vertices(filtered_graph), 2);
    EXPECT_THAT(boost::num_edges(filtered_graph), 0);
}

struct FilterComponentsFixture : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;

    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(5);
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType e05{{0.5, 0, 0}};
        SG::PointType e15{{1.5, 0, 0}};

        g[0].pos = p0;
        g[1].pos = e1;
        g[2].pos = e2;

        SG::SpatialEdge se05;
        se05.edge_points.insert(std::end(se05.edge_points), {e05});
        add_edge(0, 1, se05, g);
        SG::SpatialEdge se15;
        se15.edge_points.insert(std::end(se15.edge_points), {e15});
        add_edge(1, 2, se15, g);

        SG::PointType z3{{5, 0, 0}};
        SG::PointType z4{{7, 0, 0}};
        SG::PointType e34{{6, 0, 0}};
        g[3].pos = z3;
        g[4].pos = z4;
        SG::SpatialEdge se34;
        se34.edge_points.insert(std::end(se34.edge_points), {e34});
        add_edge(3, 4, se34, g);
    }
};

TEST_F(FilterComponentsFixture, works) {
    auto graph_components = filter_component_graphs(g);
    EXPECT_EQ(graph_components.size(), 2);
    const auto num_verts = boost::num_vertices(g);
    const auto num_edges = boost::num_edges(g);
    const auto &first_component = graph_components[0];
    const auto &second_component = graph_components[1];
    // Filtered graph does not modify the number of vertices or edges.
    EXPECT_THAT(boost::num_vertices(first_component), num_verts);
    EXPECT_THAT(boost::num_edges(first_component), num_edges);
    EXPECT_THAT(boost::num_vertices(second_component), num_verts);
    EXPECT_THAT(boost::num_edges(second_component), num_edges);
    // But when iterating, it filters correctly.
    size_t first_component_vertices = 0;
    const size_t first_component_vertices_expected = 3;
    auto first_verts = boost::vertices(first_component);
    for (auto vi = first_verts.first; vi != first_verts.second;
         vi++, first_component_vertices++) {
    }
    EXPECT_EQ(first_component_vertices, first_component_vertices_expected);
    size_t first_component_edges = 0;
    const size_t first_component_edges_expected = 2;
    auto first_edges = boost::edges(first_component);
    for (auto ei = first_edges.first; ei != first_edges.second;
         ei++, first_component_edges++) {
    }
    EXPECT_EQ(first_component_edges, first_component_edges_expected);

    size_t second_component_vertices = 0;
    const size_t second_component_vertices_expected = 2;
    auto second_verts = boost::vertices(second_component);
    for (auto vi = second_verts.first; vi != second_verts.second;
         vi++, second_component_vertices++) {
    }
    EXPECT_EQ(second_component_vertices, second_component_vertices_expected);
    size_t second_component_edges = 0;
    const size_t second_component_edges_expected = 1;
    auto second_edges = boost::edges(second_component);
    for (auto ei = second_edges.first; ei != second_edges.second;
         ei++, second_component_edges++) {
    }
    EXPECT_EQ(second_component_edges, second_component_edges_expected);

    std::cout << "First component:" << std::endl;
    SG::print_degrees(first_component);
    SG::print_spatial_edges(first_component);
    std::cout << "Second component:" << std::endl;
    SG::print_degrees(second_component);
    SG::print_spatial_edges(second_component);
}
