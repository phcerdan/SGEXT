/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FIXTURE_SQUARE_CROSS_GRAPH_HPP
#define FIXTURE_SQUARE_CROSS_GRAPH_HPP

#include "gmock/gmock.h"
#include "spatial_graph.hpp"
#include "bounding_box.hpp"

/**
 *    1   2
 *      0
 *    4   3
 *   ____
 *   |\/|
 *   |/\|
 *
 *
 */
struct FixtureSquareCrossGraph : public ::testing::Test {
  using GraphType = SG::GraphType;
  SG::BoundingBox box = {{-10., -10., -10.}, {10., 10., 10.}};
  GraphType g_square_cross;
  GraphType g_square;
  GraphType g_cross;

  SG::PointType v0{{ 0,  0, 0}};
  SG::PointType v1{{-2,  2, 0}};
  SG::PointType v2{{ 2,  2, 0}};
  SG::PointType v3{{ 2, -2, 0}};
  SG::PointType v4{{-2, -2, 0}};

  SG::PointType ep12{{ 0,  2, 0}};
  SG::PointType ep23{{ 2,  0, 0}};
  SG::PointType ep34{{ 0, -2, 0}};
  SG::PointType ep14{{-2,  0, 0}};

  SG::PointType ep01{{ -1,  1, 0}};
  SG::PointType ep02{{  1,  1, 0}};
  SG::PointType ep03{{  1, -1, 0}};
  SG::PointType ep04{{ -1, -1, 0}};


  void CreateSquareCross();
  void CreateSquare();
  void CreateCross();
  void SetUp() override;
};

#endif
