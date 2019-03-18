#include "gmock/gmock.h"
#include "edge_points_utilities.hpp"

using ::testing::DoubleEq;

TEST(edge_points, edge_points_length) {
  SG::SpatialEdge se;
  EXPECT_EQ(SG::edge_points_length(se), 0.0);
  SG::SpatialEdge::PointType p0 = {{0, 0, 0}};
  se.edge_points.push_back(p0);
  EXPECT_EQ(SG::edge_points_length(se), 0.0);
  SG::SpatialEdge::PointType p1 = {{1, 0, 0}};
  se.edge_points.push_back(p1);
  EXPECT_THAT(SG::edge_points_length(se), DoubleEq(1.0));
  SG::SpatialEdge::PointType p2 = {{2, 0, 0}};
  se.edge_points.push_back(p2);
  EXPECT_THAT(SG::edge_points_length(se), DoubleEq(2.0));
}

TEST(contour_length, with_multiple_edge_points) {
  auto sg = SG::GraphType(2);
  SG::SpatialEdge::PointType p0 = {{0, 0, 0}};
  SG::SpatialEdge::PointType p3 = {{3, 0, 0}};
  sg[0].pos = p0;
  sg[1].pos = p3;
  SG::SpatialEdge::PointType p1 = {{1, 0, 0}};
  SG::SpatialEdge::PointType p2 = {{2, 0, 0}};
  SG::SpatialEdge se;
  se.edge_points = {{p1, p2}};
  auto edge = boost::add_edge(0, 1, se, sg);
  auto l = SG::contour_length(edge.first, sg);
  EXPECT_THAT(l, DoubleEq(3.0));
}

// TODO Disabled until contour_length throws again when
// spacing is properly handled.
TEST(contour_length, DISABLED_with_disconnected_points) {
  auto sg = SG::GraphType(2);
  SG::SpatialEdge::PointType p0 = {{0, 0, 0}};
  SG::SpatialEdge::PointType p3 = {{20, 0, 0}};
  sg[0].pos = p0;
  sg[1].pos = p3;
  SG::SpatialEdge::PointType p1 = {{1, 0, 0}};
  SG::SpatialEdge::PointType p2 = {{2, 0, 0}};
  SG::SpatialEdge se;
  se.edge_points = {{p1, p2}};
  auto edge = boost::add_edge(0, 1, se, sg);
  EXPECT_ANY_THROW(SG::contour_length(edge.first, sg));
}

TEST(contour_length, with_only_one_edge_point) {
  auto sg = SG::GraphType(2);
  SG::SpatialEdge::PointType p0 = {{0, 0, 0}};
  SG::SpatialEdge::PointType p3 = {{2, 0, 0}};
  sg[0].pos = p0;
  sg[1].pos = p3;
  SG::SpatialEdge::PointType p1 = {{1, 0, 0}};
  SG::SpatialEdge se;
  se.edge_points = {{p1}};
  auto edge = boost::add_edge(0, 1, se, sg);
  auto l = SG::contour_length(edge.first, sg);
  EXPECT_THAT(l, DoubleEq(2.0));
}
TEST(contour_length, with_zero_edge_point) {
  auto sg = SG::GraphType(2);
  SG::SpatialEdge::PointType p0 = {{0, 0, 0}};
  SG::SpatialEdge::PointType p3 = {{1, 0, 0}};
  sg[0].pos = p0;
  sg[1].pos = p3;
  SG::SpatialEdge se;
  auto edge = boost::add_edge(0, 1, se, sg);
  auto l = SG::contour_length(edge.first, sg);
  EXPECT_THAT(l, DoubleEq(1.0));
}

// see images/contour_length_cornercase1
TEST(contour_length, corner_case_1) {
  auto sg = SG::GraphType(2);

  SG::SpatialEdge::PointType p0 = {{179, 160, 46}};
  SG::SpatialEdge::PointType p3 = {{179, 162, 45}};
  sg[0].pos = p0;
  sg[1].pos = p3;
  SG::SpatialEdge se;
  se.edge_points.insert(std::end(se.edge_points), {{178, 160, 47},
                                                   {178, 160, 48},
                                                   {178, 160, 49},
                                                   {178, 160, 50},
                                                   {179, 159, 51},
                                                   {180, 160, 52},
                                                   {181, 161, 51},
                                                   {180, 161, 50},
                                                   {180, 161, 49},
                                                   {180, 161, 48},
                                                   {180, 161, 47},
                                                   {180, 161, 46}});
  auto edge = boost::add_edge(0, 1, se, sg);
  EXPECT_NO_THROW(SG::contour_length(edge.first, sg));
}

// TEST_(contour_length, corner_case_2)
// {
//     auto sg = SG::GraphType(2);
//
//     SG::SpatialEdge::PointType n0 = {{348, 158, 30}};
//     SG::SpatialEdge::PointType n00 = {{348, 158, 29}};
//     SG::SpatialEdge::PointType n01 = {{348, 159, 28}};
//     SG::SpatialEdge::PointType n1 = {{350, 157, 33}};
//     SG::SpatialEdge::PointType n2 = {{350, 160, 30}};
//     SG::SpatialEdge::PointType n20 = {{351, 160, 31}};
//     SG::SpatialEdge::PointType n21 = {{351, 161, 29}};
//     SG::SpatialEdge::PointType e0 = {{349, 158, 31}};
//     SG::SpatialEdge::PointType e1 = {{350, 158, 32}};
//     SG::SpatialEdge::PointType e10 = {{351, 158, 33}};
//     SG::SpatialEdge::PointType e11 = {{351, 159, 33}};
//     SG::SpatialEdge::PointType e2 = {{350, 159, 31}};
// }

TEST(insert_edge_point, with_distance_order) {
  SG::SpatialEdge::PointType new_point1 = {{-1, 0, 0}};
  SG::SpatialEdge::PointType p0 = {{0, 0, 0}};
  SG::SpatialEdge::PointType p1 = {{1, 0, 0}};
  SG::SpatialEdge::PointContainer edge_points = {{p0, p1}};
  // Insert it, should go to the front.
  SG::insert_unique_edge_point_with_distance_order(edge_points, new_point1);
  SG::SpatialEdge::PointContainer expected_edge_points1 = {
      {new_point1, p0, p1}};
  EXPECT_EQ(edge_points, expected_edge_points1);

  // Insert it, should go to the back
  SG::SpatialEdge::PointType new_point2 = {{2, 0, 0}};
  SG::insert_unique_edge_point_with_distance_order(edge_points, new_point2);
  SG::SpatialEdge::PointContainer expected_edge_points2 = {
      {new_point1, p0, p1, new_point2}};
  EXPECT_EQ(edge_points, expected_edge_points2);

  SG::SpatialEdge::PointType new_point3 = {{10, 10, 10}};
  EXPECT_ANY_THROW(SG::insert_unique_edge_point_with_distance_order(
      edge_points, new_point3));

  SG::SpatialEdge::PointType new_point4 = {{1, 1, 0}};
  EXPECT_ANY_THROW(SG::insert_unique_edge_point_with_distance_order(
      edge_points, new_point4));
}
