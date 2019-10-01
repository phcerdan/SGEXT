# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

import sgext.core.array as array
import unittest

class TestArray3D(unittest.TestCase):
    ######### CONSTRUCTORS ##############
    def test_constructors(self):
        a0 = array.array3d()
        a1 = array.array3d(1, 1, 1)
        ax = array.array3d(1, 0, 0)
    ######### MEMBER FUNCTIONS ##########
    def test_member_functions(self):
        arr = array.array3d(1,1,1)
        # __get_items__ and x,y,z
        self.assertEqual(arr[0], arr.x);
        self.assertEqual(arr[1], arr.y);
        self.assertEqual(arr[2], arr.z);
        # __set_items__
        new_v = 2
        arr[0] = new_v;
        arr[1] = new_v;
        arr[2] = new_v;
        self.assertEqual(arr[0], new_v);
        self.assertEqual(arr[1], new_v);
        self.assertEqual(arr[2], new_v);
        # len
        self.assertEqual(len(arr), 3);
        # fill
        arr_old = array.array3d()
        arr_old.fill(1)
        # eq
        self.assertFalse(arr_old == arr)
        self.assertTrue(arr_old != arr)
        self.assertTrue(arr_old <= arr)
        self.assertTrue(arr_old < arr)
        self.assertFalse(arr_old >= arr)
        self.assertFalse(arr_old > arr)
        # iter
        for m in arr:
            self.assertEqual(m, new_v)
        # The following is not possible in Python, not a pybind problem
        # The iterator is not able to change the value.
        # See: https://github.com/pybind/pybind11/issues/1233
        for m in arr:
            m = 1
        for m in arr:
            # self.assertEqual(m, 1) # not happening in python
            self.assertNotEqual(m, 1)


    ######### FREE FUNCTIONS ############
    def test_free_functions(self):
        a1 = array.array3d(1, 1, 1)
        ax = array.array3d(1, 0, 0)
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

if __name__ == '__main__':
    unittest.main()
