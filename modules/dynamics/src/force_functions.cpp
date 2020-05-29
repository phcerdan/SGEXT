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

#include "force_functions.hpp"

namespace SG {
ArrayUtilities::Array3D force_function_wlc_petrosyan_normalized(
        const SG::Particle &a, const SG::Particle &b, const SG::Bond &chain) {
    const auto d_ete_vector = ArrayUtilities::minus(b.pos, a.pos); // F_{a, b}
    const auto d_ete_modulo = ArrayUtilities::norm(d_ete_vector);
    const auto &l_contour_length =
            static_cast<const SG::BondChain &>(chain).length_contour;
    const double relative_extension = d_ete_modulo / l_contour_length;
    // TODO handle relative_extension ~ 1 (wlc_petrosyan_normalized
    // would diverge)
    const auto force =
            SG::force_extension_wlc_petrosyan_normalized(relative_extension);
    // d_ete_vector/d_ete_modulo is the unitary vector, in the direction
    // F_{a,b}
    return ArrayUtilities::product_scalar(d_ete_vector, force / d_ete_modulo);
};
} // end namespace SG
