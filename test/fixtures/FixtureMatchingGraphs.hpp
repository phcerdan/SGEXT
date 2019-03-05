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

#ifndef FIXTURE_MATCHING_GRAPHS_HPP
#define FIXTURE_MATCHING_GRAPHS_HPP

#include "gmock/gmock.h"
#include "spatial_graph.hpp"
#include "bounding_box.hpp"

/*   g0)
 *       |
 *       |
 *      / \
 *     /   \
 *    /     \
 *
 *   g1)
 *       |
 *       |
 *      / \
 *     /___\
 *    /     \__
 *
 *   gR)
 *       |
 *       |
 *      / \
 *     /   \
 *    /     \__
 *
 *  We want to keep the edges with end-points, and remove edges connecting
 *  old edges.
 *
 */
struct FixtureMatchingGraphs : public ::testing::Test {
    using GraphType = SG::GraphType;
    SG::BoundingBox box = {{-10., -10., -10.}, {10., 10., 10.}};
    GraphType g0;
    GraphType g1;
    GraphType gR;

    SG::PointType p0 {{ 0,  0, 0}};
    SG::PointType u2 {{ 0,  2, 0}};
    SG::PointType dr3{{ 3, -3, 0}};
    SG::PointType dl3{{-3, -3, 0}};

    SG::PointType u1  {{ 0,  1, 0}};
    SG::PointType dr1 {{ 1, -1, 0}};
    SG::PointType dr2 {{ 2, -2, 0}};
    SG::PointType dl1 {{-1, -1, 0}};
    SG::PointType dl2 {{-2, -2, 0}};

    // g1 extras
    SG::PointType dr3_1{{ 4, -3, 0}};
    SG::PointType dl2_dr2_1{{-1, -2, 0}};
    SG::PointType dl2_dr2_2{{ 0, -2, 0}};
    SG::PointType dl2_dr2_3{{ 1, -2, 0}};

    void CreateG0();
    void CreateG1();
    void CreateGR();
    void SetUp() override;
};

#endif

