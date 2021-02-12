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
        cls.dirname = os.path.dirname(os.path.abspath(__file__))
        cls.input_file = os.path.join(cls.dirname, '../../../images/bX3D_white.nrrd')
        cls.dmap_file = os.path.join(cls.dirname, '../../../images/bX3D_white_DMAP.nrrd')
        if not os.path.exists(cls.input_file):
            raise "Input image for script used in test_view_image not found: " + cls.input_file

    # TODO closing is not automated
    def test_from_file(self):
        itk.view_image(input_file=self.input_file, win_title = "Original image (foreground=black)")

    def test_from_sgext_itk_img(self):
        dmap_image = itk.read_as_float(input_file=self.dmap_file)
        itk.view_image(input=dmap_image, win_title = "DMap")

    def test_view_image_with_label(self):
        label_file = os.path.join(self.dirname, '../../../images/bX3D_white.nrrd')
        itk.view_image_with_label(input_file=self.input_file, label_file=label_file, win_title="with_label, from files")

        dmap_image = itk.read_as_float(input_file=self.dmap_file)
        label_image = itk.read_as_binary(input_file=label_file)
        itk.view_image_with_label(input=dmap_image, label_image=label_image, win_title = "with_label, from images")
