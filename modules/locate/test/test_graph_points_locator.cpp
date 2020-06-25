/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"
#include "print_locator_points.hpp"
#include "spatial_graph_functors.hpp"
#include "spatial_graph_utilities.hpp"
#include "gmock/gmock.h"

/*
 * g0:        g1:
 *     o          o   v:0
 *     |          |
 *     .          .   e:(0,1)
 *     |          |
 *     o          o   v:1
 *     |          |
 *     .          .   e:(1,2)
 *     |          |
 *     o          o   v:2
 *                |
 *                .   e:(2,3)
 *                |
 *                o   v:3
 */
struct GraphPointLocatorMatchingFixture : public ::testing::Test {
    using GraphType = SG::GraphType;
    GraphType g0;
    GraphType g1;
    SG::BoundingBox box = {{-10, -10, -10}, {10, 10, 10}};
    void SetUp() override {
        using boost::add_edge;
        using boost::add_vertex;
        this->g0 = GraphType(3);

        // Add edge with an associated SpatialEdge at construction.
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        g0[0].pos = p0;
        g0[1].pos = e1;
        g0[2].pos = e2;

        SG::PointType e05{{0.5, 0, 0}};
        SG::PointType e15{{1.5, 0, 0}};
        SG::SpatialEdge se05;
        se05.edge_points.insert(std::end(se05.edge_points), {e05});
        add_edge(0, 1, se05, g0);
        SG::SpatialEdge se15;
        se15.edge_points.insert(std::end(se15.edge_points), {e15});
        add_edge(1, 2, se15, g0);
        // Add an extra node and edge to g1
        this->g1 = g0;
        SG::PointType e3{{3, 0, 0}};
        SG::SpatialNode sn3;
        sn3.pos = e3;
        auto new_vertex_id = add_vertex(sn3, g1);
        SG::PointType e25{{2.5, 0, 0}};
        SG::SpatialEdge se25;
        se25.edge_points.insert(std::end(se25.edge_points), {e25});
        add_edge(2, new_vertex_id, se25, g1);
    };
};

/*
 * MatchingFixture with a constant shift in all the positions.
 * The topology of the graph is conserved.
 *
 * g0:    moved_g0:        g1:       moved_g1:
 *     o                       o
 *     |           o           |             o
 *     .           |           .             |
 *     |           .           |             .
 *     o           |           o             |
 *     |           o           |             o
 *     .           |           .             |
 *     |           .           |             .
 *     o           |           o             |
 *                 o           |             o
 *                             .             |
 *                             |             .
 *                             o             |
 *                                           o
 *
 * Usually the tests use moved_g1 and g0 in locator functions.
 */
struct GraphPointLocatorCloseFixture : public GraphPointLocatorMatchingFixture {
    using GraphType = SG::GraphType;
    SG::PointType disturbance{{0.0, 0.1, 0.5}};
    GraphType moved_g0;
    GraphType moved_g1;
    void SetUp() override {
        GraphPointLocatorMatchingFixture::SetUp();
        this->moved_g0 = this->g0;
        this->moved_g1 = this->g1;
        auto func_plus = [&](SG::PointType &pos) {
            pos = ArrayUtilities::plus(pos, this->disturbance);
        };
        SG::operate_in_graph_pos(moved_g0, func_plus);
        SG::operate_in_graph_pos(moved_g1, func_plus);
    }
};

/**************************************/
/******* MatchingGraphs ********/
/**************************************/

TEST_F(GraphPointLocatorMatchingFixture,
       get_vtk_points_from_graph_works_for_g0) {
    auto point_map_pair = SG::get_vtk_points_from_graph(g0);
    size_t expected_num_points_g0 = 3 + 2;
    EXPECT_EQ(point_map_pair.first->GetNumberOfPoints(),
              expected_num_points_g0);
}

TEST_F(GraphPointLocatorMatchingFixture,
       get_vtk_points_from_graph_works_for_g1) {
    auto point_map_pair = SG::get_vtk_points_from_graph(g1);
    size_t expected_num_points_g1 = 4 + 3;
    EXPECT_EQ(point_map_pair.first->GetNumberOfPoints(),
              expected_num_points_g1);
}

