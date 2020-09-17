/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "tree_generation.hpp"

#include "sgext_fixture_images.hpp"
#include "gmock/gmock.h"

#include "spatial_graph.hpp"
#include <boost/graph/copy.hpp>

#include <unordered_map>

/**
 *           o gen:1
 *           |
 *           |
 *           |
 *  gen:1  __o__
 *        /     \  gen:2 --- gen:3
 *       /       o ----- o
 *      /         \
 *    _o_         _o_  gen: 2
 *   /   \       /   \
 *  /     \     /     \
 * o       o   o       o   gen: 3
 *
 */
struct TreeFixture : public ::testing::Test {
    using GraphType = SG::GraphType;
    using VertexToRadiusMap =
            std::unordered_map<GraphType::vertex_descriptor, double>;
    GraphType g0;
    VertexToRadiusMap vertex_to_radius_map;
    SG::FloatImageType::Pointer distance_map_image = SG::FloatImageType::New();
    const bool spatial_nodes_position_are_in_physical_space = false;

    void AllocateDistanceMapImage() {
        using RegionType = typename SG::FloatImageType::RegionType;
        typename RegionType::IndexType itk_index;
        itk_index[0] = 0;
        itk_index[1] = 0;
        itk_index[2] = 0;
        typename RegionType::SizeType itk_size;
        itk_size[0] = 50;
        itk_size[1] = 50;
        itk_size[2] = 1;
        RegionType itk_region(itk_index, itk_size);
        distance_map_image->SetRegions(itk_region);
        distance_map_image->Allocate();
    }
    // Allow to shift_x_position to create shifted graphs.
    GraphType CreateGraphAndPopulateImage(const double shift_x_position = 0) {
        auto graph = GraphType(10);

        SG::PointType root{{shift_x_position + 4, 8, 0}};
        SG::PointType b0{{shift_x_position + 4, 6, 0}};
        SG::PointType l1{{shift_x_position + 2, 4, 0}};
        SG::PointType r0{{shift_x_position + 5, 5, 0}};
        SG::PointType r0r{{shift_x_position + 8, 5, 0}};
        SG::PointType r1{{shift_x_position + 6, 4, 0}};
        SG::PointType l2l{{shift_x_position + 1, 2, 0}};
        SG::PointType l2r{{shift_x_position + 3, 2, 0}};
        SG::PointType r2l{{shift_x_position + 5, 2, 0}};
        SG::PointType r2r{{shift_x_position + 7, 2, 0}};
        graph[0].pos = root;
        graph[1].pos = b0;
        graph[2].pos = l1;
        graph[3].pos = r0;
        graph[4].pos = r0r;
        graph[5].pos = r1;
        graph[6].pos = l2l;
        graph[7].pos = l2r;
        graph[8].pos = r2l;
        graph[9].pos = r2r;

        add_edge(0, 1, SG::SpatialEdge(), graph); // root -> b0
        add_edge(1, 2, SG::SpatialEdge(), graph); // b0 -> l1
        add_edge(1, 3, SG::SpatialEdge(), graph); // b0 -> r0
        add_edge(3, 4, SG::SpatialEdge(), graph); // r0 -> r0r
        add_edge(3, 5, SG::SpatialEdge(), graph); // r0 -> r1
        add_edge(2, 6, SG::SpatialEdge(), graph); // l1 -> l2l
        add_edge(2, 7, SG::SpatialEdge(), graph); // l1 -> l2r
        add_edge(5, 8, SG::SpatialEdge(), graph); // r1 -> r2l
        add_edge(5, 9, SG::SpatialEdge(), graph); // r1 -> r2r

        // Populate the vertex_to_radius_map map (obtained from binary image)
        vertex_to_radius_map.emplace(0, 20); // root;
        vertex_to_radius_map.emplace(1, 18); // b0;
        vertex_to_radius_map.emplace(2, 15); // l1;
        vertex_to_radius_map.emplace(3, 16); // r0;
        vertex_to_radius_map.emplace(4, 8);  // r0r;
        vertex_to_radius_map.emplace(5, 15); // r1;
        vertex_to_radius_map.emplace(6, 6);  // l2l;
        vertex_to_radius_map.emplace(7, 6);  // l2r;
        vertex_to_radius_map.emplace(8, 6);  // r2l;
        vertex_to_radius_map.emplace(9, 6);  // r2r;


        using IndexType = typename SG::FloatImageType::IndexType;

        IndexType root_index;
        root_index[0] = root[0];
        root_index[1] = root[1];
        root_index[2] = root[2];
        distance_map_image->SetPixel(root_index, vertex_to_radius_map.at(0));
        IndexType b0_index;
        b0_index[0] = b0[0];
        b0_index[1] = b0[1];
        b0_index[2] = b0[2];
        distance_map_image->SetPixel(b0_index, vertex_to_radius_map.at(1));
        IndexType l1_index;
        l1_index[0] = l1[0];
        l1_index[1] = l1[1];
        l1_index[2] = l1[2];
        distance_map_image->SetPixel(l1_index, vertex_to_radius_map.at(2));
        IndexType r0_index;
        r0_index[0] = r0[0];
        r0_index[1] = r0[1];
        r0_index[2] = r0[2];
        distance_map_image->SetPixel(r0_index, vertex_to_radius_map.at(3));
        IndexType r0r_index;
        r0r_index[0] = r0r[0];
        r0r_index[1] = r0r[1];
        r0r_index[2] = r0r[2];
        distance_map_image->SetPixel(r0r_index, vertex_to_radius_map.at(4));
        IndexType r1_index;
        r1_index[0] = r1[0];
        r1_index[1] = r1[1];
        r1_index[2] = r1[2];
        distance_map_image->SetPixel(r1_index, vertex_to_radius_map.at(5));
        IndexType l2l_index;
        l2l_index[0] = l2l[0];
        l2l_index[1] = l2l[1];
        l2l_index[2] = l2l[2];
        distance_map_image->SetPixel(l2l_index, vertex_to_radius_map.at(6));
        IndexType l2r_index;
        l2r_index[0] = l2r[0];
        l2r_index[1] = l2r[1];
        l2r_index[2] = l2r[2];
        distance_map_image->SetPixel(l2r_index, vertex_to_radius_map.at(7));
        IndexType r2l_index;
        r2l_index[0] = r2l[0];
        r2l_index[1] = r2l[1];
        r2l_index[2] = r2l[2];
        distance_map_image->SetPixel(r2l_index, vertex_to_radius_map.at(8));
        IndexType r2r_index;
        r2r_index[0] = r2r[0];
        r2r_index[1] = r2r[1];
        r2r_index[2] = r2r[2];
        distance_map_image->SetPixel(r2r_index, vertex_to_radius_map.at(9));
        return graph;
    }

