# Copyright (C) 2019 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

import _sgext.scripts as scripts
import unittest
import os, tempfile

class TestThin(unittest.TestCase):
    def setUp(self):
        # Create temp directory
        self.test_dir = tempfile.mkdtemp()
        dirname = os.path.dirname(os.path.abspath(__file__))
        self.input = os.path.join(dirname, '../../../images/bX3D.tif')
        if not os.path.exists(self.input):
            raise "Input image for test thin script not found: " + self.input

    def test_thin(self):
        scripts.thin(input=self.input,
                     out_foldername=self.test_dir,
                     foreground="black",
                     skel_type="end", select_type="first",
                     persistence=2,
                     visualize=False,
                     verbose=True)
