/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "../core/sgcore_common_py.hpp" // to make array3d opaque

#include "simulated_annealing_generator.hpp"

namespace py = pybind11;
using namespace SG;

void init_simulated_annealing_generator_parameters(py::module &m) {
    py::class_<domain_parameters>(m, "domain_parameters")
            .def(py::init())
            .def_readwrite("boundary_condition",
                           &domain_parameters::boundary_condition)
            .def_readwrite("domain", &domain_parameters::domain)
            .def("__repr__", [](const domain_parameters &p) {
                std::stringstream os;
                p.print(os);
                return os.str();
            });

    py::class_<physical_scaling_parameters>(m, "physical_scaling_parameters")
            .def(py::init())
            .def_readwrite("num_vertices",
                           &physical_scaling_parameters::num_vertices)
            .def_readwrite("node_density",
                           &physical_scaling_parameters::node_density)
            .def_readwrite("length_scaling_factor",
                           &physical_scaling_parameters::length_scaling_factor)
            .def("__repr__", [](const physical_scaling_parameters &p) {
                std::stringstream os;
                p.print(os);
                return os.str();
            });

    py::class_<transition_parameters>(m, "transition_parameters")
            .def(py::init())
            .def_readwrite("energy", &transition_parameters::energy)
            .def_readwrite("energy_initial",
                           &transition_parameters::energy_initial)
            .def_readwrite("accepted_transitions",
                           &transition_parameters::accepted_transitions)
            .def_readwrite("rejected_transitions",
                           &transition_parameters::rejected_transitions)
            .def_readwrite("high_temp_transitions",
                           &transition_parameters::high_temp_transitions)
            .def_readwrite("consecutive_failures",
                           &transition_parameters::consecutive_failures)
            .def_readwrite("time_elapsed", &transition_parameters::time_elapsed)
            .def_readwrite("temp_initial", &transition_parameters::temp_initial)
            .def_readwrite("temp_current", &transition_parameters::temp_current)
            .def_readwrite("temp_cooling_rate",
                           &transition_parameters::temp_cooling_rate)
            .def_readwrite("MAX_CONSECUTIVE_FAILURES",
                           &transition_parameters::MAX_CONSECUTIVE_FAILURES)
            .def_readwrite("MAX_ENGINE_ITERATIONS",
                           &transition_parameters::MAX_ENGINE_ITERATIONS)
            .def_readwrite("ENERGY_CONVERGENCE",
                           &transition_parameters::ENERGY_CONVERGENCE)
            .def_readwrite(
                    "UPDATE_STEP_MOVE_NODE_PROBABILITY",
                    &transition_parameters::UPDATE_STEP_MOVE_NODE_PROBABILITY)
            .def_readwrite("update_step_move_node_max_step_distance",
                           &transition_parameters::
                                   update_step_move_node_max_step_distance)
            .def("__repr__", [](const transition_parameters &p) {
                std::stringstream os;
                p.print(os);
                return os.str();
            });

    py::class_<degree_distribution_parameters>(m,
                                               "degree_distribution_parameters")
            .def(py::init())
            .def_readwrite("mean", &degree_distribution_parameters::mean)
            .def_readwrite("min_degree",
                           &degree_distribution_parameters::min_degree)
            .def_readwrite("max_degree",
                           &degree_distribution_parameters::max_degree)
            .def("__repr__", [](const degree_distribution_parameters &p) {
                std::stringstream os;
                p.print(os);
                return os.str();
            });

    py::class_<end_to_end_distances_distribution_parameters>(
            m, "end_to_end_distances_distribution_parameters")
            .def(py::init())
            .def_readwrite("physical_normal_mean",
                           &end_to_end_distances_distribution_parameters::
                                   physical_normal_mean)
            .def_readwrite("physical_normal_std_deviation",
                           &end_to_end_distances_distribution_parameters::
                                   physical_normal_std_deviation)
            .def_readwrite("normalized_normal_mean",
                           &end_to_end_distances_distribution_parameters::
                                   normalized_normal_mean)
            .def_readwrite("normalized_normal_std_deviation",
                           &end_to_end_distances_distribution_parameters::
                                   normalized_normal_std_deviation)
            .def_readwrite("normalized_log_std_deviation",
                           &end_to_end_distances_distribution_parameters::
                                   normalized_log_std_deviation)
            .def_readwrite("normalized_log_mean",
                           &end_to_end_distances_distribution_parameters::
                                   normalized_log_mean)
            .def_readwrite(
                    "num_bins",
                    &end_to_end_distances_distribution_parameters::num_bins)
            .def("set_normalized_log_std_deviation",
                 &end_to_end_distances_distribution_parameters::
                         set_normalized_log_std_deviation)
            .def("set_normalized_log_mean",
                 &end_to_end_distances_distribution_parameters::
                         set_normalized_log_mean)
            .def("__repr__",
                 [](const end_to_end_distances_distribution_parameters &p) {
                     std::stringstream os;
                     p.print(os);
                     return os.str();
                 });

    py::class_<cosine_directors_distribution_parameters>(
            m, "cosine_directors_distribution_parameters")
            .def(py::init())
            .def_readwrite("b1", &cosine_directors_distribution_parameters::b1)
            .def_readwrite("b2", &cosine_directors_distribution_parameters::b2)
            .def_readwrite("b3", &cosine_directors_distribution_parameters::b3)
            .def_readwrite("num_bins",
                           &cosine_directors_distribution_parameters::num_bins)
            .def("__repr__",
                 [](const cosine_directors_distribution_parameters &p) {
                     std::stringstream os;
                     p.print(os);
                     return os.str();
                 });

    py::class_<simulated_annealing_generator_config_tree>(
            m, "simulated_annealing_generator_config_tree")
            .def(py::init())
            .def_readwrite("transition_params",
                           &simulated_annealing_generator_config_tree::
                                   transition_params)
            .def_readwrite(
                    "degree_params",
                    &simulated_annealing_generator_config_tree::degree_params)
            .def_readwrite("ete_distance_params",
                           &simulated_annealing_generator_config_tree::
                                   ete_distance_params)
            .def_readwrite(
                    "cosine_params",
                    &simulated_annealing_generator_config_tree::cosine_params)
            .def_readwrite(
                    "domain_params",
                    &simulated_annealing_generator_config_tree::domain_params)
            .def_readwrite("physical_scaling_params",
                           &simulated_annealing_generator_config_tree::
                                   physical_scaling_params)
            .def("load", &simulated_annealing_generator_config_tree::load)
            .def("save", &simulated_annealing_generator_config_tree::save);
}

