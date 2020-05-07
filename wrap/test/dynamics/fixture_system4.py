# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

