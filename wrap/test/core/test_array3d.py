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

import _sgext.core.array as array
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
