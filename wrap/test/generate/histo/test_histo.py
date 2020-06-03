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

from sgext import generate
import unittest

histo = generate.histo

class TestHisto(unittest.TestCase):
    def setUp(self):
        self.data = [1,1,1,2,2,2,2,3,3,3,3,3,4,4,4,4,4,4,4,5,5,5,6,6,7]
        self.breaks = [1.0,2.0,3.0,4.0,5.0,6.0,7.0]
        self.range = [1,7]

    def test_constructors(self):
        h_default = histo.histo()
        h_data = histo.histo(self.data)
        h_data_breaks = histo.histo(self.data, self.breaks)
        h_data_range = histo.histo(self.data, self.range)
        print(h_data_breaks)

    def test_member_functions(self):
        h_data_breaks = histo.histo(self.data, self.breaks)
        index = h_data_breaks.index_from_value(4.5)
        self.assertEqual(index, 3, "index_from_value fails")

        h_data_breaks2 = histo.histo(self.data, self.breaks)
        h_data_breaks2.reset_counts()
        h_data_breaks2.fill_counts(self.data)
        index2 = h_data_breaks2.index_from_value(4.5)
        self.assertEqual(index, index2)

