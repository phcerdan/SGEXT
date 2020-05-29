/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
