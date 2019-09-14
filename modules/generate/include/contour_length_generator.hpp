/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CONTOUR_LENGTH_GENERATOR_HPP
#define CONTOUR_LENGTH_GENERATOR_HPP
#include "generate_common.hpp" // for Histogram
#include "spatial_edge.hpp"
#include "spatial_graph.hpp"
#include "update_step_generate_contour_length.hpp"

namespace SG {

/**
 * Given a populated Spatial Graph,
 * for example the graph after running simulated_annealing_generator
 * contour_length_generator populates the list of edge_points of each edge.
 *
 * It uses a montercalo PERM algorithm
 */
class contour_length_generator {
  public:
    /************ CONSTRUCTORS *************/
    contour_length_generator(GraphType &graph)
            : graph_(graph), step_generate_contour_length_(
                                     graph, histo_contour_length_distances_){};
    contour_length_generator() = delete;
    contour_length_generator(const contour_length_generator &) = delete;
    contour_length_generator(contour_length_generator &&) = delete;
    contour_length_generator &
    operator=(const contour_length_generator &) = delete;
    contour_length_generator &operator=(contour_length_generator &&) = delete;
    ~contour_length_generator() = default;

    /************ FUNCTIONS *************/
    std::pair<PointContainer, double>
    generate_contour_length(const PointType &start_point,
                            const PointType &end_point,
                            const double &k_bending,
                            const size_t &monomers = 100) const;

    /**
     * Perform von-mises test using histogram and target_cumulative_distro
     *
     * @return energy
     */
    double energy_contour_length_distances() const;
    /************ DATA *************/
    GraphType &graph_;
    Histogram histo_contour_length_distances_;
    std::vector<double>
            target_cumulative_distro_histo_contour_length_distances_;
    update_step_generate_contour_length step_generate_contour_length_;
};
} // end namespace SG

#endif
