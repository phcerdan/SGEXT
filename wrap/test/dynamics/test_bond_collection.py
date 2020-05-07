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
