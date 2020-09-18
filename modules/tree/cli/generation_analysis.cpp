/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "convert_to_vtk_unstructured_grid.hpp"
#include "detect_clusters.hpp"      // for assign_label_to_spatial_node_id
#include "filter_spatial_graph.hpp" // for copy_largest_connected_component
#include "spatial_graph_io.hpp"
#include "tree_generation.hpp"
#include "voxelize_graph.hpp"
#include "image_types.hpp" // For BinaryImageType

#include <vtkAppendFilter.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include "reconstruct_from_distance_map.hpp"

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char *const argv[]) {
    /*-------------- Parse command line -----------------------------*/
    po::options_description opt_desc("Allowed options are: ");
    opt_desc.add_options()("help,h", "display this message.");
    opt_desc.add_options()("graph,i", po::value<std::string>()->required(),
                           "Input spatial graph.");
    opt_desc.add_options()("distance_map,d",
                           po::value<std::string>()->required(),
                           "Input 3D Distance Map Image generated from script "
                           "sgext/cpp-scripts/create_distance_map.");
    opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                           "verbose output");
    opt_desc.add_options()(
            "visualize,t", po::bool_switch()->default_value(false),
            "Visualize thin result. Requires VISUALIZE option at build");
    opt_desc.add_options()(
            "output_folder,o", po::value<std::string>()->required(),
            "Folder to export the resulting graph, where spatial_node::id "
            "holds the generation number for each vertex.");
    opt_desc.add_options()("output_filename",
                           po::value<std::string>()->default_value(""),
                           "Name for the filename of output graph, if empty, a "
                           "name will be constructed based on the filename of "
                           "the input graph and input parameters.");
    opt_desc.add_options()("graphviz,z",
                           po::bool_switch()->default_value(false),
                           "Input graph is in graphviz format. When false, "
                           "assume it is in the inner serialized format.");
    opt_desc.add_options()(
            "reconstruct", po::bool_switch()->default_value(false),
            "Reconstruct the resulting graph into an unstructured grid with"
            "spheres of radius given by the distance map\n"
            "The graph stores the generation in its spatial_node_id, and the "
            "spheres are coloured accordingly to the generation.");
    opt_desc.add_options()(
            "voxelize", po::bool_switch()->default_value(false),
            "Create an output label image with pixel values equal to generation."
            );
    // Parameters
    opt_desc.add_options()("graph_positions_in_physical_space,p",
                           po::bool_switch()->default_value(false),
                           "positions in the graph are in physical space");
    opt_desc.add_options()("decrease_radius_ratio_to_increase_generation,r",
                           po::value<double>()->default_value(0.1),
                           "decrease_radius_ratio_to_increase_generation "
                           "controls when the generation of a node will be "
                           "increased, based on the radius decrease.");
    opt_desc.add_options()("keep_generation_if_angle_less_than,k",
                           po::value<double>()->default_value(0.1),
                           "Keep the same generation if the angle (in degrees) "
                           "is less than this value.");
    opt_desc.add_options()(
            "increase_generation_if_angle_greater_than,f",
            po::value<double>()->default_value(40),
            R"(Increase generation if the angle is greater than this value.
Angle analysis is used in a second pass analysis, and it only
applies to the cases where the first pass( based on radius) generates at least
two branches with the same generation.It fails to increase the target branch generation,
when one of the sibling branches (with same source) got its generation
increased by the radius first pass.)");
    opt_desc.add_options()(
            "num_of_edge_points_to_compute_angle,n",
            po::value<size_t>()->default_value(5),
            R"(When computing angles between edges with same source,
use the edge point number m_num_of_edge_points_to_compute_angle closer to source.
Warning: This is not an index, but a count.)");
      opt_desc.add_options()(
          "input_roots", po::value<std::vector<size_t> >()->multitoken()->default_value(std::vector<size_t>{},""),
          R"(Vector holding user specified root nodes. If empty, the algorithm takes
  as root the vertex with greatest radius (from the distance map), and
  runs only in the largest connected component of the graph.
  If multiple roots are provided, the algorithm will start visiting from them.
  Also, it will assume there are disconnected components in the graph, one for
  each root.)");
    opt_desc.add_options()(
            "input_fixed_generation_map_file,m",
            po::value<std::string>()->default_value(""),
            R"(File pointing to CSV file with lines: vertex_id, generation.
  This file will populate a vertex->generation map to fix the generation for certain nodes.
  The algorithm won't modify the generation on those vertex.
  Empty by default.)");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        if (vm.count("help") || argc <= 1) {
            std::cout << "Basic usage:\n" << opt_desc << "\n";
            return EXIT_SUCCESS;
        }
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Parse options
    const std::string graph_filename = vm["graph"].as<std::string>();
    const std::string distance_map_filename =
            vm["distance_map"].as<std::string>();
    const bool verbose = vm["verbose"].as<bool>();
    const bool visualize = vm["visualize"].as<bool>();
    const std::string output_folder = vm["output_folder"].as<std::string>();
    const std::string output_filename = vm["output_filename"].as<std::string>();
    const bool graphviz = vm["graphviz"].as<bool>();
    const bool reconstruct = vm["reconstruct"].as<bool>();
    const bool voxelize = vm["voxelize"].as<bool>();
    // Parameters
    const bool spatial_nodes_position_are_in_physical_space =
            vm["graph_positions_in_physical_space"].as<bool>();
    const double decrease_radius_ratio_to_increase_generation =
            vm["decrease_radius_ratio_to_increase_generation"].as<double>();
    const double keep_generation_if_angle_less_than =
            vm["keep_generation_if_angle_less_than"].as<double>();
    const double increase_generation_if_angle_greater_than =
            vm["increase_generation_if_angle_greater_than"].as<double>();
    const size_t num_of_edge_points_to_compute_angle =
            vm["num_of_edge_points_to_compute_angle"].as<size_t>();
    const std::vector<size_t> input_roots =
            vm["input_roots"].as<std::vector<size_t>>();
    const std::string input_fixed_generation_map_file =
            vm["input_fixed_generation_map_file"].as<std::string>();

    /* ************ Validation ************/

    const fs::path graph_path{graph_filename};
    if (!fs::exists(graph_path)) {
        std::cerr << "input graph filename doesn't exist : "
                  << graph_path.string() << std::endl;
        throw po::validation_error(po::validation_error::invalid_option_value,
                                   "graph");
    }
    const fs::path distance_map_path{distance_map_filename};
    if (!fs::exists(distance_map_path)) {
        std::cerr << "input distance map filename doesn't exist : "
                  << distance_map_path.string() << std::endl;
        throw po::validation_error(po::validation_error::invalid_option_value,
                                   "distance_map");
    }
    const fs::path output_folder_path{output_folder};
    if (!fs::exists(output_folder_path)) {
        std::cerr << "output folder doesn't exist : "
                  << output_folder_path.string() << std::endl;
        throw po::validation_error(po::validation_error::invalid_option_value,
                                   "output_folder");
    }
    const fs::path input_fixed_generation_map_path{
            input_fixed_generation_map_file};
    if (!input_fixed_generation_map_file
                 .empty()) { // It's optional, empty by default.
        if (!fs::exists(input_fixed_generation_map_path)) {
            std::cerr << "input_fixed_generation_map_file doesn't exist : "
                      << input_fixed_generation_map_path.string() << std::endl;
            throw po::validation_error(
                    po::validation_error::invalid_option_value,
                    "input_fixed_generation_map_file");
        }
    }
    /* ******** Read files *****************/
    // graph
    SG::GraphType input_sg; // modified only for the output
    if (graphviz) {
        SG::read_graphviz_sg(graph_path.string(), input_sg);
    } else {
        SG::read_serialized_sg(graph_path.string(), input_sg);
    }

    // distance map image
    using ReaderType = itk::ImageFileReader<SG::FloatImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(distance_map_path.string());
    reader->Update();
    const auto distance_map_image = reader->GetOutput();

    // input_fixed_generation_map (optional, empty by default)
    const SG::VertexGenerationMap input_fixed_generation_map =
            input_fixed_generation_map_file.empty()
                    ? SG::VertexGenerationMap()
                    : SG::read_vertex_to_generation_map(
                              input_fixed_generation_map_path.string());

    /* ******** Execute tree_generation *************/

    const auto vertex_to_generation_map =
            SG::tree_generation(input_sg, distance_map_image,
                                spatial_nodes_position_are_in_physical_space,
                                decrease_radius_ratio_to_increase_generation,
                                keep_generation_if_angle_less_than,
                                increase_generation_if_angle_greater_than,
                                num_of_edge_points_to_compute_angle,
                                input_roots,
                                input_fixed_generation_map, verbose);

    /* ******** Write Output ***************/
    const std::string parameters_append =
            "_gen_r" +
            std::to_string(decrease_radius_ratio_to_increase_generation) +
            "_kAng" + std::to_string(keep_generation_if_angle_less_than) +
            "_fAng" +
            std::to_string(increase_generation_if_angle_greater_than) +
            "_nAng" + std::to_string(num_of_edge_points_to_compute_angle);

    /* *** Save graph to a vtu file ****/
    SG::assign_label_to_spatial_node_id(input_sg, vertex_to_generation_map);
    const std::string graph_vtu_suffix = ".vtu";
    auto graph_vtu_path = output_folder_path;
    if (!output_filename.empty()) {
        graph_vtu_path /= (output_filename + graph_vtu_suffix);
    } else {
        const auto graph_path_stem = graph_path.stem();
        graph_vtu_path /= (graph_path_stem.string() + parameters_append +
                           graph_vtu_suffix);
    }
    // No need of edge points
    auto ugrid = SG::convert_to_vtk_unstructured_grid(input_sg);
    SG::write_vtk_unstructured_grid(ugrid, graph_vtu_path.string());

    /* *** Save vertex_to_generation_map to a .txt. file ****/
    auto output_vertex_to_generation_path = output_folder_path;
    const std::string output_vertex_to_generation_path_suffix = ".txt";
    const auto graph_path_stem = graph_path.stem();
    output_vertex_to_generation_path /=
            (graph_path_stem.string() + parameters_append + +"_GMAP" +
             output_vertex_to_generation_path_suffix);
    SG::write_vertex_to_generation_map(
            vertex_to_generation_map,
            output_vertex_to_generation_path.string());

    /* ********  Reconstruct from distance map ***************/

    if (reconstruct) {
        const bool distance_map_image_use_image_spacing = false;
        const bool apply_color_to_edges = true;
        auto reconstruct_result = SG::reconstruct_from_distance_map(
                input_sg, distance_map_image,
                spatial_nodes_position_are_in_physical_space,
                distance_map_image_use_image_spacing, vertex_to_generation_map,
                apply_color_to_edges);

        if (visualize) {
            SG::visualize_poly_data_and_graph(reconstruct_result.poly_data,
                                              input_sg, reconstruct_result.lut,
                                              graph_vtu_path.stem().string());
        }
        // Transform vtkPolyData to vtkUnstructuredGrid
        vtkSmartPointer<vtkAppendFilter> appendFilter =
                vtkSmartPointer<vtkAppendFilter>::New();
        appendFilter->AddInputData(reconstruct_result.poly_data);
        appendFilter->Update();

        vtkSmartPointer<vtkUnstructuredGrid> ugrid_reconstruct =
                vtkSmartPointer<vtkUnstructuredGrid>::New();
        ugrid_reconstruct->ShallowCopy(appendFilter->GetOutput());

        // Write the unstructured grid of the reconstructed ugrid
        auto reconstruct_vtu_path = output_folder_path;
        if (!output_filename.empty()) {
            reconstruct_vtu_path /=
                    (output_filename + "_reconstruct" + graph_vtu_suffix);
        } else {
            auto graph_path_stem = graph_path.stem();
            reconstruct_vtu_path /=
                    (graph_path_stem.string() + parameters_append +
                     "_reconstruct" + graph_vtu_suffix);
        }
        vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
                vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
        writer->SetFileName(reconstruct_vtu_path.string().c_str());
        writer->SetInputData(ugrid_reconstruct);
        writer->Write();
    }

    /* *** Generate an output label image with values equal to generations ***/
    if(voxelize) {
      const auto edge_to_label_map =
        SG::create_edge_to_label_map_from_vertex_to_label_map_using_max(
          input_sg,
          vertex_to_generation_map);

      using CastToBinaryFilterType =
        itk::CastImageFilter<SG::FloatImageType, SG::BinaryImageType>;
      auto cast_to_binary_filter = CastToBinaryFilterType::New();
      cast_to_binary_filter->SetInput(distance_map_image);
      cast_to_binary_filter->Update();

      const auto voxelized_graph = SG::voxelize_graph(input_sg,
          cast_to_binary_filter->GetOutput(),
          vertex_to_generation_map,
          edge_to_label_map,
          spatial_nodes_position_are_in_physical_space);

      auto voxelize_path = output_folder_path;
      const std::string voxelize_suffix = ".nrrd";
      if (!output_filename.empty()) {
        voxelize_path /=
          (output_filename + "_voxelize" + voxelize_suffix);
      } else {
        auto graph_path_stem = graph_path.stem();
        voxelize_path /=
          (graph_path_stem.string() + parameters_append +
           "_voxelize" + voxelize_suffix);
      }
      // Write voxelized image
      using WriterType = itk::ImageFileWriter<SG::BinaryImageType>;
      auto writer = WriterType::New();
      writer->SetFileName(voxelize_path.string().c_str());
      writer->SetInput(voxelized_graph);
      writer->Update();
    }

    if (verbose) {
        std::cout << "generation analysis ends" << std::endl;
    }
}
