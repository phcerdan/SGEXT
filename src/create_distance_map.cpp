#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"
#include "DGtal/images/imagesSetsUtils/ImageFromSet.h"
#include <DGtal/io/readers/ITKReader.h>
#include "DGtal/io/writers/ITKWriter.h"

#include "DGtal/geometry/volumes/distance/ExactPredicateLpSeparableMetric.h"
#include "DGtal/geometry/volumes/distance/DistanceTransformation.h"
#include "DGtal/images/SimpleThresholdForegroundPredicate.h"

// ITKWriter
#include <itkImageFileWriter.h>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// Boost Filesystem
#include <boost/filesystem.hpp>

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
    ( "outputFolder,o", po::value<string>()->required(), "Output folder for the distance map." )
    // ( "visualize,t", po::bool_switch()->default_value(false), "Visualize object with DGtal. Requires VISUALIZE option enabled at build.")
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

  std::string filename = vm["input"].as<string>();
  std::string outputFolder = vm["outputFolder"].as<string>();;
  bool verbose = vm["verbose"].as<bool>();

  // Get filename without extension (and without folders).
  const fs::path input_stem = fs::path(filename).stem();
  const fs::path output_file_path = fs::path(
      input_stem.string() + "_DMAP");

  // Typedefs
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

  auto start = std::chrono::system_clock::now();
  trace.beginBlock("Create Distance Map");
  // using Predicate = Z3i::DigitalSet;
  using Predicate = DGtal::functors::SimpleThresholdForegroundPredicate<Image>;
  Predicate aPredicate(image,0);
  using L3Metric = ExactPredicateLpSeparableMetric<Z3i::Space, 3>;
  using DT       = DistanceTransformation<Z3i::Space, Predicate, L3Metric>;
  L3Metric l3;
  DT dt(image.domain(),aPredicate, l3);
  trace.endBlock();
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds> (end - start) ;
  if (verbose) std::cout <<"Time elapsed: " << elapsed.count() << std::endl;

  // Write distance map
  const fs::path output_folder_path{outputFolder};
  fs::path output_full_path = output_folder_path / fs::path(output_file_path.string() + ".nrrd");
  bool write_status = ITKWriter<DT>::exportITK(
          output_full_path.string().c_str(),
          dt);
  if(!write_status)
      std::cerr << "Failure writing file" << std::endl;
}
