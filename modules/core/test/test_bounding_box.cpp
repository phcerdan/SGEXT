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

#include "bounding_box.hpp"
#include "gmock/gmock.h"

TEST(bounding_box, constructors) {
    SG::BoundingBox defaultBox;
    SG::PointType expectedIni = {{0., 0., 0.}};
    SG::PointType expectedEnd = {{1., 1., 1.}};
    EXPECT_TRUE(defaultBox.ini == expectedIni);
    EXPECT_TRUE(defaultBox.end == expectedEnd);

    expectedIni = {{0.5, 0.5, 0.5}};
    expectedEnd = {{1., 1., 1.}};
    SG::BoundingBox iniEndBox(expectedIni, expectedEnd);
    EXPECT_TRUE(iniEndBox.ini == expectedIni);
    EXPECT_TRUE(iniEndBox.end == expectedEnd);

    SG::PointType center = {{0, 0, 0}};
    const size_t radius = 3;
    SG::BoundingBox radiusBox(center, radius);
    expectedIni = {{-3.0, -3.0, -3.0}};
    expectedEnd = {{3.0, 3., 3.}};
    EXPECT_TRUE(radiusBox.ini == expectedIni);
    EXPECT_TRUE(radiusBox.end == expectedEnd);
}

TEST(bounding_box, get_methods) {
    SG::BoundingBox box;
    box.ini = {{0, 0, 0}};
    box.end = {{1, 1, 1}};
    SG::PointType expectedSize = {{1.0, 1.0, 1.0}};
    SG::PointType expectedRadius = {{0.5, 0.5, 0.5}};
    SG::PointType expectedCenter = {{0.5, 0.5, 0.5}};
    EXPECT_EQ(box.GetSize(), expectedSize);
    EXPECT_EQ(box.GetRadius(), expectedRadius);
    EXPECT_EQ(box.GetCenter(), expectedCenter);

    box.ini = {{-2, -1, 0}};
    box.end = {{-1, 0, 1}};
    expectedSize = {{1., 1., 1.}};
    expectedRadius = {{0.5, 0.5, 0.5}};
    expectedCenter = {{-1.5, -0.5, 0.5}};
    EXPECT_EQ(box.GetSize(), expectedSize);
    EXPECT_EQ(box.GetRadius(), expectedRadius);
    EXPECT_EQ(box.GetCenter(), expectedCenter);
}

TEST(bounding_box, is_inside) {
    SG::BoundingBox box;
    box.ini = {{0, 0, 0}};
    box.end = {{1, 1, 1}};
    SG::PointType pos;
    pos = {{0.5, 0.5, 0.5}};
    EXPECT_TRUE(SG::is_inside(pos, box));
    pos = {{1.5, 0.5, 0.5}};
    EXPECT_FALSE(SG::is_inside(pos, box));
    pos = {{0.5, 1.5, 0.5}};
    EXPECT_FALSE(SG::is_inside(pos, box));
    pos = {{0.5, 0.5, 1.5}};
    EXPECT_FALSE(SG::is_inside(pos, box));
    pos = {{-0.5, 0.5, 0.5}};
    EXPECT_FALSE(SG::is_inside(pos, box));

    // Border is included
    pos = {{0.0, 0.0, 0.0}};
    EXPECT_TRUE(SG::is_inside(pos, box));
    pos = {{1.0, 1.0, 1.0}};
    EXPECT_TRUE(SG::is_inside(pos, box));
}

TEST(bounding_box, build_enclosing_box) {
    SG::BoundingBox box0;
    box0.ini = {{0, 1, 2}};
    box0.end = {{1, 2, 3}};

    SG::BoundingBox box1;
    box1.ini = {{0.5, 0.5, 0.5}};
    box1.end = {{4, 5, 6}};

    SG::BoundingBox box2;
    box2.ini = {{-2, -3, -4}};
    box2.end = {{0.5, 0.5, 0.5}};

    std::vector<SG::BoundingBox> boxes = {box0, box1, box2};
    const auto enclosing_box = SG::BoundingBox::BuildEnclosingBox(boxes);
    EXPECT_FLOAT_EQ(enclosing_box.ini[0], -2);
    EXPECT_FLOAT_EQ(enclosing_box.ini[1], -3);
    EXPECT_FLOAT_EQ(enclosing_box.ini[2], -4);
    EXPECT_FLOAT_EQ(enclosing_box.end[0], 4);
    EXPECT_FLOAT_EQ(enclosing_box.end[1], 5);
    EXPECT_FLOAT_EQ(enclosing_box.end[2], 6);
}

TEST(bounding_box, are_bounds_inside) {
    SG::BoundingBox box;
    box.ini = {{0, 0, 0}};
    box.end = {{1, 1, 1}};

    SG::BoundingBox external_box;
    box.ini = {{0, 0, 0}};
    box.end = {{2, 2, 2}};

    EXPECT_TRUE(box.are_bounds_inside(external_box));
}
