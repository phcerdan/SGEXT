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

#ifndef SPATIAL_HISTOGRAMS_HPP
#define SPATIAL_HISTOGRAMS_HPP

#include "histo.hpp"
#include "spatial_graph.hpp"

namespace SG {

/**
 * print/write histogram to any ostream in the following format.
 *  os << "# " << histo.name << ": L0:centers of bins, L1:counts, L2:breaks" <<
 * std::endl; histo.PrintCenters(os); histo.PrintCounts(os);
 *  histo.PrintBreaks(os);
 *
 * Use @read_histogram as the complementary function to read the histogram.
 *
 * @param histo input histo
 * @param os
 */
void print_histogram(const histo::Histo<double> &histo, std::ostream &os);

/**
 * read histogram from istream after being printed in the format
 * imposed by @print_histogram
 *
 * @param is stream
 *
 * @return  histo<double>
 */
histo::Histo<double> read_histogram(std::istream &is);
/**
 * Create histogram of degrees.
 * @sa compute_ete_distances
 *
 * @param degrees input vector of degrees
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 * bins should be greater than max_degrees + 1
 * @param histo_name populates histogram name.
 *
 * @return  histogram of degrees
 */
histo::Histo<double>
histogram_degrees(const std::vector<unsigned int> &degrees,
                  size_t bins = 0,
                  const std::string &histo_name = "degrees");

histo::Histo<double>
histogram_degrees(const std::vector<double> &degrees,
                  size_t bins = 0,
                  const std::string &histo_name = "degrees");

/**
 * Create histogram of End to End distances between nodes.
 * @sa compute_ete_distances
 *
 * @param distances input vector of distances
 * @param width fixed width between breaks,
 * if width == 0.0, the breaks are generated using Scott method.
 * @param histo_name populates histogram name.
 *
 * @return histogram
 */
histo::Histo<double>
histogram_distances(const std::vector<double> &distances,
                    double width = 0.0,
                    const std::string &histo_name = "distances");

/**
 * helper. Calls @histogram_distances with histo_name = "ete_distances"
 *
 * @param distances input ete_distances @compute_ete_distances
 * @param width of histo breaks
 *
 * @return histogram
 */
histo::Histo<double>
histogram_ete_distances(const std::vector<double> &distances,
                        double width = 0.0);

/**
 * helper. Calls @histogram_distances with histo_name = "contour_lengths"
 *
 * @param distances input ete_distances @compute_contour_lengths
 * @param width of histo breaks
 *
 * @return histogram
 */
histo::Histo<double>
histogram_contour_lengths(const std::vector<double> &distances,
                          double width = 0.0);

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
histo::Histo<double> histogram_angles(const std::vector<double> &angles,
                                      size_t bins = 100,
                                      const std::string &histo_name = "angles");

/**
 * Histogram between -1.0, 1.0
 * @sa compute_cosines
 *
 * @param cosines input data
 * @param bins number of bins of the histogram,
 * if bins == 0, the breaks are generated using Scott method.
 * @return histo
 */
histo::Histo<double>
histogram_cosines(const std::vector<double> &cosines,
                  size_t bins = 100,
                  const std::string &histo_name = "cosines");
} // namespace SG
#endif
