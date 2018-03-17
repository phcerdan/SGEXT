#include "catch_header.h"
#include "edge_points_utilities.hpp"

TEST_CASE("edge_points_length",
          "[edge_points_length")
{
    SG::SpatialEdge se;
    CHECK(SG::edge_points_length(se) == 0.0);
    SG::SpatialEdge::PointType p0 = {{0,0,0}};
    se.edge_points.push_back(p0);
    CHECK(SG::edge_points_length(se) == 0.0);
    SG::SpatialEdge::PointType p1 = {{1,0,0}};
    se.edge_points.push_back(p1);
    CHECK(SG::edge_points_length(se) == Approx(1.0));
    SG::SpatialEdge::PointType p2 = {{2,0,0}};
    se.edge_points.push_back(p2);
    CHECK(SG::edge_points_length(se) == Approx(2.0));
}

TEST_CASE("contour_length with multiple edge points",
          "[contour_length")
{
    auto sg = SG::GraphType(2);
    SG::SpatialEdge::PointType p0 = {{0,0,0}};
    SG::SpatialEdge::PointType p3 = {{3,0,0}};
    sg[0].pos = p0;
    sg[1].pos = p3;
    SG::SpatialEdge::PointType p1 = {{1,0,0}};
    SG::SpatialEdge::PointType p2 = {{2,0,0}};
    SG::SpatialEdge se;
    se.edge_points = {{p1,p2}};
    auto edge = boost::add_edge(0,1,se,sg);
    auto l = SG::contour_length(edge.first, sg);
    CHECK(l == Approx(3.0));
}
TEST_CASE("contour_length with disconnected points",
          "[contour_length")
{
    auto sg = SG::GraphType(2);
    SG::SpatialEdge::PointType p0 = {{0,0,0}};
    SG::SpatialEdge::PointType p3 = {{20,0,0}};
    sg[0].pos = p0;
    sg[1].pos = p3;
    SG::SpatialEdge::PointType p1 = {{1,0,0}};
    SG::SpatialEdge::PointType p2 = {{2,0,0}};
    SG::SpatialEdge se;
    se.edge_points = {{p1,p2}};
    auto edge = boost::add_edge(0,1,se,sg);
    CHECK_THROWS(SG::contour_length(edge.first, sg));
}
TEST_CASE("contour_length with only one edge_point",
          "[contour_length")
{
    auto sg = SG::GraphType(2);
    SG::SpatialEdge::PointType p0 = {{0,0,0}};
    SG::SpatialEdge::PointType p3 = {{2,0,0}};
    sg[0].pos = p0;
    sg[1].pos = p3;
    SG::SpatialEdge::PointType p1 = {{1,0,0}};
    SG::SpatialEdge se;
    se.edge_points = {{p1}};
    auto edge = boost::add_edge(0,1,se,sg);
    auto l = SG::contour_length(edge.first, sg);
    CHECK(l == Approx(2.0));
}
TEST_CASE("contour_length with zero edge_point",
          "[contour_length")
{
    auto sg = SG::GraphType(2);
    SG::SpatialEdge::PointType p0 = {{0,0,0}};
    SG::SpatialEdge::PointType p3 = {{1,0,0}};
    sg[0].pos = p0;
    sg[1].pos = p3;
    SG::SpatialEdge se;
    auto edge = boost::add_edge(0,1,se,sg);
    auto l = SG::contour_length(edge.first, sg);
    CHECK(l == Approx(1.0));
}

TEST_CASE("insert_edge_point_with_distance_order",
          "[insert_edge_point]")
{
    SG::SpatialEdge::PointType new_point1 = {{-1,0,0}};
    SG::SpatialEdge::PointType p0 = {{0,0,0}};
    SG::SpatialEdge::PointType p1 = {{1,0,0}};
    SG::SpatialEdge::PointContainer edge_points = {{p0, p1}};
    // Insert it, should go to the front.
    SG::insert_edge_point_with_distance_order(edge_points, new_point1);
    SG::SpatialEdge::PointContainer expected_edge_points1 = {{new_point1, p0, p1}};
    CHECK(edge_points == expected_edge_points1);

    // Insert it, should go to the back
    SG::SpatialEdge::PointType new_point2 = {{2,0,0}};
    SG::insert_edge_point_with_distance_order(edge_points, new_point2);
    SG::SpatialEdge::PointContainer expected_edge_points2 = {{new_point1, p0, p1, new_point2}};
    CHECK(edge_points == expected_edge_points2);

    SG::SpatialEdge::PointType new_point3 = {{10,10,10}};
    CHECK_THROWS( SG::insert_edge_point_with_distance_order(edge_points, new_point3) );

    SG::SpatialEdge::PointType new_point4 = {{1,1,0}};
    CHECK_THROWS( SG::insert_edge_point_with_distance_order(edge_points, new_point4) );
}
