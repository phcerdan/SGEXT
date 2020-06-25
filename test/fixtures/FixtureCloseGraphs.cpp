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

#include "FixtureCloseGraphs.hpp"
#include "spatial_graph_functors.hpp"

void FixtureCloseGraphs::CreateMovedG0() {
  this->CreateG0();
  this->moved_g0 = this->g0;
  auto func_plus = [&](SG::PointType& pos) {
    pos = ArrayUtilities::plus(pos, this->disturbance);
  };
  SG::operate_in_graph_pos(moved_g0, func_plus);
};

void FixtureCloseGraphs::CreateMovedG1() {
  this->CreateG1();
  this->moved_g1 = this->g1;
  auto func_plus = [&](SG::PointType& pos) {
    pos = ArrayUtilities::plus(pos, this->disturbance);
  };
  SG::operate_in_graph_pos(moved_g1, func_plus);
};

void FixtureCloseGraphs::SetUp() {
  this->CreateMovedG0();
  this->CreateMovedG1();
  // this->CreateMovedGR();
};
