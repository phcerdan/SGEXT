/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "boundary_conditions.hpp"
#include "gmock/gmock.h"

using namespace testing;

class PeriodicBCFixture : public ::testing::Test {
  public:
    const double pi = atan(1) * 4;
    const double eps = std::numeric_limits<double>::epsilon();
    const ArrayUtilities::Array3D box_size = {{1.0, 1.0, 1.0}};
    const ArrayUtilities::Array3D box_size_inverse = {{1.0, 1.0, 1.0}};
    ArrayUtilities::Array3D a0 = {{0.2, 0.0, 0.0}};
    ArrayUtilities::Array3D a1 = {{0.9, 0.0, 0.0}};
    // a0 + a1 or a1 + a0
    ArrayUtilities::Array3D expected_sum = {{0.1, 0.0, 0.0}};
    // a0 - a1
    ArrayUtilities::Array3D expected_diff_01 = {{0.3, 0.0, 0.0}};
    // a1 - a0
    ArrayUtilities::Array3D expected_diff_10 = {{-0.3, 0.0, 0.0}};
    double expected_distance = 0.3;

    ArrayUtilities::Array3D a0_2d = {{0.1, 0.0, 0.0}};
    ArrayUtilities::Array3D a1_2d = {{0.9, 0.5, 0.0}};
};

/************ Plus *************/
TEST_F(PeriodicBCFixture, plus) {
    auto result_default_box =
            ArrayUtilities::plus_with_boundary_condition_periodic(a0, a1);
    EXPECT_THAT(result_default_box,
                ElementsAre(DoubleNear(expected_sum[0], eps),
                            DoubleNear(expected_sum[1], eps),
                            DoubleNear(expected_sum[2], eps)));
    auto result = ArrayUtilities::plus_with_boundary_condition_periodic(
            a0, a1, box_size, box_size_inverse);
    EXPECT_THAT(result,
                ElementsAre(DoubleNear(expected_sum[0], eps),
                            DoubleNear(expected_sum[1], eps),
                            DoubleNear(expected_sum[2], eps)));
}

/************ Minus *************/
TEST_F(PeriodicBCFixture, minus) {
    // a0 - a1
    auto result_default_box =
            ArrayUtilities::minus_with_boundary_condition_periodic(a0, a1);
    EXPECT_THAT(result_default_box,
                ElementsAre(DoubleNear(expected_diff_01[0], eps),
                            DoubleNear(expected_diff_01[1], eps),
                            DoubleNear(expected_diff_01[2], eps)));
    auto result = ArrayUtilities::minus_with_boundary_condition_periodic(
            a0, a1, box_size, box_size_inverse);
    EXPECT_THAT(result,
                ElementsAre(DoubleNear(expected_diff_01[0], eps),
                            DoubleNear(expected_diff_01[1], eps),
                            DoubleNear(expected_diff_01[2], eps)));

    // a1 - a0
    result_default_box =
            ArrayUtilities::minus_with_boundary_condition_periodic(a1, a0);
    EXPECT_THAT(result_default_box,
                ElementsAre(DoubleNear(expected_diff_10[0], eps),
                            DoubleNear(expected_diff_10[1], eps),
                            DoubleNear(expected_diff_10[2], eps)));
    result = ArrayUtilities::minus_with_boundary_condition_periodic(
            a1, a0, box_size, box_size_inverse);
    EXPECT_THAT(result,
                ElementsAre(DoubleNear(expected_diff_10[0], eps),
                            DoubleNear(expected_diff_10[1], eps),
                            DoubleNear(expected_diff_10[2], eps)));
}

/************ Distance *************/
TEST_F(PeriodicBCFixture, distance) {
    auto result_default_box =
            ArrayUtilities::distance_with_boundary_condition_periodic(a0, a1);
    EXPECT_DOUBLE_EQ(result_default_box, expected_distance);
    auto result = ArrayUtilities::distance_with_boundary_condition_periodic(
            a0, a1, box_size, box_size_inverse);
    EXPECT_DOUBLE_EQ(result, expected_distance);
}

TEST_F(PeriodicBCFixture, regular_distance_using_closest_image) {
    auto result_default_box =
            ArrayUtilities::distance_with_boundary_condition_periodic(a0, a1);
    auto a1_image = ArrayUtilities::closest_image_from_reference(a0, a1);
    auto regular_distance_using_closest_image =
            ArrayUtilities::distance(a0, a1_image);
    EXPECT_DOUBLE_EQ(regular_distance_using_closest_image, result_default_box);
}

TEST_F(PeriodicBCFixture, closest_image_from_reference) {
    auto a1_prime = ArrayUtilities::closest_image_from_reference(a0, a1);
    auto a0_prime = ArrayUtilities::closest_image_from_reference(a1, a0);

    EXPECT_DOUBLE_EQ(a1_prime[0], -0.1);
    EXPECT_DOUBLE_EQ(a0_prime[0], 1.2);
}

TEST_F(PeriodicBCFixture, angle_using_closest_image) {
    auto angle = ArrayUtilities::angle(a0_2d, a1_2d);
    EXPECT_DOUBLE_EQ(angle * 180 / pi, 29.054604099077146);
    auto a1_2d_image =
            ArrayUtilities::closest_image_from_reference(a0_2d, a1_2d);
    auto angle_image = ArrayUtilities::angle(a0_2d, a1_2d_image);
    EXPECT_DOUBLE_EQ(angle_image * 180 / pi, 101.30993247402021);
}
