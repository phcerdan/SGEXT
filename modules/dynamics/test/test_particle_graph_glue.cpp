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
        SG::PointType n1{{1, 4, 0}};
        graph0[0].pos = p0;
        graph0[1].pos = e1;
        graph0[2].pos = e2;
        graph0[3].pos = n1;

        SG::PointType e05{{0.5, 0.3, 0}};
        SG::PointType e15{{1.5, -0.2, 0}};
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
    std::cout << "ParticleGraphGlueData_Fixture: works" << std::endl;
    auto particle_graph_data = SG::particles_from_graph(graph0);

    EXPECT_EQ(std::size(particle_graph_data.particle_collection.particles), 4);
    EXPECT_EQ(std::size(*(particle_graph_data.particle_graph_map)), 4);
    EXPECT_EQ(
            std::size(
                    particle_graph_data.connected_list[0].neighbors),
            1);
    EXPECT_EQ(
            std::size(
                    particle_graph_data.connected_list[1].neighbors),
            3);
    EXPECT_EQ(
            std::size(
                    particle_graph_data.connected_list[2].neighbors),
            1);
    EXPECT_EQ(
            std::size(
                    particle_graph_data.connected_list[3].neighbors),
            1);
    auto &neighbors =
            particle_graph_data.connected_list[1].neighbors;
    // Check ids of neighbors are correct
    EXPECT_EQ(neighbors.at(0), 0);
    EXPECT_EQ(neighbors.at(1), 2);
    EXPECT_EQ(neighbors.at(2), 3);

    // Check map works
    auto &pgmap = *(particle_graph_data.particle_graph_map);
    EXPECT_EQ(pgmap[1], 1);
}

TEST_F(ParticleGraphGlueData_Fixture, bonds_have_contour_length) {
    std::cout << "ParticleGraphGlueData_Fixture: bonds_have_contour_length"
              << std::endl;
    auto particle_graph_data = SG::particles_from_graph(graph0);
    auto &bond_collection = particle_graph_data.bond_collection;
    SG::print(bond_collection, std::cout);
    EXPECT_DOUBLE_EQ(
            std::static_pointer_cast<SG::BondChain>(bond_collection.bonds[0])
                    ->length_contour,
            1.16619037896906); // e05 with y: 0.3
    EXPECT_DOUBLE_EQ(
            std::static_pointer_cast<SG::BondChain>(bond_collection.bonds[1])
                    ->length_contour,
            1.077032961426901); // e15 with y: -0.2
    EXPECT_DOUBLE_EQ(
            std::static_pointer_cast<SG::BondChain>(bond_collection.bonds[2])
                    ->length_contour,
            4);
}
