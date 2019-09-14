/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#ifndef CONTOUR_LENGTH_GENERATOR_FUNCTIONS_HPP
#define CONTOUR_LENGTH_GENERATOR_FUNCTIONS_HPP

#include "spatial_edge.hpp"
#include "spatial_graph.hpp"

namespace SG {

/**
 * Given start and end point (in the simulation/real space), performs a
 * Montecarlo PERM simulation (in a 3D-26neighbors lattice), and transform the
 * generated chain into the simulation space.
 *
 * caveats:
 * - the way the transformation from lattice to real works, there is more
 * variance in the dimension where the differences between end and start point
 * are bigger.
 * - TODO: Maybe the amount of monomers and k_bending should depend on the
 *   end_to_end distance between end and start points.
 *   Some kind of monomer_density per unit of length. At the end, we are
 *   interested in the ratio contour_length / ete_distance. Please note that
 *   an increase in the number of monomers would need an increase of k_bending
 *   to keep the ratio contour/ete similar.
 *
 *
 * @param start_point
 * @param end_point
 * @param k_bending
 * @param monomers
 *
 * @return
 */
std::pair<PointContainer, double>
generate_contour_length(const PointType &start_point,
                        const PointType &end_point,
                        const double &k_bending,
                        const size_t &monomers = 100);

}// end ns SG
#endif
