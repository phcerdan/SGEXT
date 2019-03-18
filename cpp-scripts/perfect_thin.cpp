#include <iostream>
#include <chrono>
#include <unordered_map>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/GenericReader.h>
#include <DGtal/io/readers/ITKReader.h>
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>
// #include <DGtal/images/SimpleThresholdForegroundPredicate.h>
#include <DGtal/images/ImageSelector.h>

#include <DGtal/topology/SurfelAdjacency.h>
#include <DGtal/io/boards/Board2D.h>
#include <DGtal/topology/CubicalComplex.h>
#include <DGtal/topology/CubicalComplexFunctions.h>

#include <DGtal/topology/VoxelComplex.h>
#include <DGtal/topology/VoxelComplexFunctions.h>
#include <DGtal/topology/NeighborhoodConfigurations.h>
#include <DGtal/topology/tables/NeighborhoodTables.h>
// Invert
#include <itkInvertIntensityImageFilter.h>
#include <DGtal/io/viewers/Viewer3D.h>
#include <itkConstantPadImageFilter.h>
#include <itkNumericTraits.h>
// boost::program_options
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
// Distance Map
#include <DGtal/kernel/BasicPointPredicates.h>
#include <DGtal/images/SimpleThresholdForegroundPredicate.h>
#include <DGtal/geometry/volumes/distance/ExactPredicateLpSeparableMetric.h>
#include <DGtal/geometry/volumes/distance/VoronoiMap.h>
#include <DGtal/geometry/volumes/distance/DistanceTransformation.h>

// Boost Graph:
#include <DGtal/topology/Object.h>
#include <DGtal/graph/ObjectBoostGraphInterface.h>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
using namespace DGtal;
using namespace std;
using namespace DGtal::Z3i;
namespace po = boost::program_options;

