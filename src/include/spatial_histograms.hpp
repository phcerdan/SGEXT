#ifndef SPATIAL_HISTOGRAMS_HPP
#define SPATIAL_HISTOGRAMS_HPP

#include "spatial_graph.hpp"
#include "histo.hpp"

namespace SG {


/**
 * Create histogram of degrees.
 * @sa compute_ete_distances
 *
 * @param degrees input vector of degrees
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 * bins should be greater than max_degrees + 1
 *
 * @return  histogram of degrees
 */
histo::Histo<double> histogram_degrees(
        const std::vector<unsigned int> & degrees,
        size_t bins = 0);

/**
 * Create histogram of End to End distances between nodes.
 * @sa compute_ete_distances
 *
 * @param distances input vector of distances
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 *
 * @return histogram
 */
histo::Histo<double> histogram_distances(const std::vector<double> & distances, size_t bins = 0);


/**
 * Histogram between -pi, pi
 * @sa compute_angles
 *
 * @param angles input data
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 *
 * @return histo
 */
histo::Histo<double> histogram_angles(const std::vector<double> & angles, size_t bins = 0);


/**
 * Histogram between -1.0, 1.0
 * @sa compute_cosines
 *
 * @param cosines input data
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 * @return histo
 */
histo::Histo<double> histogram_cosines(const std::vector<double> & cosines, size_t bins = 0);
} // end namespace
#endif
