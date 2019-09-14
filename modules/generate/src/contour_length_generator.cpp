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

