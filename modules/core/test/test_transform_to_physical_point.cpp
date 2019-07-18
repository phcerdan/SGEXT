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

#include "itkImage.h"
#include "spatial_graph.hpp"
#include "transform_to_physical_point.hpp"
#include "gmock/gmock.h"

struct default_image : public ::testing::Test {
    using ImageType = itk::Image<unsigned char, 3>;
    void SetUp() override {
        ImageType::RegionType region;
        ImageType::IndexType start;
        start[0] = 0;
        start[1] = 0;
        start[2] = 0;
        ImageType::SizeType size;
        size[0] = 10;
        size[1] = 10;
        size[2] = 10;
        region.SetSize(size);
        region.SetIndex(start);

        this->image = ImageType::New();
        this->image->SetRegions(region);
        this->image->Allocate();
    }
    ImageType::Pointer image;
};

struct non_default_image : public default_image {
    void SetUp() override {
        default_image::SetUp();
        ImageType::PointType origin;
        origin.Fill(100.0);
        this->image->SetOrigin(origin);

        ImageType::SpacingType spacing;
        spacing[0] = 1.0; // spacing along X
        spacing[1] = 0.1; // spacing along Y
        spacing[2] = 10;  // spacing along Z
        this->image->SetSpacing(spacing);

        // Do not modify direction
        ImageType::DirectionType direction;
        direction.SetIdentity();
        this->image->SetDirection(direction);
    }
};

TEST_F(default_image, index_array_to_physical_space_array) {
    auto input_array = SG::PointType{{1, 1, 1}};
    auto trans_array = SG::index_array_to_physical_space_array<ImageType>(
            input_array, this->image);
    EXPECT_THAT(trans_array, input_array);
}

TEST_F(non_default_image, index_array_to_physical_space_array) {
    auto input_array = SG::PointType{{1, 1, 1}};
    auto trans_array = SG::index_array_to_physical_space_array<ImageType>(
            input_array, this->image);
    EXPECT_DOUBLE_EQ(trans_array[0], 101.0);
    EXPECT_DOUBLE_EQ(trans_array[1], 100.1);
    EXPECT_DOUBLE_EQ(trans_array[2], 110.0);
}

TEST_F(non_default_image, physical_space_array_to_index_array) {
    auto input_array = SG::PointType{{101.0, 100.1, 110.0}};
    auto trans_array = SG::physical_space_array_to_index_array<ImageType>(
            input_array, this->image);
    EXPECT_DOUBLE_EQ(trans_array[0], 1);
    EXPECT_DOUBLE_EQ(trans_array[1], 1);
    EXPECT_DOUBLE_EQ(trans_array[2], 1);
}

TEST_F(default_image, transform_graph_to_physical_space) {
    using GraphType = SG::GraphAL;
    GraphType g(2);
    using boost::add_edge;
    g[0].pos = {{0, 0, 0}};
    g[1].pos = {{1, 1, 0}};

    SG::SpatialEdge se01_1;
    se01_1.edge_points.insert(std::end(se01_1.edge_points), {{1, 0, 0}});
    auto edge1 = add_edge(0, 1, se01_1, g);
    SG::SpatialEdge se01_2;
    se01_2.edge_points.insert(std::end(se01_2.edge_points), {{0, 1, 0}});
    auto edge2 = add_edge(0, 1, se01_2, g);

    SG::transform_graph_to_physical_space<ImageType>(g, this->image);

    EXPECT_DOUBLE_EQ(g[0].pos[0], 0.0);
    EXPECT_DOUBLE_EQ(g[0].pos[1], 0.0);
    EXPECT_DOUBLE_EQ(g[0].pos[2], 0.0);
    EXPECT_DOUBLE_EQ(g[1].pos[0], 1.0);
    EXPECT_DOUBLE_EQ(g[1].pos[1], 1.0);
    EXPECT_DOUBLE_EQ(g[1].pos[2], 0.0);
    auto ep1 = g[edge1.first].edge_points[0];
    auto ep2 = g[edge2.first].edge_points[0];
    EXPECT_DOUBLE_EQ(ep1[0], 1.0);
    EXPECT_DOUBLE_EQ(ep1[1], 0.0);
    EXPECT_DOUBLE_EQ(ep1[2], 0.0);
    EXPECT_DOUBLE_EQ(ep2[0], 0.0);
    EXPECT_DOUBLE_EQ(ep2[1], 1.0);
    EXPECT_DOUBLE_EQ(ep2[2], 0.0);
}

