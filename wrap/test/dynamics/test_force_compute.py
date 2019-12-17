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

import _sgext.dynamics as dynamics
import _sgext.core.array as array
import unittest
from fixture_system4 import System4Fixture

def a_particle_force(a_particle):
    return a_particle.pos

class TestDynamicsForceCompute(unittest.TestCase):
    def setUp(self):
        self.fixture = System4Fixture()

    def test_particle_force_compute(self):
        self.force_compute = dynamics.particle_force_compute(self.fixture.system)
        self.assertEqual(self.force_compute.get_type(), "ParticleForceCompute")
    def test_force_function_wrapping_works_with_python_functions(self):
        self.force_compute = dynamics.particle_force_compute(self.fixture.system)
        self.force_compute.force_function = a_particle_force
        self.force_compute.compute()
