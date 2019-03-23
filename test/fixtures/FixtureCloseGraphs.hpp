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

#ifndef FIXTURE_CLOSE_GRAPHS_HPP
#define FIXTURE_CLOSE_GRAPHS_HPP

#include "gmock/gmock.h"
#include "spatial_graph.hpp"
#include "FixtureMatchingGraphs.hpp"
#include "array_utilities.hpp"
#include "spatial_node.hpp"
#include "spatial_edge.hpp"

/**
 * Equivalent to FixtureMatchingGraphs but g1 positions are shifted
 */
struct FixtureCloseGraphs : public FixtureMatchingGraphs {
  using GraphType = SG::GraphType;
  SG::PointType disturbance{{0.0, 0.1, 0.5}};
  GraphType moved_g0;
  GraphType moved_g1;
  GraphType moved_gR;

  SG::PointType moved_p0{ArrayUtilities::plus(this->p0, disturbance)};
  SG::PointType moved_u2{ArrayUtilities::plus(this->u2, disturbance)};
  SG::PointType moved_dr3{ArrayUtilities::plus(this->dr3, disturbance)};
  SG::PointType moved_dl3{ArrayUtilities::plus(this->dl3, disturbance)};

  SG::PointType moved_u1{ArrayUtilities::plus(this->u1, disturbance)};
  SG::PointType moved_dr1{ArrayUtilities::plus(this->dr1, disturbance)};
  SG::PointType moved_dr2{ArrayUtilities::plus(this->dr2, disturbance)};
  SG::PointType moved_dl1{ArrayUtilities::plus(this->dl1, disturbance)};
  SG::PointType moved_dl2{ArrayUtilities::plus(this->dl2, disturbance)};

  // g1 extras
  SG::PointType moved_dr3_1{ArrayUtilities::plus(this->dr3_1, disturbance)};
  SG::PointType moved_dl2_dr2_1{
      ArrayUtilities::plus(this->dl2_dr2_1, disturbance)};
  SG::PointType moved_dl2_dr2_2{
      ArrayUtilities::plus(this->dl2_dr2_2, disturbance)};
  SG::PointType moved_dl2_dr2_3{
      ArrayUtilities::plus(this->dl2_dr2_3, disturbance)};

  void CreateMovedG0();
  void CreateMovedG1();
  // void CreateMovedGR();
  void SetUp() override;
};
#endif
