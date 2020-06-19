# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import core
import unittest

class TestBoundingBox(unittest.TestCase):
    def test_bounding_box_constructors(self):
        box_default = core.bounding_box() # default
        print(box_default)
