/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
