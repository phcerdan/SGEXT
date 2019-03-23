/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FixtureSquareCrossGraph.hpp"

/*
 *    1   2
 *
 *    4   3
 */
void FixtureSquareCrossGraph::CreateSquare() {
  using boost::add_edge;
  using boost::add_vertex;

  this->g_square = GraphType(4);
  g_square[0].pos = v1;
  g_square[1].pos = v2;
  g_square[2].pos = v3;
  g_square[3].pos = v4;

  /*
   *   ---
   */
  SG::SpatialEdge e12;
  e12.edge_points.insert(std::end(e12.edge_points), {ep12});
  add_edge(0, 1, e12, g_square);

  /*   |
   *   |
   */
  SG::SpatialEdge e23;
  e23.edge_points.insert(std::end(e23.edge_points), {ep23});
  add_edge(1, 2, e23, g_square);

  /*
   *   ---
   */
  SG::SpatialEdge e34;
  e34.edge_points.insert(std::end(e34.edge_points), {ep34});
  add_edge(2, 3, e34, g_square);

  /*   |
   *   |
   */
  SG::SpatialEdge e14;
  e14.edge_points.insert(std::end(e14.edge_points), {ep14});
  add_edge(0, 3, e14, g_square);
};

/*
 *    1   2
 *      0
 *    4   3
 */

void FixtureSquareCrossGraph::CreateCross() {
  using boost::add_edge;
  using boost::add_vertex;
  this->g_cross = GraphType(5);

  g_cross[0].pos = v0;
  g_cross[1].pos = v1;
  g_cross[2].pos = v2;
  g_cross[3].pos = v3;
  g_cross[4].pos = v4;

  /*
   *   \
   *    \
   */
  SG::SpatialEdge e01;
  e01.edge_points.insert(std::end(e01.edge_points), {ep01});
  add_edge(0, 1, e01, g_cross);

  /*
   *    /
   *   /
   */
  SG::SpatialEdge e02;
  e02.edge_points.insert(std::end(e02.edge_points), {ep02});
  add_edge(0, 2, e02, g_cross);

  /*
   *   \
   *    \
   */
  SG::SpatialEdge e03;
  e03.edge_points.insert(std::end(e03.edge_points), {ep03});
  add_edge(0, 3, e03, g_cross);

  /*
   *    /
   *   /
   */
  SG::SpatialEdge e04;
  e04.edge_points.insert(std::end(e04.edge_points), {ep04});
  add_edge(0, 4, e04, g_cross);

};

/*
 *    1   2
 *      0
 *    4   3
 */
void FixtureSquareCrossGraph::CreateSquareCross() {
  using boost::add_edge;
  using boost::add_vertex;

  this->g_square_cross = GraphType(5);

  g_square_cross[0].pos = v0;
  g_square_cross[1].pos = v1;
  g_square_cross[2].pos = v2;
  g_square_cross[3].pos = v3;
  g_square_cross[4].pos = v4;

  /************ Square *************/

  /*
   *   ---
   */
  SG::SpatialEdge e12;
  e12.edge_points.insert(std::end(e12.edge_points), {ep12});
  add_edge(1, 2, e12, g_square_cross);

  /*   |
   *   |
   */
  SG::SpatialEdge e23;
  e23.edge_points.insert(std::end(e23.edge_points), {ep23});
  add_edge(2, 3, e23, g_square_cross);

  /*
   *   ---
   */
  SG::SpatialEdge e34;
  e34.edge_points.insert(std::end(e34.edge_points), {ep34});
  add_edge(3, 4, e34, g_square_cross);

  /*   |
   *   |
   */
  SG::SpatialEdge e14;
  e14.edge_points.insert(std::end(e14.edge_points), {ep14});
  add_edge(1, 4, e14, g_square_cross);

  /******* Cross ********/

  /*
   *   \
   *    \
   */
  SG::SpatialEdge e01;
  e01.edge_points.insert(std::end(e01.edge_points), {ep01});
  add_edge(0, 1, e01, g_square_cross);

  /*
   *    /
   *   /
   */
  SG::SpatialEdge e02;
  e02.edge_points.insert(std::end(e02.edge_points), {ep02});
  add_edge(0, 2, e02, g_square_cross);

  /*
   *   \
   *    \
   */
  SG::SpatialEdge e03;
  e03.edge_points.insert(std::end(e03.edge_points), {ep03});
  add_edge(0, 3, e03, g_square_cross);

  /*
   *    /
   *   /
   */
  SG::SpatialEdge e04;
  e04.edge_points.insert(std::end(e04.edge_points), {ep04});
  add_edge(0, 4, e04, g_square_cross);

};

void FixtureSquareCrossGraph::SetUp() {
  this->CreateSquare();
  this->CreateCross();
  this->CreateSquareCross();
};
