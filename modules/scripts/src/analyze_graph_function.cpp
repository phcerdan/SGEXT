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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graphviz.hpp>
// serialize
#include "spatial_graph_io.hpp"
// Boost Filesystem
#include <boost/filesystem.hpp>

#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/ImageFromSet.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>
#include <DGtal/io/readers/GenericReader.h>
#include <DGtal/io/readers/ITKReader.h>
#include <chrono>
#include <iostream>
#include <unordered_map>
// graph
#include <DGtal/graph/ObjectBoostGraphInterface.h>
#include <DGtal/topology/Object.h>

// Reduce graph via dfs:
#include "merge_nodes.hpp"
#include "reduce_spatial_graph_via_dfs.hpp"
#include "remove_extra_edges.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_from_object.hpp"
#include "spatial_graph_utilities.hpp"

#ifdef SG_MODULE_VISUALIZE_ENABLED_WITH_QT
// Viewer
#include <DGtal/io/Color.h>
#include <DGtal/io/DrawWithDisplay3DModifier.h>
#include <DGtal/io/colormaps/GradientColorMap.h>
#include <DGtal/io/viewers/Viewer3D.h>
#endif

#ifdef SG_MODULE_VISUALIZE_ENABLED
#include "visualize_spatial_graph.hpp"
#include "visualize_spatial_graph_with_image.hpp"
// #include "itkViewImage.h"
#include "convert_to_vtk_unstructured_grid.hpp"
#endif

// compute graph properties
#include "compute_graph_properties.hpp"
// #include "spatial_histograms.hpp"

namespace fs = boost::filesystem;

