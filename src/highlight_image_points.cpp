#include <iostream>
#include <chrono>
#include <unordered_map>
#include <sstream>
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

// Boost Filesystem
#include <boost/filesystem.hpp>

// Viewer
#include "DGtal/io/Color.h"
#include "DGtal/io/colormaps/GradientColorMap.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include <DGtal/io/viewers/Viewer3D.h>

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
    ( "exportSDP,o", po::value<string>(), "Write .sdp file the digital set points." )
    ( "highlightPoints,p", po::value<string>(), "Give a string of points to highlight them (require visualize ON). Example: \"1 3 4, 43 45 53\" " )
    ( "visualize,t", po::bool_switch()->default_value(false), "Visualize object with DGtal.")
    ( "verbose,v",  po::bool_switch()->default_value(false), "verbose output." );

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
    if (vm.count ( "help" ) || argc<=1 )
    {
      std::cout << "Basic usage:\n" << general_opt << "\n";
      return false;
    }
    po::notify ( vm );
  } catch ( const std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  string filename = vm["input"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  bool visualize = vm["visualize"].as<bool>();
  bool highlightPoints = vm.count("highlightPoints");
  string highlightPoints_input = highlightPoints ? vm["highlightPoints"].as<string>() : "";
  bool exportSDP = vm.count("exportSDP");
  string exportSDP_filename = exportSDP ? vm["exportSDP"].as<string>(): "";
  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(
      input_stem.string() + "_points");

  using Domain = Z3i::Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;
  Image image = ITKReader<Image>::importITK(filename);

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

  if(visualize)
  {
      KSpace ks;
      // Domain of kspace must be padded.
      ks.init(image.domain().lowerBound(),
          image.domain().upperBound(), true);
      int argc(1);
      char** argv(nullptr);
      QApplication app(argc, argv);
      Viewer3D<> viewer(ks);
      viewer.show();

      viewer.setFillColor(Color(255, 255, 255, 50));
      viewer << image_set;

      DigitalSet highlight_set(image.domain());
      if(highlightPoints)
      {
        std::cout << "Highlighting points" << std::endl;
        std::istringstream ss(highlightPoints_input);
        std::cout << ss.str() << std::endl;
        std::string spoint;
        Domain::Point p;
        //12 9 131, 23 44 5
        while(std::getline(ss, spoint, ',')) {
          std::istringstream sspoint( spoint );
          std::cout << sspoint.str() << std::endl;
          //12 9 131
          sspoint >> p[0] >> p[1] >> p[2];
          std::cout << p << std::endl;
          highlight_set.insert(p);
        }
        // Highlighted
        viewer.setFillColor(Color(255, 0, 0, 255));
        viewer << highlight_set;
      }

      viewer << Viewer3D<>::updateDisplay;


      app.exec();
  }

  if(exportSDP)
  {
    const fs::path output_folder_path{exportSDP_filename};
    fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".sdp");
    std::ofstream out;
    out.open(output_full_path.string().c_str());
    out << "# file : " << input_stem.string() << std::endl;
    for (auto &p : image_set)
    {
      out << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
    if(verbose)
    {
      std::cout << "list of points (SDP) output in: " << output_full_path.string() << std::endl;
    }
  }

}