    void SetUp() override {
        using boost::add_edge;
        using boost::add_vertex;
        this->AllocateDistanceMapImage();
        this->g0 = this->CreateGraphAndPopulateImage();
    };
};

TEST_F(TreeFixture, tree_generation) {
    const double decrease_radius_ratio_to_increase_generation = 0.1;
    const double keep_generation_if_angle_less_than = 10;
    const double increase_generation_if_angle_greater_than = 40;
    const size_t num_of_edge_points_to_compute_angle = 5;
    const std::vector<SG::GraphType::vertex_descriptor> input_roots;
    const SG::VertexGenerationMap input_fix_generation_map;
    const bool verbose = true;
    const auto vertex_generation_map =
            SG::tree_generation(g0, distance_map_image,
                                spatial_nodes_position_are_in_physical_space,
                                decrease_radius_ratio_to_increase_generation,
                                keep_generation_if_angle_less_than,
                                increase_generation_if_angle_greater_than,
                                num_of_edge_points_to_compute_angle,
                                input_roots,
                                input_fix_generation_map, verbose);
    const std::unordered_map<GraphType::vertex_descriptor, size_t>
            expected_vertex_generation_map({
                    {0, 1}, // root
                    {1, 1}, // b0
                    {2, 2}, // l1
                    {3, 2}, // r0
                    {4, 3}, // r0r
                    {5, 2}, // r1
                    {6, 3}, // l2l
                    {7, 3}, // l2r
                    {8, 3}, // r2l
                    {9, 3}  // r2r
            });
    EXPECT_EQ(vertex_generation_map, expected_vertex_generation_map);

    // Write map to file (relative from where the test is executed)
    const std::string output_filename = "test_vertex_generation_map.txt";
    SG::write_vertex_to_generation_map(vertex_generation_map, output_filename);
    // Read that same file to get the map back
    auto map_from_file = SG::read_vertex_to_generation_map(output_filename);
    EXPECT_EQ(map_from_file, expected_vertex_generation_map);
}

