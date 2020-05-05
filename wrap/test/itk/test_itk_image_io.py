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

import _sgext.itk as itk
import unittest
import os
import numpy as np

class TestImageIO(unittest.TestCase):
    # This setup only occurs once for the the whole class
    @classmethod
    def setUpClass(cls):
        # Create temp directory
        dirname = os.path.dirname(os.path.abspath(__file__))
        cls.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(cls.input):
            raise "Input image for test_itk_image_io not found: " + cls.input

    def test_read_as_float(self):
        input_file = self.input;
        print(input_file)
        image = itk.read_as_float(input_file=input_file)
        # Read-only functions
        self.assertEqual(image.dimension(), 3)
        self.assertNotEqual(image.size().all(), 0.)

    def test_read_as_binary(self):
        input_file = self.input;
        image = itk.read_as_binary(input_file=input_file)
        # Read-only functions
        self.assertEqual(image.dimension(), 3)
        self.assertNotEqual(image.size().all(), 0.)

