/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dynamics_common_fixtures.hpp"

namespace SG {
struct IntegratorPairBondForce_Fixture : public ::testing::Test {
    System sys;
    void SetUp() override { sys = System4Fixture(); }
};
} // namespace SG
