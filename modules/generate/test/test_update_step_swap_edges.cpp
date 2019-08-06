#include "common_types.hpp"
#include "generate_common.hpp"
#include "rng.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "update_step_swap_edges.hpp"
#include "gmock/gmock.h"

using namespace ::testing;

struct UpdateStepSwapEdgesFixture : public ::testing::Test {
    void SetUp() override {
        // Fix seed in static local thread engine for this test
        // Avoiding the chances of not moving the position if the random
        // generated modulus is zero.
        RNG::engine().seed(9999); // is_swap_parallel == true
        this->CreateGraphTooSmall();
        this->CreateGraph();
        this->CreateHistograms();
    }
    void CreateGraph() {
        g = SG::GraphType(4);
        p0 = SG::PointType{{0.0, 0.0, 0.0}};
        p1 = SG::PointType{{0.5, 0.0, 0.0}};
        p2 = SG::PointType{{0.5, 0.5, 0.0}};
        p3 = SG::PointType{{0.0, 0.5, 0.0}};
        g[0].pos = p0;
        g[1].pos = p1;
        g[2].pos = p2;
        g[3].pos = p3;
        boost::add_edge(0, 1, SG::SpatialEdge(), g);
        boost::add_edge(2, 3, SG::SpatialEdge(), g);
    }
    void CreateGraphTooSmall() {
        g_small = SG::GraphType(3);
        p0 = SG::PointType{{0.0, 0.0, 0.0}};
        p1 = SG::PointType{{0.5, 0.0, 0.0}};
        p2 = SG::PointType{{0.5, 0.5, 0.0}};
        g_small[0].pos = p0;
        g_small[1].pos = p1;
        g_small[2].pos = p2;
        boost::add_edge(0, 1, SG::SpatialEdge(), g_small);
        boost::add_edge(1, 2, SG::SpatialEdge(), g_small);
    }
    void CreateHistograms() {
        // Create histograms involved
        auto all_distances = SG::get_all_end_to_end_distances_of_edges(g, bc);
        histo_distances = SG::Histogram(
                all_distances,
                histo::GenerateBreaksFromRangeAndBins(
                        0.0, sqrt(2.0) + std::numeric_limits<double>::epsilon(),
                        histo_bins_distances));
        std::vector<double> dummy_histo_data;
        histo_cosines = SG::Histogram(dummy_histo_data,
                                      histo::GenerateBreaksFromRangeAndBins(
                                              -pi, pi, histo_bins_cosines));
    }
    const double pi = atan(1.0) * 4.0;
    SG::GraphType g_small;
    SG::GraphType g;
    SG::PointType p0, p1, p2, p3;
    size_t histo_bins_distances = 10;
    size_t histo_bins_cosines = 10;
    SG::Histogram histo_distances;
    SG::Histogram histo_cosines;
    ArrayUtilities::boundary_condition bc =
            ArrayUtilities::boundary_condition::PERIODIC;
};

TEST_F(UpdateStepSwapEdgesFixture, constructor_works) {
    auto step = SG::update_step_swap_edges(g, histo_distances, histo_cosines);
    EXPECT_THAT(histo_distances.counts, Each(AllOf(Ge(0))));
    EXPECT_THAT(histo_cosines.counts, Each(AllOf(Eq(0))));
    std::cout << "Distances histogram, initial" << std::endl;
    histo_distances.PrintBreaksAndCounts(std::cout);
    std::cout << "Cosines histogram, initial" << std::endl;
    histo_cosines.PrintBreaksAndCounts(std::cout);
}
TEST_F(UpdateStepSwapEdgesFixture, select_two_valid_edges_works) {
    auto step = SG::update_step_swap_edges(g, histo_distances, histo_cosines);
    // g_small is too small (and connected) to get two valid edges
    EXPECT_ANY_THROW(step.select_two_valid_edges(g_small));
    auto valid_edges = step.select_two_valid_edges(g);
    auto edge1 = valid_edges.first;
    auto edge2 = valid_edges.second;
    EXPECT_TRUE(edge1 == edge(0, 1, g).first || edge1 == edge(2, 3, g).first);
    EXPECT_TRUE(edge2 == edge(0, 1, g).first || edge2 == edge(2, 3, g).first);
    std::cout << "Edge1: " << edge1.m_source << "--" << edge1.m_target
              << std::endl;
    std::cout << "Edge2: " << edge2.m_source << "--" << edge2.m_target
              << std::endl;
}

TEST_F(UpdateStepSwapEdgesFixture, perform_works) {
    auto step = SG::update_step_swap_edges(g, histo_distances, histo_cosines);
    step.perform();
    EXPECT_THAT(histo_distances.counts, Each(AllOf(Ge(0))));
    // There is no cosines computation because degree of all nodes is 2.
    EXPECT_THAT(histo_cosines.counts, Each(AllOf(Eq(0))));
    std::cout << "Distances histogram after perform():" << std::endl;
    histo_distances.PrintBreaksAndCounts(std::cout);
    std::cout << "Cosines histogram after perform():" << std::endl;
    histo_cosines.PrintBreaksAndCounts(std::cout);
}

TEST_F(UpdateStepSwapEdgesFixture, undo_works) {
    auto step = SG::update_step_swap_edges(g, histo_distances, histo_cosines);
    auto histo_distances_counts_initial = histo_distances.counts;
    auto histo_cosines_counts_initial = histo_cosines.counts;
    step.perform();
    step.undo();
    EXPECT_THAT(histo_distances.counts, Each(AllOf(Ge(0))));
    EXPECT_EQ(histo_distances.counts, histo_distances_counts_initial);
    EXPECT_THAT(histo_cosines.counts, Each(AllOf(Ge(0))));
    EXPECT_EQ(histo_cosines.counts, histo_cosines_counts_initial);
}

TEST_F(UpdateStepSwapEdgesFixture, update_graph_works_when_is_swap_parallel) {
    RNG::engine().seed(9999); // is_swap_parallel == true
    auto step = SG::update_step_swap_edges(g, histo_distances, histo_cosines);
    // update_graph before perform()
    EXPECT_ANY_THROW(step.update_graph());
    step.perform();
    step.update_graph();
    SG::print_spatial_edges(g);
    EXPECT_TRUE(step.is_swap_parallel_);
    EXPECT_EQ(step.new_edges_.first.m_source, 2);
    EXPECT_EQ(step.new_edges_.first.m_target, 0);
    EXPECT_EQ(step.new_edges_.second.m_source, 3);
    EXPECT_EQ(step.new_edges_.second.m_target, 1);
}

TEST_F(UpdateStepSwapEdgesFixture,
       update_graph_works_when_is_NOT_swap_parallel) {
    RNG::engine().seed(2); // is_swap_parallel == false
    auto step = SG::update_step_swap_edges(g, histo_distances, histo_cosines);
    step.perform();
    step.update_graph();
    SG::print_spatial_edges(g);
    EXPECT_FALSE(step.is_swap_parallel_);
    EXPECT_EQ(step.new_edges_.first.m_source, 0);
    EXPECT_EQ(step.new_edges_.first.m_target, 3);
    EXPECT_EQ(step.new_edges_.second.m_source, 2);
    EXPECT_EQ(step.new_edges_.second.m_target, 1);
}
