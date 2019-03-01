/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

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

TEST_F(FilterBySetsFixture, remove_nodes)
{
    std::unordered_set<GraphType::vertex_descriptor> remove_nodes;
    remove_nodes.insert(2);
    GraphType filtered_graph = SG::filter_by_sets({}, remove_nodes, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    EXPECT_THAT(boost::num_vertices(filtered_graph), 2);
    EXPECT_THAT(boost::num_edges(filtered_graph), 1);
}

TEST_F(FilterBySetsFixture, remove_edges)
{
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

TEST_F(FilterBySetsFixture, remove_nodes_and_edges)
{
    std::unordered_set<GraphType::vertex_descriptor> remove_nodes;
    remove_nodes.insert(2);
    SG::EdgeDescriptorUnorderedSet remove_edges;
    GraphType::edge_descriptor ed;
    bool ed_exist;
    std::tie(ed, ed_exist) = boost::edge(0, 1, g);
    remove_edges.insert(ed);
    GraphType filtered_graph = SG::filter_by_sets(remove_edges, remove_nodes, g);
    SG::print_degrees(filtered_graph);
    SG::print_spatial_edges(filtered_graph);
    EXPECT_THAT(boost::num_vertices(filtered_graph), 2);
    EXPECT_THAT(boost::num_edges(filtered_graph), 0);
}
