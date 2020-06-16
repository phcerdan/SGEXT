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

from sgext import itk
from sgext import scripts
import unittest
import os, tempfile

class TestITKViewImage(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Create temp directory
        cls.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        cls.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(cls.input):
            raise "Input image for script used in test_view_image not found: " + cls.input

    # TODO closing is not automated
    def test_from_file(self):
        itk.view_image(input_file=self.input, win_title = "Original image (foreground=black)")

    def test_from_sgext_itk_img(self):
        dmap =scripts.create_distance_map_io(input_file=self.input,
                          out_folder=self.test_dir,
                          foreground="black",
                          use_itk=False,
                          verbose=True)
        itk.view_image(input=dmap, win_title = "DMap")
