#ifndef phcCCfunctions_h_
#define phcCCfunctions_h_

#include <DGtal/topology/CubicalComplexFunctions.h>

namespace DGtal {
  namespace functions {

//DEF1: is_reducible

    /**
     * Predicate returning if input complex [K] is reducible.
     * Following definition of G.Bertrand and M.Couprie: Asymmetric parallel 3D thinning.
     * Informally, is_reducible is analogous to the classical is_simple in Digital Topology (DT)
     * A voxel-complex is a finit set composed solely of voxels.
     * In the Cubical Complex framework, the set contains also the faces of lesser dimensions.
     * @note: A voxel is a cell type CC::Maximal of d=3 in the Cubical Complex framework (of dim=3)
     * Definition (recursive): Let X be a voxel-complex. X is reducible if:
     *    i)  X is composed of a single voxel.
     *    ii) there exists voxel x in X that Neigborhood(x)\{x} intersection (X) is reducible
     *        and X\{x} is reducible.
     * So, a complex X is reducible iff it is possible to reduce it to a single pixel.
     * Noting that a reducible complex is necessarily non-empty and connected.
     *
     * @tparam TKSpace
     * @tparam TCellContainer
     * @tparam CellConstIterator
     * @param K cubical complex
     * @param S_it iterator to cell
     * @param hintIsKClosed
     * @param verbose
     *
     * @return true if cell is reducible
     */
    template <typename TKSpace, typename TCellContainer>
    bool is_reducible( CubicalComplex< TKSpace, TCellContainer > & K,
                       bool hintIsKClosed = false,
                       bool verbose = true );

    template <typename TKSpace, typename TCells = typename TKSpace::Cells>
    bool is_reducible(TCells & set_of_cells);

//DEF2: is_simple
    template <typename TKSpace, typename TCellContainer, typename TCell = typename TKSpace::Cell>
    bool voxel_is_simple( CubicalComplex< TKSpace, TCellContainer > & K,
                       TCell& c,
                       bool verbose = false );
/**
 * DEF3: kappa-neighborhood of Voxel: kappa(S) is the set made of all voxels that are adjacent to each voxel in S. kappa*(S)=kappa(S)\S.
 * @note kappa(S)=Neighborhood(x) when S is made of a single voxel x.
 * @note S is on kappa(S) whenever S is a clique.
 */

/**
 * DEF4.1: C is a clique essential for X if we have C=D whenever D is a clique such that:
 * (i) C is contained, equal to D, contained, equal to X. and
 * (ii) intersection of voxels of C equals intersection of voxels of D.
 */

/**
 * DEF4.2 Let C be a clique essential for X. C is regular for X if kappa*(C) intersecction X is reducible. C is critical if  C is not regular for X.
 */

/**
 * Theorem5:
 * Y is contained on X.
 * The complex Y is a thinning of X if any clique that is critital for X contains at least on voxel of Y.
 */


  } //functions
} //DGtal

///////////////////////////////////////////////////////////////////////////////
// Includes inline functions.
#include "phcCCfunctions.ih"

//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#endif
