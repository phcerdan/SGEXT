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

#ifndef ANALYZE_GRAPH_FUNCTION_HPP
#define ANALYZE_GRAPH_FUNCTION_HPP

#include "spatial_graph.hpp"
#include <itkImage.h>
#include <itkImageFileReader.h>
#include "transform_to_physical_point.hpp"
#include <string>

namespace SG {

template< typename ItkImageType>
typename ItkImageType::Pointer itk_image_from_file( const std::string & filename) {
    using ReaderType = itk::ImageFileReader<ItkImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    return reader->GetOutput();
}
/**
 * Read graph from a binary image file using ITK and DGtal
 *
 * @param filename
 *
 * @return SpatialGraph
 */
GraphType spatial_graph_from_file(const std::string & filename);

/**
 * Merge nodes optionally using all merge nodes methods
 *
 * @param reduced_g input/output spatial graph
 * @param mergeThreeConnectedNodes
 * @param mergeFourConnectedNodes
 * @param mergeTwoThreeConnectedNodes
 * @param verbose
 * @param inPlace if true (default), remove the filtered nodes in the graph,
 *                if false, the nodes will have a degree 0.
 *
 */
void merge_nodes_interface(
        GraphType &reduced_g,
        bool mergeThreeConnectedNodes = true,
        bool mergeFourConnectedNodes = true,
        bool mergeTwoThreeConnectedNodes = true,
        bool verbose = false,
        bool inPlace = true
        );

void check_parallel_edges_interface(GraphType & reduced_g, bool verbose = false);

template <typename ItkImageType>
void transform_to_physical_point_interface(
        GraphType & reduced_g,
        typename ItkImageType::Pointer itk_image,
        const std::string &spacing,
        bool verbose = false
        ) {
    typename ItkImageType::SpacingType itk_spacing;
    itk_spacing.Fill(1.0);
    if (verbose) {
        // Print metadata of itk image
        std::cout << "Origin: " << itk_image->GetOrigin()
            << std::endl;
        std::cout << "Spacing: " << itk_image->GetSpacing()
            << std::endl;
        std::cout << "Direction:\n "
            << itk_image->GetDirection() << std::endl;
    }
    itk_spacing = itk_image->GetSpacing();
    SG::transform_graph_to_physical_space<ItkImageType>(
            reduced_g, itk_image);
    if (spacing != "") {
        std::istringstream in(spacing);
        double sp;
        for (size_t i = 0; i < ItkImageType::ImageDimension; i++) {
            in >> sp;
            itk_spacing[i] = sp;
        }

        if (verbose) {
            std::cout << "Changing Spacing to: " << itk_spacing
                << std::endl;
        }

        itk_image->SetSpacing(itk_spacing);
        SG::transform_graph_to_physical_space<ItkImageType>(
                reduced_g, itk_image);
    }
    // // Format itk_spacing into a string:
    // std::ostringstream sp_stream;
    // sp_stream << itk_spacing[0] << "_" << itk_spacing[1] << "_"
    //     << itk_spacing[2];
    // auto sp_string = sp_stream.str();
    // std::cout << "spacing: " << sp_string << std::endl;
}


/**
 * GraphType not contstant because write_graphviz is not const-correct (boost side)
 *
 * @param reduced_g
 * @param exportReducedGraph_foldername
 * @param output_full_string
 * @param exportSerialized
 * @param exportVtu
 * @param exportGraphviz
 * @param verbose
 */
void export_graph_interface(GraphType & reduced_g,
        const std::string & exportReducedGraph_foldername,
        std::string output_full_string,
        bool exportSerialized = true,
        bool exportVtu = false,
        bool exportVtuWithEdgePoints = false,
        bool exportGraphviz = false,
        const bool verbose = false
        );

void export_graph_data_interface(const GraphType & reduced_g,
        const std::string & exportData_foldername,
        std::string output_full_string,
        bool ignoreAngleBetweenParallelEdges = false,
        bool ignoreEdgesToEndNodes = false,
        size_t ignoreEdgesShorterThan = 0,
        const bool verbose = false
        );
/**
 * Given an input binary image file (that can be read internally ITK)
 * Transform to it a DGtal Object, skeletonize it, and transform it into a
 * GraphType.
 */
GraphType analyze_graph_function(
        const std::string & filename,
        bool removeExtraEdges = true,
        bool mergeThreeConnectedNodes = true,
        bool mergeFourConnectedNodes = true,
        bool mergeTwoThreeConnectedNodes = true,
        bool checkParallelEdges = false,
        bool avoid_transformToPhysicalPoints = true,
        const std::string & spacing = "",
        bool output_filename_simple = false,
        const std::string & exportReducedGraph_foldername = "",
        bool exportSerialized = true,
        bool exportVtu = false,
        bool exportVtuWithEdgePoints = false,
        bool exportGraphviz = false,
        const std::string &exportData_foldername = "",
        bool ignoreAngleBetweenParallelEdges = false,
        bool ignoreEdgesToEndNodes = false,
        size_t ignoreEdgesShorterThan = 0,
        bool verbose = false,
        bool visualize = false);

} // end namespace SG
#endif
