# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
array = core.array
import unittest

class TestArray3D(unittest.TestCase):
    ######### FREE FUNCTIONS ############
    def test_free_functions(self):
        a1 = [1, 1, 1]
        ax = [1, 0, 0]
        array.norm(a1)
        array.distance(a1,ax)
        array.dot_product(a1,ax)
        array.cross_product(a1,ax)
        array.angle(a1,ax)
        array.cos_director(a1,ax)
        array.plus(a1,ax)
        array.plus_scalar(a1, 1.0)
        array.minus(a1,ax)
        array.minus_scalar(a1, 1.0)
        a1_negated = array.negate(a1)
        self.assertEqual(a1_negated[0], -a1[0]);
        self.assertEqual(a1_negated[1], -a1[1]);
        self.assertEqual(a1_negated[2], -a1[2]);

if __name__ == '__main__':
    unittest.main()
