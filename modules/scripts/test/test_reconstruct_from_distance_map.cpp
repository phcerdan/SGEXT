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

#include "reconstruct_from_distance_map.hpp"

#include "gmock/gmock.h"

struct TreeFixture : public ::testing::Test {
    using GraphType = SG::GraphType;
    using VertexToRadiusMap =
            std::unordered_map<GraphType::vertex_descriptor, double>;
    GraphType g0;
    VertexToRadiusMap vertex_to_radius_map;
    SG::FloatImageType::Pointer distance_map_image = SG::FloatImageType::New();
    const bool spatial_nodes_position_are_in_physical_space = false;

    void AllocateDistanceMapImage() { //NOLINT
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
    void SetUp() override {
        using boost::add_edge;
        using boost::add_vertex;
        this->g0 = GraphType(10);

        SG::PointType root{{20, 40, 0}};
        SG::PointType b0{{20, 30, 0}};
        SG::PointType l1{{10, 20, 0}};
        SG::PointType r0{{25, 25, 0}};
        SG::PointType r0r{{40, 25, 0}};
        SG::PointType r1{{30, 20, 0}};
        SG::PointType l2l{{5, 10, 0}};
        SG::PointType l2r{{15, 10, 0}};
        SG::PointType r2l{{25, 10, 0}};
        SG::PointType r2r{{35, 10, 0}};
        g0[0].pos = root;
        g0[1].pos = b0;
        g0[2].pos = l1;
        g0[3].pos = r0;
        g0[4].pos = r0r;
        g0[5].pos = r1;
        g0[6].pos = l2l;
        g0[7].pos = l2r;
        g0[8].pos = r2l;
        g0[9].pos = r2r;

        add_edge(0, 1, SG::SpatialEdge(), g0); // root -> b0
        add_edge(1, 2, SG::SpatialEdge(), g0); // b0 -> l1
        add_edge(1, 3, SG::SpatialEdge(), g0); // b0 -> r0
        add_edge(3, 4, SG::SpatialEdge(), g0); // r0 -> r0r
        add_edge(3, 5, SG::SpatialEdge(), g0); // r0 -> r1
        add_edge(2, 6, SG::SpatialEdge(), g0); // l1 -> l2l
        add_edge(2, 7, SG::SpatialEdge(), g0); // l1 -> l2r
        add_edge(5, 8, SG::SpatialEdge(), g0); // r1 -> r2l
        add_edge(5, 9, SG::SpatialEdge(), g0); // r1 -> r2r

        // Populate the vertex_to_radius_map map (obtained from binary image)
        vertex_to_radius_map.emplace(0, 2);   // root;
        vertex_to_radius_map.emplace(1, 1.8); // b0;
        vertex_to_radius_map.emplace(2, 1.5); // l1;
        vertex_to_radius_map.emplace(3, 1.6); // r0;
        vertex_to_radius_map.emplace(4, 0.8); // r0r;
        vertex_to_radius_map.emplace(5, 1.5); // r1;
        vertex_to_radius_map.emplace(6, 0.6); // l2l;
        vertex_to_radius_map.emplace(7, 0.6); // l2r;
        vertex_to_radius_map.emplace(8, 0.6); // r2l;
        vertex_to_radius_map.emplace(9, 0.6); // r2r;

        this->AllocateDistanceMapImage();

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
    };
};

TEST_F(TreeFixture, reconstruct_from_distance_map) {
    std::unordered_map<GraphType::vertex_descriptor, size_t>
            vertex_to_label_map;
    vertex_to_label_map.emplace(0, 0); // root;
    vertex_to_label_map.emplace(1, 0); // b0;
    vertex_to_label_map.emplace(2, 1); // l1;
    vertex_to_label_map.emplace(3, 1); // r0;
    vertex_to_label_map.emplace(4, 2); // r0r;
    vertex_to_label_map.emplace(5, 1); // r1;
    vertex_to_label_map.emplace(6, 2); // l2l;
    vertex_to_label_map.emplace(7, 2); // l2r;
    vertex_to_label_map.emplace(8, 2); // r2l;
    vertex_to_label_map.emplace(9, 2); // r2r;
    const bool spatial_nodes_position_are_in_physical_space = false;
    const bool distance_map_image_use_image_spacing = false;
    const bool apply_color_to_edges = false;
    auto poly_data = SG::reconstruct_from_distance_map(
            g0, distance_map_image,
            spatial_nodes_position_are_in_physical_space,
            distance_map_image_use_image_spacing, vertex_to_label_map,
            apply_color_to_edges);
}
