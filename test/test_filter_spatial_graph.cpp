/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "spatial_graph.hpp"
#include <iostream>
#include "filter_spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "array_utilities.hpp"

struct FilterByBoundingBoxFixture : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;
    SG::BoundingBox box;

    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(3);
        this->box.ini = {{0,0,0}};
        this->box.end = {{1,1,1}};

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


TEST_F(FilterByBoundingBoxFixture, node_outside_bounding_box)
{
    GraphType filtered_graph = SG::filter_by_bounding_box(box, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    // There is one node outside the unit box that gets filtered out
    EXPECT_THAT(boost::num_vertices(filtered_graph), 2);
    EXPECT_THAT(boost::num_edges(filtered_graph), 1);
}

TEST_F(FilterByBoundingBoxFixture, node_outside_but_edge_inside_bounding_box)
{
    SG::PointType e05{{0.5, 0, 0}};
    SG::SpatialEdge se_inside_box;
    se_inside_box.edge_points.insert(std::end(se_inside_box.edge_points), {e05});
    add_edge(1, 2, se_inside_box, g);

    GraphType filtered_graph = SG::filter_by_bounding_box(box, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    // There is one node outside of the unit box, but now it contains
    // an edge with edge_points inside the bounding box, so it is kept.
    EXPECT_THAT(boost::num_vertices(filtered_graph), 3);
    EXPECT_THAT(boost::num_edges(filtered_graph), 2);
}

