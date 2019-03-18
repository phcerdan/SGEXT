/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <chrono>
#include <unordered_map>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// graph
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graphviz.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

#include <itkImageFileReader.h>
#include <itkImageRegionIteratorWithIndex.h>
// Reduce graph via dfs:
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "serialize_spatial_graph.hpp"

#include "visualize_spatial_graph.hpp"
#include "visualize_spatial_graph_with_image.hpp"
#include <cmath>  // for std::abs

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]) {
  /*-------------- Parse command line -----------------------------*/
  po::options_description opt_desc("Allowed options are: ");
  opt_desc.add_options()("help,h", "display this message.");
  opt_desc.add_options()("inputGraph,g", po::value<string>()->required(),
                         "Input graph.");
  opt_desc.add_options()(
      "inputImage,i", po::value<string>()->default_value(""),
      "Input Binary Image. Skeletonized or not. If inputImage and thinImage "
      "are not provided, only the graph will be visualized.");
  opt_desc.add_options()("useSerialized,u",
                         po::bool_switch()->default_value(true),
                         "Use stored serialized graphs. If off, it will "
                         "require .dot graphviz files.");
  opt_desc.add_options()(
      "thinImage,s", po::value<string>()->default_value(""),
      "Input Thin Image (Skeletonized). If inputImage and thinImage are not "
      "provided, only the graph will be visualized.");
  opt_desc.add_options()("opacity,o", po::value<double>()->default_value(0.8),
                         "Opacity value.");
  opt_desc.add_options()("verbose,v", po::bool_switch()->default_value(false),
                         "verbose output.");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, opt_desc), vm);
    if(vm.count("help") || argc <= 1) {
      std::cout << "Basic usage:\n" << opt_desc << "\n";
      return EXIT_SUCCESS;
    }
    po::notify(vm);
  } catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  string filenameImage = vm["inputImage"].as<string>();
  string filenameGraph = vm["inputGraph"].as<string>();
  string filenameThinImage = vm["thinImage"].as<string>();
  double opacity = vm["opacity"].as<double>();
  bool verbose = vm["verbose"].as<bool>();
  if(verbose) {
    std::cout << "Filename Input Image: " << filenameImage << std::endl;
    std::cout << "Filename Thin Image: " << filenameThinImage << std::endl;
    std::cout << "Filename Input Graph: " << filenameGraph << std::endl;
  }
  bool useSerialized = vm["useSerialized"].as<bool>();

  SG::GraphType sg;
  if(!useSerialized) {  // read graphviz
    boost::dynamic_properties dp0;
    {
      dp0.property("node_id", boost::get(&SG::SpatialNode::label, sg));
      dp0.property("spatial_node", boost::get(boost::vertex_bundle, sg));
      dp0.property("spatial_edge", boost::get(boost::edge_bundle, sg));
      std::ifstream ifileLow(filenameGraph);
      boost::read_graphviz(ifileLow, sg, dp0, "node_id");
    }
  } else {
    sg = SG::read_serialized_graph(filenameGraph);
  }

  auto repeated_points_sg = SG::check_unique_points_in_graph(sg);
  if(repeated_points_sg.second) {
    std::cout << "Warning: duplicated points exist in the graph (sg)"
                 "Repeated Points: "
              << repeated_points_sg.first.size() << std::endl;
    for(const auto& p : repeated_points_sg.first) {
      SG::print_pos(std::cout, p);
      std::cout << std::endl;
    }
  }

  auto nvertices_sg = boost::num_vertices(sg);
  auto nedges_sg = boost::num_edges(sg);
  std::cout << "** sg:  vertices: " << nvertices_sg << ". edges: " << nedges_sg
            << std::endl;

  // Make the comparison between low and high and take the result

  bool image_provided = false;
  bool image_is_thin = false;
  if(filenameThinImage != "") image_is_thin = true;
  if(filenameImage != "" || image_is_thin) {
    image_provided = true;
  }
  if(!image_provided) {
    SG::visualize_spatial_graph(sg);
    return EXIT_SUCCESS;
  }
  const unsigned int Dim = 3;
  using PixelType = unsigned char;
  using ItkImageType = itk::Image<PixelType, Dim>;
  using ReaderType = itk::ImageFileReader<ItkImageType>;
  auto reader = ReaderType::New();
  if(image_is_thin) {
    reader->SetFileName(filenameThinImage);
    reader->Update();
    auto image = reader->GetOutput();
    // Get the points different than zero
    itk::ImageRegionIteratorWithIndex<ItkImageType> imageIterator(
        image, image->GetLargestPossibleRegion());
    auto points = vtkSmartPointer<vtkPoints>::New();
    while(!imageIterator.IsAtEnd()) {
      if(imageIterator.Get() > 0) {
        ItkImageType::PointType physical_point;
        image->TransformIndexToPhysicalPoint(imageIterator.GetIndex(),
                                             physical_point);
        points->InsertNextPoint(physical_point[0], physical_point[1],
                                physical_point[2]);
      }
      ++imageIterator;
    }
    ItkImageType::IndexType index0;
    index0.Fill(0);
    ItkImageType::PointType physical_point0;
    image->TransformIndexToPhysicalPoint(index0, physical_point0);
    ItkImageType::IndexType index1;
    index1.Fill(1);
    ItkImageType::PointType physical_point1;
    image->TransformIndexToPhysicalPoint(index1, physical_point1);
    auto rotated_length = physical_point1 - physical_point0;
    SG::visualize_spatial_graph_with_points(
        sg, points, opacity, std::abs(rotated_length[0]),
        std::abs(rotated_length[1]), std::abs(rotated_length[2]));
  } else {
    reader->SetFileName(filenameImage);
    reader->Update();
    SG::visualize_spatial_graph_with_image(sg, reader->GetOutput());
  }

}  // end main
