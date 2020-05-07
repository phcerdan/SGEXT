# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from sgext import scripts
import unittest
import os, tempfile

class TestCreateDistanceMap(unittest.TestCase):
    def setUp(self):
        # Create temp directory
        self.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        self.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(self.input):
            raise "Input image for test thin script not found: " + self.input

    def test_create_distance_map_io(self):
        scripts.create_distance_map_io(
            input_file=self.input,
            out_folder=self.test_dir,
            foreground="black",
            verbose=True)
