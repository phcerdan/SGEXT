#include "gmock/gmock.h"
#include "array_utilities.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"
#include <boost/graph/graphviz.hpp>
#include <fstream>
#include <iostream>

struct SpatialGraph3DFixture : public ::testing::Test {
  using GraphType = SG::GraphAL;
  GraphType g;
  using vertex_iterator =
      typename boost::graph_traits<GraphType>::vertex_iterator;
  using edge_iterator = typename boost::graph_traits<GraphType>::edge_iterator;

  void SetUp() override {
    this->g = GraphType(4);

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

TEST_F(SpatialGraph3DFixture, write_graphviz) {
  boost::dynamic_properties dp;
  dp.property("node_id", boost::get(boost::vertex_index, g));
  dp.property("spatial_node", boost::get(boost::vertex_bundle, g));
  dp.property("spatial_edge", boost::get(boost::edge_bundle, g));
  boost::write_graphviz_dp(std::cout, g, dp);
  {
    std::ofstream ofile("graphviz_test_out.dot");
    boost::write_graphviz_dp(ofile, g, dp);
  }
}

/** Please note that node_id "label" is meaningless, but required for the
 * reader. I haven't found a way, ie with: static_property_map, ref_property_map
 * or dummy_property_map, to ignore it. Don't expect that the node_id in the
 * .dot files are the same ids than in the graph after read.
 */
TEST_F(SpatialGraph3DFixture, read_graphviz) {
  // From: http://programmingexamples.net/wiki/Boost/BGL/RelabelInputVertices
  boost::dynamic_properties dp;
  GraphType g2;
  dp.property("node_id", boost::get(&SG::SpatialNode::label, g2));
  dp.property("spatial_node", boost::get(boost::vertex_bundle, g2));
  dp.property("spatial_edge", boost::get(boost::edge_bundle, g2));
  {
    std::ifstream ifile("graphviz_test_out.dot");
    boost::read_graphviz(ifile, g2, dp, "node_id");
  }
  boost::write_graphviz_dp(std::cout, g2, dp);
  EXPECT_EQ(boost::num_vertices(g), boost::num_vertices(g2));
  EXPECT_EQ(boost::num_edges(g), boost::num_edges(g2));
}
