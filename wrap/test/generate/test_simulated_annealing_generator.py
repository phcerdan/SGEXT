"""
Copyright (C) 2020 Pablo Hernande-Cerdan.

This file is part of SGEXT: http://github.com/phcerdan/sgext.

This file may be used under the terms of the GNU General Public License
version 3 as published by the Free Software Foundation and appearing in
the file LICENSE.GPL included in the packaging of this file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

import _sgext.generate as generate
import unittest

# The order of the test is important.
# They are run alphabetically, ie
# test_a_x will run before test_b_y
# we write the files before reading them

class TestSimulatedAnnealingAConfigTree(unittest.TestCase):
    def setUp(self):
        self.parameters_file = "fixture_sa_parameters.txt"

    def test_a_config_tree_save(self):
        config_tree = generate.simulated_annealing_generator_config_tree()
        config_tree.degree_params.max_degree = 20
        config_tree.physical_scaling_params.num_vertices = 100
        config_tree.cosine_params.num_bins = 100
        config_tree.ete_distance_params.num_bins = 100
        config_tree.transition_params.MAX_ENGINE_ITERATIONS = 1000
        config_tree.save(self.parameters_file)

    def test_b_config_tree_load(self):
        config_tree = generate.simulated_annealing_generator_config_tree()
        config_tree.load(self.parameters_file)
        self.assertEqual(config_tree.degree_params.max_degree, 20)

class TestSimulatedAnnealingBParameters(unittest.TestCase):

    def test_a_domain_parameters(self):
        par = generate.domain_parameters()
        print(par)

    def test_b_physical_scaling_parameters(self):
        par = generate.physical_scaling_parameters()
        print(par)

    def test_c_transition_parameters(self):
        par = generate.transition_parameters()
        print(par)

    def test_d_degree_distribution_parameters(self):
        par = generate.degree_distribution_parameters()
        print(par)

    def test_e_end_to_end_distances_distribution_parameters(self):
        par = generate.end_to_end_distances_distribution_parameters()
        print(par)

    def test_f_cosine_directors_distribution_parameters(self):
        par = generate.cosine_directors_distribution_parameters()
        print(par)


class TestSimulatedAnnealingC(unittest.TestCase):
    def setUp(self):
        self.parameters_file = "fixture_sa_parameters.txt"
        self.output_file = "fixture_sa_parameters_output.txt"

    def test_simulated_annealing(self):
        gen = generate.simulated_annealing_generator(self.parameters_file)
        gen.engine()
        gen.save_parameters_to_file(self.output_file)

if __name__ == '__main__':
    unittest.main()
