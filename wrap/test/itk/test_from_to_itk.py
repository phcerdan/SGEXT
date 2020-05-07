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
import unittest
import os, tempfile
import numpy as np
from sgext import from_to

class TestFromToITK(unittest.TestCase):
    # This setup only occurs once for the the whole class
    @classmethod
    def setUpClass(cls):
        # Create temp directory
        cls.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        cls.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(cls.input):
            raise "Input image for thin script in test_from_to_itk not found: " + cls.input
        cls.img =scripts.thin(input=cls.input,
                          out_folder=cls.test_dir,
                          foreground="black",
                          skel_type="end", select_type="first",
                          persistence=2,
                          visualize=False,
                          verbose=False)

    def test_sgext_to_itk(self):
        try:
            import itk
        except:
            print("Warning: itk python package not found. Not testing sgext_to_itk")
            return
        img = self.img
        itk_img = from_to.sgext_to_itk(img)

        itk_size = itk_img.GetLargestPossibleRegion().GetSize()
        np.testing.assert_array_almost_equal([itk_size[0], itk_size[1], itk_size[2]], img.size() )
        itk_spacing = itk_img.GetSpacing()
        np.testing.assert_array_almost_equal([itk_spacing[0], itk_spacing[1], itk_spacing[2]], img.spacing() )

        itk_direction = itk_img.GetDirection().GetVnlMatrix()
        row0 = [itk_direction.get(0,0), itk_direction.get(0, 1), itk_direction(0,2)]
        row1 = [itk_direction.get(1,0), itk_direction.get(1, 1), itk_direction(1,2)]
        row2 = [itk_direction.get(2,0), itk_direction.get(2, 1), itk_direction(2,2)]
        np.testing.assert_array_almost_equal([row0, row1, row2], img.direction() )
