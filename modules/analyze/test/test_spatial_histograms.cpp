/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#include "gmock/gmock.h"
#include "spatial_histograms.hpp"
#include "compute_graph_properties.hpp"
#include <limits>

/**
 * Spatial Graph.
 *     o
 *     |
 *     o-o
 *     |
 *     o
 */
struct SpatialGraphFixture : public ::testing::Test {
  using GraphType = SG::GraphAL;
  GraphType g;
  void SetUp() override {
    using boost::add_edge;
    this->g = GraphType(4);
    // Add edge with an associated SpatialEdge at construction.
    SG::PointType n3{{0, 3, 0}};
    SG::PointType n2{{0, 2, 0}};
    SG::PointType n1{{0, 1, 0}};
    SG::PointType p0{{0, 0, 0}};
    SG::PointType e1{{1, 0, 0}};
    SG::PointType e2{{2, 0, 0}};
    SG::PointType s1{{0, -1, 0}};
    SG::PointType s2{{0, -2, 0}};
    SG::PointType s3{{0, -3, 0}};

    g[0].pos = n3;
    g[1].pos = p0;
    g[2].pos = s3;
    g[3].pos = e2;

    SG::SpatialEdge se01;
    se01.edge_points.insert(std::end(se01.edge_points), {n1, n2});
    add_edge(0, 1, se01, g);
    SG::SpatialEdge se12;
    se12.edge_points.insert(std::end(se12.edge_points), {s1, s2});
    add_edge(1, 2, se12, g);
    SG::SpatialEdge se13;
    se13.edge_points.insert(std::end(se13.edge_points), {e1});
    add_edge(1, 3, se13, g);
  }
};

TEST_F(SpatialGraphFixture, histogram_degrees) {
  auto degrees = SG::compute_degrees(g);
  auto histo_degrees = SG::histogram_degrees(degrees);
  std::cout << "Degrees Histogram" << std::endl;
  EXPECT_EQ(histo_degrees.name, "degrees");
  std::cout << "Print counts with breaks" << std::endl;
  histo_degrees.PrintBreaksAndCounts(std::cout);
  std::cout << "Print counts with centers" << std::endl;
  histo_degrees.PrintCentersAndCounts(std::cout);
  std::cout << "Print centers" << std::endl;
  histo_degrees.PrintCenters(std::cout);
  std::cout << "Print counts" << std::endl;
  histo_degrees.PrintCounts(std::cout);
  std::cout << "Print breaks" << std::endl;
  histo_degrees.PrintBreaks(std::cout);
}

TEST_F(SpatialGraphFixture, histogram_degrees_with_bins) {
  auto degrees = SG::compute_degrees(g);
  size_t bins = 10;
  auto histo_degrees = SG::histogram_degrees(degrees, bins);
  std::cout << "Degrees Histogram with bins = " << bins << std::endl;
  EXPECT_EQ(histo_degrees.name, "degrees");
  EXPECT_EQ(histo_degrees.bins, bins);
  histo_degrees.PrintBreaksAndCounts(std::cout);
}

TEST_F(SpatialGraphFixture, histogram_distances) {
  auto distances = SG::compute_ete_distances(g);
  auto histo_distances = SG::histogram_distances(distances);
  std::cout << "Distance Histogram" << std::endl;
  EXPECT_EQ(histo_distances.name, "distances");
  histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_F(SpatialGraphFixture, histogram_distances_with_width) {
  auto distances = SG::compute_ete_distances(g);
  double width = 0.5;
  auto histo_distances = SG::histogram_distances(distances, width);
  std::cout << "Distance Histogram with bins" << std::endl;
  EXPECT_EQ(histo_distances.name, "distances");
  double max_length = 3.0;
  size_t expected_bins = static_cast<size_t>(max_length / width);
  EXPECT_EQ(histo_distances.bins, expected_bins);
  histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_F(SpatialGraphFixture, histogram_angles) {
  auto angles = SG::compute_angles(g);
  auto histo_angles = SG::histogram_angles(angles);
  std::cout << "Angles Histogram" << std::endl;
  EXPECT_EQ(histo_angles.name, "angles");
  histo_angles.PrintBreaksAndCounts(std::cout);

  // compute cosines from angles, not from g
  auto cosines = SG::compute_cosines(angles);
  auto histo_cosines = SG::histogram_cosines(cosines);
  std::cout << "Cosines Histogram" << std::endl;
  EXPECT_EQ(histo_cosines.name, "cosines");
  histo_cosines.PrintBreaksAndCounts(std::cout);
}

TEST_F(SpatialGraphFixture, histogram_angles_with_bins) {
  auto angles = SG::compute_angles(g);
  size_t bins = 10;
  auto histo_angles = SG::histogram_angles(angles, bins);
  std::cout << "Angles Histogram with bins = " << bins << std::endl;
  EXPECT_EQ(histo_angles.name, "angles");
  EXPECT_EQ(histo_angles.bins, bins);
  histo_angles.PrintBreaksAndCounts(std::cout);

  // compute cosines from angles, not from g
  auto cosines = SG::compute_cosines(angles);
  auto histo_cosines = SG::histogram_cosines(cosines, bins);
  std::cout << "Cosines Histogram with bins = " << bins << std::endl;
  EXPECT_EQ(histo_cosines.name, "cosines");
  EXPECT_EQ(histo_cosines.bins, bins);
  histo_cosines.PrintBreaksAndCounts(std::cout);
}

TEST(IO, print_and_read_histogram) {
  std::cout << "Print and Read" << std::endl;
  std::vector<unsigned int> data({1, 2, 3, 5, 6, 4, 2, 2, 5});
  size_t bins = 3;
  auto histo = SG::histogram_degrees(data, bins);
  histo.name = "Whatever";
  // SG::print_histogram(histo, std::cout);
  // stringstream as memory i,o,stream
  std::stringstream buffer;
  SG::print_histogram(histo, buffer);
  std::cout << "printed into buffer" << std::endl;
  EXPECT_EQ(buffer.str().empty(), false);
  std::cout << buffer.str() << std::endl;
  auto histo_read = SG::read_histogram(buffer);
  std::cout << "already readed from buffer" << std::endl;
  EXPECT_EQ(histo_read.bins, histo.bins);
  EXPECT_EQ(histo_read.counts, histo.counts);
  // EXPECT_EQ(histo_read.breaks, histo.breaks); double comparisson failure,
  // instead:
  EXPECT_EQ(histo_read.breaks.size(), histo.breaks.size());
  EXPECT_EQ(histo_read.breaks.size(), bins + 1);
  EXPECT_NEAR(histo_read.breaks[0], histo.breaks[0], 0.00001);
  EXPECT_NEAR(histo_read.breaks[1], histo.breaks[1], 0.00001);
  EXPECT_NEAR(histo_read.breaks[2], histo.breaks[2], 0.00001);
  EXPECT_NEAR(histo_read.breaks[3], histo.breaks[3], 0.00001);
  EXPECT_EQ(histo_read.range, histo.range);
  EXPECT_EQ(histo_read.name, histo.name);
}
