#ifndef VISUALIZE_HISTOGRAMS_HPP
#define VISUALIZE_HISTOGRAMS_HPP

#include "histo.hpp"

namespace SG {

/**
 * Uses Qt and vtk to visualize a vector of histograms.
 *
 * @sa histo::visualize_histo
 *
 * @param input_histograms
 */
void visualize_histograms(
    const std::vector<histo::Histo<double>>& input_histograms);

}  // namespace SG
#endif
