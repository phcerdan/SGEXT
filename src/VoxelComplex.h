#ifndef VoxelComplex_h_
#define VoxelComplex_h_
#include <DGtal/topology/CubicalComplex.h>
namespace DGtal {

template < typename TKSpace,
           typename TCellContainer =
             typename TKSpace::template CellMap< CubicalCellData >::Type >
class VoxelComplex :
  public CubicalComplex< TKSpace, TCellContainer >
{
public:
//--------Typedefs-------//
  typedef CubicalComplex< TKSpace, TCellContainer > Parent;
  typedef VoxelComplex< TKSpace, TCellContainer >   Self;
  typedef typename Parent::Cell  Cell;
  typedef typename Parent::Cells Cells;
public:
  // Inherit all constructors from parent CubicalComplex.
  using CubicalComplex<TKSpace, TCellContainer>::CubicalComplex;
  // bool isReducible( Cells& input_cells, bool verbose = false) const;
  bool isReducible( bool verbose = false) ;

  //------ Voxels ------//
  // Link khalimsky faces of greater dimension with Voxels.
  using VoxelMap = std::map<uint32_t, TCellContainer> ;
  VoxelMap m_voxel_map;
  bool voxelIsSimple(Cell&, bool verbose) const;
  bool voxelIsIsolated(Cell&) const;
  Cells& getVoxelNeighbors(Cell&) const;
  Self getComplexExceptVoxel(Cell&) const;

  //------ Cliques ------//
  // Cliques, union of adjacent voxels. The intersection of all voxels of the clique define the type. 0-clique, 1-clique, 2-clique. 3-clique are isolated voxels.
  // CliqueIsEssential.
  using Clique = Self;
  using CliqueContainer = std::vector<Clique>;
  CliqueContainer Cliques() const;
  CliqueContainer EssentialCliques() const;
  bool CliqueIsEssential(Clique&) const;

  // friend functions::complexIsConnected(Self &) const;
  // friend functions::complexIsConnected(Parent &) const;

protected:
  CliqueContainer m_cliques;
  CliqueContainer m_essential_cliques;

};

} // end DGtal
#include "VoxelComplex.ih"
#endif
