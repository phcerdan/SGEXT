# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import itk
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

