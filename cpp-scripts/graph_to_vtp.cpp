#include <vtkVersion.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

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

#include "get_vtk_points_from_graph.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "serialize_spatial_graph.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]) {
  po::options_description opt_desc("Allowed options are: ");
  opt_desc.add_options()("help,h", "display this message.");
  opt_desc.add_options()("inputGraph,i", po::value<std::string>()->required(),
                         "Input graph.");
  opt_desc.add_options()("useGraphviz,g",
                         po::bool_switch()->default_value(false),
                         "Input graph is stored in graphviz (.dot) format. If off, it will "
                         "use serialized graphs (.txt).");
  opt_desc.add_options()(
      "outputFolder,o", po::value<std::string>()->required(),
      "Output folder. The filename would be based on the input, but the suffix "
      "would be .vtp");
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

  std::string filenameGraph = vm["inputGraph"].as<std::string>();
  const fs::path outputFolderPath{vm["outputFolder"].as<std::string>()};
  bool useGraphviz = vm["useGraphviz"].as<bool>();
  bool verbose = vm["verbose"].as<bool>();
  if(verbose) {
    std::cout << "Filename Input Graph: " << filenameGraph << std::endl;
  }

  SG::GraphType sg;
  if(useGraphviz) {  // read graphviz
    boost::dynamic_properties dp0;
    {
      dp0.property("node_id", boost::get(&SG::SpatialNode::id, sg));
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

  // Get the vtk points from the graph, and write them to file.
  const auto points_map_pair = SG::get_vtk_points_from_graph(sg);
  const auto &points = points_map_pair.first;

  // Create a polydata object and add the points to it.
  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  // Write the file
  const fs::path input_stem = fs::path(filenameGraph).stem();
  if(!fs::exists(outputFolderPath)) {
      throw std::runtime_error("output folder doesn't exist : " +
              outputFolderPath.string());
  }
  fs::path output_full_path =
      outputFolderPath / fs::path(input_stem.string() + ".vtp");

  auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(output_full_path.string().c_str());
  writer->SetInputData(polydata);

  // Optional - set the mode. The default is binary.
  //writer->SetDataModeToBinary();
  //writer->SetDataModeToAscii();

  writer->Write();
  if(verbose) {
    std::cout << "Output vtp file: " << output_full_path.string() << std::endl;
  }

  return EXIT_SUCCESS;
}
