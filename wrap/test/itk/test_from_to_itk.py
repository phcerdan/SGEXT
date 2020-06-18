# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import scripts
from sgext import tables_folder
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
        cls.img =scripts.thin_io(input_file=cls.input,
                          out_folder=cls.test_dir,
                          foreground="black",
                          skel_type="end", select_type="first",
                          tables_folder=tables_folder,
                          persistence=2,
                          visualize=False,
                          verbose=False)

    def test_sgext_to_itk(self):
        try:
            import itk
        except:
            print("Warning: itk python package not found. Not testing sgext_to_itk")
            return
        print("test_sgext_to_itk")
        img = self.img
        itk_img = from_to.sgext_to_itk(img)

        itk_size = itk_img.GetLargestPossibleRegion().GetSize()
        np.testing.assert_array_almost_equal([itk_size[0], itk_size[1], itk_size[2]],
                                             img.size() )
        itk_spacing = itk_img.GetSpacing()
        np.testing.assert_array_almost_equal([itk_spacing[0], itk_spacing[1], itk_spacing[2]],
                                             img.spacing() )

        itk_direction = itk_img.GetDirection().GetVnlMatrix()
        row0 = [itk_direction.get(0,0), itk_direction.get(0, 1), itk_direction(0,2)]
        row1 = [itk_direction.get(1,0), itk_direction.get(1, 1), itk_direction(1,2)]
        row2 = [itk_direction.get(2,0), itk_direction.get(2, 1), itk_direction(2,2)]
        np.testing.assert_array_almost_equal([row0, row1, row2], img.direction() )

    def test_itk_to_sgext(self):
        try:
            import itk
        except:
            print("Warning: itk python package not found. Not testing itk_to_sgext")
            return
        print("test_itk_to_sgext")
        itk_img = itk.imread(self.input)
        print(type(itk_img))
        sgext_img = from_to.itk_to_sgext(itk_img)
        itk_spacing = itk_img.GetSpacing()
        np.testing.assert_array_almost_equal(sgext_img.spacing(),
                                             [itk_spacing[0], itk_spacing[1], itk_spacing[2]])
        import sgext
        sgext.itk.view_image(sgext_img, "itk_to_sgext")
