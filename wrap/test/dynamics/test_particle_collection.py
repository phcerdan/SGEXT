# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import dynamics
import unittest

class TestDynamicsParticleCollection(unittest.TestCase):
    def setUp(self):
        self.collection = dynamics.particle_collection()
        self.particle1 = dynamics.particle()
        self.particle1.id = 1
        self.particle1.pos = [1,1,1]
        self.particle2 = dynamics.particle()
        self.particle2.id = 2
        self.particles = dynamics.VectorParticles()
        self.particles.append(self.particle1)
        self.particles.append(self.particle2)
        self.collection.particles = self.particles
        self.collection.sort()
    def test_binary_find(self):
        print("test_binary_find")
        id_to_find = 1
        p_found = self.collection.binary_find(id_to_find)
        self.assertEqual(p_found.id, id_to_find)
        new_id = 22
        p_found.id = new_id
        self.assertEqual(p_found.id, new_id)
        self.assertEqual(self.collection.particles[0].id, new_id)
        print(self.collection)
    def test_find_index(self):
        print("test_find_index")
        id_to_find = 1
        index_found = self.collection.find_index(id_to_find)
        self.assertEqual(self.collection.particles[index_found].id, id_to_find)
    def test_print(self):
        print("Collection:")
        print(self.collection)
        print("print_id_pos")
        print(dynamics.print_id_pos(self.collection))