TEST_F(non_default_image, transform_graph_to_physical_space) {
    using GraphType = SG::GraphAL;
    GraphType g(2);
    using boost::add_edge;
    g[0].pos = {{0, 0, 0}};
    g[1].pos = {{1, 1, 0}};

    SG::SpatialEdge se01_1;
    se01_1.edge_points.insert(std::end(se01_1.edge_points), {{1, 0, 0}});
    auto edge1 = add_edge(0, 1, se01_1, g);
    SG::SpatialEdge se01_2;
    se01_2.edge_points.insert(std::end(se01_2.edge_points), {{0, 1, 0}});
    auto edge2 = add_edge(0, 1, se01_2, g);

    SG::transform_graph_to_physical_space<ImageType>(g, this->image);

    EXPECT_DOUBLE_EQ(g[0].pos[0], 100.0);
    EXPECT_DOUBLE_EQ(g[0].pos[1], 100.0);
    EXPECT_DOUBLE_EQ(g[0].pos[2], 100.0);
    EXPECT_DOUBLE_EQ(g[1].pos[0], 101.0);
    EXPECT_DOUBLE_EQ(g[1].pos[1], 100.1);
    EXPECT_DOUBLE_EQ(g[1].pos[2], 100.0);
    auto ep1 = g[edge1.first].edge_points[0];
    auto ep2 = g[edge2.first].edge_points[0];
    EXPECT_DOUBLE_EQ(ep1[0], 101.0);
    EXPECT_DOUBLE_EQ(ep1[1], 100.0);
    EXPECT_DOUBLE_EQ(ep1[2], 100.0);
    EXPECT_DOUBLE_EQ(ep2[0], 100.0);
    EXPECT_DOUBLE_EQ(ep2[1], 100.1);
    EXPECT_DOUBLE_EQ(ep2[2], 100.0);
}

TEST_F(non_default_image, transform_graph_to_index_space) {
    using GraphType = SG::GraphAL;
    GraphType g(2);
    using boost::add_edge;
    // g[0].pos = {{0, 0, 0}};
    g[0].pos = {{100.0, 100.0, 100.0}};
    // g[0].pos = {{1, 1, 0}};
    g[1].pos = {{101.0, 100.1, 100.0}};

    SG::SpatialEdge se01_1;
    se01_1.edge_points.insert(std::end(se01_1.edge_points),
                              // 1, 0, 0
                              {{101, 100.0, 100.0}});
    auto edge1 = add_edge(0, 1, se01_1, g);
    SG::SpatialEdge se01_2;
    se01_2.edge_points.insert(std::end(se01_2.edge_points),
                              // 0, 1, 0
                              {{100.0, 100.1, 100.0}});
    auto edge2 = add_edge(0, 1, se01_2, g);

    SG::transform_graph_to_index_space<ImageType>(g, this->image);

    EXPECT_DOUBLE_EQ(g[0].pos[0], 0.);
    EXPECT_DOUBLE_EQ(g[0].pos[1], 0.);
    EXPECT_DOUBLE_EQ(g[0].pos[2], 0.);
    EXPECT_DOUBLE_EQ(g[1].pos[0], 1.);
    EXPECT_DOUBLE_EQ(g[1].pos[1], 1.);
    EXPECT_DOUBLE_EQ(g[1].pos[2], 0.);
    auto ep1 = g[edge1.first].edge_points[0];
    auto ep2 = g[edge2.first].edge_points[0];
    EXPECT_DOUBLE_EQ(ep1[0], 1.);
    EXPECT_DOUBLE_EQ(ep1[1], 0.);
    EXPECT_DOUBLE_EQ(ep1[2], 0.);
    EXPECT_DOUBLE_EQ(ep2[0], 0.);
    EXPECT_DOUBLE_EQ(ep2[1], 1.);
    EXPECT_DOUBLE_EQ(ep2[2], 0.);
}
