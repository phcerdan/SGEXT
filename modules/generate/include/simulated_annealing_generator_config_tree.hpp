/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SIMULATED_ANNEALING_GENERATOR_CONFIG_TREE_HPP
#define SIMULATED_ANNEALING_GENERATOR_CONFIG_TREE_HPP

#include "simulated_annealing_generator_parameters.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace SG {

/**
 * Struct with the experimental parameters feeding the simulation
 * The conversion to simulation parameters needs a linear scale factor S,
 * defined as S = (num_vertices/node_density)^1/3, and the volume of the
 * simulation box (1 m^3 by default).
 */
struct simulated_annealing_generator_config_tree {

    transition_parameters transition_params;
    degree_distribution_parameters degree_params;
    end_to_end_distances_distribution_parameters ete_distance_params;
    cosine_directors_distribution_parameters cosine_params;
    domain_parameters domain_params;
    physical_scaling_parameters physical_scaling_params;

    /**
     * Load configuration from json
     *
     * @param input json file
     */
    void load(const std::string &filename);
    void load_transition(boost::property_tree::ptree &tree);
    void load_domain(boost::property_tree::ptree &tree);
    void load_degree(boost::property_tree::ptree &tree);
    void load_ete_distance(boost::property_tree::ptree &tree);
    void load_cosine(boost::property_tree::ptree &tree);
    void load_physical_scaling(boost::property_tree::ptree &tree);

    /**
     * Save configuration tree to json file
     *
     * @param output json file
     */
    void save(const std::string &filename);
    void save_transition(boost::property_tree::ptree &tree);
    void save_domain(boost::property_tree::ptree &tree);
    void save_degree(boost::property_tree::ptree &tree);
    void save_ete_distance(boost::property_tree::ptree &tree);
    void save_cosine(boost::property_tree::ptree &tree);
    void save_physical_scaling(boost::property_tree::ptree &tree);
};
} // namespace SG
#endif