TEST_F(TreeFixture, tree_generation_with_input_fixture_map) {
    const double decrease_radius_ratio_to_increase_generation = 0.1;
    const double keep_generation_if_angle_less_than = 10;
    const double increase_generation_if_angle_greater_than = 40;
    const size_t num_of_edge_points_to_compute_angle = 5;
    const std::vector<SG::GraphType::vertex_descriptor> input_roots;
    const bool verbose = true;

    const std::string input_fix_generation_map_file =
            SG::sgext_fixture_images_path + "/test_vertex_generation_map.txt";
    const SG::VertexGenerationMap input_fix_generation_map =
            SG::read_vertex_to_generation_map(input_fix_generation_map_file);
    EXPECT_EQ(input_fix_generation_map.size(), 1);
    EXPECT_EQ(input_fix_generation_map.at(5), 3);

    const auto vertex_generation_map =
            SG::tree_generation(g0, distance_map_image,
                                spatial_nodes_position_are_in_physical_space,
                                decrease_radius_ratio_to_increase_generation,
                                keep_generation_if_angle_less_than,
                                increase_generation_if_angle_greater_than,
                                num_of_edge_points_to_compute_angle,
                                input_roots,
                                input_fix_generation_map, verbose);
    std::unordered_map<GraphType::vertex_descriptor, size_t>
            expected_vertex_generation_map({
                    {0, 1}, // root
                    {1, 1}, // b0
                    {2, 2}, // l1
                    {3, 2}, // r0
                    {4, 3}, // r0r
                    {5, 3}, // r1-> FIXED: Modified by the user (+1) from 1 to 2
                    {6, 3}, // l2l
                    {7, 3}, // l2r
                    {8, 4}, // r2l-> Because 5 was modified, this is +1 after
                            // algorithm
                    {9, 4}  // r2r-> Because 5 was modiefied, this is +1 after
                            // algorithm
            });
    EXPECT_EQ(vertex_generation_map, expected_vertex_generation_map);
}

struct TreeFixtureMultipleComponents : public TreeFixture {
    GraphType g1;
    void SetUp() override {
        TreeFixture::SetUp();
        double shift_x_position = 10;
        this->g1 = this->CreateGraphAndPopulateImage(shift_x_position);
        // Merge g0 into g1 to have two components there.
        using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
        using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
        using edge_iterator = boost::graph_traits<GraphType>::edge_iterator;
        vertex_iterator g0_vit, g0_vit_end;
        edge_iterator g0_eit, g0_eit_end;
        std::tie(g0_vit, g0_vit_end) = boost::vertices(g0);
        std::tie(g0_eit, g0_eit_end) = boost::edges(g0);
        std::unordered_map<vertex_descriptor, vertex_descriptor> g0_to_g1_map;
        for(; g0_vit != g0_vit_end; ++g0_vit) {
            auto added_vertex = boost::add_vertex(g0[*g0_vit], g1);
            g0_to_g1_map.emplace(*g0_vit, added_vertex);
        }
        for(; g0_eit != g0_eit_end; ++g0_eit) {
            const auto source = boost::source(*g0_eit, g0);
            const auto target = boost::target(*g0_eit, g0);
            boost::add_edge(g0_to_g1_map[source], g0_to_g1_map[target], g0[*g0_eit], g1);
        }
    };
};

TEST_F(TreeFixtureMultipleComponents, tree_generation_with_multiple_components) {

    const double decrease_radius_ratio_to_increase_generation = 0.1;
    const double keep_generation_if_angle_less_than = 10;
    const double increase_generation_if_angle_greater_than = 40;
    const size_t num_of_edge_points_to_compute_angle = 5;
    const std::vector<SG::GraphType::vertex_descriptor> input_roots = {0,10};
    const bool verbose = true;

    const std::string input_fix_generation_map_file =
            SG::sgext_fixture_images_path + "/test_vertex_generation_map.txt";
    const SG::VertexGenerationMap input_fix_generation_map =
            SG::read_vertex_to_generation_map(input_fix_generation_map_file);
    EXPECT_EQ(input_fix_generation_map.size(), 1);
    EXPECT_EQ(input_fix_generation_map.at(5), 3);

    const auto vertex_generation_map =
            SG::tree_generation(g1, distance_map_image,
                                spatial_nodes_position_are_in_physical_space,
                                decrease_radius_ratio_to_increase_generation,
                                keep_generation_if_angle_less_than,
                                increase_generation_if_angle_greater_than,
                                num_of_edge_points_to_compute_angle,
                                input_roots,
                                input_fix_generation_map, verbose);
    std::unordered_map<GraphType::vertex_descriptor, size_t>
            expected_vertex_generation_map({
                    {0, 1}, // root
                    {1, 1}, // b0
                    {2, 2}, // l1
                    {3, 2}, // r0
                    {4, 3}, // r0r
                    {5, 3}, // r1-> FIXED: Modified by the user (+1) from 1 to 2
                    {6, 3}, // l2l
                    {7, 3}, // l2r
                    {8, 4}, // r2l-> Because 5 was modified, this is +1 after
                            // algorithm
                    {9, 4},  // r2r-> Because 5 was modiefied, this is +1 after
                            // algorithm
                    //This is the g0 (with no fix nodes) that was merged into g1
                    {10, 1}, // root
                    {11, 1}, // b0
                    {12, 2}, // l1
                    {13, 2}, // r0
                    {14, 3}, // r0r
                    {15, 2}, // r1
                    {16, 3}, // l2l
                    {17, 3}, // l2r
                    {18, 3}, // r2l
                    {19, 3}  // r2r
                            // algorithm
            });
    EXPECT_EQ(vertex_generation_map, expected_vertex_generation_map);
}
