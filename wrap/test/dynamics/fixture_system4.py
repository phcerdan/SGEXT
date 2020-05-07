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

class System4Fixture:
    def __init__(self):
        self.system = dynamics.system()
        # Define particles
        self.particle0 = dynamics.particle()
        self.particle0.id = 0
        self.particle0.pos = [0,0,0]
        self.particle1 = dynamics.particle()
        self.particle1.id = 1
        self.particle1.pos = [1,0,0]
        self.particle2 = dynamics.particle()
        self.particle2.id = 2
        self.particle2.pos = [2,0,0]
        self.particle3 = dynamics.particle()
        self.particle3.id = 3
        self.particle3.pos = [1,2,0]
        # Define VectorParticles
        self.particles = dynamics.VectorParticles(
            [self.particle0, self.particle1,
             self.particle2, self.particle3 ])
        # Define particle_collection and assign to system
        self.particle_collection = dynamics.particle_collection()
        self.particle_collection.particles = self.particles;
        self.system.all = self.particle_collection

        self.bond01 = dynamics.bond_chain(0, 1, 1.4)
        self.bond12 = dynamics.bond_chain(1, 2, 1.2)
        self.bond13 = dynamics.bond_chain(1, 3, 3.0)
        self.bond_collection = dynamics.bond_collection()
        self.bond_collection.bonds = [self.bond01, self.bond12, self.bond13]
        self.system.bonds = self.bond_collection

