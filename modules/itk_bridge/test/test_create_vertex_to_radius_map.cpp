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

#include "analyze_graph_function.hpp"
#include "create_vertex_to_radius_map.hpp"
#include "image_types.hpp"
#include "thin_function.hpp"

#include "itkImageFileReader.h"

#include "sgext_fixture_images.hpp"
#include "gmock/gmock.h"

#include <DGtal/topology/tables/NeighborhoodTables.h>
#include <boost/filesystem.hpp>

TEST(create_vertex_to_radius_map, create_vertex_to_radius_map) {
    // Read the image and thin it
    const std::string image_name_stem("bX3D_white");
    const std::string filename =
            SG::sgext_fixture_images_path + "/" + image_name_stem + ".nrrd";
    using ReaderType = itk::ImageFileReader<SG::BinaryImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    SG::BinaryImageType::Pointer input_image = reader->GetOutput();

    // Read the distance map fixture
    const std::string dmap_filename = SG::sgext_fixture_images_path + "/" +
                                      image_name_stem + "_DMAP.nrrd";
    using DistanceMapReaderType = itk::ImageFileReader<SG::FloatImageType>;
    auto dmap_reader = DistanceMapReaderType::New();
    dmap_reader->SetFileName(dmap_filename);
    dmap_reader->Update();
    SG::FloatImageType::Pointer dmap_image = dmap_reader->GetOutput();

    const std::string skel_type_str = "end";
    const std::string select_type_str = "dmax";
    const size_t persistence = 0;
    const boost::filesystem::path maybe_wrong_tableSimple26_6{
            DGtal::simplicity::tableSimple26_6};
    const std::string tables_folder =
            maybe_wrong_tableSimple26_6.parent_path().string();
    auto thin_image = SG::thin_function(reader->GetOutput(), skel_type_str,
                                        select_type_str, tables_folder,
                                        persistence, dmap_image);

    auto graph = SG::analyze_graph_function(thin_image, image_name_stem);

    const bool spatial_nodes_position_are_in_physical_space = false;
    const bool verbose = true;
    const auto vertex_to_radius_map = SG::create_vertex_to_radius_map(
            dmap_image, graph, spatial_nodes_position_are_in_physical_space,
            verbose);
    for(const auto & map_pair: vertex_to_radius_map) {
        std::cout << map_pair.first << ": " << map_pair.second << std::endl;
    }
    EXPECT_FLOAT_EQ(vertex_to_radius_map.at(1), 3);

}
