# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import dynamics
import unittest
from fixture_system4 import System4Fixture

def a_particle_force(a_particle):
    return a_particle.pos

class TestDynamicsForceCompute(unittest.TestCase):
    def setUp(self):
        self.fixture = System4Fixture()

    def test_particle_force_compute(self):
        self.force_compute = dynamics.force_compute_particle(self.fixture.system)
        self.assertEqual(self.force_compute.get_type(), "ParticleForceCompute")
    def test_force_function_wrapping_works_with_python_functions(self):
        self.force_compute = dynamics.force_compute_particle(self.fixture.system)
        self.force_compute.force_function = a_particle_force
        self.force_compute.compute()
    def test_force_function_wrapping_works_with_cpp_wrapped_functions(self):
        self.fixture.system.all.sort()
        self.force_compute = dynamics.force_compute_pair_bond_with_bond(self.fixture.system)
        self.force_compute.force_function = dynamics.forces.force_function_wlc_petrosyan
        # Expected exception because bonds.properties are not populated with persistence_length and kT
        with self.assertRaises(RuntimeError):
            self.force_compute.compute()
        # Populate bonds.properties
        props = dynamics.bond_properties_physical(1.0, 1.0)
        for bond in self.fixture.system.bonds.bonds:
            bond.properties = props

        self.force_compute.compute()
