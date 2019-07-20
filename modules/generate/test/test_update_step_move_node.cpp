#include "common_types.hpp"
#include "generate_common.hpp"
#include "rng.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "update_step_move_node.hpp"
#include "gmock/gmock.h"

using namespace ::testing;

struct UpdateStepMoveNodeFixture : public ::testing::Test {
    void SetUp() override {
        // Fix seed in static local thread engine for this test
        // Avoiding the chances of not moving the position if the random
        // generated modulus is zero.
        RNG::engine().seed(9999);
        this->CreateGraph();
        this->CreateHistograms();
    }
    void CreateGraph() {
        g = SG::GraphType(3);
        p0 = SG::PointType{{0.0, 0.0, 0.0}};
        p1 = SG::PointType{{0.5, 0.0, 0.0}};
        p2 = SG::PointType{{0.5, 0.5, 0.0}};
        g[0].pos = p0;
        g[1].pos = p1;
        g[2].pos = p2;
        boost::add_edge(0, 1, SG::SpatialEdge(), g);
        boost::add_edge(1, 2, SG::SpatialEdge(), g);
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
    SG::GraphType g;
    SG::PointType p0, p1, p2;
    size_t histo_bins_distances = 10;
    size_t histo_bins_cosines = 10;
    SG::Histogram histo_distances;
    SG::Histogram histo_cosines;
    ArrayUtilities::boundary_condition bc =
            ArrayUtilities::boundary_condition::PERIODIC;
};

TEST_F(UpdateStepMoveNodeFixture, constructor_works) {
    auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
    std::cout << "Distances histogram, initial" << std::endl;
    histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_F(UpdateStepMoveNodeFixture, perform_works) {
    auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
    step.max_step_distance_ = sqrt(3.0); // Allow big jump for test
    step.perform();
    EXPECT_THAT(histo_distances.counts, Each(AllOf(Ge(0))));
    std::cout << "Distances histogram after perform():" << std::endl;
    histo_distances.PrintBreaksAndCounts(std::cout);
}
TEST_F(UpdateStepMoveNodeFixture, undo_works) {
    auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
    auto histo_distances_counts_initial = histo_distances.counts;
    step.perform();
    step.undo();
    EXPECT_THAT(histo_distances.counts, Each(AllOf(Ge(0))));
    EXPECT_EQ(histo_distances.counts, histo_distances_counts_initial);
}

TEST_F(UpdateStepMoveNodeFixture, update_graph_works) {
    auto step = SG::update_step_move_node(g, histo_distances, histo_cosines);
    // update_graph before perform()
    EXPECT_ANY_THROW(step.update_graph());
    step.perform();
    EXPECT_THAT(step.selected_node_, Lt(3));
    step.update_graph();
    // Check that pos of the node has changed from its initial state
    // Note that there is a non-zero chance (extremely low) for the
    // random movement to get zero modulus. The seed is fixed in the test
    // fixture to avoid failure.
    if (step.selected_node_ == 0)
        EXPECT_NE(g[step.selected_node_].pos, p0);
    if (step.selected_node_ == 1)
        EXPECT_NE(g[step.selected_node_].pos, p1);
    if (step.selected_node_ == 2)
        EXPECT_NE(g[step.selected_node_].pos, p2);
}
