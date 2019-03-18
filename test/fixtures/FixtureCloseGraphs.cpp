/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FixtureCloseGraphs.hpp"
#include "spatial_graph_functors.hpp"

void FixtureCloseGraphs::CreateMovedG0() {
  this->CreateG0();
  this->moved_g0 = this->g0;
  auto nverts = boost::num_vertices(this->moved_g0);
  auto func_plus = [&](SG::PointType& pos) {
    pos = ArrayUtilities::plus(pos, this->disturbance);
  };
  SG::operate_in_graph_pos(moved_g0, func_plus);
};

void FixtureCloseGraphs::CreateMovedG1() {
  this->CreateG1();
  this->moved_g1 = this->g1;
  auto nverts = boost::num_vertices(this->moved_g1);
  auto func_plus = [&](SG::PointType& pos) {
    pos = ArrayUtilities::plus(pos, this->disturbance);
  };
  SG::operate_in_graph_pos(moved_g1, func_plus);
};

// void FixtureCloseGraphs::CreateMovedGR()
// {
// using boost::add_edge;
// using boost::add_vertex;
// /*   moved_gR)
//  *       |
//  *       |
//  *      / \
    //  *     /   \
    //  *    /     \__
//  */
//
// this->moved_gR = GraphType(4);
//
// moved_gR[0].pos = moved_p0;
// moved_gR[1].pos = moved_u2;
// moved_gR[2].pos = moved_dr3_1;
// moved_gR[3].pos = moved_dl3;
//
// /*   |
//  *   |
//  */
// SG::SpatialEdge se_p0u2;
// se_p0u2.edge_points.insert(std::end(se_p0u2.edge_points), {moved_u1});
// add_edge(0, 1, se_p0u2, moved_gR);
//
// /*  \
    //  *   \
    //  *    \__
//  */
// SG::SpatialEdge se_p0dr3_1;
// se_p0dr3_1.edge_points.insert(std::end(se_p0dr3_1.edge_points),
//         {moved_dr1, moved_dr2, moved_dr3});
// add_edge(0, 2, se_p0dr3_1, moved_gR);
//
// /*    /
//  *   /
//  *  /
//  */
// SG::SpatialEdge se_p0dl3;
// se_p0dl3.edge_points.insert(std::end(se_p0dl3.edge_points), {moved_dl1,
// moved_dl2}); add_edge(0, 3, se_p0dl3, moved_gR);
// };

void FixtureCloseGraphs::SetUp() {
  this->CreateMovedG0();
  this->CreateMovedG1();
  // this->CreateMovedGR();
};
