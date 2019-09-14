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

#include "contour_length_generator_functions.hpp"
#include "perm.hpp"

namespace SG {

std::pair<PointContainer, double>
generate_contour_length(const PointType &start_point,
                        const PointType &end_point,
                        const double &k_bending,
                        const size_t &monomers) {
    auto edge_points = PointContainer();

    // Perform a PERM with input number of monomers
    const size_t max_tries = 1000;
    const auto lattice = perm::lattice_3D_26n;
    perm::parameters_in_t parameters_in;
    parameters_in.monomers = monomers;
    parameters_in.max_tries = max_tries;
    parameters_in.lattice = lattice;
    parameters_in.energy_grow_func =
            [k = k_bending](const perm::single_chain_t<int> &input_chain,
                            const perm::vec3D_t<int> &new_monomer) {
                return perm::energy_grow_bending(input_chain, new_monomer, k);
            };
    // std::cout << "Parameters = " << std::endl;
    // parameters_in.print(std::cout);
    auto chain_weight_pair = perm::mc_saw_perm(parameters_in);
    auto &chain = chain_weight_pair.first;
    chain.print(std::cout);
    // lattice distances are in a square/cubic grid of unit spacing
    // lattice_start_point is always {0,0,0}
    // Compute distance between start and end in lattice space (for PERM)
    auto real_ete_vector = ArrayUtilities::minus(end_point, start_point);
    auto lattice_ete_vector = perm::end_to_end_vector(chain);
    std::cout << "lattice_ete_vector" << std::endl;
    std::cout << lattice_ete_vector.x << ", " << lattice_ete_vector.y
              << ", " << lattice_ete_vector.z << std::endl;
    // for(const auto & l : lattice_ete_vector.e) {
    //     std::cout << l << ", ";
    // }
    // std::cout << std::endl;
    // auto dist_lattice = perm::end_to_end_distance(chain);
    ArrayUtilities::Array3D scaling_factor_lattice_to_real = {{0, 0, 0}};

    for (size_t d = 0; d < real_ete_vector.size(); d++) {
        if (lattice_ete_vector[d] == 0) {
            scaling_factor_lattice_to_real[d] = 1;
        } else {
            scaling_factor_lattice_to_real[d] =
                    real_ete_vector[d] / lattice_ete_vector[d];
        }
    }

    size_t num_chain_points = chain.points.size();
    edge_points.resize(num_chain_points);
    edge_points[0] = start_point;

    for (size_t i = 1; i < num_chain_points; i++) {
        auto lattice_monomer_vector =
                perm::minus(chain.points[i], chain.points[i - 1]);
        for (size_t d = 0; d < 3; d++) {
            edge_points[i][d] = edge_points[i - 1][d] +
                                lattice_monomer_vector[d] *
                                        scaling_factor_lattice_to_real[d];
        }
    }
    std::cout << "scaling_factor: "
              << ArrayUtilities::to_string(scaling_factor_lattice_to_real)
              << std::endl;
    print_edge_points(edge_points, std::cout);
    // Remove first and last points of edge_points (these are the
    // source/target nodes)

    edge_points.erase(std::begin(edge_points));
    edge_points.pop_back();
    std::cout << "After deletion" << std::endl;
    print_edge_points(edge_points, std::cout);
    double contour_end_to_end_ratio =
            perm::contour_length(chain) / perm::end_to_end_distance(chain);
    return std::make_pair(edge_points, contour_end_to_end_ratio);
}


} // end ns SG

