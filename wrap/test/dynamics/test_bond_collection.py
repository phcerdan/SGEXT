# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import dynamics
import unittest

class TestDynamicsBondCollection(unittest.TestCase):
    def setUp(self):
        self.bond1 = dynamics.bond(1,2)
        self.bond_collection = dynamics.bond_collection()
        self.bond_collection.bonds = [self.bond1, self.bond1]
        # TODO Append is not working using standard stl binding, require MAKE_OPAQUE
        self.bond_collection.bonds.append(self.bond1)
        self.assertEqual(len(self.bond_collection.bonds), 2)
    def test_print(self):
        print("bond_collection:")
        print(self.bond_collection)
