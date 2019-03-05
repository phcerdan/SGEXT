/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
    SG::PointType disturbance {{0.0, 0.1, 0.5}};
    GraphType moved_g0;
    GraphType moved_g1;
    GraphType moved_gR;

    SG::PointType moved_p0  { ArrayUtilities::plus(this->p0, disturbance)};
    SG::PointType moved_u2  { ArrayUtilities::plus(this->u2, disturbance)};
    SG::PointType moved_dr3 { ArrayUtilities::plus(this->dr3, disturbance)};
    SG::PointType moved_dl3 { ArrayUtilities::plus(this->dl3, disturbance)};

    SG::PointType moved_u1  { ArrayUtilities::plus(this->u1 , disturbance)};
    SG::PointType moved_dr1 { ArrayUtilities::plus(this->dr1, disturbance)};
    SG::PointType moved_dr2 { ArrayUtilities::plus(this->dr2, disturbance)};
    SG::PointType moved_dl1 { ArrayUtilities::plus(this->dl1, disturbance)};
    SG::PointType moved_dl2 { ArrayUtilities::plus(this->dl2, disturbance)};

    // g1 extras
    SG::PointType moved_dr3_1{ ArrayUtilities::plus(this->dr3_1, disturbance)};
    SG::PointType moved_dl2_dr2_1{ ArrayUtilities::plus(this->dl2_dr2_1, disturbance)};
    SG::PointType moved_dl2_dr2_2{ ArrayUtilities::plus(this->dl2_dr2_2, disturbance)};
    SG::PointType moved_dl2_dr2_3{ ArrayUtilities::plus(this->dl2_dr2_3, disturbance)};

    void CreateMovedG0();
    void CreateMovedG1();
    // void CreateMovedGR();
    void SetUp() override;
};
