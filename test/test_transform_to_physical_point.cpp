/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "transform_to_physical_point.hpp"
#include "spatial_graph.hpp"
#include "itkImage.h"

struct default_image : public ::testing::Test
{
    using ImageType = itk::Image<unsigned char, 3>;
    void SetUp() override{
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

struct non_default_image: public default_image
{
    void SetUp() override {
        default_image::SetUp();
        ImageType::PointType origin;
        origin.Fill(100.0);
        this->image->SetOrigin(origin);

        ImageType::SpacingType spacing;
        spacing[0] = 1.0; // spacing along X
        spacing[1] = 0.1; // spacing along Y
        spacing[2] = 10; // spacing along Z
        this->image->SetSpacing(spacing);

        // Do not modify direction
        ImageType::DirectionType direction;
        direction.SetIdentity();
        this->image->SetDirection( direction );
    }
};

TEST_F(default_image, index_array_to_physical_point_array)
{
    auto input_array = SG::PointType{{1, 1, 1}};
    auto trans_array = SG::index_array_to_physical_point_array<ImageType>(
            input_array,
            this->image);
    EXPECT_THAT(trans_array, input_array);
}

TEST_F(non_default_image, index_array_to_physical_point_array)
{
    auto input_array = SG::PointType{{1, 1, 1}};
    auto trans_array = SG::index_array_to_physical_point_array<ImageType>(
            input_array,
            this->image);
    EXPECT_EQ(trans_array[0], 101.0);
    EXPECT_EQ(trans_array[1], 100.1);
    EXPECT_EQ(trans_array[2], 110.0);
}

TEST_F(default_image, transform_graph_to_physical_point)
{
    using GraphType = SG::GraphAL;
    GraphType g(2);
    using boost::add_edge;
    g[0].pos = {{0, 0, 0}};
    g[1].pos = {{1, 1, 0}};

    SG::SpatialEdge se01_1;
    se01_1.edge_points.insert(std::end(se01_1.edge_points),
            {{1,0,0}});
    auto edge1 = add_edge(0, 1, se01_1, g);
    SG::SpatialEdge se01_2;
    se01_2.edge_points.insert(std::end(se01_2.edge_points),
            {{0,1,0}});
    auto edge2 = add_edge(0, 1, se01_2, g);

    SG::transform_graph_to_physical_point<ImageType>(g, this->image);

    EXPECT_EQ(g[0].pos[0], 0.0);
    EXPECT_EQ(g[0].pos[1], 0.0);
    EXPECT_EQ(g[0].pos[2], 0.0);
    EXPECT_EQ(g[1].pos[0], 1.0);
    EXPECT_EQ(g[1].pos[1], 1.0);
    EXPECT_EQ(g[1].pos[2], 0.0);
    auto ep1 = g[edge1.first].edge_points[0];
    auto ep2 = g[edge2.first].edge_points[0];
    EXPECT_EQ(ep1[0], 1.0);
    EXPECT_EQ(ep1[1], 0.0);
    EXPECT_EQ(ep1[2], 0.0);
    EXPECT_EQ(ep2[0], 0.0);
    EXPECT_EQ(ep2[1], 1.0);
    EXPECT_EQ(ep2[2], 0.0);
}

TEST_F(non_default_image, transform_graph_to_physical_point)
{
    using GraphType = SG::GraphAL;
    GraphType g(2);
    using boost::add_edge;
    g[0].pos = {{0, 0, 0}};
    g[1].pos = {{1, 1, 0}};

    SG::SpatialEdge se01_1;
    se01_1.edge_points.insert(std::end(se01_1.edge_points),
            {{1,0,0}});
    auto edge1 = add_edge(0, 1, se01_1, g);
    SG::SpatialEdge se01_2;
    se01_2.edge_points.insert(std::end(se01_2.edge_points),
            {{0,1,0}});
    auto edge2 = add_edge(0, 1, se01_2, g);

    SG::transform_graph_to_physical_point<ImageType>(g, this->image);

    EXPECT_EQ(g[0].pos[0], 100.0);
    EXPECT_EQ(g[0].pos[1], 100.0);
    EXPECT_EQ(g[0].pos[2], 100.0);
    EXPECT_EQ(g[1].pos[0], 101.0);
    EXPECT_EQ(g[1].pos[1], 100.1);
    EXPECT_EQ(g[1].pos[2], 100.0);
    auto ep1 = g[edge1.first].edge_points[0];
    auto ep2 = g[edge2.first].edge_points[0];
    EXPECT_EQ(ep1[0], 101.0);
    EXPECT_EQ(ep1[1], 100.0);
    EXPECT_EQ(ep1[2], 100.0);
    EXPECT_EQ(ep2[0], 100.0);
    EXPECT_EQ(ep2[1], 100.1);
    EXPECT_EQ(ep2[2], 100.0);
}
