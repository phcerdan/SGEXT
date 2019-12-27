# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import _sgext.dynamics as dynamics
import _sgext.core.array as array
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