namespace SG {

GraphType raw_graph_from_image(
        const SG::BinaryImageType::Pointer & thin_image) {

    using PixelType = SG::BinaryImagePixelType; //uchar
    using Domain = DGtal::Z3i::Domain;
    using Image = DGtal::ImageContainerByITKImage<Domain, PixelType>;

    // Convert to DGtal Container
    Image image(thin_image);

    using DigitalTopology = DGtal::Z3i::DT26_6;
    using DigitalSet = DGtal::DigitalSetByAssociativeContainer<
            Domain, std::unordered_set<typename Domain::Point> >;
    using Object = DGtal::Object<DigitalTopology, DigitalSet>;

    DigitalSet image_set(image.domain());
    DGtal::SetFromImage<DGtal::Z3i::DigitalSet>::append<Image>(image_set, image, 0, 255);

    DGtal::Z3i::KSpace ks;
    // Domain of kspace must be padded.
    ks.init(image.domain().lowerBound(), image.domain().upperBound(), true);
    DigitalTopology::ForegroundAdjacency adjF;
    DigitalTopology::BackgroundAdjacency adjB;
    DigitalTopology topo(adjF, adjB,
                         DGtal::DigitalTopologyProperties::JORDAN_DT);
    Object obj(topo, image_set);

    const GraphType sg =
        SG::spatial_graph_from_object<Object, GraphType>(obj);
    return sg;
}

GraphType raw_graph_from_image(const std::string & filename) {
    // Get filename without extension (and without folders).
    return SG::raw_graph_from_image(SG::itk_image_from_file<SG::BinaryImageType>(filename));
}

void merge_nodes_interface(
        GraphType &reduced_g,
        bool mergeThreeConnectedNodes,
        bool mergeFourConnectedNodes,
        bool mergeTwoThreeConnectedNodes,
        bool verbose,
        bool inPlace
        ) {
    if (mergeThreeConnectedNodes) {
        if (verbose) {
            std::cout << "Merging three connecting nodes... " << std::endl;
        }
        auto nodes_merged =
            SG::merge_three_connected_nodes(reduced_g, inPlace);
        if (verbose) {
            std::cout
                << nodes_merged
                << " interconnected nodes with degree 3 were merged."
                "Those nodes have now degree 0 if inPlace is not set"
                << std::endl;
        }
    }

    if (mergeFourConnectedNodes) {
        if (verbose) {
            std::cout << "Merging four connecting nodes... " << std::endl;
        }
        auto nodes_merged =
            SG::merge_four_connected_nodes(reduced_g, inPlace);
        if (verbose) {
            std::cout
                << nodes_merged
                << " interconnected nodes with degree 4 were merged."
                "Those nodes have now degree 0 if inPlace is not set"
                << std::endl;
        }
    }

    if (mergeTwoThreeConnectedNodes) {
        if (verbose) {
            std::cout << "Merging two degree 3 nodes... " << std::endl;
        }
        auto nodes_merged =
            SG::merge_two_three_connected_nodes(reduced_g, inPlace);
        if (verbose) {
            std::cout
                << nodes_merged
                << " two interconnected nodes with degree 3 "
                "and no edge points between them  were merged. "
                "Those nodes have now degree 0 if inPlace is not set"
                << std::endl;
        }
    }
}

void check_parallel_edges_interface(GraphType &reduced_g, bool verbose) {
    if (verbose) {
        std::cout << "Checking parallel edges... " << std::endl;
    }

    const bool unique_undirected_parallel_edges = true;
    auto parallel_edges =
            SG::get_parallel_edges(reduced_g, unique_undirected_parallel_edges);
    auto equal_parallel_edges =
            SG::get_equal_parallel_edges(parallel_edges, reduced_g);
    if (verbose) {
        std::cout << "Found " << parallel_edges.size() << " parallel edges. "
                  << equal_parallel_edges.size() << " are equal!." << std::endl;

        if (!equal_parallel_edges.empty()) {
            std::cout << "Equal parallel edges between vertex:\n";
            for (const auto &edge_pair : equal_parallel_edges) {
                std::cout << boost::source(edge_pair.first, reduced_g) << "---"
                          << boost::target(edge_pair.first, reduced_g)
                          << std::endl;
            }
        }
    }
}

void export_graph_interface(GraphType & reduced_g,
        const std::string & exportReducedGraph_foldername,
        const std::string & output_full_string,
        bool exportSerialized,
        bool exportVtu,
        bool exportVtuWithEdgePoints,
        bool exportGraphviz,
        const bool verbose
        ) {
    const fs::path output_folder_path{exportReducedGraph_foldername};
    if (!fs::exists(output_folder_path)) {
        throw std::runtime_error("output folder doesn't exist : " +
                output_folder_path.string());
    }
    if(exportSerialized) {
        fs::path output_full_path =
            output_folder_path / fs::path(output_full_string + "_serialized.txt");
        SG::write_serialized_sg(output_full_path.string(), reduced_g);
        if (verbose) {
            std::cout << "Output reduced graph (serialized) to: "
                << output_full_path.string() << std::endl;
        }
    }
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(boost::vertex_index, reduced_g));
    dp.property("spatial_node",
            boost::get(boost::vertex_bundle, reduced_g));
    dp.property("spatial_edge",
            boost::get(boost::edge_bundle, reduced_g));
    if(exportGraphviz) {
        fs::path output_full_path =
            output_folder_path /
            fs::path(output_full_string + ".dot");
        std::ofstream out;
        out.open(output_full_path.string().c_str());

        boost::write_graphviz_dp(out, reduced_g, dp);
        if (verbose) {
            std::cout << "Output reduced graph (graphviz) to: "
                << output_full_path.string() << std::endl;
        }
    }
#ifdef SG_MODULE_VISUALIZE_ENABLED
    // export vtu file
    if(exportVtu) {
        fs::path output_full_path =
            output_folder_path / fs::path(output_full_string + ".vtu");
        auto ugrid =
            SG::convert_to_vtk_unstructured_grid(reduced_g);
        SG::write_vtk_unstructured_grid(ugrid, output_full_path.string());
        if (verbose) {
            std::cout << "Output reduced graph (vtu) to: "
                << output_full_path.string() << std::endl;
        }
    }
    if(exportVtuWithEdgePoints) {
        fs::path output_full_path =
            output_folder_path / fs::path(output_full_string + "WithEdgePoints.vtu");
        auto ugrid =
            SG::convert_to_vtk_unstructured_grid_with_edge_points(reduced_g);
        SG::write_vtk_unstructured_grid(ugrid, output_full_path.string());
        if (verbose) {
            std::cout << "Output reduced graph (vtu with edge points) to: "
                << output_full_path.string() << std::endl;
        }
    }
#endif

