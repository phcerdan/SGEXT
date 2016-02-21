#include "graph_connected_components.h"
#include <iostream>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/io/readers/ITKReader.h>
#include "itkInvertIntensityImageFilter.h"
#include <DGtal/images/ImageContainerByITKImage.h>
#include <DGtal/images/imagesSetsUtils/SetFromImage.h>
#include <DGtal/topology/SurfelAdjacency.h>
#include <DGtal/topology/SetOfSurfels.h>
#include <DGtal/io/boards/Board2D.h>

#include <DGtal/topology/DigitalSurface.h>
#include <boost/graph/connected_components.hpp>
#include <boost/property_map/property_map.hpp>
using namespace DGtal;
using namespace std;
using namespace DGtal::Z2i;

int main(int argc, char *argv[])
{
  cout << "Reading ITK Image" << endl;
  typedef Domain Domain;
  typedef ImageContainerByITKImage<Domain, unsigned char> Image;
  string filename = "./bX2D_2comp.png";
  Image imageRead = ITKReader<Image>::importITK(filename);
  // Invert using ITK.
  const unsigned int Dim = 2;
  typedef unsigned char PixelType;
  typedef itk::Image<PixelType, Dim> itkImageType;
  typedef itk::InvertIntensityImageFilter<itkImageType, itkImageType> InverterType;
  auto inverter = InverterType::New();
  inverter->SetInput(imageRead.getITKImagePointer());
  inverter->Update();
  Image::ITKImagePointer handle_out = inverter->GetOutput();
  Image image(handle_out);

  DigitalSet set2d (image.domain());
  SetFromImage<Z2i::DigitalSet>::append<Image>(set2d, image, 1, 255);

  KSpace ks;
  ks.init(image.domain().lowerBound(), image.domain().upperBound(), true);

  typedef SurfelAdjacency<KSpace::dimension> MySurfelAdjacency;
  MySurfelAdjacency surfAdj( true );

  trace.beginBlock( "Extracting boundary by scanning the space. " );
  typedef KSpace::SurfelSet SurfelSet;
  typedef SetOfSurfels< KSpace, SurfelSet > MySetOfSurfels;
  typedef DigitalSurface< MySetOfSurfels > MyDigitalSurface;
  MySetOfSurfels theSetOfSurfels( ks, surfAdj );
  Surfaces<KSpace>::sMakeBoundary( theSetOfSurfels.surfelSet(),
      ks, set2d,
      image.domain().lowerBound(),
      image.domain().upperBound() );
  MyDigitalSurface digSurf( theSetOfSurfels );
  trace.info() << "Digital surface has " << digSurf.size() << " surfels."
    << std::endl;
  trace.endBlock();

  trace.beginBlock ( "Testing Graph concepts for DigitalSurface ..." );
  typedef MyDigitalSurface Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor; // ie DigitalSurface::Vertex
  typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor; // ie DigitalSurface::Arc
  typedef boost::graph_traits<Graph>::vertices_size_type vertices_size_type; // ie DigitalSurface::Size
  typedef boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
  typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;
  typedef boost::graph_traits<Graph>::edge_iterator edge_iterator;

  BOOST_CONCEPT_ASSERT(( boost::VertexListGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT(( boost::AdjacencyGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT(( boost::IncidenceGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT(( boost::EdgeListGraphConcept<Graph> ));
  // BOOST_CONCEPT_ASSERT(( boost::MultiPassInputIterator<vertex_iterator>));
  trace.endBlock();
  trace.beginBlock( "Define property maps for algorithm");
  // get the property map for coloring vertices (used for not visiting twice the same vertex).
  typedef std::map< vertex_descriptor, boost::default_color_type > StdColorMap; // the container type
  StdColorMap colorMap;                                                         // the container instance (will store computations).
  boost::associative_property_map< StdColorMap > propColorMap( colorMap );      // a facade aroundcolorMap
  // get the property map for labelling vertices (the mapping Vertex -> Size that stores the component label for each vertex)
  typedef std::map< vertex_descriptor, vertices_size_type > StdComponentMap;
  StdComponentMap componentMap;
  boost::associative_property_map< StdComponentMap > propComponentMap( componentMap );
  trace.endBlock();

  trace.beginBlock( "Boost connected_components");
  int num = boost::connected_components(
      digSurf,
      propComponentMap,
      boost::color_map(propColorMap)
      );
  trace.info() << "num_components = " << num << std::endl;
  trace.endBlock();

  trace.beginBlock( "Visualize");
  trace.endBlock();


}
