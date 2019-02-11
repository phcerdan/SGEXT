/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "graph_points_locator.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_locator_fixtures.hpp"
#include "filter_spatial_graph.hpp"

struct GraphPointLocatorFixture : public ::testing::Test {
    using GraphType = SG::GraphType;
    GraphType g0;
    GraphType g1;

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

TEST_F(GraphPointLocatorFixture, just_works)
{
    auto point_map_pair = SG::get_vtk_points_from_graph(g0);
    auto kdtree = SG::build_kdtree_locator(point_map_pair.first);
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), point_map_pair.first->GetNumberOfPoints());
}

TEST_F(GraphPointLocatorFixture, is_able_to_match_equal_graphs)
{
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g0));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(),
            merger_map_pair.first->GetPoints()->GetNumberOfPoints());
    SG::PointType testPoint = {{2,0,0}}; // last vertex
    vtkIdType idClosest = kdtree->FindClosestPoint(testPoint.data());
    vtkIdType expectedId = 2;
    EXPECT_EQ(idClosest, expectedId);
}

TEST_F(GraphPointLocatorFixture, is_able_to_match_almost_equal_graphs)
{
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(),
            merger_map_pair.first->GetPoints()->GetNumberOfPoints());
    SG::PointType testPoint = {{3,0,0}};
    vtkIdType idClosest = kdtree->FindClosestPoint(testPoint.data());
    // expectedId: it is the first point added in the second graph, the first graph
    // defines 3 nodes + 2 edges, the index of the next is 5.
    vtkIdType expectedId = 5;
    EXPECT_EQ(idClosest, expectedId);
    //Get the coordinates of the closest point
    double closestPoint[3];
    kdtree->GetDataSet()->GetPoint(idClosest, closestPoint);
    EXPECT_EQ(closestPoint[0], 3.0);
    EXPECT_EQ(closestPoint[1], 0.0);
    EXPECT_EQ(closestPoint[2], 0.0);

    // test the graph_descriptors
    const auto & graph_descriptor_map = merger_map_pair.second;
    const auto & graph_descs = graph_descriptor_map.at(idClosest);
    EXPECT_EQ(graph_descs.size(), 2);
    SG::print_graph_descriptor(graph_descs[0], "graph0, id " + std::to_string(idClosest));
    SG::print_graph_descriptor(graph_descs[1], "graph1, id " + std::to_string(idClosest));
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

TEST_F(GraphPointLocatorFixture, graph_closest_n_points_locator) {

    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 7);

    SG::PointType testPoint = {{3,0,0}};
    auto out_gdescs = SG::graph_closest_n_points_locator(testPoint, kdtree, merger_map_pair.second);
    const auto & gdesc0 = out_gdescs[0];
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto & gdesc1 = out_gdescs[1];
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);
}

TEST_F(GraphPointLocatorFixture, graph_closest_points_by_radius_locator) {

    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 7);

    SG::PointType testPoint = {{3.0,0,0}};
    double radius = 10.0;
    auto out_gdescs = SG::graph_closest_points_by_radius_locator(testPoint, kdtree, merger_map_pair.second, radius);
    const auto & gdesc0 = out_gdescs[0];
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto & gdesc1 = out_gdescs[1];
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);
}

TEST_F(GraphPointLocatorFixture, graph_closest_points_by_radius_locator_small_radius) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 7);

    SG::PointType testPoint = {{3.0,0,0}};
    double radius = 2.0;
    auto out_gdescs = SG::graph_closest_points_by_radius_locator(testPoint, kdtree, merger_map_pair.second, radius);
    const auto & gdesc0 = out_gdescs[0];
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto & gdesc1 = out_gdescs[1];
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);
}

// #include "visualize_spatial_graph.hpp"
// TEST_F(MatchingGraphsFixture, visualize_it)
// {
//     SG::visualize_spatial_graph(g0);
//     SG::visualize_spatial_graph(g1);
//     SG::visualize_spatial_graph(gR);
// }
//

TEST_F(MatchingGraphsFixture, works)
{
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    // g0 and g1 should have 13 unique points when combined
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 13);
    // testing::print_vtk_points(kdtree.GetPointer());

    // So... the big question: how do we compare graphs and construct the result?
    // a)
    // - iterate over every unique point of the kdtree.
    // Each point has a vector<graph_descriptor> associated to it.
    // - our candidates to take action on are points that do not exist in all the graphs.
    //  - kdtree locator can be used to find nearest point in other graphs.
    //
    // b) construct the graph
    // Use a filtered_graph (view)
    // filter by a set of vertex:
    // filter by a set of edges:
    // And optionally copy it into a new graph a the end

    std::unordered_set<GraphType::vertex_descriptor> removed_nodes;
    SG::EdgeDescriptorUnorderedSet removed_edges;
    // Iterate over all nodes of high-freq graph.
    // - Work on points that do not exist in the other graph
    // A) vertex in highG
    //  - edge_point in lowG ---> Means: new branch!
    //    Study neighbors of the vertex:
    // B) edge_point in highG
    //  - vertex in lowG ---> Means: branch is extended!
    //


};
