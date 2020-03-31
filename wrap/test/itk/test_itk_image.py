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
import _sgext.scripts as scripts
import unittest
import os, tempfile
import numpy as np

class TestITKImagePointer(unittest.TestCase):
    # This setup only occurs once for the the whole class
    @classmethod
    def setUpClass(cls):
        # Create temp directory
        cls.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        cls.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(cls.input):
            raise "Input image for thin script in test_itk_image not found: " + cls.input
        cls.img =scripts.thin(input=cls.input,
                          out_folder=cls.test_dir,
                          foreground="black",
                          skel_type="end", select_type="first",
                          persistence=2,
                          visualize=False,
                          verbose=True)


    def test_basics(self):
        # Default Constructor (empty)
        image = itk.IUC3P()
        # Read-only functions
        self.assertEqual(image.dimension(), 3)
        self.assertAlmostEqual(image.size().all(), 0.)
        self.assertAlmostEqual(image.origin().all(), 0.)
        self.assertAlmostEqual(image.spacing().all(), 1.)
        self.assertAlmostEqual(image.direction()[0,0], 1.)

    def test_to_pyarray(self):
        img = self.img
        self.assertEqual(img.dimension(), 3)
        np.testing.assert_array_equal(img.size(), [50,50,7])
        self.assertAlmostEqual(img.origin().all(), 0.)
        np.testing.assert_array_almost_equal(img.spacing(), [0.352734, 0.352734, 1])
        self.assertAlmostEqual(img.direction()[0,0], 1.)

        # Fortran contiguous (Default)
        arr_f = img.to_pyarray(contig="F")
        np.testing.assert_array_equal(arr_f.shape, [7, 50, 50])
        # C contiguous
        arr_c = img.to_pyarray(contig="C")
        np.testing.assert_array_equal(arr_c.shape, img.size())

    def test_export_to_itk(self):
        try:
            import itk
        except:
            print("Warning: itk python package not found. Not testing exporting to itk")
            return

        img = self.img
        # Test with C contiguous
        arr_c = img.to_pyarray(contig='C')
        itk_img_c = itk.GetImageFromArray(arr_c)
        self.assertEqual(arr_c.shape[0], 50)
        # ITK always assume than input numpy array will be F contig (which is easier!)
        self.assertEqual(itk_img_f.GetLargestPossibleRegion().GetSize()[0], 7)
        subs = itk.subtract_image_filter(itk_img_c, itk_img_c)
        subs_np = itk.GetArrayViewFromImage(subs) # no-copy
        self.assertEqual(subs_np.max(), 0)

        # Test with Fortran contiguous
        arr_f = img.to_pyarray(contig='F')
        itk_img_f = itk.GetImageFromArray(arr_f)
        self.assertEqual(arr_f.shape[0], 7)
        self.assertEqual(itk_img_f.GetLargestPossibleRegion().GetSize()[0], 50)

        # Check usability with a filter
        itk_img_f.GetLargestPossibleRegion().GetSize()[0]
        subs = itk.subtract_image_filter(itk_img_f, itk_img_f)
        subs_np = itk.GetArrayViewFromImage(subs) # no-copy
        self.assertEqual(subs_np.max(), 0)





