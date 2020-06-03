# Copyright (C) 2020 Pablo Hernandez-Cerdan
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/

from sgext import generate
import unittest

histo = generate.histo

class TestHisto(unittest.TestCase):
    def setUp(self):
        self.data = [1,1,1,2,2,2,2,3,3,3,3,3,4,4,4,4,4,4,4,5,5,5,6,6,7]
        self.breaks = [1.0,2.0,3.0,4.0,5.0,6.0,7.0]
        self.range = [1,7]

    def test_constructors(self):
        h_default = histo.histo()
        h_data = histo.histo(self.data)
        h_data_breaks = histo.histo(self.data, self.breaks)
        h_data_range = histo.histo(self.data, self.range)
        print(h_data_breaks)

    def test_member_functions(self):
        h_data_breaks = histo.histo(self.data, self.breaks)
        index = h_data_breaks.index_from_value(4.5)
        self.assertEqual(index, 3, "index_from_value fails")

        h_data_breaks2 = histo.histo(self.data, self.breaks)
        h_data_breaks2.reset_counts()
        h_data_breaks2.fill_counts(self.data)
        index2 = h_data_breaks2.index_from_value(4.5)
        self.assertEqual(index, index2)

