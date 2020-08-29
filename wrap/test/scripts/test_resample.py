# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

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
