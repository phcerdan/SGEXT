/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_BONDED_FORCES_HPP
#define SG_BONDED_FORCES_HPP

#include <cmath> // for std::sin

namespace SG {

/**
 * Articles of interest:
 * Single Polymer Dynamics For Molecular Rheology
 * (2017). C.M Schroeder
 * http://arxiv.org/abs/1712.03555
 */

/**
 * From reference:
 *
 * Modeling the stretching of wormlike chains in the presence of excluded volume
 * (2015). C.M Schroeder and K.D Dorfman
 * http://xlink.rsc.org/?DOI=C5SM01333J
 *
 * @param relative_extension
 * @param monomer_anisotropy_inverse backbone_width / l_persistence
 * @param fit_constant 0.21 in article
 *
 * @return  normalized force = f * l_persistence / kT
 */
inline double force_extension_ev_wlc_low_force_normalized(
        const double &relative_extension,
        const double
                &monomer_anisotropy_inverse, // backbone_width / l_persistence
        const double &fit_constant = 0.21) {
    const auto &x = relative_extension;
    const auto xsqrt = sqrt(x); // x^{1/2}
    return x * xsqrt /
           (fit_constant * sqrt(monomer_anisotropy_inverse) + xsqrt * (2 / 3.));
}
inline double
force_extension_ev_wlc_high_force_normalized(const double &relative_extension) {
    return 0.25 / (1 - relative_extension) * (1 - relative_extension) -
           (0.25 + 0.5 * relative_extension);
}
inline double force_extension_ev_wlc_normalized(
        const double &relative_extension,
        const double
                &monomer_anisotropy_inverse, // backbone_width / l_persistence
        const double &fit_constant = 0.21) {
    return force_extension_ev_wlc_low_force_normalized(
                   relative_extension, monomer_anisotropy_inverse,
                   fit_constant) +
           force_extension_ev_wlc_high_force_normalized(relative_extension);
}

/**
 * An ideal chain described by random-walk statistics or theta-solvent
 * conditions yields a low-force linear elasticity.
 *
 * @param relative_extension x/L_contour
 *
 * @return normalized force = f * L_persistence / kT
 */
inline double force_extension_low_regime_ideal_chain_normalized(
        const double &relative_extension) {
    return relative_extension;
}
/**
 * Pincus, effect of excluded volume interactions at low force.
 * Assumes excluded volume exponent of v = 3/5 (good solvents).
 * Applied force generates a tensile screening length, known as tensile blob
 * or a Pincus blob \f[ \xi_P = \frac{k_{B}T}{f} \f]
 *
 * @param relative_extension x/L_contour
 *
 * @return normalized force = f * L_persistence / kT
 */
inline double force_extension_low_regime_excluded_volume_pincus_normalized(
        const double &relative_extension) {
    return std::pow(relative_extension, 1.5);
}
/** TODO(phcerdan)
 * In the case of infinite forces in the unextensible WLC.
 * What does an infinite force mean? It means that it's impossible to extend
 * the chain.
 * If talking about extension-force (instead of force-extension) it means that
 * relative_extension ~ 1 - 1/f.
 * In terms on simulations, this can be captured transforming a WLC into a rigid
 * rod, where the force exerted in one end is not "wasted" in extending the rod,
 * but the force gets "transduced" to the other end (time? instantly?)
 */
/** In force_extension experimentts, one end is fixed, and the other is being
 * pulled. What about in the case of a network, where one or both ends are not
 * completely fixed. Where is the ratio of the force that is used to stretch
 * the polymer versus the translation of the whole chain because of it?
 * Answer: We should assume there are no knots, and the force is used to un-fold
 * or straight-up the chain, instead of moving the other end...
 * Except when it is completely unfolded already.
 */

/**
 * Not analytical extension for Freely Jointed Chain, following approximation
 * has an error less than 0.18%
 *
 * f_normalized = f * l_persistence / kT
 *
 * Based on article:
 * Improved approximations for some polymer extension models
 * 2016, Rafayel Petrosyan
 * https://arxiv.org/abs/1606.02519
 *
 * @param relative_extension
 *
 * @return normalized force
 */
inline double
force_extension_fjc_petrosyan_normalized(const double &relative_extension) {
    const auto &x = relative_extension;
    const auto x2 = x * x;
    const auto x3 = x2 * x;
    return 3 * x + x2 * std::sin(7 * x / 2) + x3 / (1 - x);
}

/**
 * Based on article:
 * Improved approximations for some polymer extension models
 * 2016, Rafayel Petrosyan
 * https://arxiv.org/abs/1606.02519
 *
 * f_normalized = f * l_persistence / kT
 * relative_extension = z_extension/l_contour
 *
 * @param relative_extension
 * @param l_contour
 * @param l_persistence
 * @param kT
 *
 * @return force
 */
inline double
force_extension_wlc_petrosyan_normalized(const double &relative_extension) {
    // TODO if relative_extension is ~1, the force diverges
    const auto one_minus_relative_extension = 1 - relative_extension;
    return relative_extension - 0.8 * std::pow(relative_extension, 2.15) +
           0.25 / (one_minus_relative_extension *
                   one_minus_relative_extension) -
           0.25;
}
inline double force_extension_wlc_petrosyan(const double &z_extension,
                                            const double &l_contour,
                                            const double &l_persistence,
                                            const double &kT) {
    const double relative_extension = z_extension / l_contour;
    return kT / l_persistence *
           force_extension_wlc_petrosyan_normalized(relative_extension);
};

/**
 * Instead of the force given the extension, the inverse, it returns the
 * extension given the force.
 *
 * @param normalized_force
 *
 * @return the relative_extension
 */
inline double extension_force_wlc_petrosyan(const double &normalized_force) {
    const double &f = normalized_force;
    const double four_thirds = 4.0 / 3.0;
    const double exp_f = exp(std::pow(900 / f, 0.25));
    return four_thirds - four_thirds / sqrt(f + 1) -
           10 * exp_f / (sqrt(f) * (exp_f - 1) * (exp_f - 1)) +
           std::pow(f, 1.62) / (3.55 + 3.8 * std::pow(f, 2.2));
};

/**
 * Based on article:
 * Stretching DNA
 * 1995, J.F Marko and E. Siggia
 * https://pubs.acs.org/doi/abs/10.1021/ma00130a008
 *
 * relative_extension = z_extension/l_contour
 *
 * @param z_extension
 * @param l_contour
 * @param l_persistence
 * @param kT
 *
 * @return force
 */
inline double
force_extension_wlc_marko_siggia_normalized(const double &relative_extension) {
    // TODO if relative_extension is ~1, the force diverges
    return relative_extension - 0.25 + 0.25 / (1.0 - relative_extension);
}
inline double force_extension_wlc_marko_siggia(const double &z_extension,
                                               const double &l_contour,
                                               const double &l_persistence,
                                               const double &kT) {
    const double relative_extension = z_extension / l_contour;
    return kT / l_persistence *
           force_extension_wlc_marko_siggia_normalized(relative_extension);
};
}; // namespace SG
#endif