TEST_F(GraphPointLocatorMatchingFixture, get_vtk_points_from_graphs) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    EXPECT_NO_THROW(SG::get_vtk_points_from_graphs(graphs));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    size_t expected_num_unique_points = 4 + 3;
    EXPECT_EQ(mergePoints->GetPoints()->GetNumberOfPoints(),
              expected_num_unique_points);
}

TEST_F(GraphPointLocatorMatchingFixture,
       octree_locator_works_for_exactly_equal_graphs) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g0));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());
    size_t expected_num_unique_points = 3 + 2;
    EXPECT_EQ(merger_map_pair.first->GetPoints()->GetNumberOfPoints(),
              expected_num_unique_points);
    EXPECT_EQ(octree->GetDataSet()->GetNumberOfPoints(),
              expected_num_unique_points);
    SG::PointType testPoint = {{2, 0, 0}}; // last vertex
    vtkIdType idClosest = octree->FindClosestPoint(testPoint.data());
    vtkIdType expectedId = 2;
    EXPECT_EQ(idClosest, expectedId);
}

TEST_F(GraphPointLocatorMatchingFixture,
       octree_locator_works_for_almost_equal_graphs) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());
    size_t expected_num_unique_points = 4 + 3;
    EXPECT_EQ(merger_map_pair.first->GetPoints()->GetNumberOfPoints(),
              expected_num_unique_points);
    EXPECT_EQ(octree->GetDataSet()->GetNumberOfPoints(),
              expected_num_unique_points);
    SG::PointType testPoint = {{3, 0, 0}};
    vtkIdType idClosest = octree->FindClosestPoint(testPoint.data());
    // expectedId: it is the first point added in the second graph, the first
    // graph defines 3 nodes + 2 edges, the index of the next is 5.
    vtkIdType expectedId = 5;
    EXPECT_EQ(idClosest, expectedId);
    // Get the coordinates of the closest point
    double closestPoint[3];
    octree->GetDataSet()->GetPoint(idClosest, closestPoint);
    EXPECT_EQ(closestPoint[0], 3.0);
    EXPECT_EQ(closestPoint[1], 0.0);
    EXPECT_EQ(closestPoint[2], 0.0);

    // examine the graph_descriptors
    const auto &graph_descriptor_map = merger_map_pair.second;
    const auto &graph_descs = graph_descriptor_map.at(idClosest);
    EXPECT_EQ(graph_descs.size(), 2);
    SG::print_graph_descriptor(graph_descs[0],
                               "graph0, id " + std::to_string(idClosest));
    SG::print_graph_descriptor(graph_descs[1],
                               "graph1, id " + std::to_string(idClosest));
    EXPECT_FALSE(graph_descs[0].exist);
    EXPECT_TRUE(graph_descs[1].exist);

    // for(const auto & key_value_pair : graph_descriptor_map){
    //     const auto & id = key_value_pair.first;
    //     const auto & gdescs = key_value_pair.second;
    //     for(const auto & gdesc : gdescs){
    //         SG::print_graph_descriptor(gdesc, std::to_string(id));
    //     }
    // }
}

TEST_F(GraphPointLocatorMatchingFixture, graph_closest_n_points_locator) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

    std::cout << "Print octree:" << std::endl;
    SG::print_locator_points(octree);

    SG::PointType testPoint = {{3, 0, 0}};
    const size_t closest_n_points = 1;
    auto closest_id_list = SG::graph_closest_n_points_locator(
            testPoint, octree, idMap, closest_n_points);
    std::cout << "Print test point: ";
    SG::print_pos(std::cout, testPoint);
    std::cout << std::endl;
    std::cout << "Print closest id_list for n_points " << closest_n_points
              << std::endl;
    vtkIdType expected_closest_id = 5;
    EXPECT_EQ(closest_id_list->GetId(0), expected_closest_id);

    testPoint = {{3.1, 0, 0}};
    EXPECT_EQ(SG::graph_closest_n_points_locator(testPoint, octree, idMap,
                                                 closest_n_points)
                      ->GetId(0),
              expected_closest_id);
}

