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
