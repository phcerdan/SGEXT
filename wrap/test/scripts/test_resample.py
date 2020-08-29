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

from sgext import scripts
from sgext import itk
import unittest
import os
import tempfile
import numpy as np

class TestResample(unittest.TestCase):
    def setUp(self):
        # Create temp directory
        self.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        self.input_filename = os.path.join(dirname, '../../../images/bX3D_white.nrrd')
        if not os.path.exists(self.input_filename):
            raise "Input image for test resample not found: " + self.input_filename
        self.input = itk.read_as_binary(self.input_filename)

    def test_resample(self):
        shrink_factor = 2
        resampled_image = scripts.resample_image(
            input=self.input,
            shrink_factor=shrink_factor,
            verbose=True)
        # original_size = [50, 50, 7]
        expected_size  = [25, 25, 3]
        expected_spacing  = [0.35273368132073296, 0.35273368132073296, 1]
        expected_origin  = [0, 0, 0]
        output_size = resampled_image.size()
        np.testing.assert_array_almost_equal([output_size[0], output_size[1], output_size[2]],
                                             expected_size)
