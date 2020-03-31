# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import _sgext.itk as itk
import _sgext.scripts as scripts
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
        cls.img =scripts.create_distance_map_io(input_file=cls.input,
                          out_folder=cls.test_dir,
                          foreground="black",
                          verbose=True)

    # TODO closing is not automated
    def test_from_file(self):
        itk.view_image(input_file=self.input, win_title = "Original image (foreground=black)")

    def test_from_sgext_itk_img(self):
        itk.view_image(input=self.img, win_title = "DMap")

