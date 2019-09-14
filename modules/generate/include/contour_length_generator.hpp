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
