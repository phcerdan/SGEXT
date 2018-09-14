#include "catch_header.h"
#include "edge_points_utilities.hpp"

TEST_CASE("edge_points_length",
          "[edge_points_length]")
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
          "[contour_length]")
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
          "[contour_length]")
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
          "[contour_length]")
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
          "[contour_length]")
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

// see images/contour_length_cornercase1
TEST_CASE("contour_length corner case 1",
          "[contour_length]")
{
    auto sg = SG::GraphType(2);

    SG::SpatialEdge::PointType p0 = {{179,160,46}};
    SG::SpatialEdge::PointType p3 = {{179,162,45}};
    sg[0].pos = p0;
    sg[1].pos = p3;
    SG::SpatialEdge se;
    se.edge_points.insert(std::end(se.edge_points), {
            {178,160,47},{178,160,48},{178,160,49},{178,160,50},{179,159,51},{180,160,52},{181,161,51},{180,161,50},{180,161,49},{180,161,48},{180,161,47},{180,161,46}
            });
    auto edge = boost::add_edge(0,1,se,sg);
    CHECK_NOTHROW(SG::contour_length(edge.first, sg));
}

// TEST_CASE_("contour_length_corner case 2",
//                  "[contour_length]")
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
