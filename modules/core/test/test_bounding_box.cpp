/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "bounding_box.hpp"

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