int main(int argc, char* const argv[]){

  /*-------------- Parse command line -----------------------------*/
  po::options_description opt_desc ( "Allowed options are: " );
  opt_desc.add_options()( "help,h", "display this message." );
  opt_desc.add_options()( "input,i", po::value<string>()->required(), "Input sdp file." );
  opt_desc.add_options()( "verbose,v",  po::bool_switch()->default_value(false), "verbose output" );
  opt_desc.add_options()( "exportSDP,e", po::value<std::string>(), "Export the resulting set of points in a simple (sequence of discrete point (sdp)).");
  opt_desc.add_options()( "exportGraph,g", po::value<std::string>(), "Export the resulting set of points as a graph. It saves a list of nodes (.nod) and a list of edges (.edg)");
  bool parseOK=true;
  po::variables_map vm;

  try {
    po::store(po::parse_command_line(argc, argv, opt_desc), vm);
  } catch(const exception& ex) {
    parseOK=false;
    trace.info()<< "Error checking program options: "<< ex.what()<< endl;
  }
  po::notify ( vm );
  if (!parseOK || vm.count ( "help" ) || argc<=1 )
  {
    trace.info() <<
    "Compute the thinning of a volume using an AsymetricThinningScheme"<< endl
    << endl << "Basic usage: "<< endl
    << "perfect_thin -i <sdp file>"
    " [-e <export result as sdp file>" << endl
    << opt_desc << "\n";
    return 0;
  }
  //Parse options
  string filename = vm["input"].as<string>();
  bool verbose = vm["verbose"].as<bool>();
  /*-------------- End of parse -----------------------------*/

  using Domain = Z3i::Domain ;
  using Image = ImageContainerByITKImage<Domain, unsigned char> ;
  Image imageReader = ITKReader<Image>::importITK(filename);
  const unsigned int Dim = 3;
  using PixelType = unsigned char ;
  using ItkImageType = itk::Image<PixelType, Dim> ;
  // Pad image:
  using PadType = itk::ConstantPadImageFilter<ItkImageType, ItkImageType> ;
  auto padder = PadType::New();
  ItkImageType::PixelType constantValue;
  constantValue =(invert_image == true) ?
    itk::NumericTraits<PixelType>::max() : itk::NumericTraits<PixelType>::min();
  ItkImageType::SizeType padRegion;
  for( unsigned int i = 0 ; i<Dim ; ++i)
    padRegion[i] = 1;
  padder->SetInput(imageReader.getITKImagePointer());
  padder->SetPadLowerBound(padRegion);
  padder->SetPadUpperBound(padRegion);
  padder->SetConstant(constantValue);
  padder->Update();
  /*----------------*/

  // Invert Filter using ITK.
  using InverterType =
    itk::InvertIntensityImageFilter<ItkImageType, ItkImageType> ;
  auto inverter = InverterType::New();
  inverter->SetInput(padder->GetOutput());
  inverter->Update();
  /*----------------*/
  // Apply filters if neccesary
  Image::ITKImagePointer handle_out = (invert_image) ?
    Image::ITKImagePointer(inverter->GetOutput()) :
    Image::ITKImagePointer(padder->GetOutput());
  Image image(handle_out);

  DigitalSet image_set (image.domain());
  SetFromImage<Z3i::DigitalSet>::append<Image>(
      image_set, image,
      thresholdMin, thresholdMax);


  // Create a VoxelComplex from the set

  using DigitalTopology = DT26_6;
  using DigitalSet =
    DGtal::DigitalSetByAssociativeContainer<Domain ,
      std::unordered_set< typename Domain::Point> >;
  using Object =
    DGtal::Object<DigitalTopology, DigitalSet>;
  using Complex =
    DGtal::VoxelComplex<KSpace, Object>;

  auto & sk = sk_string;
  KSpace ks;
  ks.init(image.domain().lowerBound() ,
      image.domain().upperBound() , true);

  DigitalTopology::ForegroundAdjacency adjF;
  DigitalTopology::BackgroundAdjacency adjB;
  DigitalTopology topo(adjF, adjB, DGtal::DigitalTopologyProperties::JORDAN_DT);
  Object obj(topo,image_set);

  trace.beginBlock("construct with table");
  Complex vc(ks);
  // vc.construct(obj);
  vc.construct(obj.pointSet(), functions::loadTable(simplicity::tableSimple26_6 ));
  trace.endBlock();
  trace.beginBlock("load isthmus table");
  boost::dynamic_bitset<> isthmus_table;
  if (sk == "isthmus") isthmus_table = *functions::loadTable(isthmusicity::tableIsthmus);
  else if (sk == "1isthmus") isthmus_table = *functions::loadTable(isthmusicity::tableOneIsthmus);
  auto pointMap = *functions::mapZeroPointNeighborhoodToConfigurationMask<Point>();

  trace.endBlock();
  using namespace DGtal::functions ;
  // SKEL FUNCTION:
  std::function< bool(const Complex&, const Cell&) > Skel ;
  if (sk == "ulti") Skel = skelUltimate<Complex>;
  else if (sk == "end") Skel = skelEnd<Complex>;
  else if (sk == "1is") Skel = oneIsthmus<Complex>;
  else if (sk == "is") Skel = skelIsthmus<Complex>;
  else if (sk == "1isthmus")
      Skel = [&isthmus_table, &pointMap](const Complex & fc,
               const Complex::Cell & c){
        return skelWithTable(isthmus_table, pointMap, fc, c);
      };
  else if (sk == "isthmus")
      Skel = [&isthmus_table, &pointMap](const Complex & fc,
               const Complex::Cell & c){
        return skelWithTable(isthmus_table, pointMap, fc, c);
      };
  else throw std::runtime_error("Invalid skel string");
  auto start = std::chrono::system_clock::now();

  // SELECT FUNCTION
  /* TODO fix this(new exe?) DT has no default, no copy constructor so we have
   * to calculate for every image....
   */

  trace.beginBlock("Create Distance Map");
  using Predicate = Z3i::DigitalSet;
  using L3Metric = ExactPredicateLpSeparableMetric<Z3i::Space, 3>;
  using DT       = DistanceTransformation<Z3i::Space, Predicate, L3Metric>;
  L3Metric l3;
  DT dt(obj.domain(),obj.pointSet(), l3);
  trace.endBlock();

  std::function< std::pair<typename Complex::Cell, typename Complex::Data>(const Complex::Clique&) > Select ;
  auto & sel = select_string;
  if (sel == "random") Select = selectRandom<Complex>;
  else if (sel == "first") Select = selectFirst<Complex>;
  else if (sel == "dmax"){
    Select =
      [&dt](const Complex::Clique & clique){
        return selectMaxValue<DT, Complex>(dt,clique);
      };
  } else throw std::runtime_error("Invalid skel string");

  Complex vc_new(ks);
  if (persistence == 0)
    vc_new = asymetricThinningScheme< Complex >(
        vc, Select,  Skel, verbose);
  else
    vc_new = persistenceAsymetricThinningScheme< Complex >(
        vc, Select,  Skel, persistence, verbose);

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds> (end - start) ;
  if (profile) std::cout <<"Time elapsed: " << elapsed.count() << std::endl;

  Object shape = vc_new.object();
  if (perfect)
  {
    trace.beginBlock("Computing skeleton");
    // (6,18), (18,6), (26,6) seem ok.
    // (6,26) gives sometimes weird results (but perhaps ok !).
    int nb_simple=0;
    int layer = 1;
    std::queue<DigitalSet::Iterator> Q;
    do
      {
        trace.info() << "Layer: "<< layer << std::endl;
        int nb=0;
        DigitalSet & S = shape.pointSet();

        trace.progressBar(0, (double)S.size());
        for ( DigitalSet::Iterator it = S.begin(); it != S.end(); ++it )
          {
            if ( nb % 100 == 0 ) trace.progressBar((double)nb, (double)S.size());
            nb++;
            if (dt( *it ) <= layer)
              {
                if ( shape.isSimple( *it ) )
                  Q.push( it );
              }
          }
        trace.progressBar( (double)S.size(), (double)S.size() );
        nb_simple = 0;
        while ( ! Q.empty() )
          {
            DigitalSet::Iterator it = Q.front();
            Q.pop();
            if ( shape.isSimple( *it ))// && fixedSet.find(*it) == fixedSet.end() )
              {
                S.erase( *it );
                ++nb_simple;
              }
          }
        trace.info() << "Nb simple points : "<<nb_simple<< " " << std::endl;
        ++layer;
      }
    while ( nb_simple != 0 );
    trace.endBlock();
  }
  // THEN( "visualize the cells" )
  {
    int argc(1);
    char** argv(nullptr);
    QApplication app(argc, argv);
    Viewer3D<> viewer(ks);
    viewer.show();
    const auto & thin_set = vc_new.objectSet();
    const auto & all_set = obj.pointSet();

    if(!perfect)
    {
      viewer.setFillColor(Color(255, 255, 255, 255));
      viewer << thin_set;
    }
    else
    {
      viewer.setFillColor(Color(255, 255, 255, 255));
      DigitalSet & S = shape.pointSet();
      viewer << S;
    }

    // All kspace voxels
    viewer.setFillColor(Color(40, 200, 55, 10));
    viewer << all_set;

    viewer << Viewer3D<>::updateDisplay;

    // Export it as a simple list point
    if (vm.count("exportSDP"))
    {
      std::ofstream out;
      out.open(vm["exportSDP"].as<std::string>().c_str());
      for (auto &p : thin_set)
      {
        out << p[0] << " " << p[1] << " " << p[2] << std::endl;
      }
    }
    if (vm.count("exportGraph"))
    {
      // Object models a boost graph.
      using Graph = Object;
      const Graph & graph = vc_new.object();
      auto num_verts = boost::num_vertices(graph);
      auto verts = boost::vertices(graph);
      for(auto&& p = verts.first; p != verts.second; ++p)
      {
        std::cout << *p << std::endl;
        auto out_degree = boost::out_degree(*p, graph);
        std::cout << out_degree << std::endl;

        // typedef typename boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
        // std::pair<adjacency_iterator,adjacency_iterator> vp1 = boost::adjacent_vertices( *p, graph );
      };
      // Edges takes forever, for real. it must be way to many.
      // auto num_edges = boost::num_edges(graph);
      // auto edges = boost::edges(graph);
      std::cout << "#Verts: " << num_verts << std::endl;
      std::cout << "#Verts perfect: " << boost::num_vertices(shape) << std::endl;
      // std::cout << "#Edges: " << num_edges << std::endl;
      std::ofstream out;
      out.open(vm["exportGraph"].as<std::string>().c_str());
      for (auto &p : thin_set)
      {
        out << p[0] << " " << p[1] << " " << p[2] << std::endl;
      }
    }
    app.exec();
  }
}

////////////////////////////////////////////////////////
// copiers between Object and boost::adjacency_list
////////////////////////////////////////////////////////
struct vertex_position_t {
  using kind = boost::vertex_property_tag ;
};

struct vertex_position {
  Z3i::Point myP;
  vertex_position():myP(){}
};
using VertexProperties = boost::property<
  boost::vertex_index_t, std::size_t,
  boost::property<vertex_position_t, vertex_position> > ;
