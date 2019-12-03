/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dynamics_graph_glue.hpp"
#include "gmock/gmock.h"

/*
 * graph0:
 *       o
 *       |
 *     o-o-o
 *
 */
struct ParticleGraphGlueData_Fixture : public ::testing::Test {
    using GraphType = SG::GraphType;
    GraphType graph0;
    void SetUp() override {
        this->graph0 = GraphType(4);
        // Add edge with an associated SpatialEdge at construction.
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType n1{{1, 1, 0}};
        graph0[0].pos = p0;
        graph0[1].pos = e1;
        graph0[2].pos = e2;
        graph0[3].pos = n1;

        SG::PointType e05{{0.5, 0, 0}};
        SG::PointType e15{{1.5, 0, 0}};
        SG::SpatialEdge se05;
        se05.edge_points.insert(std::end(se05.edge_points), {e05});
        boost::add_edge(0, 1, se05, graph0);
        SG::SpatialEdge se15;
        se15.edge_points.insert(std::end(se15.edge_points), {e15});
        boost::add_edge(1, 2, se15, graph0);

        boost::add_edge(1, 3, {}, graph0);
    };
};

TEST_F(ParticleGraphGlueData_Fixture, works) {
    std::cout << "ParticleGraphGlueData_Fixture" << std::endl;
    auto particle_graph_data = SG::particles_from_graph(graph0);

    EXPECT_EQ(std::size(particle_graph_data.particle_collection.particles), 4);
    EXPECT_EQ(std::size(*(particle_graph_data.particle_graph_map)), 4);
    EXPECT_EQ(std::size(particle_graph_data.connected_list.collection[0].neighbors), 1);
    EXPECT_EQ(std::size(particle_graph_data.connected_list.collection[1].neighbors), 3);
    EXPECT_EQ(std::size(particle_graph_data.connected_list.collection[2].neighbors), 1);
    EXPECT_EQ(std::size(particle_graph_data.connected_list.collection[3].neighbors), 1);
    auto &neighbors = particle_graph_data.connected_list.collection[1].neighbors;
    // Check ids of neighbors are correct
    EXPECT_EQ(neighbors.at(0), 0);
    EXPECT_EQ(neighbors.at(1), 2);
    EXPECT_EQ(neighbors.at(2), 3);

    // Check map works
    auto &pgmap = *(particle_graph_data.particle_graph_map);
    EXPECT_EQ(pgmap[1], 1);
}
