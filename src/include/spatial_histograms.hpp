#ifndef SPATIAL_HISTOGRAMS_HPP
#define SPATIAL_HISTOGRAMS_HPP

#include "spatial_graph.hpp"
#include "histo.hpp"

namespace SG {


/**
 * print/write histogram to any ostream in the following format.
 *  os << "# " << histo.name << ": L0:centers of bins, L1:counts, L2:breaks" << std::endl;
 *  histo.PrintCenters(os);
 *  histo.PrintCounts(os);
 *  histo.PrintBreaks(os);
 *
 * Use @read_histogram as the complementary function to read the histogram.
 *
 * @param histo input histo
 * @param os
 */
void print_histogram(const histo::Histo<double> & histo, std::ostream & os);

/**
 * read histogram from istream after being printed in the format
 * imposed by @print_histogram
 *
 * @param is stream
 * @param name of the histogram
 *
 * @return  histo<double>
 */
histo::Histo<double> read_histogram(std::istream & is, const std::string & name);
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
