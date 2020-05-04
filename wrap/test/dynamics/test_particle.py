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

class TestDynamicsParticle(unittest.TestCase):
    def setUp(self):
        self.part = dynamics.particle()
        self.part.id = 2
        self.part.pos = [1,1,1]
        self.part.dynamics = dynamics.particle_dynamics()
        self.part.dynamics.vel = [0.1,0.1,0.1]
        self.part.dynamics.acc = [0.2,0.2,0.2]
        self.part.material = dynamics.particle_material()
        self.part.material.mass = 2.0;
    def test_particle(self):
        self.assertEqual(self.part.id, 2)
        self.assertEqual(self.part.pos, [1,1,1])
        self.assertEqual(self.part.material.mass, 2.0)
    def test_print(self):
        print("ParticleMaterial:")
        print(self.part.material)
        print("ParticleDynamic:")
        print(self.part.dynamics)
        print("Particle:")
        print(self.part)
