#include <iostream>
#include <chrono>
#include <unordered_map>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/GenericReader.h>
#include <DGtal/io/readers/ITKReader.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"
#include "DGtal/images/imagesSetsUtils/ImageFromSet.h"
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// graph
#include "DGtal/topology/Object.h"
#include "DGtal/graph/ObjectBoostGraphInterface.h"
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graphviz.hpp>

// Boost Filesystem
#include <boost/filesystem.hpp>

// Viewer
#include "DGtal/io/Color.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include <DGtal/io/viewers/Viewer3D.h>
// Reduce graph via dfs:
#include "spatial_graph.hpp"
#include "reduce_dfs_visitor.hpp"
#include "spatial_graph_from_object.hpp"
#include "remove_extra_edges.hpp"
#include "merge_nodes.hpp"
#include "visualize_spatial_graph.hpp"
// #include "itkViewImage.h"
#include "visualize_spatial_graph_with_image.hpp"

// compute histograms
#include "spatial_histograms.hpp"

using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* const argv[]){

  /*-------------- Parse command line -----------------------------*/
  po::options_description general_opt ( "Allowed options are: " );
  general_opt.add_options()
    ( "help,h", "display this message." )
    ( "input,i", po::value<string>()->required(), "Input thin image." )
    ( "reduceGraph,r", po::bool_switch()->default_value(false), "Reduce obj graph into a new SpatialGraph, converting chain nodes (degree=2) into edge_points.")
    ( "removeExtraEdges,c", po::bool_switch()->default_value(false), "Remove extra edges created because connectivity of object.")
    ( "mergeThreeConnectedNodes,m", po::bool_switch()->default_value(false), "Merge three connected nodes (between themselves) into one node.")
    ( "mergeParallelEdges,p", po::bool_switch()->default_value(false), "Check and merge parallel edges that compare equal.")
    ( "binsHistoDegrees,d", po::value<size_t>()->default_value(0), "Bins for the histogram of degrees ." )
    ( "binsHistoDistances,l", po::value<size_t>()->default_value(0), "Bins for the histogram of distances ." )
    ( "binsHistoAngles,a", po::value<size_t>()->default_value(0), "Bins for the histogram of angles ." )
    ( "binsHistoCosines,n", po::value<size_t>()->default_value(0), "Bins for the histogram of cosines ." )
    ( "exportReducedGraph,o", po::value<string>(), "Write .dot file with the reduced spatial graph." )
    ( "exportHistograms,e", po::value<string>(), "Export histogram." )
    ( "visualize,t", po::bool_switch()->default_value(false), "Visualize object with DGtal.")
    ( "verbose,v",  po::bool_switch()->default_value(false), "verbose output." );

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
    if (vm.count ( "help" ) || argc<=1 )
    {
      std::cout << "Basic usage:\n" << general_opt << "\n";
      return EXIT_SUCCESS;
    }
    po::notify ( vm );
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  string filename = vm["input"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  bool reduceGraph = vm["reduceGraph"].as<bool>();
  bool removeExtraEdges = vm["removeExtraEdges"].as<bool>();
  bool mergeThreeConnectedNodes = vm["mergeThreeConnectedNodes"].as<bool>();
  bool mergeParallelEdges = vm["mergeParallelEdges"].as<bool>();
  bool visualize = vm["visualize"].as<bool>();
  bool exportHistograms = vm.count("exportHistograms");
  size_t binsHistoDegrees= vm["binsHistoDegrees"].as<size_t>();
  size_t binsHistoDistances= vm["binsHistoDistances"].as<size_t>();
  size_t binsHistoAngles= vm["binsHistoAngles"].as<size_t>();
  size_t binsHistoCosines= vm["binsHistoCosines"].as<size_t>();
  bool exportReducedGraph = vm.count("exportReducedGraph");
  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(
      input_stem.string() + "_reduced");

  using Domain = Z3i::Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;
  const unsigned int Dim = 3;
  using PixelType = unsigned char ;
  using ItkImageType = itk::Image<PixelType, Dim> ;
  // Read Image using ITK
  using ReaderType = itk::ImageFileReader<ItkImageType> ;
  auto reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();

  // Convert to DGtal Container
  Image image(reader->GetOutput());

  using DigitalTopology = DT26_6;
  using DigitalSet =
    DGtal::DigitalSetByAssociativeContainer<Domain ,
      std::unordered_set< typename Domain::Point> >;
  using Object =
    DGtal::Object<DigitalTopology, DigitalSet>;

  DigitalSet image_set (image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(
      image_set, image,
      0, 255);

  KSpace ks;
  // Domain of kspace must be padded.
  ks.init(image.domain().lowerBound(),
              image.domain().upperBound(), true);
  DigitalTopology::ForegroundAdjacency adjF;
  DigitalTopology::BackgroundAdjacency adjB;
  DigitalTopology topo(adjF, adjB, DGtal::DigitalTopologyProperties::JORDAN_DT);
  Object obj(topo,image_set);

  if(visualize)
  {
      int argc(1);
      char** argv(nullptr);
      QApplication app(argc, argv);
      Viewer3D<> viewer(ks);
      viewer.show();

      viewer.setFillColor(Color(255, 255, 255, 255));
      viewer << image_set;

      // All kspace voxels
      // viewer.setFillColor(Color(40, 200, 55, 10));
      // viewer << all_set;

      viewer << Viewer3D<>::updateDisplay;

      app.exec();
  }

  if(reduceGraph)
  {
    using Graph = Object;
    const Graph & graph = obj;
    using SpatialGraph = SG::GraphAL;
    SpatialGraph sg = SG::spatial_graph_from_object<Object, SpatialGraph>(graph);
    // Remove extra edges
    if(removeExtraEdges)
    {
      if(verbose)
        std::cout <<  "Removing extra edges" << std::endl;
      size_t iterations = 0;
      while(true) {
        bool any_edge_removed = SG::remove_extra_edges(sg);
        if(any_edge_removed)
          iterations++;
        else
          break;
      }
      if(verbose)
        std::cout <<  "Removed extra edges iteratively " << iterations << " times" << std::endl;
    }
    SpatialGraph reduced_g = SG::reduce_spatial_graph_via_dfs<SpatialGraph>(sg);

    if(mergeThreeConnectedNodes)
    {
      if(verbose){
        std::cout <<  "Merging three connecting nodes... " << std::endl;
      }
      auto nodes_merged = SG::merge_three_connected_nodes(reduced_g);
      if(verbose){
        std::cout << nodes_merged <<  " nodes were merged. Those nodes have now degree 0" << std::endl;
      }
    }

    if(mergeParallelEdges)
    {
      if(verbose){
        std::cout <<  "Merging equal parallel edges... " << std::endl;
      }
      auto parallel_edges = SG::get_parallel_edges(reduced_g);
      auto equal_parallel_edges = SG::get_equal_parallel_edges(parallel_edges, reduced_g);
      if(verbose){
        std::cout << "Found " << parallel_edges.size()
          << " parallel edges. " << equal_parallel_edges.size() <<
          " are equal!." << std::endl;

          std::cout << "Equal parallel edges between vertex:\n";
        for (const auto & edge_pair : equal_parallel_edges)
          std::cout
            << boost::source(edge_pair.first, reduced_g)
            << "---"
            << boost::target(edge_pair.first, reduced_g)
            << std::endl;
      }
    }

    if(exportReducedGraph)
    {
      string exportReducedGraph_filename = vm["exportReducedGraph"].as<string>();
      boost::dynamic_properties dp;
      dp.property("node_id", boost::get(boost::vertex_index, reduced_g));
      dp.property("spatial_node", boost::get(boost::vertex_bundle, reduced_g));
      dp.property("spatial_edge", boost::get(boost::edge_bundle, reduced_g));
      {
      const fs::path output_folder_path{exportReducedGraph_filename};
        fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".dot");
        std::ofstream out;
        out.open(output_full_path.string().c_str());
        boost::write_graphviz_dp(out, reduced_g, dp);
        if(verbose)
          std::cout << "Output reduced graph (graphviz) to: " << output_full_path.string() << std::endl;
      }
    }

    if(visualize)
    {
      SG::visualize_spatial_graph(reduced_g);
      // itk::Testing::ViewImage(reader->GetOutput());
      SG::visualize_spatial_graph_with_image(reduced_g, reader->GetOutput());
    }

    if(exportHistograms)
    {
      string exportHistograms_filename = vm["exportHistograms"].as<string>();
      const fs::path output_folder_path{exportHistograms_filename};
      fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".histo");
      std::ofstream out;
      out.open(output_full_path.string().c_str());
      // Degrees
      {
        auto degrees = SG::compute_degrees(reduced_g);
        auto histo_degrees = SG::histogram_degrees(degrees, binsHistoDegrees);
        out << "# Degrees: L0:centers of bins, L1:counts, L2:breaks" << std::endl;
        histo_degrees.PrintCenters(out);
        histo_degrees.PrintCounts(out);
        histo_degrees.PrintBreaks(out);
      }
      // EndToEnd Distances
      {
        auto distances = SG::compute_distances(reduced_g);
        if(verbose)
        {
          auto range_ptr = std::minmax_element(distances.begin(), distances.end());
          std::cout << "Min Distance: " << *range_ptr.first << std::endl;
          std::cout << "Max Distance: " << *range_ptr.second << std::endl;
        }

        auto histo_distances = SG::histogram_distances(distances, binsHistoDistances);
        out << "# Distances: L0:centers of bins, L1:counts, L2:breaks" << std::endl;
        histo_distances.PrintCenters(out);
        histo_distances.PrintCounts(out);
        histo_distances.PrintBreaks(out);
      }
      // Angles between adjacent edges
      {
        auto angles = SG::compute_angles(reduced_g);
        auto histo_angles = SG::histogram_angles( angles, binsHistoAngles );
        out << "# Angles: L0:centers of bins, L1:counts, L2:breaks" << std::endl;
        histo_angles.PrintCenters(out);
        histo_angles.PrintCounts(out);
        histo_angles.PrintBreaks(out);
        // Cosines of those angles
        {
          auto cosines = SG::compute_cosines(angles);
          auto histo_cosines = SG::histogram_cosines( cosines, binsHistoCosines );
          out << "# Cosines: L0:centers of bins, L1:counts, L2:breaks" << std::endl;
          histo_cosines.PrintCenters(out);
          histo_cosines.PrintCounts(out);
          histo_cosines.PrintBreaks(out);
        }
      }
      if(verbose)
      {
        std::cout << "Output histograms to: " << output_full_path.string() << std::endl;
      }
    } // end export histograms
  }

}