    if(!exportGraphviz && !exportSerialized && !exportVtu) {
        if (verbose) {
            std::cout
                << "export_reduced_graph is not exporting graphviz, "
                "serialized graph, or vtu. Turn ON any of these options."
                << std::endl;
        }
    }
}

void export_graph_data_interface(const GraphType & reduced_g,
        const std::string & exportData_foldername,
        const std::string &output_full_string,
        bool ignoreAngleBetweenParallelEdges,
        bool ignoreEdgesToEndNodes,
        size_t ignoreEdgesShorterThan,
        const bool verbose
        ) {
    fs::path data_output_folder_path = fs::path(exportData_foldername);

    if (!fs::exists(data_output_folder_path)) {
        throw std::runtime_error("data_output folder doesn't exist : " +
                data_output_folder_path.string());
    }

    fs::path data_output_full_path =
        data_output_folder_path /
        fs::path(output_full_string + "_data.txt");
    std::ofstream data_out;
    data_out.setf(std::ios_base::fixed, std::ios_base::floatfield);
    data_out.open(data_output_full_path.string().c_str());
    // Degrees
    {
        auto degrees = SG::compute_degrees(reduced_g);
        // auto histo_degrees = SG::histogram_degrees(degrees,
        // binsHistoDegrees); SG::print_histogram(histo_degrees,
        // histo_out);
        {
            data_out.precision(
                    std::numeric_limits<decltype(
                        degrees)::value_type>::max_digits10);
            data_out << "# degrees" << std::endl;
            std::ostream_iterator<decltype(degrees)::value_type>
                out_iter(data_out, " ");
            std::copy(std::begin(degrees), std::end(degrees), out_iter);
            data_out << std::endl;
        }
    }
    // EndToEnd Distances
    {
        auto ete_distances = SG::compute_ete_distances(
                reduced_g, ignoreEdgesShorterThan,
                ignoreEdgesToEndNodes);

        auto range_ptr = std::minmax_element(ete_distances.begin(),
                ete_distances.end());
        if (verbose) {
            std::cout << "Min Distance: " << *range_ptr.first
                << std::endl;
            std::cout << "Max Distance: " << *range_ptr.second
                << std::endl;
        }
        // auto histo_ete_distances =
        // SG::histogram_ete_distances(ete_distances,
        // widthHistoDistances);
        // SG::print_histogram(histo_ete_distances, histo_out);
        {
            data_out.precision(
                    std::numeric_limits<decltype(
                        ete_distances)::value_type>::max_digits10);
            data_out << "# ete_distances" << std::endl;
            std::ostream_iterator<decltype(ete_distances)::value_type>
                out_iter(data_out, " ");
            std::copy(std::begin(ete_distances),
                    std::end(ete_distances), out_iter);
            data_out << std::endl;
        }
    }
    // Angles between adjacent edges
    {
        auto angles = SG::compute_angles(
                reduced_g, ignoreEdgesShorterThan,
                ignoreAngleBetweenParallelEdges, ignoreEdgesToEndNodes);
        // auto histo_angles = SG::histogram_angles( angles,
        // binsHistoAngles ); SG::print_histogram(histo_angles,
        // histo_out);
        {
            data_out.precision(
                    std::numeric_limits<decltype(
                        angles)::value_type>::max_digits10);
            data_out << "# angles" << std::endl;
            std::ostream_iterator<decltype(angles)::value_type>
                out_iter(data_out, " ");
            std::copy(std::begin(angles), std::end(angles), out_iter);
            data_out << std::endl;
        }
        // Cosines of those angles
        {
            auto cosines = SG::compute_cosines(angles);
            // auto histo_cosines = SG::histogram_cosines( cosines,
            // binsHistoCosines ); SG::print_histogram(histo_cosines,
            // histo_out);
            {
                data_out.precision(
                        std::numeric_limits<decltype(
                            cosines)::value_type>::max_digits10);
                data_out << "# cosines" << std::endl;
                std::ostream_iterator<decltype(cosines)::value_type>
                    out_iter(data_out, " ");
                std::copy(std::begin(cosines), std::end(cosines),
                        out_iter);
                data_out << std::endl;
            }
        }
    }
    // Contour length
    {
        auto contour_lengths = SG::compute_contour_lengths(
                reduced_g, ignoreEdgesShorterThan,
                ignoreEdgesToEndNodes);
        // auto histo_contour_lengths =
        // SG::histogram_contour_lengths(contour_lengths,
        // widthHistoDistances);
        // SG::print_histogram(histo_contour_lengths, histo_out);
        {
            data_out.precision(std::numeric_limits<decltype(
                        contour_lengths)::value_type>::
                    max_digits10);
            data_out << "# contour_lengths" << std::endl;
            std::ostream_iterator<decltype(contour_lengths)::value_type>
                out_iter(data_out, " ");
            std::copy(std::begin(contour_lengths),
                    std::end(contour_lengths), out_iter);
            data_out << std::endl;
        }
    }
    if (verbose) {
        std::cout << "Output data to: "
            << data_output_full_path.string() << std::endl;
    }
}

