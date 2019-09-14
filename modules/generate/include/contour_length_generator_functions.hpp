/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
