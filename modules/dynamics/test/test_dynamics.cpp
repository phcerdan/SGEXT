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

#include "dynamics.hpp"
#include "dynamics_common_data.hpp"
#include "gmock/gmock.h"

struct ParticleCollection_Fixture : public ::testing::Test {
    SG::ParticleCollection collection;
    SG::Particle a;
    SG::Particle b;
    SG::Particle c;
    void SetUp() override {
        a.id = 5;
        a.pos = {{1, 1, 1}};
        b.id = 6;
        b.pos = {{2, 2, 2}};
        // c id is the smallest so far, even though the position is greater
        c.id = 2;
        c.pos = {{100, 100, 100}};
        collection.particles.emplace_back(b);
        collection.particles.emplace_back(a);
        collection.particles.emplace_back(c);
    }
};

TEST_F(ParticleCollection_Fixture, ParticleCollection) {
    std::cout << "--COLLECTION--" << std::endl;
    SG::print_id_pos(collection, std::cout);
    collection.sort();
    std::cout << "--COLLECTION AFTER SORT--" << std::endl;
    SG::print_id_pos(collection, std::cout);

    auto it = collection.binary_find(a.id);
    EXPECT_EQ((*it).id, a.id);
    auto index = std::distance(std::begin(collection.particles), it);
    EXPECT_EQ(index, 1);

    it = collection.binary_find(b.id);
    EXPECT_EQ((*it).id, b.id);
    index = std::distance(std::begin(collection.particles), it);
    EXPECT_EQ(index, 2);
    std::cout << "binary_find(" << b.id << ") result: " << (*it).id
              << std::endl;
    std::cout << "Found at index: " << index << std::endl;

    it = collection.binary_find(c.id);
    EXPECT_EQ((*it).id, c.id);
    index = std::distance(std::begin(collection.particles), it);
    EXPECT_EQ(index, 0);
}
