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

#include "bond.hpp"
#include "bond_collection.hpp"
#include "dynamics_common_fixtures.hpp"
#include "gmock/gmock.h"

struct BondCollection_Fixture : public ::testing::Test {
    SG::System sys = SG::System4Fixture();
    SG::BondCollection collection;
    SG::BondCollection unsorted_collection;
    void SetUp() override {
        const double default_contour_length = 10.0;
        collection.bonds.push_back(std::make_shared<SG::BondChain>(
                10, 11, default_contour_length));
        collection.bonds.push_back(std::make_shared<SG::BondChain>(
                11, 12, default_contour_length));
        collection.bonds.push_back(std::make_shared<SG::BondChain>(
                11, 13, default_contour_length));
        collection.sorted = true; // manually flagged as sorted
        // unsorted_collection
        unsorted_collection.bonds.push_back(std::make_shared<SG::BondChain>(
                11, 13, default_contour_length));
        unsorted_collection.bonds.push_back(std::make_shared<SG::BondChain>(
                11, 12, default_contour_length));
        unsorted_collection.bonds.push_back(std::make_shared<SG::BondChain>(
                10, 11, default_contour_length));
    }
};

TEST_F(BondCollection_Fixture, fixture_works) {
    EXPECT_EQ(collection.bonds.size(), 3);
    print(collection, std::cout);
}

TEST_F(BondCollection_Fixture, sort_works) {
    EXPECT_EQ(unsorted_collection.sorted, false);
    unsorted_collection.sort();
    std::cout << "unsorted_collection after sort:" << std::endl;
    print(unsorted_collection, std::cout);
    EXPECT_EQ(*(unsorted_collection.bonds[0]), *(collection.bonds[0]));
    EXPECT_EQ(*(unsorted_collection.bonds[1]), *(collection.bonds[1]));
    EXPECT_EQ(unsorted_collection.sorted, true);
}

TEST_F(BondCollection_Fixture, find_all_bonds_with_id_works) {
    std::cout << "Find all bonds with id=11:" << std::endl;
    auto bonds_with_11 = collection.find_all_bonds_with_id(11);
    for (const auto &b : bonds_with_11) {
        SG::print(*b, std::cout);
    }
    EXPECT_EQ(bonds_with_11.size(), 3);
    EXPECT_EQ(*(bonds_with_11[0]), *(collection.bonds[0]));
    EXPECT_EQ(*(bonds_with_11[1]), *(collection.bonds[1]));
    EXPECT_EQ(*(bonds_with_11[2]), *(collection.bonds[2]));

    std::cout << "Find all bonds with id=12:" << std::endl;
    auto bonds_with_12 = collection.find_all_bonds_with_id(12);
    for (const auto &b : bonds_with_12) {
        SG::print(*b, std::cout);
    }
    EXPECT_EQ(bonds_with_12.size(), 1);
    EXPECT_EQ(*(bonds_with_12[0]), *(collection.bonds[1]));

    std::cout << "Find all bonds with id=10:" << std::endl;
    auto bonds_with_10 = collection.find_all_bonds_with_id(10);
    for (const auto &b : bonds_with_10) {
        SG::print(*b, std::cout);
    }
    EXPECT_EQ(bonds_with_10.size(), 1);
    EXPECT_EQ(*(bonds_with_10[0]), *(collection.bonds[0]));

    std::cout << "Find all bonds with id=13:" << std::endl;
    auto bonds_with_13 = collection.find_all_bonds_with_id(13);
    for (const auto &b : bonds_with_13) {
        SG::print(*b, std::cout);
    }
    EXPECT_EQ(bonds_with_13.size(), 1);
    EXPECT_EQ(*(bonds_with_13[0]), *(collection.bonds[2]));
}

TEST_F(BondCollection_Fixture, binary_find_bond_id_a_works) {
    {
        size_t id = 10;
        std::cout << "check binary_find_bond_id_a with id " << id << std::endl;
        auto it = collection.binary_find_bond_id_a(
                std::begin(collection.bonds), std::end(collection.bonds), id);
        EXPECT_EQ((*it)->id_a, id);
        std::cout << (*it)->id_a << std::endl;
    }
    {
        size_t id = 8;
        std::cout << "check binary_find_bond_id_a with id " << id << std::endl;
        auto it = collection.binary_find_bond_id_a(
                std::begin(collection.bonds), std::end(collection.bonds), id);
        EXPECT_EQ(it, std::end(collection.bonds));
    }
}

TEST_F(BondCollection_Fixture, find_bond_works) {
    {
        size_t id_a = 8;
        size_t id_b = 9;
        std::cout << "check find_bond with non-existant bond: id_a " << id_a
                  << ", id_b: " << id_b << std::endl;
        auto it = collection.find_bond(SG::Bond(id_a, id_b));
        EXPECT_EQ(it, std::end(collection.bonds));
    }
    {
        size_t id_a = 10;
        size_t id_b = 11;
        std::cout << "check find_bond with existant bond: id_a " << id_a
                  << ", id_b: " << id_b << std::endl;
        const auto bond = SG::Bond(id_a, id_b);
        auto it = collection.find_bond(bond);
        EXPECT_EQ(*(*it), bond);
    }
}