void init_simulated_annealing_generator(py::module &m) {
    py::class_<simulated_annealing_generator>(m,
                                              "simulated_annealing_generator")
            .def(py::init())
            .def(py::init<size_t>())
            .def(py::init<GraphType>())
            .def(py::init<simulated_annealing_generator_config_tree>())
            .def(py::init<std::string>())
            .def("init_graph_degree",
                 &simulated_annealing_generator::init_graph_degree)
            .def("init_graph_vertex_positions",
                 &simulated_annealing_generator::init_graph_vertex_positions)
            .def("init_histograms",
                 &simulated_annealing_generator::init_histograms)
            .def("set_parameters_from_file",
                 &simulated_annealing_generator::set_parameters_from_file)
            .def("set_parameters_from_configuration_tree",
                 &simulated_annealing_generator::
                         set_parameters_from_configuration_tree)
            .def("save_parameters_to_file",
                 &simulated_annealing_generator::save_parameters_to_file)
            .def("save_parameters_to_configuration_tree",
                 &simulated_annealing_generator::save_parameters_to_configuration_tree)
            .def("engine",
                 &simulated_annealing_generator::engine)
            .def_readwrite("graph",
                 &simulated_annealing_generator::graph_)
            // TODO wrap histograms
            .def_readwrite("target_cumulative_distro_histo_ete_distances",
                 &simulated_annealing_generator::target_cumulative_distro_histo_ete_distances_)
            .def_readwrite("target_cumulative_distro_histo_cosines",
                 &simulated_annealing_generator::target_cumulative_distro_histo_cosines_)
            .def("__repr__", [](const simulated_annealing_generator &sa) {
                std::stringstream os;
                sa.print(os);
                return os.str();
            });
}
