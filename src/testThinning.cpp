#include "catch_header.h"
#include "VoxelComplex.h"
#include "DGtal/topology/CubicalComplexFunctions.h"
#include "DGtal/helpers/StdDefs.h"
#include <iostream>
// Shape construction
#include "DGtal/shapes/GaussDigitizer.h"
#include "DGtal/shapes/Shapes.h"
#include "DGtal/shapes/EuclideanShapesDecorator.h"
#include "DGtal/shapes/parametric/Ball3D.h"
// Board
#include <DGtal/io/boards/Board3D.h>
using namespace std;
using namespace DGtal;

template<typename TKSpace, typename TCellContainer>
VoxelComplex<TKSpace, TCellContainer> create_star_shaped_voxel_complex()
{
  using namespace Z3i;
  typedef Space::RealPoint RealPoint;
  typedef Ball3D<Space> EuclideanShape;
  typedef GaussDigitizer<Space,EuclideanShape> DigitalShape;

  // Creating the shape
  RealPoint c1(0, 0, 0 );
  EuclideanShape ball1( c1, 12.2 );

  // GaussDigitizing
  DigitalShape dshape;
  dshape.attach( ball1 );
  RealPoint p1 =RealPoint( -5.0, -5.0, -5.0 );
  RealPoint p2 =RealPoint( 5.0, 5.0, 5.0 );
  dshape.init( RealPoint( p1 ), RealPoint( p2 ), 1.0);
  Domain domain = dshape.getDomain();
  DigitalSet a_set( domain );
  Shapes<Domain>::digitalShaper( a_set, dshape );

  //  Khalimskhy space
  TKSpace K;
  bool space_ok = K.init( domain.lowerBound(), domain.upperBound(), true );
  if (!space_ok)
  {
    cout << "create_shape Khalimskhy space failed" << endl;
  }

  // Voxel Complex from K
  typedef VoxelComplex<TKSpace, TCellContainer> VC;
  VC vcomplex(K);
  vcomplex.construct(a_set);
  return vcomplex;

}
// SCENARIO( "Thinning using cubical complexes", "[functions][reducible]" ){
//    GIVEN( "A connected cubical complex" ){
//      using namespace Z3i;
//      typedef std::map<Cell, CubicalCellData> CCMap;
//      typedef VoxelComplex<KSpace, CCMap> VC;
//      auto X = create_star_shaped_voxel_complex<KSpace, CCMap>();
//      THEN( "is_reducible" ){
//        REQUIRE( X.isReducible(true) == false );
//      }
//    }
// }
SCENARIO( "Collapsing Voxel Complexes", "[collapse]"){
   GIVEN( "A connected cubical complex" ){
     using namespace Z3i;
     typedef std::map<Cell, CubicalCellData> CCMap;
     typedef VoxelComplex<KSpace, CCMap> VC;
     auto X = create_star_shaped_voxel_complex<KSpace, CCMap>();
     {
       Board3D<> board;
       typedef VC::CellMapConstIterator CellMapConstIterator;
       for(unsigned int d = 3 ; d <=3 ; ++d)
         for ( CellMapConstIterator it = X.begin( d ), itE = X.end( d );
             it != itE; ++it )
         {
           board << it->first;
         }
       board.saveOBJ("/home/phc/tmp/original.obj");
     }
     WHEN( "collapse" ){
       VC::DefaultCellMapIteratorPriority P;
       // auto removed = functions::collapse(X, X.begin(), X.end(), P, true, true, true);
       auto removed = functions::collapse(X, X.begin(), X.end(), P, false, false, true);
       // VISUALIZE
       {
         Board3D<> board;
         typedef VC::CellMapConstIterator CellMapConstIterator;
         for(unsigned int d = 3 ; d <=3 ; ++d)
           for ( CellMapConstIterator it = X.begin( d ), itE = X.end( d );
               it != itE; ++it )
           {
             board << it->first;
           }
         board.saveOBJ("/home/phc/tmp/thin.obj");
       }
       THEN( "one pixel left" ){
         std::cout << X.size() << std::endl;
         std::cout << removed << std::endl;

         for(unsigned int d = 0 ; d <=3 ; ++d){
           size_t n{0};
           for(auto it = X.begin(d); it!=X.end(d); ++it)
             ++n;
           std::cout << "D" << d << ": "<< n << std::endl;
         }
         REQUIRE( X.size() == 0);
       }
     }
   }
}
