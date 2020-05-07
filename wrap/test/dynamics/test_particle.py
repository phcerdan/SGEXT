# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import dynamics

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
