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

#include "gmock/gmock.h"
#include "graph_points_locator.hpp"
#include "get_vtk_points_from_graph.hpp"

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
    auto closest_id_list = SG::graph_closest_n_points_locator(testPoint, kdtree, merger_map_pair.second);
    auto out_gdescs = SG::closest_existing_descriptors_by_graph(closest_id_list, merger_map_pair.second);
    const auto & gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto & gdesc1 = out_gdescs[1].descriptor;
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
    auto closest_id_list = SG::graph_closest_points_by_radius_locator(testPoint, kdtree, merger_map_pair.second, radius);
    auto out_gdescs = SG::closest_existing_descriptors_by_graph(closest_id_list, merger_map_pair.second);
    const auto & gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto & gdesc1 = out_gdescs[1].descriptor;
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
    auto closest_id_list = SG::graph_closest_points_by_radius_locator(testPoint, kdtree, merger_map_pair.second, radius);
    auto out_gdescs = SG::closest_existing_descriptors_by_graph(closest_id_list, merger_map_pair.second);
    const auto & gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_TRUE(gdesc0.exist);
    EXPECT_TRUE(gdesc0.is_vertex);
    EXPECT_FALSE(gdesc0.is_edge);
    EXPECT_EQ(gdesc0.vertex_d, 2);
    const auto & gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1);
    EXPECT_TRUE(gdesc1.exist);
    EXPECT_TRUE(gdesc1.is_vertex);
    EXPECT_FALSE(gdesc1.is_edge);
    EXPECT_EQ(gdesc1.vertex_d, 3);
}

TEST_F(GraphPointLocatorFixture, graph_closest_points_by_radius_no_points_found) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto kdtree = SG::build_kdtree_locator(merger_map_pair.first->GetPoints());
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 7);

    SG::PointType testPoint = {{100.0,0,0}};
    double radius = 2.0;
    auto closest_id_list = SG::graph_closest_points_by_radius_locator(testPoint, kdtree, merger_map_pair.second, radius);
    auto out_gdescs = SG::closest_existing_descriptors_by_graph(closest_id_list, merger_map_pair.second);
    EXPECT_FALSE(out_gdescs[0].exist);
    EXPECT_FALSE(out_gdescs[1].exist);
    const auto & gdesc0 = out_gdescs[0].descriptor;
    SG::print_graph_descriptor(gdesc0);
    EXPECT_FALSE(gdesc0.exist);
    const auto & gdesc1 = out_gdescs[1].descriptor;
    SG::print_graph_descriptor(gdesc1);
    EXPECT_FALSE(gdesc1.exist);
}