GraphType analyze_graph_function(
        const SG::BinaryImageType::Pointer & thin_image,
        const std::string & output_base_name,
        bool removeExtraEdges,
        bool mergeThreeConnectedNodes,
        bool mergeFourConnectedNodes,
        bool mergeTwoThreeConnectedNodes,
        bool checkParallelEdges,
        bool transformToPhysicalPoints,
        const std::string & spacing,
        bool output_filename_simple,
        const std::string & exportReducedGraph_foldername,
        bool exportSerialized,
        bool exportVtu,
        bool exportVtuWithEdgePoints,
        bool exportGraphviz,
        const std::string &exportData_foldername,
        bool ignoreAngleBetweenParallelEdges,
        bool ignoreEdgesToEndNodes,
        size_t ignoreEdgesShorterThan,
        bool verbose,
        bool visualize) {
    (void)visualize; // hack to remove visualize warning
    GraphType sg = raw_graph_from_image(thin_image);

    // Remove extra edges where connectivity in DGtal generates too many edges
    // in intersections
    if (removeExtraEdges) {
        if (verbose) {
            std::cout << "Removing extra edges" << std::endl;
        }
        size_t iterations = 0;
        while (true) {
            bool any_edge_removed = SG::remove_extra_edges(sg);
            if (any_edge_removed) {
                iterations++;
            } else {
                break;
            }
        }
        if (verbose) {
            std::cout << "Removed extra edges iteratively " << iterations
                << " times" << std::endl;
        }
    }
    // Reduce graph, removing nodes with degree 2
    GraphType reduced_g = SG::reduce_spatial_graph_via_dfs(sg);

    const bool inPlace = true;
    SG::merge_nodes_interface(reduced_g,
            mergeThreeConnectedNodes,
            mergeFourConnectedNodes,
            mergeTwoThreeConnectedNodes,
            verbose,
            inPlace);

    if (checkParallelEdges) {
        SG::check_parallel_edges_interface(reduced_g, verbose);
    }

    if (transformToPhysicalPoints) {
        SG::transform_to_physical_point_interface<SG::BinaryImageType>(
                reduced_g, thin_image, spacing, verbose);
    }
    // Check unique points of graph
    if (verbose) {
        auto repeated_points = SG::check_unique_points_in_graph(reduced_g);
        if (repeated_points.second) {
            std::cout << "Warning: duplicated points exist in reduced_g"
                "Repeated Points: "
                << repeated_points.first.size() << std::endl;
            for (const auto &p : repeated_points.first) {
                SG::print_pos(std::cout, p);
                std::cout << std::endl;
            }
        }
    }
#ifdef VISUALIZE_USING_QT
    if (visualize) {
        SG::visualize_spatial_graph(reduced_g);
        SG::visualize_spatial_graph_with_image(reduced_g, thin_image);
    }
#endif
    // Export data to files
    // Generate the base filename of the output
    const fs::path output_file_path =
            fs::path(output_base_name + "_REDUCED");
    // full file name without suffix (.txt, .vtu, etc)
    std::string output_full_string = output_file_path.string();
    if (!output_filename_simple) {
        output_full_string +=
            "_sp" + spacing + (removeExtraEdges ? "_c" : "") +
            (mergeThreeConnectedNodes ? "_m" : "");
    }
    // export using functions
    if(!exportReducedGraph_foldername.empty()) {
        export_graph_interface(reduced_g,
                exportReducedGraph_foldername,
                output_full_string,
                exportSerialized,
                exportVtu,
                exportVtuWithEdgePoints,
                exportGraphviz,
                verbose);
    }

    if(!exportData_foldername.empty()) {
        std::string data_output_full_string =
            output_file_path.string() + "_DATA";
        if (!output_filename_simple) {
            data_output_full_string +=
                ("_sp" + spacing) + (removeExtraEdges ? "_c" : "") +
                (mergeThreeConnectedNodes ? "_m" : "") +
                (ignoreAngleBetweenParallelEdges ? "_iPA" : "") +
                (ignoreEdgesToEndNodes ? "_x" : "") +
                (static_cast<bool>(ignoreEdgesShorterThan)
                 ? "_iShort" + std::to_string(
                     ignoreEdgesShorterThan)
                 : "");
        }
        export_graph_data_interface(reduced_g,
                exportData_foldername,
                output_full_string,
                ignoreAngleBetweenParallelEdges,
                ignoreEdgesToEndNodes,
                ignoreEdgesShorterThan,
                verbose);
    }

    return reduced_g;
};

