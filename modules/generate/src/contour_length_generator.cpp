/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "contour_length_generator.hpp"
#include "contour_length_generator_functions.hpp"

namespace SG {

std::pair<PointContainer, double>
contour_length_generator::generate_contour_length(
        const PointType &start_point,
        const PointType &end_point,
        const double &k_bending,
        const size_t &monomers) const {
    return SG::generate_contour_length(start_point, end_point, k_bending,
                                       monomers);
}
} // namespace SG

