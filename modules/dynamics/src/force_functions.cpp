/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "force_functions.hpp"

namespace SG {
ArrayUtilities::Array3D force_function_wlc_petrosyan(const SG::Particle &a,
                                                     const SG::Particle &b,
                                                     const SG::Bond &chain) {
    const auto d_ete_vector = ArrayUtilities::minus(b.pos, a.pos); // F_{a, b}
    const auto d_ete_modulo = ArrayUtilities::norm(d_ete_vector);
    // handle chains with same start/end particles (zero force)
    if (d_ete_modulo <= 2.0 * std::numeric_limits<double>::epsilon()) {
        return ArrayUtilities::Array3D();
    }
    const auto &l_contour_length =
            static_cast<const SG::BondChain &>(chain).length_contour;
    double relative_extension = d_ete_modulo / l_contour_length;
    // TODO handle relative_extension ~ 1 (wlc_petrosyan_normalized
    // would diverge)
    // TODO: THis is a hack, if relative extension is close to one return zero.
    // Most of the close-to-one relative extension comes from really short
    // edges.
    if (relative_extension > 0.98) {
        relative_extension = 0.98;
    }
    const auto bond_properties_physical =
            std::dynamic_pointer_cast<SG::BondPropertiesPhysical>(
                    chain.properties);
    if (!bond_properties_physical) {
        throw std::runtime_error(
                "To use force_extension_wlc_petrosyan, the bonds need to have "
                "properties of type BondPropertiesPhysical with "
                "persistence_length and kT populated");
    }
    const auto persistence_length =
            bond_properties_physical->persistence_length;
    const auto kT = bond_properties_physical->kT;

    const auto force_modulo = SG::force_extension_wlc_petrosyan(
            relative_extension, persistence_length, kT);
    // d_ete_vector/d_ete_modulo is the unitary vector, in the direction
    // F_{a,b}
    return ArrayUtilities::product_scalar(d_ete_vector,
                                          force_modulo / d_ete_modulo);
};
} // end namespace SG