TEST_F(GraphPointLocatorMatchingFixture,
       graph_closest_n_points_locator_descriptors) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

    SG::PointType testPoint = {{3, 0, 0}};
    const size_t closest_n_points = 5;
    auto closest_id_list = SG::graph_closest_n_points_locator(
            testPoint, octree, idMap, closest_n_points);
    auto out_gdescs =
            SG::closest_existing_descriptors_by_graph(closest_id_list, idMap);

    const auto &gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto &gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);

    // Check ids
    const auto &id0 = out_gdescs[0].id;
    const auto &id1 = out_gdescs[1].id;
    SG::print_point_from_id(id0, mergePoints->GetPoints());
    EXPECT_EQ(id0, 2);
    SG::print_point_from_id(id1, mergePoints->GetPoints());
    EXPECT_EQ(id1, 5);
}

TEST_F(GraphPointLocatorMatchingFixture,
       graph_closest_points_by_radius_locator) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

    SG::PointType testPoint = {{3.0, 0, 0}};
    double radius = 10.0;
    auto closest_id_list = SG::graph_closest_points_by_radius_locator(
            testPoint, octree, idMap, radius);
    auto out_gdescs =
            SG::closest_existing_descriptors_by_graph(closest_id_list, idMap);
    const auto &gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto &gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);
}

TEST_F(GraphPointLocatorMatchingFixture,
       graph_closest_points_by_radius_locator_small_radius) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g1));
    graphs.push_back(std::cref(g0));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());
    octree->SetDebug(true);
    octree->Print(std::cout);

    double bounds[6];
    octree->GetDataSet()->GetBounds(bounds);
    std::cout << "Bounds" << std::endl;
    std::cout << bounds[0] << ", " << bounds[1] << ", " << bounds[2] << ", "
              << bounds[3] << ", " << bounds[4] << ", " << bounds[5]
              << std::endl;

    SG::PointType testPoint = {{2.5, 0., 0.}};
    double radius = 0.2;
    std::cout << "Print points g0, g1" << std::endl;
    SG::print_locator_points(octree);
    std::cout << "Print test point: ";
    SG::print_pos(std::cout, testPoint);
    std::cout << std::endl;

    double dist2;
    auto closestId = octree->FindClosestPointWithinRadius(
            radius, testPoint.data(), dist2);
    vtkIdType expected_closest_id = 6;
    EXPECT_EQ(closestId, expected_closest_id);
    EXPECT_TRUE(dist2 >= 0. && dist2 <= radius);

    auto unsortedIdList = vtkSmartPointer<vtkIdList>::New();
    octree->FindPointsWithinRadius(radius, testPoint.data(), unsortedIdList);
    EXPECT_TRUE(unsortedIdList->GetNumberOfIds() > 0);
    // EXPECT_EQ(unsortedIdList->GetId(0), expected_closest_id);

    auto closest_id_list = SG::graph_closest_points_by_radius_locator(
            testPoint, octree, idMap, radius);
    EXPECT_TRUE(closest_id_list->GetNumberOfIds() > 0);
    // EXPECT_EQ(closest_id_list->GetId(0), expected_closest_id);
    std::cout << "Print closest id_list for radius " << radius << std::endl;
    SG::print_point_list(closest_id_list, mergePoints->GetPoints());

    octree->FindClosestNPoints(1, testPoint.data(), closest_id_list);
    EXPECT_TRUE(closest_id_list->GetNumberOfIds() > 0);
    EXPECT_EQ(closest_id_list->GetId(0), expected_closest_id);

    if (false) {
        auto out_gdescs = SG::closest_existing_descriptors_by_graph(
                closest_id_list, idMap);
        const auto &gdesc0 = out_gdescs[0].descriptor;
        SG::print_graph_descriptor(gdesc0);
        EXPECT_FALSE(gdesc0.exist);
        const auto &gdesc1 = out_gdescs[1].descriptor;
        SG::print_graph_descriptor(gdesc1);
        EXPECT_TRUE(gdesc1.exist);
        EXPECT_TRUE(gdesc1.is_vertex);
        EXPECT_FALSE(gdesc1.is_edge);
        EXPECT_EQ(gdesc1.vertex_d, 3);
    }
}

