/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "spatial_graph.hpp"

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
struct MatchingGraphsFixture : public ::testing::Test {
    using GraphType = SG::GraphType;
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
