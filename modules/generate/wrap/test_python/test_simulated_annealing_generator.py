# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

import sggenerate
import unittest

# The order of the test is important.
# They are run alphabetically, ie
# test_a_x will run before test_b_y
# we write the files before reading them

class TestSimulatedAnnealingAConfigTree(unittest.TestCase):
    def setUp(self):
        self.parameters_file = "fixture_sa_parameters.txt"

    def test_a_config_tree_save(self):
        config_tree = sggenerate.simulated_annealing_generator_config_tree()
        config_tree.degree_params.max_degree = 20
        config_tree.physical_scaling_params.num_vertices = 100
        config_tree.cosine_params.num_bins = 100
        config_tree.ete_distance_params.num_bins = 100
        config_tree.transition_params.MAX_ENGINE_ITERATIONS = 1000
        config_tree.save(self.parameters_file)

    def test_b_config_tree_load(self):
        config_tree = sggenerate.simulated_annealing_generator_config_tree()
        config_tree.load(self.parameters_file)
        self.assertEqual(config_tree.degree_params.max_degree, 20)

class TestSimulatedAnnealingBParameters(unittest.TestCase):

    def test_a_domain_parameters(self):
        par = sggenerate.domain_parameters()
        print(par)

    def test_b_physical_scaling_parameters(self):
        par = sggenerate.physical_scaling_parameters()
        print(par)

    def test_c_transition_parameters(self):
        par = sggenerate.transition_parameters()
        print(par)

    def test_d_degree_distribution_parameters(self):
        par = sggenerate.degree_distribution_parameters()
        print(par)

    def test_e_end_to_end_distances_distribution_parameters(self):
        par = sggenerate.end_to_end_distances_distribution_parameters()
        print(par)

    def test_f_cosine_directors_distribution_parameters(self):
        par = sggenerate.cosine_directors_distribution_parameters()
        print(par)


class TestSimulatedAnnealingC(unittest.TestCase):
    def setUp(self):
        self.parameters_file = "fixture_sa_parameters.txt"
        self.output_file = "fixture_sa_parameters_output.txt"

    def test_simulated_annealing(self):
        gen = sggenerate.simulated_annealing_generator(self.parameters_file)
        gen.engine()
        gen.save_parameters_to_file(self.output_file)

if __name__ == '__main__':
    unittest.main()