TEST_F(GraphPointLocatorMatchingFixture,
       graph_closest_points_by_radius_no_points_found) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

    SG::PointType testPoint = {{100.0, 0, 0}};
    double radius = 2.0;
    auto closest_id_list = SG::graph_closest_points_by_radius_locator(
            testPoint, octree, idMap, radius);
    auto out_gdescs =
            SG::closest_existing_descriptors_by_graph(closest_id_list, idMap);
    EXPECT_FALSE(out_gdescs[0].exist);
    EXPECT_FALSE(out_gdescs[1].exist);
    const auto &gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_FALSE(gdesc0.exist);
    const auto &gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1);
    EXPECT_FALSE(gdesc1.exist);
}

/**************************************/
/******* CloseGraphs ********/
/**************************************/

TEST_F(GraphPointLocatorCloseFixture,
       get_vtk_points_from_graph_works_for_moved_g0) {
    auto point_map_pair = SG::get_vtk_points_from_graph(moved_g0);
    size_t expected_num_points_moved_g0 = 3 + 2;
    EXPECT_EQ(point_map_pair.first->GetNumberOfPoints(),
              expected_num_points_moved_g0);
}

TEST_F(GraphPointLocatorCloseFixture,
       get_vtk_points_from_graph_works_for_moved_g1) {
    auto point_map_pair = SG::get_vtk_points_from_graph(moved_g1);
    size_t expected_num_points_moved_g1 = 4 + 3;
    EXPECT_EQ(point_map_pair.first->GetNumberOfPoints(),
              expected_num_points_moved_g1);
}

TEST_F(GraphPointLocatorCloseFixture, get_vtk_points_from_graphs) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(moved_g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    size_t expected_num_unique_points = 12; // (2 + 3) + (4 + 3);
    EXPECT_EQ(mergePoints->GetPoints()->GetNumberOfPoints(),
              expected_num_unique_points);
}

TEST_F(GraphPointLocatorCloseFixture, graph_closest_n_points_locator) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(moved_g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());
    size_t expected_num_unique_points = 12; // (2 + 3) + (4 + 3);
    EXPECT_EQ(mergePoints->GetPoints()->GetNumberOfPoints(),
              expected_num_unique_points);
    EXPECT_EQ(octree->GetDataSet()->GetNumberOfPoints(),
              expected_num_unique_points);

    std::cout << "Print octree:" << std::endl;
    SG::print_locator_points(octree);

    SG::PointType testPoint = {{3, 0.1, 0.5}};
    const size_t closest_n_points = 1;
    auto closest_id_list = SG::graph_closest_n_points_locator(
            testPoint, octree, idMap, closest_n_points);

    std::cout << "Print test point: ";
    SG::print_pos(std::cout, testPoint);
    std::cout << std::endl;
    std::cout << "Print closest id_list for n_points " << closest_n_points
              << std::endl;
    SG::print_point_list(closest_id_list, mergePoints->GetPoints());
}

TEST_F(GraphPointLocatorCloseFixture,
       graph_closest_n_points_locator_descriptors) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(moved_g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

    SG::PointType testPoint = {{3, 0.1, 0.5}};
    const size_t closest_n_points = 5;
    auto closest_id_list = SG::graph_closest_n_points_locator(
            testPoint, octree, idMap, closest_n_points);

    auto out_gdescs =
            SG::closest_existing_descriptors_by_graph(closest_id_list, idMap);
    const auto &gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0, "gdesc0");
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto &gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1, "gdesc1");
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);

    // Check ids
    const auto &id0 = out_gdescs[0].id;
    const auto &id1 = out_gdescs[1].id;
    SG::print_point_from_id(id0, mergePoints->GetPoints());
    EXPECT_EQ(id0, 2);
    SG::print_point_from_id(id1, mergePoints->GetPoints());
    EXPECT_EQ(id1, 8);
}

TEST_F(GraphPointLocatorCloseFixture,
       graph_closest_points_by_radius_locator_small_radius) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(moved_g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs, &box);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

    SG::PointType testPoint = {{3.0, 0, 0}};
    double radius = 1.0;
    auto closest_id_list = SG::graph_closest_points_by_radius_locator(
            testPoint, octree, idMap, radius);
    auto out_gdescs =
            SG::closest_existing_descriptors_by_graph(closest_id_list, idMap);
    const auto &gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto &gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);
}
