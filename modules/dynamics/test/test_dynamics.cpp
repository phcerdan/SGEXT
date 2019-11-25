/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dynamics.hpp"
#include "gmock/gmock.h"
#include "dynamics_common_data.hpp"

TEST(Dynamics, works) {
    SG::hola();
}
struct ParticleCollection_Fixture : public ::testing::Test {
    SG::ParticleCollection collection;
    SG::Particle a;
    SG::Particle b;
    SG::Particle c;
    void SetUp() override {
        a.id = 5;
        a.pos = {{1,1,1}};
        b.id = 6;
        b.pos = {{2,2,2}};
        // c id is the smallest so far, even though the position is greater
        c.id = 2;
        c.pos = {{100,100,100}};
        collection.particles.emplace_back(b);
        collection.particles.emplace_back(a);
        collection.particles.emplace_back(c);
    }
};

TEST_F(ParticleCollection_Fixture, ParticleCollection) {
    std::cout << "--COLLECTION--" << std::endl;
    SG::print(collection);
    collection.sort();
    std::cout << "--COLLECTION AFTER SORT--" << std::endl;
    SG::print(collection);

    auto it = collection.binary_find(a.id);
    EXPECT_EQ((*it).id, a.id);
    auto index = std::distance(std::begin(collection.particles), it);
    EXPECT_EQ(index, 1);

    it = collection.binary_find(b.id);
    EXPECT_EQ((*it).id, b.id);
    index = std::distance(std::begin(collection.particles), it);
    EXPECT_EQ(index, 2);
    std::cout << "binary_find(" << b.id << ") result: " << (*it).id << std::endl;
    std::cout << "Found at index: " << index << std::endl;

    it = collection.binary_find(c.id);
    EXPECT_EQ((*it).id, c.id);
    index = std::distance(std::begin(collection.particles), it);
    EXPECT_EQ(index, 0);
}

