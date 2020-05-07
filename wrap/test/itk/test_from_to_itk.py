# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import itk
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
        img = self.img
        itk_img = from_to.sgext_to_itk(img)

        itk_size = itk_img.GetLargestPossibleRegion().GetSize()
        np.testing.assert_array_equal([itk_size[0], itk_size[1], itk_size[2]], img.size() )
        itk_spacing = itk_img.Getspacing()
        np.testing.assert_array_equal([itk_spacing[0], itk_spacing[1], itk_spacing[2]], img.spacing() )

        itk_direction = itk_img.Getdirection()
        np.testing.assert_array_equal([[itk_direction[0]], [itk_direction[1]], [itk_direction[2]]], img.direction() )
