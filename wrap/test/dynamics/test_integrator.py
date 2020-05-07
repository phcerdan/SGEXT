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

from sgext import dynamics
import unittest
from fixture_system4 import System4Fixture

class TestDynamicsIntegrator(unittest.TestCase):
    def setUp(self):
        self.fixture = System4Fixture()
        self.integrator = dynamics.integrator_two_step(self.fixture.system)
        self.deltaT = 1.0
        self.integrator_method = dynamics.integrator_method_verlet_velocities(self.fixture.system, self.deltaT)

    def test_print(self):
        print("Integrator:")
