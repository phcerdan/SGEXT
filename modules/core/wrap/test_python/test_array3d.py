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

import sgcore
import unittest

class TestArray3D(unittest.TestCase):
    ######### CONSTRUCTORS ##############
    def test_constructors(self):
        a0 = sgcore.array.array3d()
        a1 = sgcore.array.array3d(1, 1, 1)
        ax = sgcore.array.array3d(1, 0, 0)
    ######### MEMBER FUNCTIONS ##########
    def test_member_functions(self):
        arr = sgcore.array.array3d(1,1,1)
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
        arr_old = sgcore.array.array3d()
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
        a1 = sgcore.array.array3d(1, 1, 1)
        ax = sgcore.array.array3d(1, 0, 0)
        sgcore.array.norm(a1)
        sgcore.array.distance(a1,ax)
        sgcore.array.dot_product(a1,ax)
        sgcore.array.cross_product(a1,ax)
        sgcore.array.angle(a1,ax)
        sgcore.array.cos_director(a1,ax)
        sgcore.array.plus(a1,ax)
        sgcore.array.plus_scalar(a1, 1.0)
        sgcore.array.minus(a1,ax)
        sgcore.array.minus_scalar(a1, 1.0)

if __name__ == '__main__':
    unittest.main()