GraphType analyze_graph_function_io(
        const std::string & filename,
        bool removeExtraEdges,
        bool mergeThreeConnectedNodes,
        bool mergeFourConnectedNodes,
        bool mergeTwoThreeConnectedNodes,
        bool checkParallelEdges,
        bool transformToPhysicalPoints,
        const std::string & spacing,
        bool output_filename_simple,
        const std::string & exportReducedGraph_foldername,
        bool exportSerialized,
        bool exportVtu,
        bool exportVtuWithEdgePoints,
        bool exportGraphviz,
        const std::string &exportData_foldername,
        bool ignoreAngleBetweenParallelEdges,
        bool ignoreEdgesToEndNodes,
        size_t ignoreEdgesShorterThan,
        bool verbose,
        bool visualize) {
    const auto itk_image =
        SG::itk_image_from_file<SG::BinaryImageType>(filename);
    const std::string output_base_name = fs::path(filename).stem().string();
    return analyze_graph_function(
            itk_image,
            output_base_name,
            removeExtraEdges,
            mergeThreeConnectedNodes,
            mergeFourConnectedNodes,
            mergeTwoThreeConnectedNodes,
            checkParallelEdges,
            transformToPhysicalPoints,
            spacing,
            output_filename_simple,
            exportReducedGraph_foldername,
            exportSerialized,
            exportVtu,
            exportVtuWithEdgePoints,
            exportGraphviz,
            exportData_foldername,
            ignoreAngleBetweenParallelEdges,
            ignoreEdgesToEndNodes,
            ignoreEdgesShorterThan,
            verbose,
            visualize);

};
} // end namespace SG
