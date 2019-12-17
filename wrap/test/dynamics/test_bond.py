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

import _sgext.dynamics as dynamics
import unittest

class TestDynamicsBond(unittest.TestCase):
    def setUp(self):
        self.bond = dynamics.bond()
        self.bond.id_a = 2
        self.bond.id_b = 5
        self.unsorted_bond = dynamics.bond()
        self.unsorted_bond.id_a = 5
        self.unsorted_bond.id_b = 2
    def test_bond(self):
        self.assertEqual(self.bond.id_a, 2)
        self.assertEqual(self.bond.id_b, 5)
    def test_sort_member_function(self):
        self.unsorted_bond.sort()
        self.assertEqual(self.unsorted_bond.id_a, self.bond.id_a)
        self.assertEqual(self.unsorted_bond.id_b, self.bond.id_b)
    def test_sort_free_function(self):
        dynamics.sort(self.unsorted_bond)
        self.assertEqual(self.unsorted_bond.id_a, self.bond.id_a)
        self.assertEqual(self.unsorted_bond.id_b, self.bond.id_b)
    def test_comparison_operators(self):
        self.assertLess(self.bond, self.unsorted_bond)
        self.assertGreater(self.unsorted_bond, self.bond)
        dynamics.sort(self.unsorted_bond)
        self.assertEqual(self.unsorted_bond, self.bond)
    def test_print(self):
        print("Bond:")
        print(self.bond)

class TestDynamicsBondChain(unittest.TestCase):
    def setUp(self):
        self.bond = dynamics.bond(2, 5)
        self.chain = dynamics.bond_chain(2, 5, 2.0)
        self.unsorted_bond = dynamics.bond(5, 2)
        self.default_unsorted_chain = dynamics.bond_chain(5, 2)
    def test_sort_free_function(self):
        dynamics.sort(self.default_unsorted_chain)
        self.assertEqual(self.default_unsorted_chain, self.chain)
    def test_sort_member_function_inherited(self):
        self.default_unsorted_chain.sort()
        self.assertEqual(self.default_unsorted_chain, self.bond)
    def test_comparison_operators_inherited(self):
        self.assertLess(self.chain, self.default_unsorted_chain)
        self.assertGreater(self.default_unsorted_chain, self.chain)
        dynamics.sort(self.default_unsorted_chain)
        self.assertEqual(self.default_unsorted_chain, self.chain)
